#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>

// Включаємо заголовочні файли для сторінок та менеджера бази даних
#include "ReplayAnalyzerPage.h"
#include "PlayerProfilePage.h"
#include "DatabaseManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Слот для обробки натискань у бічному меню
    void onMenuClicked(QListWidgetItem *item);
    void showAboutDialog();
    void showSettingsDialog();

private:
    // Метод для налаштування інтерфейсу
    void setupUI();
    void createMenus();

    // Елементи інтерфейсу
    QListWidget *m_sideMenu;
    QStackedWidget *m_stackedWidget;

    // Сторінки застосунку
    ReplayAnalyzerPage *m_replayPage;
    PlayerProfilePage *m_profilePage;

    // Менеджер бази даних
    DatabaseManager* m_dbManager;
};

#endif // MAINWINDOW_H
