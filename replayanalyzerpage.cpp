#include "replayanalyzerpage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QProgressBar>
#include <QPushButton>
#include <QListWidget>
#include <QTextEdit>

ReplayAnalyzerPage::ReplayAnalyzerPage(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *topLayout = new QHBoxLayout();

    m_selectFileButton = new QPushButton("Вибрати реплей...", this);
    m_selectFileButton->setCursor(Qt::PointingHandCursor);

    // Додано нову кнопку для очищення реплеїв
    m_clearReplaysButton = new QPushButton("Очистити реплеї", this);
    m_clearReplaysButton->setCursor(Qt::PointingHandCursor);

    m_replayList = new QListWidget(this);
    m_replayList->addItem("Завантажені реплеї:");

    m_resultsTextEdit = new QTextEdit(this);
    m_resultsTextEdit->setReadOnly(true);
    m_resultsTextEdit->setPlainText("Результати аналізу будуть відображатися тут.");

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setAlignment(Qt::AlignCenter);

    topLayout->addWidget(m_selectFileButton);
    topLayout->addWidget(m_clearReplaysButton); // Розміщення нової кнопки в макеті
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_progressBar);
    mainLayout->addWidget(m_replayList);
    mainLayout->addWidget(m_resultsTextEdit);

    m_replayParser = new ReplayParser(this);

    connect(m_selectFileButton, &QPushButton::clicked, this, &ReplayAnalyzerPage::onSelectFileButtonClicked);
    connect(m_replayList, &QListWidget::itemClicked, this, &ReplayAnalyzerPage::onReplayListItemClicked);
    connect(&m_watcher, &QFutureWatcher<QVariantMap>::finished, this, &ReplayAnalyzerPage::handleAnalysisFinished);

    // Підключення нової кнопки до методу очищення
    connect(m_clearReplaysButton, &QPushButton::clicked, this, &ReplayAnalyzerPage::onClearReplaysButtonClicked);

    loadCachedReplays();
}

ReplayAnalyzerPage::~ReplayAnalyzerPage() {}

void ReplayAnalyzerPage::onSelectFileButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Вибрати реплей", "", "World of Tanks Replays (*.wotreplay)");
    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        QListWidgetItem *item = new QListWidgetItem(fileInfo.fileName());
        item->setData(Qt::UserRole, filePath);
        m_replayList->addItem(item);
        analyzeReplay(filePath);
    }
}

void ReplayAnalyzerPage::onReplayListItemClicked(QListWidgetItem *item) {
    if (!item->data(Qt::UserRole).toString().isEmpty()) {
        analyzeReplay(item->data(Qt::UserRole).toString());
    }
}

void ReplayAnalyzerPage::loadCachedReplays() {
    QVariantList cachedReplays = m_dbManager->getReplays();
    for (const QVariant& replayData : cachedReplays) {
        QVariantMap replayMap = replayData.toMap();
        QString filePath = replayMap.value("filePath").toString();
        QFileInfo fileInfo(filePath);
        if (fileInfo.exists()) {
            QListWidgetItem* item = new QListWidgetItem(fileInfo.fileName());
            item->setData(Qt::UserRole, filePath);
            m_replayList->addItem(item);
        }
    }
}

void ReplayAnalyzerPage::analyzeReplay(const QString &filePath) {
    QVariantMap cachedData = m_dbManager->getReplayData(filePath);
    if (!cachedData.isEmpty()) {
        displayStructuredResults(cachedData);
        return;
    }

    m_progressBar->setValue(0);
    m_resultsTextEdit->setPlainText("Початок аналізу файлу: " + filePath + "\n\n");

    QFuture<QVariantMap> future = QtConcurrent::run([this, filePath]() {
        return m_replayParser->parse(filePath);
    });
    m_watcher.setFuture(future);
}

void ReplayAnalyzerPage::handleAnalysisFinished() {
    m_progressBar->setValue(100);
    QVariantMap analysisResults = m_watcher.result();

    if (analysisResults.isEmpty()) {
        QMessageBox::critical(this, "Помилка", "Не вдалося розібрати файл реплею.");
        m_resultsTextEdit->setPlainText("Помилка при аналізі файлу.");
        return;
    }

    QString filePath = analysisResults.value("filePath").toString();
    m_dbManager->addReplayData(filePath, analysisResults);

    displayStructuredResults(analysisResults);
}

