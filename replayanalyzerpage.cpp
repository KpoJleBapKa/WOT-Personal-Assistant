#include "ReplayAnalyzerPage.h"
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

ReplayAnalyzerPage::ReplayAnalyzerPage(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *topLayout = new QHBoxLayout();

    m_selectFileButton = new QPushButton("Вибрати реплей...", this);
    m_selectFileButton->setCursor(Qt::PointingHandCursor);

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
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_progressBar);
    mainLayout->addWidget(m_replayList);
    mainLayout->addWidget(m_resultsTextEdit);

    m_replayParser = new ReplayParser(this);

    connect(m_selectFileButton, &QPushButton::clicked, this, &ReplayAnalyzerPage::onSelectFileButtonClicked);
    connect(m_replayList, &QListWidget::itemClicked, this, &ReplayAnalyzerPage::onReplayListItemClicked);
    connect(&m_watcher, &QFutureWatcher<QVariantMap>::finished, this, &ReplayAnalyzerPage::handleAnalysisFinished);

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

    QString playerName = data.contains("playerName") ? data.value("playerName").toString() : "Невідомо";
    QString mapName = data.contains("mapDisplayName") ? data.value("mapDisplayName").toString() : "Невідомо";
    QString vehicle = data.contains("playerVehicle") ? data.value("playerVehicle").toString() : "Невідомо";

    report += "<li><b>Гравець:</b> " + playerName + "</li>";
    report += "<li><b>Мапа:</b> " + mapName + "</li>";
    report += "<li><b>Техніка:</b> " + vehicle + "</li>";
    report += "</ul>";

    report += "<h2>Інші ключові дані</h2>";
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        QString key = it.key();
        if (key == "playerName" || key == "mapDisplayName" || key == "playerVehicle" || key == "filePath")
            continue;
        report += "<p><b>" + key + ":</b> " + it.value().toString() + "</p>";
    }

    displayResults(report);
}


void ReplayAnalyzerPage::displayResults(const QString& results) {
    m_resultsTextEdit->clear();
    m_resultsTextEdit->setHtml(results);
}
