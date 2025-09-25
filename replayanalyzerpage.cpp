#include "replayanalyzerpage.h"
#include "recommendersystem.h"
#include "behavioranalyzer.h"
#include "metricscalculator.h"
#include "replayparser.h"
#include "databasemanager.h"
#include "VehicleData.h" // Підключаємо для доступу до бази даних

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include <QtConcurrent/QtConcurrent>
#include <QPushButton>
#include <QProgressBar>
#include <QListWidget>
#include <QTextEdit>
#include <QFutureWatcher>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QLabel>

ReplayAnalyzerPage::ReplayAnalyzerPage(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager)
{
    // Створення основного layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    // Верхня панель з кнопками
    QHBoxLayout *topLayout = new QHBoxLayout();
    m_selectFileButton = new QPushButton("Вибрати реплей...", this);
    m_selectFileButton->setCursor(Qt::PointingHandCursor);

    m_clearReplaysButton = new QPushButton("Очистити список", this);
    m_clearReplaysButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(m_selectFileButton);
    topLayout->addWidget(m_clearReplaysButton);
    topLayout->addStretch();

    // Індикатор прогресу
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setAlignment(Qt::AlignCenter);
    m_progressBar->setTextVisible(false);

    // Головний layout з двома панелями
    QHBoxLayout *contentLayout = new QHBoxLayout();

    // Ліва панель: список реплеїв
    QVBoxLayout *leftLayout = new QVBoxLayout();
    m_replayList = new QListWidget(this);
    leftLayout->addWidget(new QLabel("Список реплеїв:"));
    leftLayout->addWidget(m_replayList);

    QWidget *leftWidget = new QWidget;
    leftWidget->setLayout(leftLayout);
    leftWidget->setMaximumWidth(300);

    // Права панель: результати аналізу
    QVBoxLayout *rightLayout = new QVBoxLayout();
    m_resultsTextEdit = new QTextEdit(this);
    m_resultsTextEdit->setReadOnly(true);
    m_resultsTextEdit->setPlainText("Виберіть реплей зі списку або завантажте новий, щоб побачити результати аналізу.");
    rightLayout->addWidget(new QLabel("Результати аналізу:"));
    rightLayout->addWidget(m_resultsTextEdit);

    QWidget *rightWidget = new QWidget;
    rightWidget->setLayout(rightLayout);

    contentLayout->addWidget(leftWidget);
    contentLayout->addWidget(rightWidget);

    // Додаємо всі елементи в основний layout
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_progressBar);
    mainLayout->addLayout(contentLayout);

    // Ініціалізація аналітичних модулів
    m_replayParser = new ReplayParser(this);
    m_metricsCalculator = new MetricsCalculator(this);
    m_behaviorAnalyzer = new BehaviorAnalyzer(this);
    m_recommenderSystem = new RecommenderSystem(this);

    // З'єднання сигналів та слотів
    connect(m_selectFileButton, &QPushButton::clicked, this, &ReplayAnalyzerPage::onSelectFileButtonClicked);
    connect(m_replayList, &QListWidget::itemClicked, this, &ReplayAnalyzerPage::onReplayListItemClicked);
    connect(&m_watcher, &QFutureWatcher<QVariantMap>::finished, this, &ReplayAnalyzerPage::handleAnalysisFinished);
    connect(m_clearReplaysButton, &QPushButton::clicked, this, &ReplayAnalyzerPage::onClearReplaysButtonClicked);

    loadCachedReplays();
}

ReplayAnalyzerPage::~ReplayAnalyzerPage() {}

void ReplayAnalyzerPage::onSelectFileButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Вибрати реплей", "", "World of Tanks Replays (*.wotreplay)");
    if (!filePath.isEmpty()) {
        QFileInfo fileInfo(filePath);
        for (int i = 0; i < m_replayList->count(); ++i) {
            if (m_replayList->item(i)->data(Qt::UserRole).toString() == filePath) {
                QMessageBox::information(this, "Файл вже додано", "Цей реплей вже є у списку.");
                return;
            }
        }
        QListWidgetItem *item = new QListWidgetItem(fileInfo.fileName());
        item->setData(Qt::UserRole, filePath);
        m_replayList->addItem(item);
        m_replayList->setCurrentItem(item);
        analyzeReplay(filePath);
    }
}

void ReplayAnalyzerPage::onReplayListItemClicked(QListWidgetItem *item) {
    if (item && !item->data(Qt::UserRole).toString().isEmpty()) {
        analyzeReplay(item->data(Qt::UserRole).toString());
    }
}

