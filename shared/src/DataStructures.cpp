#include "DataStructures.h"

User::User(int id, const std::string& username, const std::string& email, const std::string& password, bool hasAiAccess)
    : id(id), username(username), email(email), password(password), hasAiAccess(hasAiAccess) {}

int User::getId() const { return id;}
std::string User::getUsername() const { return username; }
std::string User::getEmail() const { return email; }
std::string User::getPassword() const { return password; }
bool User::getAiAcess() const { return hasAiAccess; }
std::vector<int> User::getGroupIds() const { return groupIds; }
std::vector<int> User::getPinnedGroupIds() const { return pinnedGroupIds; }

void User::setId(int id) { this->id = id; }
void User::setUsername(const std::string& username) { this->username = username; }
void User::setEmail(const std::string& email) { this->email = email; }
void User::setPassword(const std::string& password) { this->password = password; }
void User::setAiAccess(bool hasAiAccess) { this->hasAiAccess = hasAiAccess; }
void User::setGroupIds(const std::vector<int>& groupIds) { this->groupIds = groupIds; }
void User::setPinnedGroupIds(const std::vector<int>& pinnedGroupIds) { this->pinnedGroupIds = pinnedGroupIds; }
void User::addGroupId(int groupId) { this->groupIds.push_back(groupId); }
void User::addPinnedGroupId(int pinnedGroupId) { this->pinnedGroupIds.push_back(pinnedGroupId); }

boost::json::object User::toJson() const {
    boost::json::array gArray, pgArray;
    for (int gid : groupIds) gArray.push_back(gid);
    for (int pgid : pinnedGroupIds) pgArray.push_back(pgid);

    boost::json::object jsonObject = {
        { "id", id },
        { "username", username },
        { "email", email },
        { "password", password },
        { "aiAccess", hasAiAccess },
        { "groupIds", gArray },
        { "pinnedGroupIds", pgArray }
    };
    return jsonObject;
}

boost::json::object User::toSafeJson() const {
    boost::json::object jsonObject = toJson();
    jsonObject.erase("password");
    return jsonObject;
}

User User::fromJson(const boost::json::object& obj) {
    int id = obj.at("id").as_int64();
    std::string username = obj.at("username").as_string().c_str();
    std::string email = obj.contains("email") ? obj.at("email").as_string().c_str() : "";
    std::string password = obj.contains("password") ? obj.at("password").as_string().c_str() : "";
    bool hasAiAccess = obj.at("aiAccess").as_bool();

    User user(id, username, email, password, hasAiAccess);

    if (obj.contains("groupIds")) {
        for (const auto& item : obj.at("groupIds").as_array())
            user.addGroupId(item.as_int64());
    }
    if (obj.contains("pinnedGroupIds")) {
        for (const auto& item : obj.at("pinnedGroupIds").as_array())
            user.addPinnedGroupId(item.as_int64());
    }

    return user;
}



Task::Task(int id, const std::string &title, const std::string &tag, int ownerId)
    : id(id), title(title), tag(tag), ownerId(ownerId),
        isCompleted(false), assignedToId(0), groupId(0) {}

Task::Task(int id, const std::string &title, const std::string &tag, bool isCompleted, int ownerId, int assignedToId,int groupId)
    :id(id), title(title), tag(tag), isCompleted(isCompleted),
        ownerId(ownerId), assignedToId(assignedToId), groupId(groupId) {}

int Task::getId() const { return id; }
std::string Task::getTitle() const { return title; }
std::string Task::getTag() const { return tag; }

bool Task::getIsCompleted() const { return isCompleted; }
int Task::getOwnerId() const { return ownerId; }
int Task::getAssignedToId() const { return assignedToId; }
int Task::getGroupId() const { return groupId; }
void Task::setId(int id) { this->id = id; }
void Task::setTitle(const std::string& title) { this->title = title; }
void Task::setTag(const std::string& tag) { this->tag = tag; }
void Task::setIsCompleted(bool isCompleted) { this->isCompleted = isCompleted; }
void Task::setOwnerId(int ownerId) { this->ownerId = ownerId; }
void Task::setAssignedToId(int assignedToId) { this->assignedToId = assignedToId; }
void Task::setGroupId(int groupId) { this->groupId = groupId; }

boost::json::object Task::toJson() const {
    boost::json::object jsonObject = {
        { "id", id },
        { "title", title },
        { "tag", tag },
        { "isCompleted", isCompleted },
        { "ownerId", ownerId },
        { "assignedToId", assignedToId },
        { "groupId", groupId }
    };

    return jsonObject;
}

