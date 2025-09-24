#include "MainWindow.h"
#include "ReplayAnalyzerPage.h"
#include "PlayerProfilePage.h"
#include "DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QDebug>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QIcon>
#include <QLabel>
#include <QDialog>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("WoT Replay Analyzer");
    resize(1200, 800);

    // Встановлюємо загальну темну тему для всього застосунку
    setStyleSheet(
        "QMainWindow { background-color: #2e2e2e; }"
        "QWidget { background-color: #2e2e2e; color: #e0e0e0; font-family: 'Segoe UI', 'Helvetica', sans-serif; outline: none; }"
        "QMenuBar { background-color: #1a1a1a; color: #ffffff; border-bottom: 2px solid #5577aa; }"
        "QMenuBar::item { padding: 5px 10px; background-color: transparent; }"
        "QMenuBar::item:selected { background-color: #5a5a5a; }"
        "QMenu { background-color: #1a1a1a; border: 1px solid #4a4a4a; }"
        "QMenu::item { padding: 5px 20px; color: #e0e0e0; }"
        "QMenu::item:selected { background-color: #5a5a5a; }"
        "QListWidget { background-color: #3a3a3a; border: none; outline: none; }"
        "QListWidget::item { padding: 10px 15px; border-bottom: 1px solid #4a4a4a; outline: none; }"
        "QListWidget::item:selected { background-color: #5a5a5a; color: #ffffff; }"
        "QListWidget::item:hover { background-color: #4c4c4c; }"
        "QPushButton { background-color: #5577aa; border: 1px solid #45679a; border-radius: 5px; padding: 8px 15px; color: #ffffff; outline: none; }"
        "QPushButton:hover { background-color: #6688bb; }"
        "QPushButton:pressed { background-color: #4a6a9a; }"
        );

    // Встановлюємо іконку вікна
    setWindowIcon(QIcon("icon.ico"));

    // Ініціалізуємо менеджер бази даних
    m_dbManager = new DatabaseManager(this);

    createMenus();
    setupUI();
}

MainWindow::~MainWindow() {}

void MainWindow::createMenus() {
    QMenuBar* menuBar = new QMenuBar(this);
    menuBar->setCursor(Qt::PointingHandCursor);

    // Додаємо іконку застосунку в меню
    QLabel* iconLabel = new QLabel;
    iconLabel->setPixmap(QIcon("icon.ico").pixmap(24, 24));
    iconLabel->setContentsMargins(5, 0, 5, 0);
    menuBar->setCornerWidget(iconLabel, Qt::TopLeftCorner);

    QMenu *fileMenu = menuBar->addMenu("Файл");
    QAction *exitAction = fileMenu->addAction("Вихід");
    connect(exitAction, &QAction::triggered, this, &MainWindow::close);

    QMenu *settingsMenu = menuBar->addMenu("Налаштування");
    QAction *optionsAction = settingsMenu->addAction("Параметри");
    connect(optionsAction, &QAction::triggered, this, &MainWindow::showSettingsDialog);

    QMenu *helpMenu = menuBar->addMenu("Довідка");
    QAction *aboutAction = helpMenu->addAction("Про програму");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showAboutDialog);

    setMenuBar(menuBar);
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0); // Без відступів
    mainLayout->setSpacing(0);

    // Горизонтальний layout для бічного меню та центральної області
    QHBoxLayout *contentLayout = new QHBoxLayout();
    contentLayout->setContentsMargins(10, 10, 10, 10);
    contentLayout->setSpacing(10);

    // Бічне меню (ліва панель)
    m_sideMenu = new QListWidget(this);
    m_sideMenu->setMaximumWidth(200);
    m_sideMenu->setFrameShape(QFrame::NoFrame);
    m_sideMenu->addItem("Аналіз реплеїв");
    m_sideMenu->addItem("Профіль гравця");

    // Встановлюємо курсор для елементів бічного меню
    m_sideMenu->setCursor(Qt::PointingHandCursor);

    contentLayout->addWidget(m_sideMenu);

    // Центральна область (праворуч)
    m_stackedWidget = new QStackedWidget(this);
    m_stackedWidget->setStyleSheet("background-color: #3a3a3a; border-radius: 5px;");

    // Створення сторінок
    m_replayPage = new ReplayAnalyzerPage(m_dbManager, this);
    m_profilePage = new PlayerProfilePage(this);

    m_stackedWidget->addWidget(m_replayPage);
    m_stackedWidget->addWidget(m_profilePage);

    contentLayout->addWidget(m_stackedWidget);

    mainLayout->addLayout(contentLayout);

    setCentralWidget(centralWidget);

    // З'єднання сигналів
    connect(m_sideMenu, &QListWidget::itemClicked, this, &MainWindow::onMenuClicked);

    // Встановлюємо початкову сторінку
    m_stackedWidget->setCurrentIndex(0);
}

void MainWindow::onMenuClicked(QListWidgetItem *item) {
    if (item->text() == "Аналіз реплеїв") {
        m_stackedWidget->setCurrentIndex(0);
    } else if (item->text() == "Профіль гравця") {
        m_stackedWidget->setCurrentIndex(1);
    }
}

void MainWindow::showAboutDialog() {
    QMessageBox::about(this, "Про застосунок",
                       "<h2>Аналізатор реплеїв WoT</h2>"
                       "<p>Розроблено для аналізу ігрової поведінки та надання персоналізованих рекомендацій гравцям.</p>"
                       "<p>Версія: 1.0</p>");
}

void MainWindow::showSettingsDialog() {
    QDialog settingsDialog(this);
    settingsDialog.setWindowTitle("Налаштування");
    settingsDialog.setFixedSize(400, 200);
    settingsDialog.setStyleSheet(
        "QDialog { background-color: #2e2e2e; color: #e0e0e0; }"
        "QPushButton { background-color: #e04455; border: 1px solid #c03344; border-radius: 5px; padding: 8px 15px; color: #ffffff; outline: none; }"
        "QPushButton:hover { background-color: #f05566; }"
        "QPushButton:pressed { background-color: #d03344; }"
        );

    QVBoxLayout *layout = new QVBoxLayout(&settingsDialog);
    QLabel *label = new QLabel("Тут ви можете керувати налаштуваннями програми.", &settingsDialog);

    QPushButton *resetButton = new QPushButton("Скинути всі дані", &settingsDialog);
    resetButton->setCursor(Qt::PointingHandCursor);

    layout->addWidget(label);
    layout->addWidget(resetButton);

    connect(resetButton, &QPushButton::clicked, [&]() {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Скидання даних", "Ви впевнені, що хочете скинути всі дані аналізу?",
                                      QMessageBox::Yes | QMessageBox::No);
        if (reply == QMessageBox::Yes) {
            // Тут буде логіка для очищення бази даних
            if (m_dbManager->clearAllData()) {
                QMessageBox::information(this, "Готово", "Всі дані успішно скинуто.");
                settingsDialog.close();
            }
        }
    });

    settingsDialog.exec();
}
