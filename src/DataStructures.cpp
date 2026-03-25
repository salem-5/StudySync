#include "DataStructures.h"

User::User(int id, const std::string& username, bool hasAiAccess) : id(id), username(username), hasAiAccess(hasAiAccess) {}
int User::getId() const { return id;}
std::string User::getUsername() const { return username; }
bool User::getAiAcess() const { return hasAiAccess; }


void User::setId(int id) { this->id = id; }
void User::setUsername(const std::string& username) { this->username = username; }
void User::setAiAccess(bool hasAiAccess) { this->hasAiAccess = hasAiAccess; }

boost::json::object User::toJson() const {
    boost::json::object jsonObject = {
        { "id", id },
        { "username", username },
        { "aiAccess", hasAiAccess }
    };
    return jsonObject;
}

User User::fromJson(const boost::json::object& obj) {
    int id = obj.at("id").as_int64();
    std::string username = obj.at("username").as_string().c_str();
    bool hasAiAccess = obj.at("aiAccess").as_bool();
    return User(id, username, hasAiAccess);
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

void StudyGroup::setId(int id) { this->id = id; }
void StudyGroup::setName(const std::string& name) { this->name = name; }
void StudyGroup::setMemberIds(const std::vector<int>& memberIds) { this->memberIds = memberIds; }
void StudyGroup::addMemberId(int memberId) { this->memberIds.push_back(memberId); }

boost::json::object StudyGroup::toJson() const {
    boost::json::array membersArray;
    for (int memberId : memberIds) {
        membersArray.push_back(memberId);
    }
    boost::json::object jsonObject = {
        { "id", id },
        { "name", name },
        { "memberIds", membersArray }
    };
    return jsonObject;
}

StudyGroup StudyGroup::fromJson(const boost::json::object& obj) {
    int id = obj.at("id").as_int64();
    std::string name = obj.at("name").as_string().c_str();

    StudyGroup group(id, name);

    boost::json::array membersArray = obj.at("memberIds").as_array();
    std::vector<int> parsedMemberIds;
    for (int i = 0; i < membersArray.size(); i++)
        parsedMemberIds.push_back(membersArray[i].as_int64());
    group.setMemberIds(parsedMemberIds);

    return group;
}