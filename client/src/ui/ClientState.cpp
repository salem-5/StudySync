#include "ui/ClientState.h"
#include "LanguageManager.h"

std::unique_ptr<User> ClientState::currentUser = nullptr;
std::vector<StudyGroup> ClientState::studyGroups;
std::vector<Task> ClientState::tasks;
std::string ClientState::sessionToken;
std::unordered_map<int, std::string> ClientState::usernameCache;
std::vector<StudyGroup> ClientState::pendingInvites;

const User* ClientState::getUser() {
    return currentUser.get();
}

const std::vector<StudyGroup>& ClientState::getStudyGroups() {
    return studyGroups;
}

const std::vector<Task>& ClientState::getTasks() {
    return tasks;
}

const StudyGroup* ClientState::getGroupById(int id) {
    for (const auto& group : studyGroups) {
        if (group.getId() == id) return &group;
    }
    return nullptr;
}

bool ClientState::isGroupPinned(int groupId) {
    if (!currentUser) return false;
    std::vector<int> pinned = currentUser->getPinnedGroupIds();
    for (int id : pinned) {
        if (id == groupId) return true;
    }
    return false;
}

void ClientState::mockTogglePinGroup(int groupId) {
    if (!currentUser) return;
    std::vector<int> pinned = currentUser->getPinnedGroupIds();
    int foundIndex = -1;
    for (size_t i = 0; i < pinned.size(); ++i) {
        if (pinned[i] == groupId) {
            foundIndex = static_cast<int>(i);
            break;
        }
    }

    if (foundIndex != -1) {
        pinned.erase(pinned.begin() + foundIndex);
        currentUser->setPinnedGroupIds(pinned);
    } else {
        currentUser->addPinnedGroupId(groupId);
    }
}

void ClientState::initDummyData() {
    currentUser = std::make_unique<User>(1, "CoolDev99", "dev@example.com", "", true);
    currentUser->addGroupId(101);

    StudyGroup group(101, "Late Night C++ Hackers");
    group.addMemberId(1);
    studyGroups.push_back(group);

    StudyGroup inviteGroup(102, "Algorithm Study Squad");
    inviteGroup.addMemberId(2);
    inviteGroup.addInvitedMemberId(1);
    pendingInvites.push_back(inviteGroup);

    tasks.push_back(Task(201, "Build the UI", "frontend", false, 1, 1, 101));
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

void ClientState::mockCreateGroup(const std::string& groupName) {
    int newId = studyGroups.empty() ? 100 : studyGroups.back().getId() + 1;
    StudyGroup newGroup(newId, groupName);
    if (currentUser) {
        newGroup.addMemberId(currentUser->getId());
        currentUser->addGroupId(newId);
    }
    studyGroups.push_back(newGroup);
}
void ClientState::mockCreateTask(int groupId, const std::string& title, const std::string& category, int assigneeId) {
    if (!currentUser) return;

    int newId = tasks.empty() ? 200 : tasks.back().getId() + 1;
    Task newTask(newId, title, category, false, currentUser->getId(), assigneeId, groupId);
    tasks.push_back(newTask);
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            group.addTaskId(newId);
            break;
        }
    }
}
void ClientState::mockSendMessage(int groupId, const std::string& text) {
    if (!currentUser) return;
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            Message newMsg(currentUser->getId(), text);
            group.addMessage(newMsg);
            break;
        }
    }
}

void ClientState::mockRequestUsername(int userId) {
    std::string serverResponse;
    if (userId == 2) {
        serverResponse = "Alice";
    } else if (userId == 3) {
        serverResponse = "Bob";
    } else {
        serverResponse = "-1";
    }
    if (serverResponse == "-1") {
        usernameCache[userId] = LanguageManager::tr("user.unknown").toStdString();;
    } else {
        usernameCache[userId] = serverResponse;
    }
}

std::string ClientState::getUsername(int userId) {
    if (currentUser && currentUser->getId() == userId) {
        std::string str = LanguageManager::tr("user.me").toStdString();
        return str;
    }
    if (usernameCache.find(userId) == usernameCache.end()) {
        mockRequestUsername(userId);
    }
    return usernameCache[userId];
}

