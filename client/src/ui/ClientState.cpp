#include "ui/ClientState.h"

std::unique_ptr<User> ClientState::currentUser = nullptr;
std::vector<StudyGroup> ClientState::studyGroups;
std::vector<Task> ClientState::tasks;
std::string ClientState::sessionToken;


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
    group.addTaskId(201);

    group.addMessage(Message(2, "Did anyone figure out the networking bug?"));
    group.addMessage(Message(1, "Yeah, I'm building the UI for it right now."));
    studyGroups.push_back(group);

    Task task1(201, "Build the UI", "frontend", false, 1, 1, 101);
    Task task2(202, "Setup Postgres DB", "backend", false, 1, 0, 101);
    tasks.push_back(task1);
    tasks.push_back(task2);
}

void ClientState::loadFromPayload(const LoginPayload& payload) {
    currentUser = std::make_unique<User>(payload.getUser());
    studyGroups = payload.getStudyGroups();
    tasks = payload.getTasks();
    sessionToken = payload.getSessionToken();
}

void ClientState::clear() {
    currentUser.reset();
    studyGroups.clear();
    tasks.clear();
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
        usernameCache[userId] = "Unknown User";
    } else {
        usernameCache[userId] = serverResponse;
    }
}

std::string ClientState::getUsername(int userId) {
    if (currentUser && currentUser->getId() == userId) {
        return "Me";
    }
    if (usernameCache.find(userId) == usernameCache.end()) {
        mockRequestUsername(userId);
    }
    return usernameCache[userId];
}