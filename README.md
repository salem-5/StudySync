![Logo](client/resources/readme_logo.png)
![Showcase](client/resources/showcase.png)

# StudySync
[Visit Our Website!](http://studysync.site/)

StudySync is a tool built to help students manage their workload and collaborate with classmates. It brings tasks, communication, and focus tools into one fast desktop application.

> Note:  
> **Project Status:** StudySync is currently **under development** and is not yet fully implemented. You may experience bugs.

> **Server Requirements:** You can now connect directly to our **official public server** for an instant setup, or continue to run the server locally if preferred.

## Features
* **Dashboard:** View all your active tasks and pinned groups in one spot.
* **Study Groups:** Create groups and invite friends to work on projects together.
* **Live Chat:** Send messages to your group members in real time.
* **Task Tracking:** Create and assign tasks to specific group members.
* **Focus Timer:** Use the built-in timer to stay productive during study sessions.
* **AI Tutor:** Chat with an AI assistant to get help with difficult subjects, workload analysis, and task tracking.
* **Multilingual Support:** Study in your preferred language. Fully translated into Arabic, English, and French natively, with more languages on the way.
* **Public Server:** Skip the backend setup entirely. Connect straight to our official public server for an instant, hassle-free study environment.

## Libraries & Frameworks Used
* **Qt**
* **Boost**
* **C++**
* **SQLite**
* **Python (AI Backend)**

## Project Board
Track progress and planned features here:  
https://trello.com/b/FurIPhe7/study-sync

## Credits & Acknowledgements
* **Styling:** CSS styling used is from the [Prism Launcher Fluent-Dark theme](https://github.com/PrismLauncher/Themes/blob/main/themes/Fluent-Dark/themeStyle.css).
* **Database:** Initial SQLite database schema was generated using SQLAlchemy in Python and then manually integrated into the C++ application.
---

## Building and Running Locally

If you prefer to host the server yourself and run your own AI Tutor backend, follow these steps.

### 1. Build the Application
Run this command in your terminal to build both the client and the server C++ applications:
```bash
cmake -B build && cmake --build build
```

### 2. Set Up the AI Backend
The AI Tutor runs on a separate Python socket server that communicates with the main C++ server. It utilizes the Gemini API.

**Prerequisites:**
You need Python installed along with the required libraries:
```bash
pip install python-dotenv google-generativeai
```

**Configuration:**
Create a `.env` file in the same directory as your `ai_backend.py` script. This is where you configure the server ports and provide your own AI model's API key:

```ini
# Your personal Gemini API Key
GEMINI_API_KEY="your_api_key_here"

# Socket Server Configuration
HOST="127.0.0.1"
PORT=2570
```
*Note: You can get a free Gemini API key from Google AI Studio.*

### 3. Run the Servers
To get everything working, you must run both the Python AI backend and the C++ server.

**Start the AI Backend:**
```bash
python ai_backend.py
```
*(You should see a message saying "AI Backend listening on 127.0.0.1:2570")*

**Start the Main C++ Server:**
Navigate to your build directory and run the server executable:
```bash
./build/server # (or equivalent executable name depending on your OS)
```
*(Ensure the C++ server is configured to match the `PORT` set in your Python `.env` file if you changed it from the default)*

Once both servers are running, launch your StudySync Client application and connect!