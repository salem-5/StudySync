#pragma once
#include "AbstractChatPage.h"

class AiTutorPage : public AbstractChatPage {
    Q_OBJECT
public:
    AiTutorPage(QWidget* parent = nullptr);

protected:
    void onSendMessageRequested(const QString& text) override;
};