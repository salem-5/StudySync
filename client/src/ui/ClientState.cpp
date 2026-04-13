#include "ui/ClientState.h"
#include "LanguageManager.h"
#include <algorithm>

std::unique_ptr<User> ClientState::currentUser = nullptr;
std::vector<StudyGroup> ClientState::studyGroups;
std::vector<Task> ClientState::tasks;
std::string ClientState::sessionToken;
std::unordered_map<int, std::string> ClientState::usernameCache;
std::vector<StudyGroup> ClientState::pendingInvites;
std::shared_ptr<ServerAPI> ClientState::apiInstance = nullptr;

void ClientState::setApi(std::shared_ptr<ServerAPI> newApi) { apiInstance = newApi; }
std::shared_ptr<ServerAPI> ClientState::getApi() { return apiInstance; }
const User* ClientState::getUser() { return currentUser.get(); }
const std::vector<StudyGroup>& ClientState::getStudyGroups() { return studyGroups; }
const std::vector<Task>& ClientState::getTasks() { return tasks; }

const StudyGroup* ClientState::getGroupById(int id) {
    for (const auto& group : studyGroups) {
        if (group.getId() == id) return &group;
    }
    return nullptr;
}

bool ClientState::isGroupPinned(int groupId) {
    if (!currentUser) return false;
    std::vector<int> pinned = currentUser->getPinnedGroupIds();
    return std::find(pinned.begin(), pinned.end(), groupId) != pinned.end();
}

std::string ClientState::getUsername(int userId) {
    if (currentUser && currentUser->getId() == userId) return LanguageManager::tr("user.me").toStdString();
    if (usernameCache.find(userId) == usernameCache.end()) return LanguageManager::tr("user.unknown").toStdString();
    return usernameCache[userId];
}

int ClientState::getUserIdByUsername(const std::string& username) {
    if (currentUser && currentUser->getUsername() == username) return currentUser->getId();
    for (const auto& pair : usernameCache) {
        if (pair.second == username) return pair.first;
    }
    return -1;
}

std::vector<const StudyGroup*> ClientState::getPendingInvites(int userId) {
    std::vector<const StudyGroup*> pending;
    for (const auto& group : pendingInvites) {
        auto invited = group.getInvitedMemberIds();
        if (std::find(invited.begin(), invited.end(), userId) != invited.end()) {
            pending.push_back(&group);
        }
    }
    return pending;
}

void ClientState::loadFromPayload(const LoginPayload& payload) {
    currentUser = std::make_unique<User>(payload.getUser());
    studyGroups = payload.getStudyGroups();
    pendingInvites = payload.getPendingInvites();
    tasks = payload.getTasks();
    sessionToken = payload.getSessionToken();
}

void ClientState::clear() {
    currentUser.reset();
    studyGroups.clear();
    pendingInvites.clear();
    tasks.clear();
    usernameCache.clear();
    sessionToken = "";
}

void ClientState::initDummyData() {}
void ClientState::togglePinGroup(int groupId) {
    static std::unordered_set<int> togglingGroups;
    if (togglingGroups.count(groupId)) return;

    if (!currentUser) return;
    std::vector<int> pinned = currentUser->getPinnedGroupIds();
    auto it = std::find(pinned.begin(), pinned.end(), groupId);
    if (it != pinned.end()) pinned.erase(it);
    else pinned.push_back(groupId);
    currentUser->setPinnedGroupIds(pinned);

    if (apiInstance) {
        togglingGroups.insert(groupId);
        apiInstance->togglePinGroup(groupId, [groupId](bool success) { togglingGroups.erase(groupId); });
    }
}

void ClientState::deleteGroup(int groupId) {
    auto it = std::find_if(studyGroups.begin(), studyGroups.end(), [groupId](const StudyGroup& g) { return g.getId() == groupId; });
    if (it != studyGroups.end()) {
        studyGroups.erase(it); // Delete locally
        tasks.erase(std::remove_if(tasks.begin(), tasks.end(), [groupId](const Task& t) { return t.getGroupId() == groupId; }), tasks.end());
        if (currentUser) {
            std::vector<int> pinned = currentUser->getPinnedGroupIds();
            pinned.erase(std::remove(pinned.begin(), pinned.end(), groupId), pinned.end());
            currentUser->setPinnedGroupIds(pinned);
        }
        if (apiInstance) apiInstance->deleteGroup(groupId, nullptr); // send Network Request
    }
}

void ClientState::deleteTask(int taskId) {
    auto it = std::find_if(tasks.begin(), tasks.end(), [taskId](const Task& t) { return t.getId() == taskId; });
    if (it != tasks.end()) {
        tasks.erase(it);
        if (apiInstance) apiInstance->deleteTask(taskId, nullptr);
    }
}

void ClientState::toggleTaskCompletion(int taskId, bool completed) {
    for (auto& task : tasks) {
        if (task.getId() == taskId) {
            if (task.getIsCompleted() != completed) {
                task.setIsCompleted(completed);
                if (apiInstance) apiInstance->toggleTaskCompletion(taskId, completed, nullptr);
            }
            break;
        }
    }
}