int ClientState::mockGetUserIdByUsername(const std::string& username) {
    if (username == "CoolDev99") return 1;
    if (username == "Alice") return 2;
    if (username == "Bob") return 3;

    int fakeId = std::hash<std::string>{}(username) % 1000 + 10;
    usernameCache[fakeId] = username;
    return fakeId;
}

void ClientState::mockToggleTaskCompletion(int taskId, bool completed) {
    for (auto& task : tasks) {
        if (task.getId() == taskId) {
            task.setIsCompleted(completed);
            break;
        }
    }
}

void ClientState::mockEditTask(int taskId, const std::string& title, const std::string& tag, int assigneeId) {
    for (auto& task : tasks) {
        if (task.getId() == taskId) {
            task.setTitle(title);
            task.setTag(tag);
            task.setAssignedToId(assigneeId);
            break;
        }
    }
}

void ClientState::mockAddMemberToGroup(int groupId, const std::string& username) {
    int userId = mockGetUserIdByUsername(username);

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

void ClientState::mockRemoveMemberFromGroup(int groupId, int userId) {
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            group.removeMemberId(userId);
            break;
        }
    }
}

void ClientState::mockDeleteGroup(int groupId) {
    tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
        [groupId](const Task& t) { return t.getGroupId() == groupId; }), tasks.end());

    studyGroups.erase(std::remove_if(studyGroups.begin(), studyGroups.end(),
        [groupId](const StudyGroup& g) { return g.getId() == groupId; }), studyGroups.end());

    if (currentUser) {
        std::vector<int> pinned = currentUser->getPinnedGroupIds();
        pinned.erase(std::remove(pinned.begin(), pinned.end(), groupId), pinned.end());
        currentUser->setPinnedGroupIds(pinned);
    }
}

void ClientState::mockDeleteTask(int taskId) {
    tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
        [taskId](const Task& t) { return t.getId() == taskId; }), tasks.end());
}
void ClientState::mockInviteMemberToGroup(int groupId, const std::string& username) {
    int userId = mockGetUserIdByUsername(username);
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            auto members = group.getMemberIds();
            auto invited = group.getInvitedMemberIds();
            if (std::find(members.begin(), members.end(), userId) == members.end()
                &&std::find(invited.begin(), invited.end(), userId) == invited.end()) {
                group.addInvitedMemberId(userId);
            }
            break;
        }
    }
}

void ClientState::mockCancelInvite(int groupId, int userId) {
    for (auto& group : studyGroups) {
        if (group.getId() == groupId) {
            group.removeInvitedMemberId(userId);
            break;
        }
    }
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

void ClientState::mockAcceptInvite(int groupId, int userId) {
    auto it = std::find_if(pendingInvites.begin(), pendingInvites.end(),
        [groupId](const StudyGroup& g) { return g.getId() == groupId; });

    if (it != pendingInvites.end()) {
        StudyGroup acceptedGroup = *it;

        acceptedGroup.removeInvitedMemberId(userId);
        acceptedGroup.addMemberId(userId);

        studyGroups.push_back(acceptedGroup);
        pendingInvites.erase(it);

        if (currentUser && currentUser->getId() == userId) {
            currentUser->addGroupId(groupId);
        }
    }
}

void ClientState::mockDenyInvite(int groupId, int userId) {
    pendingInvites.erase(std::remove_if(pendingInvites.begin(),
        pendingInvites.end(), [groupId](const StudyGroup& g) {
            return g.getId() == groupId;
        }), pendingInvites.end());
}

void ClientState::mockCreateUser(const std::string& username, const std::string& email, const std::string& password) {
    if (!currentUser) {
        int fakeId = std::hash<std::string>{}(username) % 1000 + 10;
        currentUser = std::make_unique<User>(fakeId, username, email, password, false);
        usernameCache[fakeId] = username;
    }
}

void ClientState::mockDeleteUser(int userId) {
    if (currentUser && currentUser->getId() == userId) {
        clear();
    }
}