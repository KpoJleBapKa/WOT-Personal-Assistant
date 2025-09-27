// playerprofilepage.cpp

#include "playerprofilepage.h"
#include <QApplication>
#include <QChart>
#include <QChartView>
#include <QComboBox>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLocale>
#include <QPainter>
#include <QPieSeries>
#include <QPieSlice>
#include <QSplitter>
#include <QStyle>
#include <QTableWidget>
#include <QTextEdit>
#include <QVBoxLayout>

PlayerProfilePage::PlayerProfilePage(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent)
    , m_dbManager(dbManager)
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
    setStyleSheet("QWidget { background-color: #1e1e1e; color: #f0f0f0; font-family: 'Segoe UI', 'Roboto', sans-serif; }");

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
    m_playerSelector->setStyleSheet("QComboBox { background-color: #2e2e2e; color: #f0f0f0; padding: 6px 12px; border-radius: 6px; }"
                                    "QComboBox::drop-down { border: none; }"
                                    "QComboBox QAbstractItemView { background-color: #2e2e2e; selection-background-color: #55aaff; }");
    selectorLayout->addWidget(m_playerSelector);
    selectorLayout->addStretch();
    mainLayout->addLayout(selectorLayout);

    // --- KPI Cards ---
    auto gridLayout = new QGridLayout();
    gridLayout->setSpacing(20);

    auto createStatCard = [&](QLabel *&label, const QString &title) {
        auto frame = new QFrame(this);
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setStyleSheet("QFrame { background: qlineargradient(spread:pad, x1:0, y1:0, x2:1, y2:1, stop:0 #2b2b2b, stop:1 #3f3f3f); border-radius: 12px; padding: 12px; }"
                             "QFrame:hover { background-color: #4a4a4a; }");

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
    m_classStatsTable->setStyleSheet("QTableWidget { background-color: #2e2e2e; alternate-background-color: #252525; color: white; gridline-color: #444; }"
                                     "QHeaderView::section { background-color: #3a3a3a; color: white; padding: 6px; }"
                                     "QTableWidget::item { padding: 6px; }");

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
    m_analysisText->setStyleSheet("QTextEdit { background-color: #2e2e2e; color: #e0e0e0; border: 1px solid #444; border-radius: 8px; padding: 10px; }");
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
        double spotted = metrics.value("spotted").toDouble();

        profile.totalDamage += damage;
        profile.totalAssisted += assisted;
        profile.totalBlocked += blocked;
        profile.totalKills += kills;
        profile.totalSpotted += spotted;

        if (metrics.value("deathReason", -1).toInt() == -1) {
            profile.survivedBattles++;
        }

        if (damage > profile.maxDamage) profile.maxDamage = damage;
        if (kills > profile.maxKills) profile.maxKills = kills;
        if (assisted > profile.maxAssisted) profile.maxAssisted = assisted;
        if (blocked > profile.maxBlocked) profile.maxBlocked = blocked;

        // --- ВИПРАВЛЕНО: Логіка визначення перемоги ---
        // 1. Беремо команду гравця безпосередньо з надійного джерела - розрахованих метрик.
        int playerTeam = metrics.value("team", 0).toInt();

        // 2. Беремо команду-переможця з загальних даних бою.
        int winnerTeam = replayData.value("common", QVariantMap()).toMap().value("winnerTeam", 0).toInt();

        // 3. Порівнюємо. Цей блок тепер працюватиме коректно.
        if (winnerTeam != 0 && playerTeam != 0 && winnerTeam == playerTeam) {
            profile.wins++;
        }
        // --- КІНЕЦЬ ВИПРАВЛЕННЯ ---

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
    else if(m_playerSelector->count() > 0) m_playerSelector->setCurrentIndex(0);

    onPlayerSelected(m_playerSelector->currentText());
}

