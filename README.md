![Logo](client/resources/readme_logo.png)
![Showcase](client/resources/showcase.png)

# StudySync

StudySync is a tool built to help students manage their workload and collaborate with classmates. It brings tasks, communication, and focus tools into one fast desktop application.

> Note:  
> **Project Status:** StudySync is currently **under development** and is not yet fully implemented. you may experience bugs. The AI Tutor feature is currently a placeholder and has not been implemented yet.

> **Server Requirements:**  
> Currently, you must **run the server locally** at the same time as the client to use the application. A public server and easier hosting options are coming soon!

## Features
* **Dashboard:** View all your active tasks and pinned groups in one spot.
* **Study Groups:** Create groups and invite friends to work on projects together.
* **Live Chat:** Send messages to your group members in real time.
* **Task Tracking:** Create and assign tasks to specific group members.
* **Focus Timer:** Use the built-in timer to stay productive during study sessions.
* **AI Tutor:** *(In Development)* Chat with an AI assistant to get help with difficult subjects.

## Libraries & Frameworks Used
* **Qt**
* **Boost**
* **C++**
* **SQLite**

## Credits & Acknowledgements
* Initial SQLite database schema was generated using SQLAlchemy in python and then manually integrated into the C++ application.

## Building
Run this command in your terminal to build both the client and the server:
```bash
cmake -B build && cmake --build build
```