// replayanalyzerpage.cpp
#include "replayanalyzerpage.h"

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
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QSpacerItem>
#include <QScrollBar>
#include <QTimer>
#include <QPrinter>
#include <QTextDocument>
#include <QRandomGenerator>
#include <QDir>
#include <QMetaObject>

ReplayAnalyzerPage::ReplayAnalyzerPage(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager)
{
    // Dark theme and global stylesheet
    setStyleSheet(R"(
        QWidget { background-color: #171717; color: #e6eef6; font-family: 'Segoe UI', 'Roboto', sans-serif; font-size: 13px; }
        QLabel { color: #dfe8f8; }
        QPushButton {
            background-color: #252525; color: #e6eef6; border: 1px solid #333;
            border-radius: 8px; padding: 8px 14px; min-height: 34px;
        }
        QPushButton:hover { background-color: #313131; }
        QPushButton:disabled { background-color: #1a1a1a; color: #777; }
        QPushButton#danger {
            background-color: #4b1f1f; border: 1px solid #662222;
        }
        QPushButton#danger:hover { background-color: #632727; }
        QProgressBar {
            background-color: #222; color: #e6eef6; border-radius: 6px; height: 18px; text-align: center;
        }
        QProgressBar::chunk { background-color: #4fa8ff; border-radius: 6px; }
        QListWidget {
            background-color: #1f1f1f; border: 1px solid #303030; border-radius: 8px;
            padding: 6px;
        }
        QListWidget::item { padding: 8px; margin: 2px 0; }
        QListWidget::item:selected { background-color: #3a7bd5; color: white; }
        QTextEdit { background-color: #161616; border: 1px solid #2f2f2f; border-radius: 8px; padding: 10px; color: #e6eef6; }
        QScrollBar:vertical { background: #1b1b1b; width: 10px; }
    )");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    mainLayout->setSpacing(14);

    // Top controls
    auto *topLayout = new QHBoxLayout();
    topLayout->setSpacing(10);

    m_selectFileButton = new QPushButton("📁 Вибрати реплей...", this);
    m_selectFileButton->setCursor(Qt::PointingHandCursor);

    m_clearReplaysButton = new QPushButton("🧹 Очистити список", this);
    m_clearReplaysButton->setCursor(Qt::PointingHandCursor);
    m_clearReplaysButton->setObjectName("danger");

    m_deleteSelectedButton = new QPushButton("🗑️ Видалити вибраний", this);
    m_deleteSelectedButton->setCursor(Qt::PointingHandCursor);

    m_exportPdfButton = new QPushButton("📤 Експортувати як PDF", this);
    m_exportPdfButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(m_selectFileButton);
    topLayout->addWidget(m_clearReplaysButton);
    topLayout->addWidget(m_deleteSelectedButton);
    topLayout->addWidget(m_exportPdfButton);
    topLayout->addStretch();

    auto *hintLabel = new QLabel("<i>Підтримуються файли: <b>.wotreplay</b></i>");
    hintLabel->setStyleSheet("color:#9fb5df; font-size:12px;");
    topLayout->addWidget(hintLabel, 0, Qt::AlignRight);

    mainLayout->addLayout(topLayout);

    // Progress bar
    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setAlignment(Qt::AlignCenter);
    m_progressBar->setTextVisible(true);
    m_progressBar->setFormat("");
    mainLayout->addWidget(m_progressBar);

    // Content: left list + right results
    auto *contentLayout = new QHBoxLayout();
    contentLayout->setSpacing(18);

    // Left - replay list
    auto *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(8);

    auto *listTitle = new QLabel("📂 Список реплеїв:");
    listTitle->setStyleSheet("font-weight:600; font-size:15px;");
    leftLayout->addWidget(listTitle);

    m_replayList = new QListWidget(this);
    m_replayList->setSelectionMode(QAbstractItemView::SingleSelection);
    m_replayList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_replayList->setMinimumWidth(280);
    m_replayList->setMaximumWidth(360);

    auto *listShadow = new QGraphicsDropShadowEffect(this);
    listShadow->setBlurRadius(8);
    listShadow->setOffset(0, 2);
    listShadow->setColor(QColor(0, 0, 0, 160));
    m_replayList->setGraphicsEffect(listShadow);

    leftLayout->addWidget(m_replayList);

    auto *listBtnsLayout = new QHBoxLayout();
    listBtnsLayout->setSpacing(8);
    listBtnsLayout->addStretch();
    leftLayout->addLayout(listBtnsLayout);

    auto *leftWidget = new QWidget(this);
    leftWidget->setLayout(leftLayout);
    contentLayout->addWidget(leftWidget);

    // Right - results
    auto *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(10);

    auto *resultsTitle = new QLabel("📊 Результати аналізу:");
    resultsTitle->setStyleSheet("font-weight:600; font-size:15px;");
    rightLayout->addWidget(resultsTitle);

    m_resultsTextEdit = new QTextEdit(this);
    m_resultsTextEdit->setReadOnly(true);
    m_resultsTextEdit->setPlainText("Виберіть реплей зі списку або завантажте новий, щоб побачити результати аналізу.");
    m_resultsTextEdit->verticalScrollBar()->setSingleStep(20);

    auto *resultsShadow = new QGraphicsDropShadowEffect(this);
    resultsShadow->setBlurRadius(10);
    resultsShadow->setOffset(0, 3);
    resultsShadow->setColor(QColor(0,0,0,160));
    m_resultsTextEdit->setGraphicsEffect(resultsShadow);

    rightLayout->addWidget(m_resultsTextEdit);

    auto *rightWidget = new QWidget(this);
    rightWidget->setLayout(rightLayout);
    contentLayout->addWidget(rightWidget, 1);

    mainLayout->addLayout(contentLayout);

    // Init modules
    m_replayParser = new ReplayParser(this);
    m_metricsCalculator = new MetricsCalculator(this);
    m_behaviorAnalyzer = new BehaviorAnalyzer(this);
    m_recommenderSystem = new RecommenderSystem(this);

    // Connections
    connect(m_selectFileButton, &QPushButton::clicked, this, &ReplayAnalyzerPage::onSelectFileButtonClicked);
    connect(m_replayList, &QListWidget::itemClicked, this, &ReplayAnalyzerPage::onReplayListItemClicked);
    connect(&m_watcher, &QFutureWatcher<QVariantMap>::finished, this, &ReplayAnalyzerPage::handleAnalysisFinished);
    connect(m_clearReplaysButton, &QPushButton::clicked, this, &ReplayAnalyzerPage::onClearReplaysButtonClicked);
    connect(m_deleteSelectedButton, &QPushButton::clicked, this, &ReplayAnalyzerPage::onDeleteSelectedClicked);
    connect(m_exportPdfButton, &QPushButton::clicked, this, &ReplayAnalyzerPage::onExportPdfClicked);

    // Load cached
    loadCachedReplays();
}

ReplayAnalyzerPage::~ReplayAnalyzerPage() {}

QString ReplayAnalyzerPage::cleanVehicleName(const QString &technicalName)
{
    // Технічні назви часто мають формат "нація-код_назва", наприклад "usa-A12_T29"
    // Ми просто беремо частину після останнього символу '_'
    int lastUnderscorePos = technicalName.lastIndexOf('_');
    if (lastUnderscorePos != -1) {
        return technicalName.mid(lastUnderscorePos + 1);
    }

    // Якщо раптом формат інший, повертаємо назву як є
    return technicalName;
}

// -------------------- UI / interaction methods --------------------

void ReplayAnalyzerPage::onSelectFileButtonClicked() {
    QString filePath = QFileDialog::getOpenFileName(this, "Вибрати реплей", "", "World of Tanks Replays (*.wotreplay)");
    if (filePath.isEmpty()) return;

    QFileInfo fileInfo(filePath);

    for (int i = 0; i < m_replayList->count(); ++i) {
        if (m_replayList->item(i)->data(Qt::UserRole).toString() == filePath) {
            QMessageBox::information(this, "Файл вже додано", "Цей реплей вже є у списку.");
            return;
        }
    }

    QListWidgetItem *item = new QListWidgetItem(QStringLiteral("🎮 %1").arg(fileInfo.fileName()));
    item->setData(Qt::UserRole, filePath);
    m_replayList->addItem(item);
    m_replayList->setCurrentItem(item);

    analyzeReplay(filePath);
}

void ReplayAnalyzerPage::onReplayListItemClicked(QListWidgetItem *item) {
    if (!item) return;
    QString path = item->data(Qt::UserRole).toString();
    if (!path.isEmpty()) analyzeReplay(path);
}

void ReplayAnalyzerPage::loadCachedReplays() {
    m_replayList->clear();

    if (!m_dbManager) return;
    QVariantList cachedReplays = m_dbManager->getReplays();
    for (const QVariant &replayData : cachedReplays) {
        QVariantMap replayMap = replayData.toMap();
        QString filePath = replayMap.value("filePath").toString();
        QFileInfo fileInfo(filePath);
        if (fileInfo.exists()) {
            QListWidgetItem* item = new QListWidgetItem(QStringLiteral("🎮 %1").arg(fileInfo.fileName()));
            item->setData(Qt::UserRole, filePath);
            m_replayList->addItem(item);
        }
    }
}

void ReplayAnalyzerPage::analyzeReplay(const QString &filePath) {
    if (filePath.isEmpty()) return;

    if (m_dbManager) {
        QVariantMap cachedData = m_dbManager->getReplayData(filePath);
        if (!cachedData.isEmpty()) {
            displayStructuredResults(cachedData);
            return;
        }
    }

    // UX lock
    m_progressBar->setValue(0);
    m_progressBar->setFormat("📊 Аналіз файлу...");
    m_selectFileButton->setEnabled(false);
    m_clearReplaysButton->setEnabled(false);
    m_deleteSelectedButton->setEnabled(false);
    m_exportPdfButton->setEnabled(false);
    m_replayList->setEnabled(false);

    m_resultsTextEdit->setPlainText(QString("Початок аналізу файлу: %1").arg(QFileInfo(filePath).fileName()));

    // Simulate progress while parser runs
    if (auto existing = findChild<QTimer*>("replayProgressTimer")) {
        existing->stop();
        existing->deleteLater();
    }
    QTimer *progressTimer = new QTimer(this);
    progressTimer->setObjectName("replayProgressTimer");
    progressTimer->setInterval(140);
    connect(progressTimer, &QTimer::timeout, this, [this]() {
        int v = m_progressBar->value();
        if (v < 90) {
            int step = QRandomGenerator::global()->bounded(3, 11); // 3..10
            m_progressBar->setValue(qMin(90, v + step));
        }
    });
    progressTimer->start();

    QFuture<QVariantMap> future = QtConcurrent::run([this, filePath]() {
        return m_replayParser->parse(filePath);
    });
    m_watcher.setFuture(future);
}

void ReplayAnalyzerPage::handleAnalysisFinished() {
    // finalize progress
    m_progressBar->setValue(100);
    m_progressBar->setFormat("✅ Аналіз завершено");

    if (auto timer = findChild<QTimer*>("replayProgressTimer")) {
        timer->stop();
        timer->deleteLater();
    }

    m_selectFileButton->setEnabled(true);
    m_clearReplaysButton->setEnabled(true);
    m_deleteSelectedButton->setEnabled(true);
    m_exportPdfButton->setEnabled(true);
    m_replayList->setEnabled(true);

    QVariantMap analysisResults = m_watcher.result();

    if (analysisResults.isEmpty() || !analysisResults.contains("personal")) {
        QMessageBox::critical(this, "Помилка", "Не вдалося розібрати файл реплею. Можливо, файл пошкоджено або має непідтримуваний формат.");
        m_resultsTextEdit->setPlainText("❌ Помилка при аналізі файлу.");

        for (int i = 0; i < m_replayList->count(); ++i) {
            auto *it = m_replayList->item(i);
            if (it && m_watcher.future().result().value("filePath").toString() == it->data(Qt::UserRole).toString()) {
                delete m_replayList->takeItem(i);
                break;
            }
        }
        return;
    }

    QString filePath = analysisResults.value("filePath").toString();
    if (m_dbManager) m_dbManager->addReplayData(filePath, analysisResults);

    displayStructuredResults(analysisResults);
}

// replayanalyzerpage.cpp

void ReplayAnalyzerPage::displayStructuredResults(const QVariantMap &data)
{
    // --- 1. Отримання даних з аналітичних модулів ---
    QString playerName = data.value("playerName").toString();
    QVariantMap metrics = m_metricsCalculator->calculate(data);
    QVariantMap behavior = m_behaviorAnalyzer->analyze(data, metrics);
    QStringList recommendations = m_recommenderSystem->generate(data, metrics, behavior);

    // --- 2. Початок формування HTML-звіту ---
    QString report;
    QLocale locale(QLocale::English); // Для форматування чисел
    report += "<div style='font-family:Segoe UI, Roboto, sans-serif; color:#e6eef6;'>";

    // Заголовок (без змін)
    report += "<div style='display:flex; align-items:center; gap:12px;'>"
              "<h1 style='margin:0; font-size:20px;'>📄 Звіт про аналіз реплею</h1>"
              "</div>";

    // Головний контейнер (без змін)
    report += "<div style='margin-top:10px; padding:12px; background:#141414; border:1px solid #2b2b2b; border-radius:8px;'>";

    // --- Блок "Базова інформація" (без змін) ---
    report += "<h2 style='margin:6px 0 8px 0; font-size:16px; color:#9fc4ff;'>Загальна інформація про бій</h2>";
    report += "<ul style='margin:0 0 8px 18px; line-height: 1.65;'>";

    QVariantMap common = data.value("common").toMap();
    int duration = common.value("duration", 0).toInt();
    int winnerTeam = common.value("winnerTeam", 0).toInt();
    int playerTeam = metrics.value("team", 0).toInt();

    report += QString("<li><b>Гравець:</b> %1</li>").arg(playerName);
    report += QString("<li><b>Сервер:</b> %1</li>").arg(data.value("serverName", "N/A").toString());
    report += QString("<li><b>Карта:</b> %1</li>").arg(data.value("mapDisplayName", "N/A").toString());
    report += QString("<li><b>Техніка:</b> %1</li>").arg(metrics.value("cleanVehicleName").toString());

    if (winnerTeam != 0 && playerTeam != 0) {
        QString outcome = (winnerTeam == playerTeam)
        ? "<span style='color:#88ff88; font-weight:600;'>Перемога</span>"
        : "<span style='color:#ff8888; font-weight:600;'>Поразка</span>";
        report += QString("<li><b>Результат бою:</b> %1</li>").arg(outcome);
    }
    report += "</ul>";


    // --- Блок "Результати гравця" з великими картками (БЕЗ ЗМІН) ---
    report += "<h2 style='margin: 16px 0 12px 0; font-size: 18px; color: #9fc4ff;'>Результати гравця</h2>";
    report += "<div style='display: flex; flex-direction: column; gap: 36px; align-items: flex-start;'>";
    auto appendMetricSeparated = [&](const QString &label, const QString &value, const QString &color = "#ffffff") {
        report += QString("<div style='font-size: 17px; font-weight: 600; color: #cfd9ff; background: #181818; padding: 8px 14px; border-radius: 8px; display: inline-block; box-shadow: 0 0 6px rgba(0,0,0,0.5);'>%1:</div>").arg(label);
        report += QString("<div style='font-size: 40px; font-weight: 800; color: %1; background: #0f0f0f; padding: 14px 28px; border-radius: 12px; margin-top: 6px; margin-bottom: 12px; display: inline-block; box-shadow: inset 0 0 10px rgba(0,0,0,0.7); text-align: center;'>%2</div>").arg(color, value);
    };
    appendMetricSeparated("Нанесено шкоди", locale.toString(metrics.value("totalDamageDealt", 0.0).toInt()), "#ffd166");
    appendMetricSeparated("Знищено", QString::number(metrics.value("kills", 0.0).toInt()), "#ff7b7b");
    appendMetricSeparated("Допомога команді", locale.toString(metrics.value("damageAssisted", 0.0).toInt()), "#7be495");
    appendMetricSeparated("Заблоковано шкоди", locale.toString(metrics.value("damageBlockedByArmor", 0.0).toInt()), "#a0aaff");
    report += "</div>";


    // --- ВИПРАВЛЕНО: Блок "Бойова ефективність" та "Економіка" з уніфікованим стилем ---
    // Спочатку шукаємо клан гравця в правильному місці
    QString clanAbbrev = "Немає";
    if (data.contains("vehicles")) {
        QVariantMap vehicles = data.value("vehicles").toMap();
        for (auto it = vehicles.constBegin(); it != vehicles.constEnd(); ++it) {
            QVariantMap vehicleData = it.value().toMap();
            if (vehicleData.value("name").toString() == playerName) {
                QString clan = vehicleData.value("clanAbbrev").toString();
                if (!clan.isEmpty()) {
                    clanAbbrev = clan;
                }
                break;
            }
        }
    }

    report += QString("<p style='margin-top:12px;'><b>Клан:</b> %1 &nbsp;&nbsp; <b>Команда:</b> %2</p>").arg(clanAbbrev).arg(playerTeam);

    // Блок "Бойова ефективність"
    report += "<h2 style='margin-top:14px; font-size:16px; color:#9fc4ff;'>Бойова ефективність</h2>";
    report += "<ul style='margin-left:18px; line-height: 1.65;'>";
    report += QString("<li><b>Постріли / Влучання / Пробиття:</b> %1 / %2 / %3</li>")
                  .arg(metrics.value("shots", 0.0).toInt())
                  .arg(metrics.value("hits", 0.0).toInt())
                  .arg(metrics.value("piercings", 0.0).toInt());
    report += QString("<li><b>Потенційно отримана шкода:</b> %1</li>").arg(locale.toString(metrics.value("potentialDamageReceived").toInt()));
    report += QString("<li><b>Виявлено ворогів:</b> %1</li>").arg(metrics.value("spotted").toInt());

    QString achievementsText;
    int masteryMark = metrics.value("markOfMastery").toInt();
    if (masteryMark > 0 && masteryMark < 4) achievementsText += QString("Знак класності %1, ").arg(masteryMark);
    if (metrics.value("isFirstBlood").toBool()) achievementsText += "\"Перша кров\", ";
    if (metrics.value("marksOnGun").toInt() > 0) achievementsText += QString("%1 відмітк(а/и) на стволі, ").arg(metrics.value("marksOnGun").toInt());
    achievementsText += QString("%1 інш. досягнень").arg(metrics.value("achievementsCount").toInt());
    report += QString("<li><b>Досягнення:</b> %1</li>").arg(achievementsText);
    report += "</ul>";

    // Блок "Економіка та досвід"
    report += "<h2 style='margin-top:14px; font-size:16px; color:#9fc4ff;'>Економіка та досвід</h2>";
    report += "<ul style='margin-left:18px; line-height: 1.65;'>";
    report += QString("<li><b>Досвід за бій (чистий):</b> %1 (%2)</li>")
                  .arg(locale.toString(metrics.value("xp").toInt()))
                  .arg(locale.toString(metrics.value("originalXP").toInt()));
    report += QString("<li><b>Кредити за бій (чисті):</b> %1 (%2)</li>")
                  .arg(locale.toString(metrics.value("credits").toInt()))
                  .arg(locale.toString(metrics.value("originalCredits").toInt()));
    report += QString("<li><b>Вільний досвід:</b> %1</li>").arg(locale.toString(metrics.value("freeXP").toInt()));
    report += "</ul>";

    // --- Існуючі блоки аналізу (БЕЗ ЗМІН) ---
    report += "<h2 style='margin-top:14px; font-size:16px; color:#9fc4ff;'>Ключові метрики ефективності</h2>";
    if (!metrics.isEmpty()) {
        report += "<ul style='margin-left:18px; line-height: 1.65;'>";
        report += QString("<li><b>Точність стрільби:</b> %1</li>").arg(metrics.value("accuracy").toString());
        report += QString("<li><b>Ефективність пробиття:</b> %1</li>").arg(metrics.value("penetrationRatio").toString());
        report += QString("<li><b>Середня шкода за пробиття:</b> %1</li>").arg(metrics.value("avgDmgPerPen").toInt());
        report += QString("<li><b>Сумарний внесок (шкода + асист + блок):</b> %1</li>").arg(metrics.value("combinedContribution").toInt());
        report += "</ul>";
    } else {
        report += "<p style='color:#ffd966;'>Недостатньо даних для розрахунку ключових метрик.</p>";
    }

    report += "<h2 style='margin-top:14px; font-size:16px; color:#9fc4ff;'>Аналіз ігрової поведінки</h2>";
    QString performanceGrade = behavior.value("performanceGrade").toString();
    if (performanceGrade.contains("помилка даних")) {
        report += "<p style='color:#ffcc00;'><b>Не вдалося провести детальний аналіз...</b></p>";
    } else if (!behavior.isEmpty()) {
        report += "<ul style='margin-left:18px; line-height: 1.65;'>";
        report += QString("<li><b>Оцінка ефективності:</b> %1</li>").arg(performanceGrade);
        report += QString("<li><b>Виконання ролі:</b> %1</li>").arg(behavior.value("rolePerformance").toString());
        QString keySkill = behavior.value("keySkill").toString();
        if (!keySkill.isEmpty() && keySkill != "Не визначено") {
            report += QString("<li><b>Ключовий навик у бою:</b> %1</li>").arg(keySkill);
        }
        report += "</ul>";
    }

    report += "<h2 style='margin-top:14px; font-size:16px; color:#9fc4ff;'>Персональні рекомендації</h2>";
    if (!recommendations.isEmpty() && !performanceGrade.contains("помилка даних")) {
        report += "<ul style='margin-left:18px; line-height: 1.65;'>";
        for (const QString &rec : recommendations) {
            report += QString("<li> %1</li>").arg(rec);
        }
        report += "</ul>";
    } else {
        report += "<p style='color:#c0cbdc;'>Рекомендації не сформовано.</p>";
    }

    report += "<h2 style='margin-top:14px; font-size:16px; color:#9fc4ff;'>Хронологія бою</h2>";
    quint32 recorderEntityId = 0;
    if (data.contains("vehicles")) {
        QVariantMap vehiclesData = data.value("vehicles").toMap();
        for (auto it = vehiclesData.constBegin(); it != vehiclesData.constEnd(); ++it) {
            if (it.value().toMap().value("name").toString() == playerName) {
                recorderEntityId = it.key().toUInt();
                break;
            }
        }
    }
    report += generateTimelineHtml(data, recorderEntityId);

    report += "</div>"; // Закриття головного контейнера
    report += "</div>"; // Закриття обгортки

    m_resultsTextEdit->clear();
    m_resultsTextEdit->setHtml(report);
}

QString ReplayAnalyzerPage::generateTimelineHtml(const QVariantMap &data, quint32 recorderEntityId)
{
    Q_UNUSED(recorderEntityId); // Ми можемо отримати ім'я гравця напряму

    if (!data.contains("shot_events")) {
        return "<p style='color:#c0cbdc;'><i>Детальний звіт про постріли не знайдено в цьому реплеї.</i></p>";
    }

    QVariantList shotEvents = data.value("shot_events").toList();
    if (shotEvents.isEmpty()) {
        return "<p style='color:#c0cbdc;'><i>У цьому бою не зафіксовано жодної події, що завдала шкоди.</i></p>";
    }

    QString html;
    html += R"(
        <style>
            .timeline-table { width: 100%; border-collapse: collapse; margin-top: 10px; font-size: 12px; }
            .timeline-table th, .timeline-table td { padding: 7px 9px; text-align: left; border-bottom: 1px solid #2f2f2f; }
            .timeline-table th { color: #aabfff; font-weight: 600; background-color: #222; }
            .timeline-table tr:last-child td { border-bottom: none; }
            .timeline-table tr:hover { background-color: #2a2a2a; }
            .damage-dealt { color: #ffd166; font-weight: 600; }
            .damage-received { color: #ff8888; font-weight: 600; }
            .no-damage { color: #888; }
            .friendly-fire { background-color: #4b1f1f; }
            .crit { font-style: italic; color: #ffb3b3; }
            .kill { font-weight: bold; color: #ff7b7b; }
        </style>
    )";
    html += "<table class='timeline-table'>";
    html += "<thead><tr><th>Час</th><th>Атакуючий</th><th>Ціль</th><th style='text-align:right;'>Шкода</th><th>Результат</th></tr></thead><tbody>";

    // Отримуємо ім'я гравця, чий реплей ми дивимося
    QString mainPlayerName = data.value("playerName").toString();

    for (const QVariant &shotVar : shotEvents) {
        QVariantMap shot = shotVar.toMap();

        float timestamp = shot.value("timestamp").toFloat();
        QString attackerName = shot.value("attackerName").toString();
        QString targetName = shot.value("targetName").toString();
        int damage = shot.value("damage").toInt();
        bool isFriendlyFire = shot.value("isFriendlyFire").toBool();
        QStringList criticalHits = shot.value("criticalHits").toStringList();

        // Форматуємо час
        int totalSeconds = qRound(timestamp);
        int minutes = totalSeconds / 60;
        int seconds = totalSeconds % 60;
        QString timeStr = QString("%1:%2").arg(minutes, 2, 10, QChar('0')).arg(seconds, 2, 10, QChar('0'));

        // Формуємо опис результату
        QString resultDesc;
        if (shot.value("isPenetration").toBool()) resultDesc += "Пробиття";
        else if (shot.value("isRicochet").toBool()) resultDesc += "Рикошет";
        else if (shot.value("isShellExplosion").toBool()) resultDesc += "Сплеш";
        else if (shot.value("isNoDamage").toBool()) resultDesc += "Без шкоди";
        else resultDesc += "Влучання";

        bool isKill = false;
        if (!criticalHits.isEmpty()) {
            QStringList cleanCrits;
            for (const QString &crit : criticalHits) {
                if (crit == "знищено") {
                    isKill = true;
                } else {
                    cleanCrits.append(crit);
                }
            }
            if (!cleanCrits.isEmpty()) {
                resultDesc += QString(" <span class='crit'>(%1)</span>").arg(cleanCrits.join(", "));
            }
        }
        if (isKill) {
            resultDesc += " <span class='kill'>ЗНИЩЕНО!</span>";
        }

        // Визначаємо клас для рядка та шкоди
        QString rowClass;
        if (isFriendlyFire) rowClass = "friendly-fire";

        QString damageClass;
        if (damage > 0) {
            if (attackerName == mainPlayerName) damageClass = "damage-dealt";
            else if (targetName == mainPlayerName) damageClass = "damage-received";
        } else {
            damageClass = "no-damage";
        }

        html += QString("<tr class='%1'><td>%2</td><td>%3</td><td>%4</td><td style='text-align:right;' class='%5'>%6</td><td>%7</td></tr>")
                    .arg(rowClass,
                         timeStr,
                         attackerName,
                         targetName,
                         damageClass,
                         QString::number(damage),
                         resultDesc);
    }

    html += "</tbody></table>";
    return html;
}

void ReplayAnalyzerPage::onDeleteSelectedClicked()
{
    auto *item = m_replayList->currentItem();
    if (!item) {
        QMessageBox::information(this, "Нічого не вибрано", "Оберіть реплей у списку, щоб його видалити.");
        return;
    }

    QString filePath = item->data(Qt::UserRole).toString();
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Видалити реплей",
                                                              QString("Ви дійсно хочете видалити \"%1\" з списку?").arg(item->text()),
                                                              QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    delete m_replayList->takeItem(m_replayList->row(item));

    if (m_dbManager) {
        bool invoked = QMetaObject::invokeMethod(m_dbManager, "removeReplayData", Q_ARG(QString, filePath));
        if (!invoked) {
            QMetaObject::invokeMethod(m_dbManager, "removeReplay", Q_ARG(QString, filePath));
        }
    }
}

// Export current HTML report to PDF
void ReplayAnalyzerPage::onExportPdfClicked()
{
    QString html = m_resultsTextEdit->toHtml();
    if (html.trimmed().isEmpty()) {
        QMessageBox::information(this, "Немає звіту", "Спочатку виконайте аналіз реплею, щоб експортувати звіт у PDF.");
        return;
    }

    QString suggested = QDir::homePath() + "/replay_report.pdf";
    QString fileName = QFileDialog::getSaveFileName(this, "Експорт в PDF", suggested, "PDF files (*.pdf)");
    if (fileName.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));

    QTextDocument doc;
    doc.setDefaultFont(QFont("Segoe UI", 10));
    doc.setHtml(html);
   // doc.setPageSize(printer.pageRect().size());

    doc.print(&printer);

    QMessageBox::information(this, "Експортовано", QString("Звіт збережено у PDF: %1").arg(fileName));
}

// Clear list + DB
void ReplayAnalyzerPage::onClearReplaysButtonClicked() {
    QMessageBox::StandardButton reply =
        QMessageBox::question(this, "Очищення списку",
                              "Ви впевнені, що хочете очистити весь список реплеїв? Дані аналізу буде видалено.",
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