void ReplayAnalyzerPage::loadCachedReplays() {
    m_replayList->clear();
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
    m_progressBar->setTextVisible(true);
    m_progressBar->setFormat("Аналіз файлу...");
    m_selectFileButton->setEnabled(false);
    m_replayList->setEnabled(false);
    m_resultsTextEdit->setPlainText("Початок аналізу файлу: " + QFileInfo(filePath).fileName());

    QFuture<QVariantMap> future = QtConcurrent::run([this, filePath]() {
        return m_replayParser->parse(filePath);
    });
    m_watcher.setFuture(future);
}

void ReplayAnalyzerPage::handleAnalysisFinished() {
    m_progressBar->setValue(100);
    m_progressBar->setFormat("Аналіз завершено");
    m_selectFileButton->setEnabled(true);
    m_replayList->setEnabled(true);

    QVariantMap analysisResults = m_watcher.result();

    if (analysisResults.isEmpty() || !analysisResults.contains("personal")) {
        QMessageBox::critical(this, "Помилка", "Не вдалося розібрати файл реплею. Можливо, файл пошкоджено або має непідтримуваний формат.");
        m_resultsTextEdit->setPlainText("Помилка при аналізі файлу.");
        for (int i = 0; i < m_replayList->count(); ++i) {
            if (m_replayList->item(i) && m_watcher.future().result().value("filePath").toString() == m_replayList->item(i)->data(Qt::UserRole).toString()) {
                delete m_replayList->takeItem(i);
                break;
            }
        }
        return;
    }

    QString filePath = analysisResults.value("filePath").toString();
    m_dbManager->addReplayData(filePath, analysisResults);
    displayStructuredResults(analysisResults);
}