void ClientState::editTask(int taskId, const std::string& title, const std::string& tag, int assigneeId) {
    for (auto& task : tasks) {
        if (task.getId() == taskId) {
            if (task.getTitle() != title || task.getTag() != tag || task.getAssignedToId() != assigneeId) {
                task.setTitle(title);
                task.setTag(tag);
                task.setAssignedToId(assigneeId);
                if (apiInstance) apiInstance->editTask(taskId, title, tag, assigneeId, nullptr);
            }
            break;
        }
    }
}

void ClientState::removeMemberFromGroup(int groupId, int userId) {
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            auto members = group.getMemberIds();
            if (std::find(members.begin(), members.end(), userId) != members.end()) {
                group.removeMemberId(userId);
                if (apiInstance) apiInstance->removeMemberFromGroup(groupId, userId, nullptr);
            }
            break;
        }
    }
}

void ClientState::cancelInvite(int groupId, int userId) {
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            auto invited = group.getInvitedMemberIds();
            if (std::find(invited.begin(), invited.end(), userId) != invited.end()) {
                group.removeInvitedMemberId(userId);
                if (apiInstance) apiInstance->cancelInvite(groupId, userId, nullptr);
            }
            break;
        }
    }
}

void ClientState::acceptInvite(int groupId, int userId) {
    auto it = std::find_if(pendingInvites.begin(), pendingInvites.end(), [groupId](const StudyGroup& g) { return g.getId() == groupId; });
    if (it != pendingInvites.end()) {
        StudyGroup acceptedGroup = *it;
        acceptedGroup.removeInvitedMemberId(userId);
        acceptedGroup.addMemberId(userId);
        studyGroups.push_back(acceptedGroup);
        pendingInvites.erase(it);

        if (currentUser && currentUser->getId() == userId) currentUser->addGroupId(groupId);
        if (apiInstance) apiInstance->acceptInvite(groupId, userId, nullptr);
    }
}

void ClientState::denyInvite(int groupId, int userId) {
    auto it = std::find_if(pendingInvites.begin(), pendingInvites.end(), [groupId](const StudyGroup& g) { return g.getId() == groupId; });
    if (it != pendingInvites.end()) {
        pendingInvites.erase(it);
        if (apiInstance) apiInstance->denyInvite(groupId, userId, nullptr);
    }
}


void ClientState::createGroup(const std::string& groupName) {
    if (apiInstance) apiInstance->createGroup(groupName, nullptr);
}

void ClientState::createGroup(int groupId, const std::string& groupName) {
    StudyGroup newGroup(groupId, groupName);
    if (currentUser) {
        newGroup.addMemberId(currentUser->getId());
        currentUser->addGroupId(groupId);
    }
    studyGroups.push_back(newGroup);
}

void ClientState::createTask(int groupId, const std::string& title, const std::string& category, int assigneeId) {
    if (apiInstance) apiInstance->createTask(groupId, title, category, assigneeId, nullptr);
}

void ClientState::createTask(int taskId, int groupId, const std::string& title, const std::string& category, int assigneeId) {
    if (!currentUser) return;
    Task newTask(taskId, title, category, false, currentUser->getId(), assigneeId, groupId);
    tasks.push_back(newTask);
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            group.addTaskId(taskId);
            break;
        }
    }
}


void ClientState::sendMessage(int groupId, const std::string& text) {
    if (apiInstance) apiInstance->sendMessage(groupId, text, nullptr);
}

void ClientState::sendMessage(int groupId, int userId, const std::string& text) {
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            group.addMessage(Message(userId, text));
            break;
        }
    }
}

void ClientState::inviteMemberToGroup(int groupId, const std::string& username) {
    if (apiInstance) apiInstance->inviteMemberToGroup(groupId, username, nullptr);
}

void ClientState::inviteMemberToGroup(int groupId, int userId) {
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            auto members = group.getMemberIds();
            auto invited = group.getInvitedMemberIds();
            if (std::find(members.begin(), members.end(), userId) == members.end() && std::find(invited.begin(), invited.end(), userId) == invited.end()) {
                group.addInvitedMemberId(userId);
            }
            break;
        }
    }
}


void ClientState::createUser(const std::string& username, const std::string& email, const std::string& password) {
    if (apiInstance) apiInstance->createUser(username, email, password, nullptr);
}

void ClientState::deleteUser(int userId) {
    if (currentUser && currentUser->getId() == userId) {
        if (apiInstance) apiInstance->deleteUser(userId, nullptr);
        clear();
    }
}

void ClientState::requestUsername(int userId, const std::string& username) {
    usernameCache[userId] = username;
}

void ClientState::addMemberToGroup(int groupId, int userId) {
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            auto members = group.getMemberIds();
            if (std::find(members.begin(), members.end(), userId) == members.end()) {
                group.addMemberId(userId);
            }
            break;
        }
    }
}