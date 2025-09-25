// playerprofilepage.cpp
#include "playerprofilepage.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QLocale>
#include <QHeaderView>
#include <QPainter>
#include <QChartView>
#include <QPieSeries>
#include <QPieSlice>
#include <QChart>
#include <QComboBox>
#include <QLabel>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QStyle>
#include <QApplication>

// Ensure Qt Charts module is added in your .pro/CMakeLists (QT += charts)


PlayerProfilePage::PlayerProfilePage(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager)
{
    m_metricsCalculator = new MetricsCalculator(this);
    setupUI();

    // Connect signals
    connect(m_dbManager, &DatabaseManager::dataChanged, this, &PlayerProfilePage::recalculateAndDisplay);
    connect(m_playerSelector, &QComboBox::currentTextChanged, this, &PlayerProfilePage::onPlayerSelected);

    recalculateAndDisplay();
}

void PlayerProfilePage::setupUI()
{
    // Global widget stylesheet + font preference
    setStyleSheet(
        "QWidget { background-color: #1e1e1e; color: #f0f0f0; font-family: 'Segoe UI', 'Roboto', sans-serif; }"
        );

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

    // --- Top Panel: Player Selection ---
    auto selectorLayout = new QHBoxLayout();
    auto profileLabel = new QLabel("<b>Профіль гравця:</b>");
    profileLabel->setStyleSheet("font-size: 16px;");
    selectorLayout->addWidget(profileLabel);

    m_playerSelector = new QComboBox(this);
    m_playerSelector->setMinimumWidth(250);
    m_playerSelector->setStyleSheet(
        "QComboBox { background-color: #2e2e2e; color: #f0f0f0; padding: 6px 12px; border-radius: 6px; }"
        "QComboBox::drop-down { border: none; }"
        "QComboBox QAbstractItemView { background-color: #2e2e2e; selection-background-color: #55aaff; }"
        );
    selectorLayout->addWidget(m_playerSelector);
    selectorLayout->addStretch();
    mainLayout->addLayout(selectorLayout);

    // --- KPI Cards ---
    auto gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);

    auto createStatCard = [&](QLabel*& label, const QString& title) {
        auto frame = new QFrame(this);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setStyleSheet(
            "QFrame {"
            " background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #2b2b2b, stop:1 #3f3f3f);"
            " border-radius: 12px; padding: 12px;"
            "}"
            "QFrame:hover { background-color: #4a4a4a; }"
            );

        auto shadow = new QGraphicsDropShadowEffect(frame);
        shadow->setBlurRadius(16);
        shadow->setOffset(2, 2);
        shadow->setColor(QColor(0, 0, 0, 160));
        frame->setGraphicsEffect(shadow);

        auto layout = new QVBoxLayout(frame);
        layout->setContentsMargins(10, 10, 10, 10);
        layout->setSpacing(4);

        label = new QLabel("0", frame);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 34px; font-weight: bold; color: #55aaff;");
        auto titleLabel = new QLabel(title, frame);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 13px; color: #bbbbbb;");
        layout->addWidget(label);
        layout->addWidget(titleLabel);
        return frame;
    };

    gridLayout->addWidget(createStatCard(m_battlesLabel, "Проаналізовано боїв"), 0, 0);
    gridLayout->addWidget(createStatCard(m_winRateLabel, "Відсоток перемог"), 0, 1);
    gridLayout->addWidget(createStatCard(m_avgDamageLabel, "Середня шкода"), 0, 2);
    mainLayout->addLayout(gridLayout);

    // --- Detailed Statistics: Chart + Table (Splitter) ---
    auto splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setStyleSheet("QSplitter::handle { background: #444; }");

    // Left: Pie Chart
    m_chartView = new QChartView(this);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    // create chart explicitly
    QChart *chart = new QChart();
    chart->setBackgroundBrush(QBrush(QColor("#2e2e2e")));
    chart->legend()->setLabelColor(Qt::white);
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setAnimationOptions(QChart::AllAnimations);
    m_chartView->setChart(chart);

    splitter->addWidget(m_chartView);

    // Right: Table
    m_classStatsTable = new QTableWidget(this);
    m_classStatsTable->setColumnCount(3);
    m_classStatsTable->setHorizontalHeaderLabels({"Клас", "Боїв", "Середня шкода"});
    m_classStatsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_classStatsTable->setAlternatingRowColors(true);
    m_classStatsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_classStatsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_classStatsTable->setStyleSheet(
        "QTableWidget { background-color: #2e2e2e; alternate-background-color: #252525; color: white; gridline-color: #444; }"
        "QHeaderView::section { background-color: #3a3a3a; color: white; padding: 6px; }"
        "QTableWidget::item { padding: 6px; }"
        );

    splitter->addWidget(m_classStatsTable);
    splitter->setSizes({520, 420});
    mainLayout->addWidget(splitter, 1);

    // --- Analytical Conclusions ---
    auto analysisLabel = new QLabel("<b>Аналітичні висновки:</b>");
    analysisLabel->setStyleSheet("font-size: 16px; margin-top: 10px;");
    mainLayout->addWidget(analysisLabel);

    m_analysisText = new QTextEdit(this);
    m_analysisText->setReadOnly(true);
    m_analysisText->setMaximumHeight(180);
    m_analysisText->setStyleSheet(
        "QTextEdit { background-color: #2e2e2e; color: #e0e0e0; border: 1px solid #444; border-radius: 8px; padding: 10px; }"
        );
    mainLayout->addWidget(m_analysisText);
}

