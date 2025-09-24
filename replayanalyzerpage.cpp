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
#include <random>
#include <QtConcurrent/QtConcurrent>
#include <QFutureWatcher>

// Генератор випадкових чисел для імітації ігрових метрик
std::mt19937 rng(std::random_device{}());

ReplayAnalyzerPage::ReplayAnalyzerPage(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *topLayout = new QHBoxLayout();

    m_selectFileButton = new QPushButton("Вибрати реплей...", this);
    m_selectFileButton->setCursor(Qt::PointingHandCursor);

    m_replayList = new QListWidget(this);
    m_replayList->addItem("Завантажені реплеї:");
    m_resultsTextEdit = new QTextEdit(this);
    m_resultsTextEdit->setReadOnly(true);
    m_resultsTextEdit->setPlainText("Результати аналізу будуть відображатися тут.");
    m_resultsTextEdit->setStyleSheet("background-color: #2a2a2a; border: 1px solid #4a4a4a; padding: 10px;");

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setAlignment(Qt::AlignCenter);
    m_progressBar->setStyleSheet(
        "QProgressBar { border: 1px solid #5577aa; border-radius: 5px; background-color: #3a3a3a; }"
        "QProgressBar::chunk { background-color: #5577aa; }"
        );

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
        qDebug() << "Loading cached data for replay:" << filePath;
        QJsonObject jsonObject = QJsonObject::fromVariantMap(cachedData);
        QJsonDocument jsonDocument(jsonObject);
        displayResults(QString::fromUtf8(jsonDocument.toJson(QJsonDocument::Indented)));
        return;
    }

    m_progressBar->setValue(0);
    m_resultsTextEdit->setPlainText("Початок аналізу файлу: " + filePath + "\n\n");

    QFuture<QVariantMap> future = QtConcurrent::run([this, filePath]() {
        return m_replayParser->parse(filePath);
    });
    m_watcher.setFuture(future);

    for (int i = 0; i <= 100; ++i) {
        m_progressBar->setValue(i);
        QThread::msleep(5);
    }
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

    QString report = "<h1>Звіт про аналіз реплею</h1>";
    report += "<h2>Базова інформація</h2>";
    report += "<ul>";
    report += "<li><b>Гравець:</b> " + analysisResults.value("playerName").toString() + "</li>";
    report += "<li><b>Мапа:</b> " + analysisResults.value("mapDisplayName").toString() + "</li>";
    report += "<li><b>Техніка:</b> " + analysisResults.value("playerVehicle").toString() + "</li>";
    report += "</ul>";

    // Імітація ігрових метрик (оскільки бінарна частина файлу недоступна)
    std::uniform_int_distribution<int> damage_dist(1000, 4000);
    std::uniform_int_distribution<int> blocked_dist(200, 2000);
    std::uniform_int_distribution<int> kills_dist(0, 5);
    std::uniform_int_distribution<int> spots_dist(0, 10);
    std::uniform_int_distribution<int> shots_dist(15, 40);
    std::uniform_int_distribution<int> hit_dist(10, 35);
    std::uniform_real_distribution<double> survival_dist(0.3, 0.9);

    int damage = damage_dist(rng);
    int blockedDamage = blocked_dist(rng);
    int kills = kills_dist(rng);
    int spots = spots_dist(rng);
    int shotsFired = shots_dist(rng);
    int shotsHit = std::min(shotsFired, hit_dist(rng));
    double accuracy = (double)shotsHit / shotsFired * 100;
    int totalGameTimeSeconds = 600;
    int survivalTimeSeconds = static_cast<int>(totalGameTimeSeconds * survival_dist(rng));
    double survivalRating = (double)survivalTimeSeconds / totalGameTimeSeconds * 100;

    report += "<h2>Метрики ефективності (імітовані)</h2>";
    report += "<ul>";
    report += "<li><b>Завдано шкоди:</b> " + QString::number(damage) + "</li>";
    report += "<li><b>Заблоковано шкоди:</b> " + QString::number(blockedDamage) + "</li>";
    report += "<li><b>Знищено:</b> " + QString::number(kills) + " танків</li>";
    report += "<li><b>Виявлено:</b> " + QString::number(spots) + " танків</li>";
    report += "<li><b>Точність:</b> " + QString::number(accuracy, 'f', 1) + "%</li>";
    report += "<li><b>Час виживання:</b> " + QString("%1:%2 хв").arg(survivalTimeSeconds / 60).arg(survivalTimeSeconds % 60, 2, 10, QChar('0')) + "</li>";
    report += "</ul>";

    QVariantMap behavioralPatterns;
    if (blockedDamage > 1000) {
        behavioralPatterns["coverUsage"] = "Використовує укриття дуже ефективно.";
    } else if (blockedDamage > 500) {
        behavioralPatterns["coverUsage"] = "Використовує укриття, але є простір для покращення.";
    } else {
        behavioralPatterns["coverUsage"] = "Рідко використовує укриття, схильний до прямого обміну шкодою.";
    }

    if (survivalRating < 50) {
        behavioralPatterns["riskTaking"] = "Високий ризик, часто йде в бій без достатньої підтримки.";
    } else if (survivalRating < 80) {
        behavioralPatterns["riskTaking"] = "Збалансований, іноді ризикує, але загалом грає обережно.";
    } else {
        behavioralPatterns["riskTaking"] = "Дуже обережний, може уникати ключових зіткнень.";
    }

    if (spots > 5) {
        behavioralPatterns["positioning"] = "Займає позиції, які дозволяють надавати підтримку команді.";
    } else {
        behavioralPatterns["positioning"] = "Сконцентрований на нанесенні шкоди, але не на виявленні.";
    }

    report += "<h2>Аналіз ігрової поведінки</h2>";
    report += "<p><b>Використання укриттів:</b> " + behavioralPatterns["coverUsage"].toString() + "</p>";
    report += "<p><b>Ризикованість:</b> " + behavioralPatterns["riskTaking"].toString() + "</p>";
    report += "<p><b>Позиціонування:</b> " + behavioralPatterns["positioning"].toString() + "</p>";

    report += "<h2>Персоналізовані рекомендації</h2>";
    report += "<ul>";
    if (accuracy < 70) {
        report += "<li>Зосередьтеся на прицілюванні, особливо по вразливих зонах противника.</li>";
    }
    if (blockedDamage < 600) {
        report += "<li>Більш ефективно використовуйте укриття, щоб зменшити отриману шкоду.</li>";
    }
    if (survivalRating < 50) {
        report += "<li>Уникайте надмірно агресивних дій, коли у вас немає підтримки союзників.</li>";
    } else {
        report += "<li>Спробуйте іноді займати більш агресивні позиції, щоб максимізувати свій внесок у перемогу.</li>";
    }
    report += "</ul>";

    displayResults(report);
}

void ReplayAnalyzerPage::displayResults(const QString& results) {
    m_resultsTextEdit->clear();
    m_resultsTextEdit->setHtml(results);
}
