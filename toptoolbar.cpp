#include "TopToolbar.h"
#include <QHBoxLayout>
#include <QIcon>
#include <QAction>
#include <QLabel>

TopToolbar::TopToolbar(QWidget *parent) : QWidget(parent) {
    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(15, 5, 15, 5);
    layout->setSpacing(10);

    // Встановлюємо стиль для панелі
    setStyleSheet("background-color: #1a1a1a; border-bottom: 2px solid #5577aa;");

    // Заголовок
    QLabel *titleLabel = new QLabel("WoT Replay Analyzer");
    titleLabel->setStyleSheet("color: #ffffff; font-size: 18px; font-weight: bold;");

    layout->addWidget(titleLabel);
    layout->addStretch(); // Щоб кнопки були праворуч

    // Кнопка "Налаштування"
    m_settingsButton = new QPushButton(QIcon(":/icons/icon.png"), "", this);
    m_settingsButton->setToolTip("Налаштування");
    m_settingsButton->setFixedSize(32, 32);
    m_settingsButton->setIconSize(QSize(24, 24));
    m_settingsButton->setStyleSheet(
        "QPushButton { background-color: transparent; border: none; }"
        "QPushButton:hover { background-color: #3a3a3a; border-radius: 5px; }"
        );

    // Кнопка "Про застосунок"
    m_aboutButton = new QPushButton(QIcon(":/icons/info.png"), "", this);
    m_aboutButton->setToolTip("Про застосунок");
    m_aboutButton->setFixedSize(32, 32);
    m_aboutButton->setIconSize(QSize(24, 24));
    m_aboutButton->setStyleSheet(
        "QPushButton { background-color: transparent; border: none; }"
        "QPushButton:hover { background-color: #3a3a3a; border-radius: 5px; }"
        );

    layout->addWidget(m_settingsButton);
    layout->addWidget(m_aboutButton);

    // З'єднання сигналів
    connect(m_settingsButton, &QPushButton::clicked, this, &TopToolbar::settingsClicked);
    connect(m_aboutButton, &QPushButton::clicked, this, &TopToolbar::aboutClicked);
}