// ---------------------- Data processing & display -----------------------

void PlayerProfilePage::recalculateAndDisplay()
{
    m_profiles.clear();
    QVariantList replays = m_dbManager->getReplays();

    for (const QVariant& replayVar : replays) {
        QVariantMap replayMap = replayVar.toMap();
        QVariantMap replayData = replayMap.value("analysisData").toMap();
        if (replayData.isEmpty()) continue;

        QString playerName = replayData.value("playerName").toString();
        if (playerName.isEmpty()) continue;

        PlayerProfile &profile = m_profiles[playerName];
        profile.playerName = playerName;
        profile.battleCount++;

        QVariantMap metrics = m_metricsCalculator->calculate(replayData);
        double damage = metrics.value("totalDamageDealt").toDouble();
        double assisted = metrics.value("damageAssisted").toDouble();
        double blocked = metrics.value("damageBlockedByArmor").toDouble();
        double kills = metrics.value("kills").toDouble();

        profile.totalDamage += damage;
        profile.totalAssisted += assisted;
        profile.totalBlocked += blocked;
        profile.totalKills += kills;

        if (damage > profile.maxDamage) profile.maxDamage = damage;
        if (kills > profile.maxKills) profile.maxKills = kills;
        if (assisted > profile.maxAssisted) profile.maxAssisted = assisted;
        if (blocked > profile.maxBlocked) profile.maxBlocked = blocked;

        int playerTeam = 0;
        if (replayData.contains("vehicles")) {
            QVariantMap vehicles = replayData.value("vehicles").toMap();
            for (auto it = vehicles.constBegin(); it != vehicles.constEnd(); ++it) {
                QVariantMap v = it.value().toMap();
                if (v.value("name").toString() == playerName) {
                    playerTeam = v.value("team").toInt();
                    break;
                }
            }
        }
        if (replayData.contains("winnerTeam") && replayData.value("winnerTeam").toInt() == playerTeam && playerTeam != 0) {
            profile.wins++;
        }

        QString vehicleType = metrics.value("vehicleType").toString();
        if (vehicleType.isEmpty()) vehicleType = "unknown";
        if (vehicleType != "unknown") {
            profile.battlesByVehicleType[vehicleType] += 1;
            profile.damageByVehicleType[vehicleType] += damage;
        }
    }

    QString currentPlayer = m_playerSelector->currentText();
    m_playerSelector->clear();
    m_playerSelector->addItems(m_profiles.keys());

    int currentIndex = m_playerSelector->findText(currentPlayer);
    if (currentIndex != -1) m_playerSelector->setCurrentIndex(currentIndex);

    onPlayerSelected(m_playerSelector->currentText());
}

void PlayerProfilePage::onPlayerSelected(const QString &playerName)
{
    if (playerName.isEmpty() || !m_profiles.contains(playerName)) {
        m_battlesLabel->setText("0");
        m_winRateLabel->setText("0%");
        m_avgDamageLabel->setText("0");
        m_classStatsTable->setRowCount(0);
        if (m_chartView->chart()) m_chartView->chart()->removeAllSeries();
        m_analysisText->setHtml("<p>Немає даних для відображення. Завантажте та проаналізуйте реплеї.</p>");
        return;
    }

    displayProfile(m_profiles.value(playerName));
}

void PlayerProfilePage::refreshProfile()
{
    recalculateAndDisplay();
}