void PlayerProfilePage::onPlayerSelected(const QString &playerName)
{
    if (playerName.isEmpty() || !m_profiles.contains(playerName)) {
        m_battlesLabel->setText("0");
        m_winRateLabel->setText("0%");
        m_avgDamageLabel->setText("0");
        m_classStatsTable->setRowCount(0);
        if (m_chartView->chart())
            m_chartView->chart()->removeAllSeries();
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

        itemBattles->setTextAlignment(Qt::AlignCenter);
        itemAvg->setTextAlignment(Qt::AlignCenter);

        m_classStatsTable->setItem(row, 0, itemClass);
        m_classStatsTable->setItem(row, 1, itemBattles);
        m_classStatsTable->setItem(row, 2, itemAvg);
    }

    // Pie chart
    QChart *chart = m_chartView->chart();
    chart->removeAllSeries();

    QPieSeries *series = new QPieSeries();
    series->setLabelsVisible(true);
    series->setPieSize(0.7);

    for (auto it = profile.battlesByVehicleType.constBegin(); it != profile.battlesByVehicleType.constEnd(); ++it) {
        series->append(it.key(), it.value());
    }

    QStringList colors = {"#55aaff", "#ffaa55", "#55ffaa", "#ff5555", "#aa55ff", "#ffd155", "#55d6ff"};
    int i = 0;
    for (QPieSlice *slice : series->slices()) {
        slice->setBrush(QColor(colors[i % colors.size()]));
        slice->setLabelColor(Qt::white);
        slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);
        slice->setPen(QPen(QColor(40, 40, 40)));
        i++;
    }

    chart->addSeries(series);
    chart->setTitle("Розподіл боїв за класами");
    chart->setTitleBrush(QBrush(Qt::white));
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);

    // --- УНІФІКОВАНО ВИГЛЯД ТА РОЗШИРЕНО АНАЛІТИКУ ---
    QString analysisReport;
    analysisReport += "<h2 style='margin:0 0 8px 0; font-size:16px; color:#9fc4ff;'>Аналітичні висновки</h2>";
    analysisReport += "<ul style='margin:0 0 8px 18px; line-height: 1.65;'>";

    QString bestClass = "Не визначено";
    double maxAvgDmg = 0;
    for (auto it = profile.battlesByVehicleType.constBegin(); it != profile.battlesByVehicleType.constEnd(); ++it) {
        double avgDmg = it.value() > 0 ? profile.damageByVehicleType.value(it.key()) / it.value() : 0;
        if (avgDmg > maxAvgDmg) {
            maxAvgDmg = avgDmg;
            bestClass = it.key();
        }
    }

    if (bestClass != "Не визначено") {
        analysisReport += QString("<li><b>Найефективніший клас:</b> %1 (середня шкода %2).</li>")
                              .arg(bestClass, locale.toString(qRound(maxAvgDmg)));
    }

    if (profile.winRate() > 55) {
        analysisReport += "<li>🏆 <b>Сильна сторона:</b> Високий відсоток перемог свідчить про розуміння гри.</li>";
    } else if (profile.winRate() < 45 && profile.battleCount > 10) {
        analysisReport += "<li>🔍 <b>Зона росту:</b> Варто проаналізувати причини поразок для покращення результатів.</li>";
    }

    if (profile.survivalRate() > 40) {
        analysisReport += "<li>💪 <b>Сильна сторона:</b> Високий показник виживання говорить про обережну та ефективну гру.</li>";
    }

    analysisReport += "</ul>";

    // Блок рекордних показників в уніфікованому стилі
    analysisReport += "<h2 style='margin:14px 0 8px 0; font-size:16px; color:#9fc4ff;'>Рекордні показники</h2>";
    analysisReport += "<ul style='margin:0 0 8px 18px; line-height: 1.65;'>";
    analysisReport += QString("<li>🏅 <b>Максимум шкоди за бій:</b> %1</li>").arg(locale.toString(qRound(profile.maxDamage)));
    analysisReport += QString("<li>💥 <b>Максимум знищених:</b> %1</li>").arg(qRound(profile.maxKills));
    analysisReport += QString("<li>🤝 <b>Максимум допомоги:</b> %1</li>").arg(locale.toString(qRound(profile.maxAssisted)));
    analysisReport += QString("<li>🛡️ <b>Максимум заблоковано:</b> %1</li>").arg(locale.toString(qRound(profile.maxBlocked)));
    analysisReport += "</ul>";

    m_analysisText->setHtml(analysisReport);
}
