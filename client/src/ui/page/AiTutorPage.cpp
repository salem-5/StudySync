#include "ui/page/AiTutorPage.h"
#include <QVBoxLayout>
#include <QMenu>
#include "LanguageManager.h"
#include "ui/ClientState.h"

AiTutorPage::AiTutorPage(QWidget* parent) : AbstractChatPage(parent) {
    refreshMessages();
    connect(ClientNotifier::instance(), &ClientNotifier::aiResponseReceived, this, [this](bool success, QString message) {
        if (success) {
            finishLoadingMessage(message, false);
        } else {
            finishLoadingMessage(message, true);
        }
    });
    connect(ClientNotifier::instance(), &ClientNotifier::userChanged, this, [this]() { refreshMessages(); });
}

void AiTutorPage::refreshMessages() {
    clearChat();
    addMessage(LanguageManager::tr("nav.ai_tutor"), LanguageManager::tr("ai.greeting"), true);

    if (ClientState::getAiCredits() == 0) {
        setInputEnabled(false);
        addMessage(LanguageManager::tr("nav.ai_tutor"), "You are out of AI credits.", true);
    } else {
        setInputEnabled(true);
    }

    for (const auto& msg : ClientState::getAiMessages()) {
        QMap<int, QString> attachmentsMap;
        for (int id : msg.getAttachments()) {
            const Task* t = ClientState::getTaskById(id);
            if (t) {
                attachmentsMap[id] = QString::fromStdString(t->getTitle());
            }
        }
        if (msg.getRole() == "user") {
            addMessage(LanguageManager::tr("user.me"), QString::fromStdString(msg.getText()), false, attachmentsMap);
        } else {
            addMessage(LanguageManager::tr("nav.ai_tutor"), QString::fromStdString(msg.getText()), true, attachmentsMap);
        }
    }
}

void AiTutorPage::attachTask(int taskId) {
    const auto& tasks = ClientState::getTasks();
    for (const Task& task : tasks) {
        if (task.getId() == taskId) {
            addAttachmentChip(taskId, QString::fromStdString(task.getTitle()));
            break;
        }
    }
}

void AiTutorPage::onAddAttachmentClicked() {
    QMenu menu(this);
    menu.setObjectName("aiTutorMenu");

    const auto& tasks = ClientState::getTasks();
    for (const Task& task : tasks) {
        if (!attachedTasks.contains(task.getId())) {
            QAction* action = menu.addAction(QString::fromStdString("+  " + task.getTitle()));
            connect(action, &QAction::triggered, this, [this, task]() {
                addAttachmentChip(task.getId(), QString::fromStdString(task.getTitle()));
            });
        }
    }

    if (menu.isEmpty()) {
        menu.addAction(LanguageManager::tr("ai.no_tasks"))->setEnabled(false);
    }

    QPoint pos = btnAddAttachment->mapToGlobal(QPoint(0, -menu.sizeHint().height() - 5));
    menu.exec(pos);
}



void AiTutorPage::onSendMessageRequested(const QString& text) {
    if (ClientState::getAiCredits() == 0) {
        finishLoadingMessage("Out of AI credits", true);
        return;
    }

    addMessage(LanguageManager::tr("user.me"), text, false, attachedTasks);

    std::vector<int> taskIds;
    for (auto it = attachedTasks.begin(); it != attachedTasks.end(); ++it) {
        taskIds.push_back(it.key());
    }

    clearAttachments();
    addLoadingMessage(LanguageManager::tr("nav.ai_tutor"));

    ClientState::askAi(text.toStdString(), taskIds);
}

void AiTutorPage::onCancelGeneration() {
    ClientState::cancelAi();
}