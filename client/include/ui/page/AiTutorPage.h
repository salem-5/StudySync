#pragma once
#include "AbstractChatPage.h"
#include <QTimer>

class AiTutorPage : public AbstractChatPage {
    Q_OBJECT
public:
    AiTutorPage(QWidget* parent = nullptr);
    void refreshMessages();
    void attachTask(int taskId);

protected:
    void onSendMessageRequested(const QString& text) override;
    void onAddAttachmentClicked() override;
    void onCancelGeneration() override;
};