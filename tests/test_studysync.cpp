#include <gtest/gtest.h>
#include <string>
#include <vector>

//
// ============================
// 🌐 Mock Networking Layer
// ============================
//

class INetwork {
public:
    virtual std::string sendRequest(const std::string& req) = 0;
};

class MockNetwork : public INetwork {
public:
    std::string sendRequest(const std::string& req) override {
        if (req == "signup") return "success";
        if (req == "login") return "success";
        if (req == "fail_login") return "error";
        if (req == "create_task") return "task_created";
        return "unknown";
    }
};

//
// ============================
// 🔐 Auth Service
// ============================
//

class AuthService {
    INetwork* net;

public:
    AuthService(INetwork* n) : net(n) {}

    bool signup(const std::string& email, const std::string& password) {
        if (email.find("@") == std::string::npos) return false;
        return net->sendRequest("signup") == "success";
    }

    bool login(const std::string& email, const std::string& password) {
        if (password == "wrong") return false;
        return net->sendRequest("login") == "success";
    }
};

//
// ============================
// 📋 Task
// ============================
//

class Task {
    std::string title;
    std::string assignedUser;

public:
    Task(const std::string& t) : title(t) {}

    std::string getTitle() const {
        return title;
    }

    void assignTo(const std::string& user) {
        assignedUser = user;
    }

    std::string getAssignedUser() const {
        return assignedUser;
    }
};

//
// ============================
// 👥 Group
// ============================
//

class Group {
    std::string name;
    std::vector<std::string> members;

public:
    Group(const std::string& n) : name(n) {}

    std::string getName() const {
        return name;
    }

    void addMember(const std::string& user) {
        members.push_back(user);
    }

    bool hasMember(const std::string& user) const {
        for (const auto& m : members) {
            if (m == user) return true;
        }
        return false;
    }
};

//
// ============================
// 💬 Chat
// ============================
//

class Chat {
    std::vector<std::string> messages;

public:
    void send(const std::string& msg) {
        messages.push_back(msg);
    }

    std::string getLastMessage() const {
        if (messages.empty()) return "";
        return messages.back();
    }
};

//
// ============================
// ⏱ Timer
// ============================
//

class Timer {
    bool running = false;

public:
    void start(int minutes) {
        if (minutes > 0) running = true;
    }

    bool isRunning() const {
        return running;
    }
};

//
// ============================
// 🧪 UNIT TESTS (10)
// ============================
//

// 1. Signup success
TEST(AuthTest, SignupValid) {
    MockNetwork net;
    AuthService auth(&net);

    EXPECT_TRUE(auth.signup("test@mail.com", "1234"));
}

// 2. Signup invalid email
TEST(AuthTest, SignupInvalidEmail) {
    MockNetwork net;
    AuthService auth(&net);

    EXPECT_FALSE(auth.signup("invalidemail", "1234"));
}

// 3. Login success
TEST(AuthTest, LoginSuccess) {
    MockNetwork net;
    AuthService auth(&net);

    EXPECT_TRUE(auth.login("test@mail.com", "1234"));
}

// 4. Login fail
TEST(AuthTest, LoginFail) {
    MockNetwork net;
    AuthService auth(&net);

    EXPECT_FALSE(auth.login("test@mail.com", "wrong"));
}

// 5. Create task
TEST(TaskTest, CreateTask) {
    Task t("Study Math");

    EXPECT_EQ(t.getTitle(), "Study Math");
}

// 6. Assign task
TEST(TaskTest, AssignTask) {
    Task t("Study Math");
    t.assignTo("Ali");

    EXPECT_EQ(t.getAssignedUser(), "Ali");
}

// 7. Create group
TEST(GroupTest, CreateGroup) {
    Group g("CS Project");

    EXPECT_EQ(g.getName(), "CS Project");
}

// 8. Add member to group
TEST(GroupTest, AddMember) {
    Group g("CS Project");
    g.addMember("Ahmed");

    EXPECT_TRUE(g.hasMember("Ahmed"));
}

// 9. Send chat message
TEST(ChatTest, SendMessage) {
    Chat c;
    c.send("Hello World");

    EXPECT_EQ(c.getLastMessage(), "Hello World");
}

// 10. Timer start
TEST(TimerTest, StartTimer) {
    Timer t;
    t.start(25);

    EXPECT_TRUE(t.isRunning());
}
