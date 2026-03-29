#include "ui/page/AiTutorPage.h"
#include <QTimer>
#include <QVBoxLayout>

AiTutorPage::AiTutorPage(QWidget* parent) : AbstractChatPage(parent) {
    addMessage("AI Tutor", "Hello! I'm your AI Study Tutor. What subject are we tackling today?", true);
}

void AiTutorPage::onSendMessageRequested(const QString& text) {
    //implement properly later
    addMessage("Me", text, false);
    QTimer::singleShot(1000, this, [this]() {
        addMessage("AI Tutor", "That's an interesting topic! Let's dive into the details.", true);
    });
}