// 🔹 Новий метод для структурованого виводу
void ReplayAnalyzerPage::displayStructuredResults(const QVariantMap &data) {
    QString report = "<h1>Звіт про аналіз реплею</h1>";
    report += "<h2>Базова інформація</h2>";
    report += "<ul>";

    QString playerName = data.value("playerName").toString();
    QString mapName = data.value("mapDisplayName").toString();
    QString vehicle = data.value("playerVehicle").toString();

    report += "<li><b>Гравець:</b> " + (playerName.isEmpty() ? "Невідомо" : playerName) + "</li>";
    report += "<li><b>Мапа:</b> " + (mapName.isEmpty() ? "Невідомо" : mapName) + "</li>";
    report += "<li><b>Техніка:</b> " + (vehicle.isEmpty() ? "Невідомо" : vehicle) + "</li>";

    report += "<li><b>Клієнт XML:</b> " + data.value("clientVersionFromXml").toString() + "</li>";
    report += "<li><b>Клієнт EXE:</b> " + data.value("clientVersionFromExe").toString() + "</li>";
    report += "<li><b>Сервер:</b> " + data.value("serverName").toString() + "</li>";
    report += "</ul>";

    report += "<h2>Ключові показники</h2>";
    report += "<ul>";

    // Перевірка наявності ключа для бойових результатів
    if (data.contains("battleResults")) {
        QVariantMap battleResults = data.value("battleResults").toMap();
        QString personal = battleResults.value("personal").toString();

        report += "<li><b>Результат бою:</b> " + battleResults.value("winnerTeam", "Невідомо").toString() + "</li>";
        report += "<li><b>Особиста ефективність:</b> " + personal + "</li>";
    } else {
        report += "<li><b>Інформація про результати бою відсутня.</b></li>";
    }

    report += "</ul>";

    // Додаємо більше деталей, якщо вони існують
    if (data.contains("damageAssistedRadio") || data.contains("damageDealt")) {
        report += "<h2>Бойові дані</h2>";
        report += "<ul>";
        report += "<li><b>Завдана шкода:</b> " + data.value("damageDealt", 0).toString() + "</li>";
        report += "<li><b>Шкода за розвідку:</b> " + data.value("damageAssistedRadio", 0).toString() + "</li>";
        report += "</ul>";
    }

    // 🔹 Додано новий розділ для детальних результатів гравця
    report += "<h2>Результати гравця</h2>";
    report += "<ul>";

    QString playerAccountId;
    if (data.contains("vehicles")) {
        QVariantMap vehiclesData = data.value("vehicles").toMap();
        QString myPlayerName = data.value("playerName").toString();

        // Пошук accountDBID (ключа в мапі vehicles) за ім'ям гравця
        for (auto it = vehiclesData.constBegin(); it != vehiclesData.constEnd(); ++it) {
            QVariantMap vehicleInfo = it.value().toMap();
            if (vehicleInfo.value("name").toString() == myPlayerName) {
                playerAccountId = it.key();
                break;
            }
        }
    }


    if (!playerAccountId.isEmpty() && data.contains("battleResults")) {
        QVariantMap battleResults = data.value("battleResults").toMap();
        if (battleResults.contains(playerAccountId)) {
            QVariantMap playerStats = battleResults.value(playerAccountId).toMap();

            report += "<li><b>Нанесено шкоди:</b> " + playerStats.value("damageDealt", 0).toString() + "</li>";
            report += "<li><b>Шкода за розвідку:</b> " + playerStats.value("damageAssistedRadio", 0).toString() + "</li>";
            report += "<li><b>Заблоковано шкоди:</b> " + playerStats.value("damageBlockedByArmor", 0).toString() + "</li>";
            report += "<li><b>Знищено:</b> " + playerStats.value("frags", 0).toString() + "</li>";
            report += "<li><b>Постріли:</b> " + playerStats.value("shots", 0).toString() + "</li>";
            report += "<li><b>Влучання:</b> " + playerStats.value("hits", 0).toString() + "</li>";
            report += "<li><b>Пробиття:</b> " + playerStats.value("piercings", 0).toString() + "</li>";
            report += "<li><b>Отримано досвіду:</b> " + playerStats.value("xp", 0).toString() + "</li>";
            report += "<li><b>Отримано кредитів:</b> " + playerStats.value("credits", 0).toString() + "</li>";

        } else {
            report += "<li>Не вдалося знайти результати для гравця з ID " + playerAccountId + ".</li>";
        }
    } else {
        report += "<li>Дані про результати гравця не знайдено.</li>";
    }
    report += "</ul>";

    m_resultsTextEdit->clear();
    m_resultsTextEdit->setHtml(report);
}

void ReplayAnalyzerPage::displayResults(const QString& results) {
    m_resultsTextEdit->clear();
    m_resultsTextEdit->setHtml(results);
}

// 🔹 Додано новий метод для обробки натискання кнопки "Очистити реплеї"
void ReplayAnalyzerPage::onClearReplaysButtonClicked() {
    // Очищення даних з бази даних.
    // Увага: Цей метод m_dbManager->clearReplays() потрібно реалізувати
    // у класі DatabaseManager для видалення даних з бази.
    if (m_dbManager) {
        m_dbManager->clearAllData();
    }

    // Очищення UI
    m_replayList->clear();
    m_replayList->addItem("Завантажені реплеї:"); // Додаємо назад заголовок
    m_resultsTextEdit->clear();
    m_resultsTextEdit->setPlainText("Результати аналізу будуть відображатися тут.");

    QMessageBox::information(this, "Готово", "Всі збережені реплеї видалено.");
}
