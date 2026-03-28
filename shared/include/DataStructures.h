#pragma once
#include <string>
#include <vector>
#include <boost/json.hpp>

class User {
private:
    int id;
    bool hasAiAccess;
    std::string username;
    std::string email;
    std::string password;
    std::vector<int> groupIds;
    std::vector<int> pinnedGroupIds;

public:
    User(int id, const std::string& username, const std::string& email, const std::string& password, bool hasAiAccess);

    int getId() const;
    std::string getUsername() const;
    std::string getEmail() const;
    std::string getPassword() const;
    bool getAiAcess() const;
    std::vector<int> getGroupIds() const;
    std::vector<int> getPinnedGroupIds() const;

    void setId(int id);
    void setUsername(const std::string& username);
    void setEmail(const std::string& email);
    void setPassword(const std::string& password);
    void setAiAccess(bool hasAiAccess);
    void setGroupIds(const std::vector<int>& groupIds);
    void setPinnedGroupIds(const std::vector<int>& pinnedGroupIds);
    void addGroupId(int groupId);
    void addPinnedGroupId(int pinnedGroupId);

    // serialize the object to json for server communication
    boost::json::object toJson() const;
    boost::json::object toSafeJson() const; // same as toJson but without the password
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

class Message {
private:
    int userId;
    std::string text;

public:
    Message(int userId, const std::string& text);

    int getUserId() const;
    std::string getText() const;

    void setUserId(int userId);
    void setText(const std::string& text);

    boost::json::object toJson() const;
    static Message fromJson(const boost::json::object& obj);
};

class StudyGroup {
private:
    int id;
    std::string name;
    std::vector<int> memberIds;
    std::vector<int> taskIds;
    std::vector<Message> messages;

public:
    StudyGroup(int id, const std::string& name);

    int getId() const;
    std::string getName() const;
    std::vector<int> getMemberIds() const;
    std::vector<int> getTaskIds() const;
    std::vector<Message> getMessages() const;

    void setId(int id);
    void setName(const std::string& name);
    void setMemberIds(const std::vector<int>& memberIds);
    void addMemberId(int memberId);
    void setTaskIds(const std::vector<int>& taskIds);
    void addTaskId(int taskId);
    void setMessages(const std::vector<Message>& messages);
    void addMessage(const Message& message);

    boost::json::object toJson() const;
    static StudyGroup fromJson(const boost::json::object& obj);
};

class LoginPayload {
private:
    User user;
    std::vector<StudyGroup> studyGroups;
    std::vector<Task> tasks;
    std::string sessionToken;

public:
    LoginPayload(const User& user, const std::vector<StudyGroup>& studyGroups, const std::vector<Task>& tasks, const std::string& sessionToken = "");

    User getUser() const;
    std::vector<StudyGroup> getStudyGroups() const;
    std::vector<Task> getTasks() const;
    std::string getSessionToken() const;

    boost::json::object toJson() const;
    static LoginPayload fromJson(const boost::json::object& obj);
};