void PlayerProfilePage::displayProfile(const PlayerProfile &profile)
{
    QLocale locale(QLocale::English);

    // KPI
    m_battlesLabel->setText(QString::number(profile.battleCount));
    m_winRateLabel->setText(QString::number(profile.winRate(), 'f', 2) + "%");
    m_avgDamageLabel->setText(locale.toString(qRound(profile.avgDamage())));

    // Table
    m_classStatsTable->setRowCount(0);
    for (auto it = profile.battlesByVehicleType.constBegin(); it != profile.battlesByVehicleType.constEnd(); ++it) {
        int row = m_classStatsTable->rowCount();
        m_classStatsTable->insertRow(row);
        double avgDmg = it.value() > 0 ? profile.damageByVehicleType.value(it.key()) / it.value() : 0;

        auto itemClass = new QTableWidgetItem(it.key());
        auto itemBattles = new QTableWidgetItem(QString::number(it.value()));
        auto itemAvg = new QTableWidgetItem(locale.toString(qRound(avgDmg)));

        // Center numeric columns
        itemBattles->setTextAlignment(Qt::AlignCenter);
        itemAvg->setTextAlignment(Qt::AlignCenter);

        m_classStatsTable->setItem(row, 0, itemClass);
        m_classStatsTable->setItem(row, 1, itemBattles);
        m_classStatsTable->setItem(row, 2, itemAvg);
    }

    // Pie chart - build series
    if (!m_chartView->chart()) {
        m_chartView->setChart(new QChart());
    }
    QChart *chart = m_chartView->chart();
    chart->removeAllSeries();

    QPieSeries *series = new QPieSeries();
    series->setLabelsVisible(true);
    series->setPieSize(0.7);

    for (auto it = profile.battlesByVehicleType.constBegin(); it != profile.battlesByVehicleType.constEnd(); ++it) {
        series->append(it.key(), it.value());
    }

    // Colors palette (will cycle)
    QStringList colors = {"#55aaff", "#ffaa55", "#55ffaa", "#ff5555", "#aa55ff", "#ffd155", "#55d6ff"};
    int i = 0;
    for (QPieSlice *slice : series->slices()) {
        slice->setBrush(QColor(colors[i % colors.size()]));
        slice->setLabelColor(Qt::white);
        slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);
        slice->setPen(QPen(QColor(40,40,40))); // subtle outline
        ++i;
    }

    chart->addSeries(series);
    chart->setTitle("Розподіл боїв за класами");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // Analysis text generation
    QString analysisReport;
    QString bestClass = "Не визначено";
    double maxAvgDmg = 0;
    for (auto it = profile.battlesByVehicleType.constBegin(); it != profile.battlesByVehicleType.constEnd(); ++it) {
        double avgDmg = it.value() > 0 ? profile.damageByVehicleType.value(it.key()) / it.value() : 0;
        if (avgDmg > maxAvgDmg) {
            maxAvgDmg = avgDmg;
            bestClass = it.key();
        }
    }

    analysisReport += "<ul style='margin:0px 0px 8px 16px;'>";
    if (bestClass != "Не визначено") {
        analysisReport += QString("<li><b>Найефективніший клас:</b> %1 (середня шкода %2).</li>")
                              .arg(bestClass)
                              .arg(locale.toString(qRound(maxAvgDmg)));
    } else {
        analysisReport += "<li>Немає достатньо даних для визначення найефективнішого класу.</li>";
    }

    if (profile.winRate() > 55) {
        analysisReport += "<li>🏆 <b>Сильна сторона:</b> Високий відсоток перемог свідчить про розуміння гри та вміння впливати на результат бою.</li>";
    } else if (profile.winRate() < 40 && profile.battleCount > 10) {
        analysisReport += "<li>🔍 <b>Рекомендація:</b> Потрібно проаналізувати позиційні помилки або вибір техніки.</li>";
    }

    double avgBlocked = profile.battleCount > 0 ? profile.totalBlocked / profile.battleCount : 0;
    if (avgBlocked > 1500 && profile.battlesByVehicleType.contains("heavyTank")) {
        analysisReport += "<li>🛡️ <b>Сильна сторона:</b> Впевнене використання броні на важких танках.</li>";
    }
    analysisReport += "</ul>";

    analysisReport += "<b>Рекордні показники:</b><ul style='margin:4px 0px 0px 16px;'>";
    analysisReport += QString("<li>🏅 Максимум шкоди за бій: <b>%1</b></li>").arg(locale.toString(qRound(profile.maxDamage)));
    analysisReport += QString("<li>💥 Максимум знищених: <b>%1</b></li>").arg(qRound(profile.maxKills));
    analysisReport += QString("<li>🤝 Максимум допомоги: <b>%1</b></li>").arg(locale.toString(qRound(profile.maxAssisted)));
    analysisReport += QString("<li>🛡️ Максимум заблоковано: <b>%1</b></li>").arg(locale.toString(qRound(profile.maxBlocked)));
    analysisReport += "</ul>";

    m_analysisText->setHtml(analysisReport);
}
