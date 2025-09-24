#include "PlayerProfilePage.h"
#include <QVBoxLayout>
#include <QFont>

PlayerProfilePage::PlayerProfilePage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *layout = new QVBoxLayout(this);

    m_titleLabel = new QLabel("Профіль гравця", this);
    m_titleLabel->setFont(QFont("Arial", 24, QFont::Bold));
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("color: #ffffff;"); // Специфічний стиль для заголовка

    m_statsDisplay = new QTextEdit(this);
    m_statsDisplay->setReadOnly(true);
    m_statsDisplay->setPlainText("Тут будуть відображатися сукупні дані та графіки для аналізу вашого прогресу.");
    m_statsDisplay->setStyleSheet("background-color: #2a2a2a; border: 1px solid #4a4a4a; padding: 10px;");

    layout->addWidget(m_titleLabel);
    layout->addWidget(m_statsDisplay);
}