void ReplayAnalyzerPage::displayStructuredResults(const QVariantMap &data)
{
    QString report = "<h1>Звіт про аналіз реплею</h1>";

    // Спочатку розраховуємо всі метрики, щоб отримати доступ до чистої назви танка
    QVariantMap metrics = m_metricsCalculator->calculate(data);
    QVariantMap behavior = m_behaviorAnalyzer->analyze(data, metrics);
    // ❗️ ВИПРАВЛЕНО: Додано третій аргумент 'data'
    QStringList recommendations = m_recommenderSystem->generate(data, metrics, behavior);


    // --- Базова інформація ---
    report += "<h2>Базова інформація</h2>";
    report += "<ul>";

    QString playerName = data.value("playerName").toString();
    QString mapName = data.value("mapDisplayName").toString();
    QString cleanName = metrics.value("cleanVehicleName").toString(); // Беремо назву з метрик

    report += "<li><b>Гравець:</b> " + (playerName.isEmpty() ? "Невідомо" : playerName) + "</li>";
    report += "<li><b>Мапа:</b> " + (mapName.isEmpty() ? "Невідомо" : mapName) + "</li>";
    report += "<li><b>Техніка:</b> " + (cleanName.isEmpty() ? data.value("playerVehicle").toString() : cleanName) + "</li>";

    int playerTeam = 0;
    if (data.contains("vehicles")) {
        QVariantMap vehiclesData = data.value("vehicles").toMap();
        for (auto it = vehiclesData.constBegin(); it != vehiclesData.constEnd(); ++it) {
            QVariantMap vehicleInfo = it.value().toMap();
            if (vehicleInfo.value("name").toString() == playerName) {
                playerTeam = vehicleInfo.value("team").toInt();
                break;
            }
        }
    }

    if (data.contains("winnerTeam") && playerTeam != 0) {
        int winnerTeam = data.value("winnerTeam").toInt();
        QString outcome = "Нічия";
        if (winnerTeam == playerTeam) {
            outcome = "<span style='color: #88ff88;'>Перемога</span>";
        } else if (winnerTeam != 0) {
            outcome = "<span style='color: #ff8888;'>Поразка</span>";
        }
        report += "<li><b>Результат бою:</b> " + outcome + "</li>";
    } else {
        report += "<li><b>Інформація про результат бою відсутня.</b></li>";
    }
    report += "</ul>";

    // --- Результати гравця ---
    report += "<h2>Результати гравця: " + (playerName.isEmpty() ? "Невідомо" : playerName) + "</h2>";
    report += "<ul>";

    double totalDamage = metrics.value("totalDamageDealt", 0.0).toDouble();
    double shots = metrics.value("shots", 0.0).toDouble();
    double hits = metrics.value("hits", 0.0).toDouble();
    double piercings = metrics.value("piercings", 0.0).toDouble();
    report += QString("<li><b>Нанесено шкоди:</b> %1</li>").arg(qRound(totalDamage));
    report += QString("<li><b>Допомога команді:</b> %1</li>").arg(qRound(metrics.value("damageAssisted", 0.0).toDouble()));
    report += QString("<li><b>Заблоковано шкоди:</b> %1</li>").arg(qRound(metrics.value("damageBlockedByArmor", 0.0).toDouble()));
    report += QString("<li><b>Знищено:</b> %1</li>").arg(qRound(metrics.value("kills", 0.0).toDouble()));
    report += QString("<li><b>Виявлено ворогів:</b> %1</li>").arg(qRound(metrics.value("spotted", 0.0).toDouble()));
    report += QString("<li><b>Постріли / Влучання / Пробиття:</b> %1 / %2 / %3</li>").arg(qRound(shots)).arg(qRound(hits)).arg(qRound(piercings));
    report += QString("<li><b>Досвід:</b> %1</li>").arg(qRound(metrics.value("xp", 0.0).toDouble()));
    report += QString("<li><b>Кредити:</b> %1</li>").arg(qRound(metrics.value("credits", 0.0).toDouble()));
    report += "</ul>";

    // --- Ключові метрики ефективності ---
    report += "<h2>Ключові метрики ефективності</h2>";
    if (!metrics.isEmpty()) {
        report += "<ul>";
        report += "<li><b>Точність стрільби:</b> " + metrics.value("accuracy").toString() + "</li>";
        report += "<li><b>Ефективність пробиття:</b> " + metrics.value("penetrationRatio").toString() + "</li>";
        report += QString("<li><b>Середня шкода за пробиття:</b> %1</li>").arg(metrics.value("avgDmgPerPen").toInt());
        report += QString("<li><b>Сумарний внесок (шкода + асист + блок):</b> %1</li>").arg(metrics.value("combinedContribution").toInt());
        report += "</ul>";
    } else {
        report += "<p>Недостатньо даних для розрахунку.</p>";
    }

    // --- Аналіз ігрової поведінки ---
    report += "<h2>Аналіз ігрової поведінки</h2>";
    QString performanceGrade = behavior.value("performanceGrade").toString();

    if (performanceGrade.contains("помилка даних")) {
        report += "<p style='color: #ffcc00;'><b>Не вдалося провести аналіз:</b> танк не знайдено у внутрішній базі даних. Основні показники бою відображено, але оцінка ефективності неможлива.</p>";
    } else if (!behavior.isEmpty()) {
        report += "<ul>";
        report += "<li><b>Оцінка ефективності:</b> " + performanceGrade + "</li>";
        report += "<li><b>Виконання ролі:</b> " + behavior.value("rolePerformance").toString() + "</li>";
        QString keySkill = behavior.value("keySkill").toString();
        if (keySkill != "Не визначено") {
            report += "<li><b>Ключовий навик у бою:</b> " + keySkill + "</li>";
        }
        report += "</ul>";
    } else {
        report += "<p>Недостатньо даних для аналізу.</p>";
    }

    // --- Персональні рекомендації ---
    report += "<h2>Персональні рекомендації</h2>";
    if (!recommendations.isEmpty() && !performanceGrade.contains("помилка даних")) {
        report += "<ul>";
        for (const QString &rec : recommendations) {
            report += "<li>" + rec + "</li>";
        }
        report += "</ul>";
    } else if (performanceGrade.contains("помилка даних")) {
        report += "<p>Рекомендації не можуть бути сформовані, оскільки танк не знайдено у базі даних.</p>";
    }
    else {
        report += "<p>Рекомендації не сформовано.</p>";
    }

    m_resultsTextEdit->clear();
    m_resultsTextEdit->setHtml(report);
}

void ReplayAnalyzerPage::onClearReplaysButtonClicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Очищення списку", "Ви впевнені, що хочете очистити весь список реплеїв? Дані аналізу буде видалено.",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        if (m_dbManager) {
            m_dbManager->clearAllData();
        }
        m_replayList->clear();
        m_resultsTextEdit->clear();
        m_resultsTextEdit->setPlainText("Виберіть реплей зі списку або завантажте новий, щоб побачити результати аналізу.");
        QMessageBox::information(this, "Готово", "Список реплеїв та кеш аналізу очищено.");
    }
}
