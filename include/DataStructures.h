#pragma once
#include <string>
#include <vector>
#include <boost/json.hpp>


class User {
private:
    int id;
    bool hasAiAccess;
    std::string username;

public:
    User(int id, const std::string& username, bool hasAiAccess);

    int getId() const;
    std::string getUsername() const;
    bool getAiAcess() const;

    void setAiAccess(bool hasAiAccess);
    void setId(int id);
    void setUsername(const std::string& username);

    // serialize the object to json for server communication
    boost::json::object toJson() const;
    static User fromJson(const boost::json::object& obj);
};


class Task {
private:
    int id;
    std::string title;
    std::string tag;
    bool isCompleted;
    int ownerId;
    int assignedToId;
    int groupId;

public:
    Task(int id, const std::string& title, const std::string& tag, int ownerId);
    Task(int id, const std::string& title, const std::string& tag, bool isCompleted, int ownerId, int assignedToId, int groupId);

    int getId() const;
    std::string getTitle() const;
    std::string getTag() const;
    bool getIsCompleted() const;
    int getOwnerId() const;
    int getAssignedToId() const;
    int getGroupId() const;

    void setId(int id);
    void setTitle(const std::string& title);
    void setTag(const std::string& tag);
    void setIsCompleted(bool isCompleted);
    void setOwnerId(int ownerId);
    void setAssignedToId(int assignedToId);
    void setGroupId(int groupId);

    boost::json::object toJson() const;
    static Task fromJson(const boost::json::object& obj);
};

class StudyGroup {
private:
    int id;
    std::string name;
    std::vector<int> memberIds;

public:
    StudyGroup(int id, const std::string& name);

    int getId() const;
    std::string getName() const;
    std::vector<int> getMemberIds() const;

    void setId(int id);
    void setName(const std::string& name);
    void setMemberIds(const std::vector<int>& memberIds);
    void addMemberId(int memberId);

    boost::json::object toJson() const;
    static StudyGroup fromJson(const boost::json::object& obj);
};