Task Task::fromJson(const boost::json::object &obj) {
    int id = obj.at("id").as_int64();
    std::string title = obj.at("title").as_string().c_str();
    std::string tag = obj.at("tag").as_string().c_str();
    bool isCompleted = obj.at("isCompleted").as_bool();
    int ownerId = obj.at("ownerId").as_int64();
    int assignedToId = obj.at("assignedToId").as_int64();
    int groupId = obj.at("groupId").as_int64();

    return Task(id,title,tag,isCompleted,ownerId,assignedToId,groupId);
}


StudyGroup::StudyGroup(int id, const std::string& name) : id(id), name(name) {}
int StudyGroup::getId() const { return id; }
std::string StudyGroup::getName() const { return name; }
std::vector<int> StudyGroup::getMemberIds() const { return memberIds; }
std::vector<int> StudyGroup::getTaskIds() const { return taskIds; }

void StudyGroup::setId(int id) { this->id = id; }
void StudyGroup::setName(const std::string& name) { this->name = name; }
void StudyGroup::setMemberIds(const std::vector<int>& memberIds) { this->memberIds = memberIds; }
void StudyGroup::addMemberId(int memberId) { this->memberIds.push_back(memberId); }
void StudyGroup::setTaskIds(const std::vector<int>& taskIds) { this->taskIds = taskIds; }
void StudyGroup::addTaskId(int taskId) { this->taskIds.push_back(taskId); }

boost::json::object StudyGroup::toJson() const {
    boost::json::array membersArray;
    for (int memberId : memberIds) {
        membersArray.push_back(memberId);
    }
    boost::json::array tasksArray;
    for (int taskId : taskIds) {
        tasksArray.push_back(taskId);
    }

    boost::json::object jsonObject = {
        { "id", id },
        { "name", name },
        { "memberIds", membersArray },
        { "taskIds", tasksArray }
    };
    return jsonObject;
}

StudyGroup StudyGroup::fromJson(const boost::json::object& obj) {
    int id = obj.at("id").as_int64();
    std::string name = obj.at("name").as_string().c_str();

    StudyGroup group(id, name);

    if (obj.contains("memberIds")) {
        boost::json::array membersArray = obj.at("memberIds").as_array();
        for (int i = 0; i < membersArray.size(); i++)
            group.addMemberId(membersArray[i].as_int64());
    }

    if (obj.contains("taskIds")) {
        boost::json::array tasksArray = obj.at("taskIds").as_array();
        for (int i = 0; i < tasksArray.size(); i++)
            group.addTaskId(tasksArray[i].as_int64());
    }

    return group;
}

LoginPayload::LoginPayload(const User& user, const std::vector<StudyGroup>& studyGroups, const std::vector<Task>& tasks, const std::string& sessionToken)
    : user(user), studyGroups(studyGroups), tasks(tasks), sessionToken(sessionToken) {}

User LoginPayload::getUser() const { return user; }
std::vector<StudyGroup> LoginPayload::getStudyGroups() const { return studyGroups; }
std::vector<Task> LoginPayload::getTasks() const { return tasks; }
std::string LoginPayload::getSessionToken() const { return sessionToken; }

boost::json::object LoginPayload::toJson() const {
    boost::json::array groupsArray;
    for (const auto& group : studyGroups) {
        groupsArray.push_back(group.toJson());
    }

    boost::json::array tasksArray;
    for (const auto& task : tasks) {
        tasksArray.push_back(task.toJson());
    }

    return {
        { "user", user.toSafeJson() },
        { "studyGroups", groupsArray },
        { "tasks", tasksArray },
        { "sessionToken", sessionToken }
    };
}

LoginPayload LoginPayload::fromJson(const boost::json::object& obj) {
    User user = User::fromJson(obj.at("user").as_object());

    std::vector<StudyGroup> groups;
    for (const auto& item : obj.at("studyGroups").as_array()) {
        groups.push_back(StudyGroup::fromJson(item.as_object()));
    }

    std::vector<Task> tasks;
    for (const auto& item : obj.at("tasks").as_array()) {
        tasks.push_back(Task::fromJson(item.as_object()));
    }

    std::string sessionToken = obj.contains("sessionToken") ? obj.at("sessionToken").as_string().c_str() : "";

    return LoginPayload(user, groups, tasks, sessionToken);
}