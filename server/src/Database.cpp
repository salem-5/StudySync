#include "Database.h"
#include <iostream>

Database& Database::getInstance() {
    static Database instance;
    return instance;
}

Database::Database() {
    if (sqlite3_open("app_database.db", &db) != SQLITE_OK) {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db)));
    }

    executeQuery("PRAGMA foreign_keys = ON;");
    initTables();
    initTriggers();
}

Database::~Database() {
    if (db) {
        sqlite3_close(db);
    }
}

void Database::executeQuery(const std::string& query) {
    char* errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error("SQL Error: " + error);
    }
}

void Database::initTables() {
    std::lock_guard<std::mutex> lock(dbMutex);

    const char* tables = R"(
        CREATE TABLE IF NOT EXISTS SystemState (
            id INTEGER PRIMARY KEY CHECK (id = 1),
            sync_counter INTEGER NOT NULL DEFAULT 0
        );

        INSERT OR IGNORE INTO SystemState (id, sync_counter) VALUES (1, 0);

        CREATE TABLE IF NOT EXISTS Users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            email TEXT,
            password TEXT,
            has_ai_access INTEGER DEFAULT 0
        );

        CREATE TABLE IF NOT EXISTS StudyGroups (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL
        );

        CREATE TABLE IF NOT EXISTS GroupMembers (
            group_id INTEGER,
            user_id INTEGER,
            PRIMARY KEY (group_id, user_id),
            FOREIGN KEY(group_id) REFERENCES StudyGroups(id) ON DELETE CASCADE,
            FOREIGN KEY(user_id) REFERENCES Users(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS GroupInvites (
            group_id INTEGER,
            user_id INTEGER,
            PRIMARY KEY (group_id, user_id),
            FOREIGN KEY(group_id) REFERENCES StudyGroups(id) ON DELETE CASCADE,
            FOREIGN KEY(user_id) REFERENCES Users(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS PinnedGroups (
            user_id INTEGER,
            group_id INTEGER,
            PRIMARY KEY (user_id, group_id),
            FOREIGN KEY(group_id) REFERENCES StudyGroups(id) ON DELETE CASCADE,
            FOREIGN KEY(user_id) REFERENCES Users(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS Tasks (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            title TEXT NOT NULL,
            tag TEXT,
            is_completed INTEGER DEFAULT 0,
            owner_id INTEGER,
            assigned_to_id INTEGER,
            group_id INTEGER,
            FOREIGN KEY(group_id) REFERENCES StudyGroups(id) ON DELETE CASCADE,
            FOREIGN KEY(owner_id) REFERENCES Users(id) ON DELETE CASCADE
        );

        CREATE TABLE IF NOT EXISTS Messages (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            group_id INTEGER,
            user_id INTEGER,
            text TEXT NOT NULL,
            FOREIGN KEY(group_id) REFERENCES StudyGroups(id) ON DELETE CASCADE,
            FOREIGN KEY(user_id) REFERENCES Users(id) ON DELETE CASCADE
        );
    )";
    executeQuery(tables);
}

void Database::initTriggers() {
    std::vector<std::string> tables = { "Users", "StudyGroups", "GroupMembers", "GroupInvites", "PinnedGroups", "Tasks", "Messages" };

    for (const auto& table : tables) {
        executeQuery("CREATE TRIGGER IF NOT EXISTS sync_inc_ins_" + table + " AFTER INSERT ON " + table + " BEGIN UPDATE SystemState SET sync_counter = sync_counter + 1 WHERE id = 1; END;");
        executeQuery("CREATE TRIGGER IF NOT EXISTS sync_inc_upd_" + table + " AFTER UPDATE ON " + table + " BEGIN UPDATE SystemState SET sync_counter = sync_counter + 1 WHERE id = 1; END;");
        executeQuery("CREATE TRIGGER IF NOT EXISTS sync_inc_del_" + table + " AFTER DELETE ON " + table + " BEGIN UPDATE SystemState SET sync_counter = sync_counter + 1 WHERE id = 1; END;");
    }
}

int Database::getSyncCounter() {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    int counter = 0;
    if (sqlite3_prepare_v2(db, "SELECT sync_counter FROM SystemState WHERE id = 1;", -1, &stmt, nullptr) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            counter = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return counter;
}

int Database::getUserIdByUsername(const std::string& username) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    int id = -1;
    if (sqlite3_prepare_v2(db, "SELECT id FROM Users WHERE username = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            id = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);
    return id;
}

std::string Database::getUsernameById(int userId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    std::string username = "";
    if (sqlite3_prepare_v2(db, "SELECT username FROM Users WHERE id = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, userId);
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            username = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        }
    }
    sqlite3_finalize(stmt);
    return username;
}

int Database::createGroup(const std::string& groupName, int creatorId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    int newGroupId = -1;

    if (sqlite3_prepare_v2(db, "INSERT INTO StudyGroups (name) VALUES (?);", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, groupName.c_str(), -1, SQLITE_STATIC);
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            newGroupId = sqlite3_last_insert_rowid(db);
        }
    }
    sqlite3_finalize(stmt);

    if (newGroupId != -1) {
        sqlite3_prepare_v2(db, "INSERT INTO GroupMembers (group_id, user_id) VALUES (?, ?);", -1, &stmt, nullptr);
        sqlite3_bind_int(stmt, 1, newGroupId);
        sqlite3_bind_int(stmt, 2, creatorId);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
    return newGroupId;
}

void Database::deleteGroup(int groupId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "DELETE FROM StudyGroups WHERE id = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, groupId);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

bool Database::isGroupPinned(int userId, int groupId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    bool pinned = false;
    if (sqlite3_prepare_v2(db, "SELECT 1 FROM PinnedGroups WHERE user_id = ? AND group_id = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_bind_int(stmt, 2, groupId);
        if (sqlite3_step(stmt) == SQLITE_ROW) pinned = true;
    }
    sqlite3_finalize(stmt);
    return pinned;
}

void Database::togglePinGroup(int userId, int groupId) {
    if (isGroupPinned(userId, groupId)) {
        std::lock_guard<std::mutex> lock(dbMutex);
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, "DELETE FROM PinnedGroups WHERE user_id = ? AND group_id = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, userId);
            sqlite3_bind_int(stmt, 2, groupId);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
    } else {
        std::lock_guard<std::mutex> lock(dbMutex);
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, "INSERT INTO PinnedGroups (user_id, group_id) VALUES (?, ?);", -1, &stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(stmt, 1, userId);
            sqlite3_bind_int(stmt, 2, groupId);
            sqlite3_step(stmt);
        }
        sqlite3_finalize(stmt);
    }
}

void Database::addMemberToGroup(int groupId, int userId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "INSERT OR IGNORE INTO GroupMembers (group_id, user_id) VALUES (?, ?);", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, groupId);
        sqlite3_bind_int(stmt, 2, userId);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

void Database::removeMemberFromGroup(int groupId, int userId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "DELETE FROM GroupMembers WHERE group_id = ? AND user_id = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, groupId);
        sqlite3_bind_int(stmt, 2, userId);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

void Database::inviteMemberToGroup(int groupId, int userId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "INSERT OR IGNORE INTO GroupInvites (group_id, user_id) VALUES (?, ?);", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, groupId);
        sqlite3_bind_int(stmt, 2, userId);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

void Database::cancelInvite(int groupId, int userId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "DELETE FROM GroupInvites WHERE group_id = ? AND user_id = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, groupId);
        sqlite3_bind_int(stmt, 2, userId);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

void Database::acceptInvite(int groupId, int userId) {
    cancelInvite(groupId, userId);
    addMemberToGroup(groupId, userId);
}

void Database::denyInvite(int groupId, int userId) {
    cancelInvite(groupId, userId);
}

int Database::createTask(int groupId, const std::string& title, const std::string& tag, int ownerId, int assigneeId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    int newTaskId = -1;
    const char* sql = "INSERT INTO Tasks (title, tag, is_completed, owner_id, assigned_to_id, group_id) VALUES (?, ?, 0, ?, ?, ?);";

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, tag.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, ownerId);
        sqlite3_bind_int(stmt, 4, assigneeId);
        sqlite3_bind_int(stmt, 5, groupId);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            newTaskId = sqlite3_last_insert_rowid(db);
        }
    }
    sqlite3_finalize(stmt);
    return newTaskId;
}

void Database::toggleTaskCompletion(int taskId, bool completed) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "UPDATE Tasks SET is_completed = ? WHERE id = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, completed ? 1 : 0);
        sqlite3_bind_int(stmt, 2, taskId);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

void Database::editTask(int taskId, const std::string& title, const std::string& tag, int assigneeId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "UPDATE Tasks SET title = ?, tag = ?, assigned_to_id = ? WHERE id = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, title.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, tag.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_int(stmt, 3, assigneeId);
        sqlite3_bind_int(stmt, 4, taskId);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

void Database::deleteTask(int taskId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "DELETE FROM Tasks WHERE id = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, taskId);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

void Database::addMessage(int groupId, int userId, const std::string& text) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "INSERT INTO Messages (group_id, user_id, text) VALUES (?, ?, ?);", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, groupId);
        sqlite3_bind_int(stmt, 2, userId);
        sqlite3_bind_text(stmt, 3, text.c_str(), -1, SQLITE_STATIC);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}

int Database::createUser(const std::string& username, const std::string& email, const std::string& password) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;
    int newUserId = -1;

    if (sqlite3_prepare_v2(db, "INSERT INTO Users (username, email, password) VALUES (?, ?, ?);", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, username.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 2, email.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, password.c_str(), -1, SQLITE_STATIC);

        if (sqlite3_step(stmt) == SQLITE_DONE) {
            newUserId = sqlite3_last_insert_rowid(db);
        }
    }
    sqlite3_finalize(stmt);
    return newUserId; // Returns -1 if it fails
}

void Database::deleteUser(int userId) {
    std::lock_guard<std::mutex> lock(dbMutex);
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, "DELETE FROM Users WHERE id = ?;", -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, userId);
        sqlite3_step(stmt);
    }
    sqlite3_finalize(stmt);
}