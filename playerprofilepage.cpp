// playerprofilepage.cpp

#include "playerprofilepage.h"
#include "databasemanager.h"
#include "metricscalculator.h"
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
#include <QTabWidget>
#include <QProgressBar>
#include <QLineSeries>
#include <QValueAxis>
#include <QDateTimeAxis>
#include <QDateTime>
#include <algorithm>
#include <cmath>

// Реалізація методів для PlayerProfile
double PlayerProfile::wn8() const {
    if (battleCount == 0) return 0;
    
    double expectedDamage = battleCount * 1000.0;
    double expectedKills = battleCount * 0.7;
    double expectedSpotted = battleCount * 0.5;
    double expectedDefense = battleCount * 0.2;
    
    double actualDamage = totalDamage;
    double actualKills = totalKills;
    double actualSpotted = totalSpotted;
    double actualDefense = totalBlocked / 1000.0;
    
    double damageRatio = expectedDamage > 0 ? actualDamage / expectedDamage : 0;
    double killsRatio = expectedKills > 0 ? actualKills / expectedKills : 0;
    double spottedRatio = expectedSpotted > 0 ? actualSpotted / expectedSpotted : 0;
    double defenseRatio = expectedDefense > 0 ? actualDefense / expectedDefense : 0;
    
    double wn8Value = 0;
    wn8Value += damageRatio * 0.4;
    wn8Value += killsRatio * 0.3;
    wn8Value += spottedRatio * 0.2;
    wn8Value += defenseRatio * 0.1;
    
    wn8Value *= 1000;
    
    double winRateModifier = winRate() / 100.0;
    double survivalModifier = survivalRate() / 100.0;
    
    wn8Value *= (0.7 + 0.3 * winRateModifier);
    wn8Value *= (0.8 + 0.2 * survivalModifier);
    
    return std::max(0.0, wn8Value);
}

double PlayerProfile::efficiency() const {
    if (battleCount == 0) return 0;
    
    double wn8Value = wn8();
    
    double damageScore = avgDamage() / 1000.0;
    double killsScore = avgKills() * 100;
    double spottedScore = avgSpotted() * 50;
    double survivalScore = survivalRate() / 100.0;
    double winRateScore = winRate() / 100.0;
    
    double efficiency = 0;
    efficiency += damageScore * 0.3;
    efficiency += killsScore * 0.2;
    efficiency += spottedScore * 0.15;
    efficiency += survivalScore * 0.2;
    efficiency += winRateScore * 0.15;
    
    efficiency += (wn8Value / 1000.0) * 0.1;
    
    efficiency *= 1000;
    
    return std::max(0.0, efficiency);
}

QString PlayerProfile::skillLevel() const {
    double wn8Value = wn8();
    
    if (wn8Value >= 3000) return "Легенда";
    if (wn8Value >= 2500) return "Унікальний";
    if (wn8Value >= 2000) return "Експерт";
    if (wn8Value >= 1500) return "Досвідчений";
    if (wn8Value >= 1000) return "Середній";
    if (wn8Value >= 500) return "Нижче середнього";
    if (wn8Value >= 300) return "Поганий";
    return "Дуже поганий";
}

PlayerProfilePage::PlayerProfilePage(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent)
    , m_dbManager(dbManager)
{
    m_metricsCalculator = new MetricsCalculator(this);
    setupUI();

    connect(m_dbManager, &DatabaseManager::dataChanged, this, &PlayerProfilePage::recalculateAndDisplay);
    if (m_playerSelector) {
        connect(m_playerSelector, &QComboBox::currentTextChanged, this, &PlayerProfilePage::onPlayerSelected);
    }
    if (m_tabWidget) {
        connect(m_tabWidget, &QTabWidget::currentChanged, this, &PlayerProfilePage::onTabChanged);
    }

    recalculateAndDisplay();
}

void PlayerProfilePage::setupUI() {
    setStyleSheet("QWidget { background-color: #1e1e1e; color: #f0f0f0; font-family: 'Segoe UI', 'Roboto', sans-serif; }");

    auto mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(20);

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

    setupTabs();
    mainLayout->addWidget(m_tabWidget, 1);
}

void PlayerProfilePage::recalculateAndDisplay() {
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
        double experience = metrics.value("experience").toDouble();
        double credits = metrics.value("credits").toDouble();

        profile.totalDamage += damage;
        profile.totalAssisted += assisted;
        profile.totalBlocked += blocked;
        profile.totalKills += kills;
        profile.totalSpotted += spotted;
        profile.totalExperience += experience;
        profile.totalCredits += credits;

        if (metrics.value("deathReason", -1).toInt() == -1) {
            profile.survivedBattles++;
        }

        if (damage > profile.maxDamage) profile.maxDamage = damage;
        if (kills > profile.maxKills) profile.maxKills = kills;
        if (assisted > profile.maxAssisted) profile.maxAssisted = assisted;
        if (blocked > profile.maxBlocked) profile.maxBlocked = blocked;
        if (experience > profile.maxExperience) profile.maxExperience = experience;

        int playerTeam = metrics.value("team", 0).toInt();
        int winnerTeam = replayData.value("common", QVariantMap()).toMap().value("winnerTeam", 0).toInt();
        if (winnerTeam != 0 && playerTeam != 0 && winnerTeam == playerTeam) {
            profile.wins++;
        }

        QString vehicleType = metrics.value("vehicleType").toString();
        if (vehicleType.isEmpty()) vehicleType = "unknown";
        if (vehicleType != "unknown") {
            profile.battlesByVehicleType[vehicleType] += 1;
            profile.damageByVehicleType[vehicleType] += damage;
            profile.experienceByVehicleType[vehicleType] += experience;
            if (winnerTeam == playerTeam) {
                profile.winsByVehicleType[vehicleType] += 1;
            }
        }

        QString mapName = replayData.value("mapName", "Unknown").toString();
        profile.battlesByMap[mapName] += 1;
        if (winnerTeam == playerTeam) {
            profile.winRateByMap[mapName] += 1;
        }

        profile.avgDamageByMap[mapName] += damage;
        profile.avgExperienceByMap[mapName] += experience;

        QString vehicleTier = metrics.value("vehicleTier", "Unknown").toString();
        if (vehicleTier != "Unknown") {
            profile.battlesByTier[vehicleTier] += 1;
            profile.avgDamageByTier[vehicleTier] += damage;
        }

        profile.recentDamage.append(damage);
        profile.recentWinRate.append(winnerTeam == playerTeam ? 1.0 : 0.0);
        profile.battleDates.append(QDateTime::currentDateTime());
        
        if (profile.recentDamage.size() > 30) {
            profile.recentDamage.removeFirst();
            profile.recentWinRate.removeFirst();
            profile.battleDates.removeFirst();
        }
    }

    if (m_playerSelector) {
        QString currentPlayer = m_playerSelector->currentText();
        m_playerSelector->clear();
        m_playerSelector->addItems(m_profiles.keys());

        int currentIndex = m_playerSelector->findText(currentPlayer);
        if (currentIndex != -1) m_playerSelector->setCurrentIndex(currentIndex);
        else if(m_playerSelector->count() > 0) m_playerSelector->setCurrentIndex(0);

        onPlayerSelected(m_playerSelector->currentText());
    }
}

void PlayerProfilePage::onPlayerSelected(const QString &playerName) {
    if (playerName.isEmpty() || !m_profiles.contains(playerName)) {
        if (m_battlesLabel) m_battlesLabel->setText("0");
        if (m_winRateLabel) m_winRateLabel->setText("0%");
        if (m_avgDamageLabel) m_avgDamageLabel->setText("0");
        if (m_wn8Label) m_wn8Label->setText("0");
        if (m_efficiencyLabel) m_efficiencyLabel->setText("0");
        if (m_survivalLabel) m_survivalLabel->setText("0%");
        
        if (m_classStatsTable) m_classStatsTable->setRowCount(0);
        if (m_detailedStatsTable) m_detailedStatsTable->setRowCount(0);
        if (m_chartView && m_chartView->chart())
            m_chartView->chart()->removeAllSeries();
        
        if (m_analysisText) m_analysisText->setHtml("<p>Немає даних для відображення. Завантажте та проаналізуйте реплеї.</p>");
        if (m_recommendationsText) m_recommendationsText->setHtml("<p>Немає даних для рекомендацій.</p>");
        return;
    }

    displayProfile(m_profiles.value(playerName));
}

void PlayerProfilePage::refreshProfile() {
    recalculateAndDisplay();
}

void PlayerProfilePage::displayProfile(const PlayerProfile &profile) {
    updateOverviewTab(profile);
    updateStatsTab(profile);
    updateAnalysisTab(profile);
}

void PlayerProfilePage::setupTabs() {
    m_tabWidget = new QTabWidget(this);
    m_tabWidget->setStyleSheet("QTabWidget::pane { border: 1px solid #444; background-color: #2e2e2e; }"
                               "QTabBar::tab { background-color: #3a3a3a; color: #f0f0f0; padding: 8px 16px; margin-right: 2px; }"
                               "QTabBar::tab:selected { background-color: #55aaff; color: white; }"
                               "QTabBar::tab:hover { background-color: #4a4a4a; }");

    setupOverviewTab();
    setupStatsTab();
    setupAnalysisTab();
}

void PlayerProfilePage::setupOverviewTab() {
    auto overviewTab = new QWidget();
    auto layout = new QVBoxLayout(overviewTab);
    layout->setSpacing(20);

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

        auto cardLayout = new QVBoxLayout(frame);
        cardLayout->setContentsMargins(10, 10, 10, 10);
        cardLayout->setSpacing(4);

        label = new QLabel("0", frame);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("font-size: 28px; font-weight: bold; color: #55aaff;");
        auto titleLabel = new QLabel(title, frame);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 12px; color: #bbbbbb;");
        cardLayout->addWidget(label);
        cardLayout->addWidget(titleLabel);
        return frame;
    };

    gridLayout->addWidget(createStatCard(m_battlesLabel, "Боїв"), 0, 0);
    gridLayout->addWidget(createStatCard(m_winRateLabel, "Перемог %"), 0, 1);
    gridLayout->addWidget(createStatCard(m_avgDamageLabel, "Шкода"), 0, 2);
    gridLayout->addWidget(createStatCard(m_wn8Label, "WN8"), 1, 0);
    gridLayout->addWidget(createStatCard(m_efficiencyLabel, "Ефективність"), 1, 1);
    gridLayout->addWidget(createStatCard(m_survivalLabel, "Виживання %"), 1, 2);
    layout->addLayout(gridLayout);

    auto splitter = new QSplitter(Qt::Horizontal, this);
    splitter->setStyleSheet("QSplitter::handle { background: #444; }");

    m_chartView = new QChartView(this);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    QChart *chart = new QChart();
    chart->setBackgroundBrush(QBrush(QColor("#2e2e2e")));
    chart->legend()->setLabelColor(Qt::white);
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setAnimationOptions(QChart::AllAnimations);
    m_chartView->setChart(chart);
    splitter->addWidget(m_chartView);

    m_classStatsTable = new QTableWidget(this);
    m_classStatsTable->setColumnCount(4);
    m_classStatsTable->setHorizontalHeaderLabels({"Клас", "Боїв", "Перемог", "Шкода"});
    m_classStatsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_classStatsTable->setAlternatingRowColors(true);
    m_classStatsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_classStatsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_classStatsTable->setStyleSheet("QTableWidget { background-color: #2e2e2e; alternate-background-color: #252525; color: white; gridline-color: #444; }"
                                     "QHeaderView::section { background-color: #3a3a3a; color: white; padding: 6px; }"
                                     "QTableWidget::item { padding: 6px; }");
    splitter->addWidget(m_classStatsTable);
    splitter->setSizes({400, 300});
    layout->addWidget(splitter);

    m_tabWidget->addTab(overviewTab, "📊 Огляд");
}

void PlayerProfilePage::setupStatsTab() {
    auto statsTab = new QWidget();
    auto layout = new QVBoxLayout(statsTab);
    layout->setSpacing(20);

    m_detailedStatsTable = new QTableWidget(this);
    m_detailedStatsTable->setColumnCount(6);
    m_detailedStatsTable->setHorizontalHeaderLabels({"Клас", "Боїв", "Перемог", "Шкода", "Досвід", "WN8"});
    m_detailedStatsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_detailedStatsTable->setAlternatingRowColors(true);
    m_detailedStatsTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_detailedStatsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_detailedStatsTable->setStyleSheet("QTableWidget { background-color: #2e2e2e; alternate-background-color: #252525; color: white; gridline-color: #444; }"
                                        "QHeaderView::section { background-color: #3a3a3a; color: white; padding: 6px; }"
                                        "QTableWidget::item { padding: 6px; }");
    layout->addWidget(m_detailedStatsTable);

    m_tabWidget->addTab(statsTab, "📈 Статистика");
}

void PlayerProfilePage::setupAnalysisTab() {
    auto analysisTab = new QWidget();
    auto layout = new QVBoxLayout(analysisTab);
    layout->setSpacing(20);

    auto analysisLabel = new QLabel("<b>Аналітичні висновки:</b>");
    analysisLabel->setStyleSheet("font-size: 16px;");
    layout->addWidget(analysisLabel);

    m_analysisText = new QTextEdit(this);
    m_analysisText->setReadOnly(true);
    m_analysisText->setStyleSheet("QTextEdit { background-color: #2e2e2e; color: #e0e0e0; border: 1px solid #444; border-radius: 8px; padding: 10px; }");
    layout->addWidget(m_analysisText);

    auto recommendationsLabel = new QLabel("<b>Рекомендації для покращення:</b>");
    recommendationsLabel->setStyleSheet("font-size: 16px;");
    layout->addWidget(recommendationsLabel);

    m_recommendationsText = new QTextEdit(this);
    m_recommendationsText->setReadOnly(true);
    m_recommendationsText->setStyleSheet("QTextEdit { background-color: #2e2e2e; color: #e0e0e0; border: 1px solid #444; border-radius: 8px; padding: 10px; }");
    layout->addWidget(m_recommendationsText);

    m_tabWidget->addTab(analysisTab, "🔍 Аналіз");
}

void PlayerProfilePage::onTabChanged(int index) {
    Q_UNUSED(index)
    QString currentPlayer = m_playerSelector->currentText();
    if (!currentPlayer.isEmpty() && m_profiles.contains(currentPlayer)) {
        displayProfile(m_profiles.value(currentPlayer));
    }
}

void PlayerProfilePage::updateOverviewTab(const PlayerProfile &profile) {
    QLocale locale(QLocale::English);
    
    if (m_battlesLabel) m_battlesLabel->setText(QString::number(profile.battleCount));
    if (m_winRateLabel) m_winRateLabel->setText(QString::number(profile.winRate(), 'f', 1) + "%");
    if (m_avgDamageLabel) m_avgDamageLabel->setText(locale.toString(qRound(profile.avgDamage())));
    if (m_wn8Label) m_wn8Label->setText(QString::number(qRound(profile.wn8())));
    if (m_efficiencyLabel) m_efficiencyLabel->setText(QString::number(qRound(profile.efficiency())));
    if (m_survivalLabel) m_survivalLabel->setText(QString::number(profile.survivalRate(), 'f', 1) + "%");

    if (m_chartView && m_chartView->chart()) {
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
    }

    if (m_classStatsTable) {
        m_classStatsTable->setRowCount(0);
        for (auto it = profile.battlesByVehicleType.constBegin(); it != profile.battlesByVehicleType.constEnd(); ++it) {
            int row = m_classStatsTable->rowCount();
            m_classStatsTable->insertRow(row);
            
            int wins = profile.winsByVehicleType.value(it.key(), 0);
            double avgDmg = it.value() > 0 ? profile.damageByVehicleType.value(it.key()) / it.value() : 0;

            auto itemClass = new QTableWidgetItem(it.key());
            auto itemBattles = new QTableWidgetItem(QString::number(it.value()));
            auto itemWins = new QTableWidgetItem(QString::number(wins));
            auto itemDamage = new QTableWidgetItem(locale.toString(qRound(avgDmg)));

            itemBattles->setTextAlignment(Qt::AlignCenter);
            itemWins->setTextAlignment(Qt::AlignCenter);
            itemDamage->setTextAlignment(Qt::AlignCenter);

            m_classStatsTable->setItem(row, 0, itemClass);
            m_classStatsTable->setItem(row, 1, itemBattles);
            m_classStatsTable->setItem(row, 2, itemWins);
            m_classStatsTable->setItem(row, 3, itemDamage);
        }
    }
}

void PlayerProfilePage::updateStatsTab(const PlayerProfile &profile) {
    QLocale locale(QLocale::English);
    
    if (m_detailedStatsTable) {
        m_detailedStatsTable->setRowCount(0);
        for (auto it = profile.battlesByVehicleType.constBegin(); it != profile.battlesByVehicleType.constEnd(); ++it) {
            int row = m_detailedStatsTable->rowCount();
            m_detailedStatsTable->insertRow(row);
            
            int wins = profile.winsByVehicleType.value(it.key(), 0);
            double avgDmg = it.value() > 0 ? profile.damageByVehicleType.value(it.key()) / it.value() : 0;
            double avgExp = it.value() > 0 ? profile.experienceByVehicleType.value(it.key()) / it.value() : 0;
            double wn8 = profile.wn8();

            auto itemClass = new QTableWidgetItem(it.key());
            auto itemBattles = new QTableWidgetItem(QString::number(it.value()));
            auto itemWins = new QTableWidgetItem(QString::number(wins));
            auto itemDamage = new QTableWidgetItem(locale.toString(qRound(avgDmg)));
            auto itemExp = new QTableWidgetItem(locale.toString(qRound(avgExp)));
            auto itemWn8 = new QTableWidgetItem(QString::number(qRound(wn8)));

            itemBattles->setTextAlignment(Qt::AlignCenter);
            itemWins->setTextAlignment(Qt::AlignCenter);
            itemDamage->setTextAlignment(Qt::AlignCenter);
            itemExp->setTextAlignment(Qt::AlignCenter);
            itemWn8->setTextAlignment(Qt::AlignCenter);

            m_detailedStatsTable->setItem(row, 0, itemClass);
            m_detailedStatsTable->setItem(row, 1, itemBattles);
            m_detailedStatsTable->setItem(row, 2, itemWins);
            m_detailedStatsTable->setItem(row, 3, itemDamage);
            m_detailedStatsTable->setItem(row, 4, itemExp);
            m_detailedStatsTable->setItem(row, 5, itemWn8);
        }
    }
}

void PlayerProfilePage::updateAnalysisTab(const PlayerProfile &profile) {
    QLocale locale(QLocale::English);
    
    QString analysisReport;
    analysisReport += "<h2 style='margin:0 0 8px 0; font-size:16px; color:#9fc4ff;'>Аналітичні висновки</h2>";
    analysisReport += "<ul style='margin:0 0 8px 18px; line-height: 1.65;'>";

    QString bestClass = "Не визначено";
    QString worstClass = "Не визначено";
    double maxAvgDmg = 0;
    double minAvgDmg = 999999;
    
    for (auto it = profile.battlesByVehicleType.constBegin(); it != profile.battlesByVehicleType.constEnd(); ++it) {
        double avgDmg = it.value() > 0 ? profile.damageByVehicleType.value(it.key()) / it.value() : 0;
        if (avgDmg > maxAvgDmg) {
            maxAvgDmg = avgDmg;
            bestClass = it.key();
        }
        if (avgDmg < minAvgDmg && it.value() >= 3) {
            minAvgDmg = avgDmg;
            worstClass = it.key();
        }
    }

    if (bestClass != "Не визначено") {
        analysisReport += QString("<li><b>Найефективніший клас:</b> %1 (середня шкода %2).</li>")
                              .arg(bestClass, locale.toString(qRound(maxAvgDmg)));
    }

    if (worstClass != "Не визначено" && profile.battlesByVehicleType.value(worstClass) >= 3) {
        analysisReport += QString("<li><b>Клас для покращення:</b> %1 (середня шкода %2).</li>")
                              .arg(worstClass, locale.toString(qRound(minAvgDmg)));
    }

    if (profile.winRate() > 55) {
        analysisReport += "<li>🏆 <b>Сильна сторона:</b> Високий відсоток перемог свідчить про розуміння гри.</li>";
    } else if (profile.winRate() < 45 && profile.battleCount > 10) {
        analysisReport += "<li>🔍 <b>Зона росту:</b> Варто проаналізувати причини поразок для покращення результатів.</li>";
    }

    if (profile.survivalRate() > 40) {
        analysisReport += "<li>💪 <b>Сильна сторона:</b> Високий показник виживання говорить про обережну та ефективну гру.</li>";
    }

    if (profile.wn8() > 1500) {
        analysisReport += "<li>⭐ <b>Високий рівень:</b> WN8 рейтинг показує досвідчену гру.</li>";
    } else if (profile.wn8() < 800 && profile.battleCount > 10) {
        analysisReport += "<li>📈 <b>Потенціал для росту:</b> Є можливості для значного покращення показників.</li>";
    }

    QString bestTier = "Не визначено";
    QString worstTier = "Не визначено";
    double maxTierDmg = 0;
    double minTierDmg = 999999;
    
    for (auto it = profile.battlesByTier.constBegin(); it != profile.battlesByTier.constEnd(); ++it) {
        if (it.value() >= 3) {
            double avgTierDmg = profile.avgDamageByTier.value(it.key()) / it.value();
            if (avgTierDmg > maxTierDmg) {
                maxTierDmg = avgTierDmg;
                bestTier = it.key();
            }
            if (avgTierDmg < minTierDmg) {
                minTierDmg = avgTierDmg;
                worstTier = it.key();
            }
        }
    }

    if (bestTier != "Не визначено" && profile.battlesByTier.value(bestTier) >= 3) {
        analysisReport += QString("<li><b>Найкращий рівень:</b> %1 (середня шкода %2).</li>")
                              .arg(bestTier, locale.toString(qRound(maxTierDmg)));
    }

    if (worstTier != "Не визначено" && profile.battlesByTier.value(worstTier) >= 3) {
        analysisReport += QString("<li><b>Рівень для покращення:</b> %1 (середня шкода %2).</li>")
                              .arg(worstTier, locale.toString(qRound(minTierDmg)));
    }

    // Спеціальний аналіз для танків 11-го рівня
    if (profile.battlesByTier.contains("11") && profile.battlesByTier.value("11") > 0) {
        int tier11Battles = profile.battlesByTier.value("11");
        double tier11Damage = profile.avgDamageByTier.value("11", 0) / tier11Battles;
        analysisReport += QString("<li>🚀 <b>Танки 11-го рівня:</b> %1 боїв, середня шкода %2 - використовуйте нові можливості!</li>")
                              .arg(tier11Battles).arg(locale.toString(qRound(tier11Damage)));
    }

    analysisReport += "</ul>";

    QString recommendations;
    recommendations += "<h2 style='margin:0 0 8px 0; font-size:16px; color:#9fc4ff;'>Рекомендації для покращення</h2>";
    recommendations += "<ul style='margin:0 0 8px 18px; line-height: 1.65;'>";
    
    if (profile.avgDamage() < 1000) {
        recommendations += "<li>🎯 <b>Покращте точність:</b> Працюйте над покращенням середньої шкоди за бій.</li>";
    }
    
    if (profile.survivalRate() < 30) {
        recommendations += "<li>🛡️ <b>Більше обережності:</b> Намагайтеся виживати в більшій кількості боїв.</li>";
    }
    
    if (profile.winRate() < 50) {
        recommendations += "<li>🤝 <b>Командна гра:</b> Більше взаємодії з командою для покращення результатів.</li>";
    }

    if (profile.avgKills() < 0.5) {
        recommendations += "<li>⚔️ <b>Агресивність:</b> Більше активності в знищенні ворогів.</li>";
    }

    if (profile.avgSpotted() < 0.3) {
        recommendations += "<li>👁️ <b>Розвідка:</b> Покращте підсвічування ворогів для команди.</li>";
    }

    if (profile.avgAssisted() < 200) {
        recommendations += "<li>🤝 <b>Підтримка:</b> Більше уваги приділяйте підтримці союзників.</li>";
    }

    if (worstClass != "Не визначено" && profile.battlesByVehicleType.value(worstClass) >= 3) {
        recommendations += QString("<li>🔧 <b>Практика з %1:</b> Більше часу приділіть цьому класу техніки.</li>").arg(worstClass);
    }

    if (worstTier != "Не визначено" && profile.battlesByTier.value(worstTier) >= 3) {
        recommendations += QString("<li>📈 <b>Покращення на рівні %1:</b> Працюйте над тактикою для цього рівня техніки.</li>").arg(worstTier);
    }

    QString bestMap = "Не визначено";
    QString worstMap = "Не визначено";
    double maxMapWinRate = 0;
    double minMapWinRate = 100;
    
    for (auto it = profile.battlesByMap.constBegin(); it != profile.battlesByMap.constEnd(); ++it) {
        if (it.value() >= 3) {
            double mapWinRate = profile.winRateByMap.value(it.key(), 0) * 100.0 / it.value();
            if (mapWinRate > maxMapWinRate) {
                maxMapWinRate = mapWinRate;
                bestMap = it.key();
            }
            if (mapWinRate < minMapWinRate) {
                minMapWinRate = mapWinRate;
                worstMap = it.key();
            }
        }
    }

    if (bestMap != "Не визначено" && profile.battlesByMap.value(bestMap) >= 3) {
        recommendations += QString("<li>🗺️ <b>Сильна карта:</b> %1 (перемоги: %2%) - використовуйте цю перевагу.</li>")
                              .arg(bestMap).arg(maxMapWinRate, 0, 'f', 1);
    }

    if (worstMap != "Не визначено" && profile.battlesByMap.value(worstMap) >= 3) {
        recommendations += QString("<li>🗺️ <b>Карта для покращення:</b> %1 (перемоги: %2%) - вивчіть тактику для цієї карти.</li>")
                              .arg(worstMap).arg(minMapWinRate, 0, 'f', 1);
    }

    QString bestMapDamage = "Не визначено";
    QString worstMapDamage = "Не визначено";
    double maxMapDamage = 0;
    double minMapDamage = 999999;
    
    for (auto it = profile.battlesByMap.constBegin(); it != profile.battlesByMap.constEnd(); ++it) {
        if (it.value() >= 3) {
            double avgMapDamage = profile.avgDamageByMap.value(it.key()) / it.value();
            if (avgMapDamage > maxMapDamage) {
                maxMapDamage = avgMapDamage;
                bestMapDamage = it.key();
            }
            if (avgMapDamage < minMapDamage) {
                minMapDamage = avgMapDamage;
                worstMapDamage = it.key();
            }
        }
    }

    if (bestMapDamage != "Не визначено" && profile.battlesByMap.value(bestMapDamage) >= 3) {
        recommendations += QString("<li>💥 <b>Висока ефективність на карті:</b> %1 (середня шкода: %2) - розвивайте цю перевагу.</li>")
                              .arg(bestMapDamage, locale.toString(qRound(maxMapDamage)));
    }

    if (worstMapDamage != "Не визначено" && profile.battlesByMap.value(worstMapDamage) >= 3) {
        recommendations += QString("<li>🎯 <b>Покращення на карті:</b> %1 (середня шкода: %2) - працюйте над позиціонуванням.</li>")
                              .arg(worstMapDamage, locale.toString(qRound(minMapDamage)));
    }

    if (profile.battleCount < 50) {
        recommendations += "<li>📊 <b>Більше даних:</b> Зіграйте більше боїв для точнішого аналізу.</li>";
    }

    if (profile.battleCount >= 50) {
        double recentWinRate = 0;
        if (profile.recentWinRate.size() >= 10) {
            for (int i = profile.recentWinRate.size() - 10; i < profile.recentWinRate.size(); ++i) {
                recentWinRate += profile.recentWinRate[i];
            }
            recentWinRate = recentWinRate / 10.0 * 100.0;
            
            if (recentWinRate > profile.winRate() + 10) {
                recommendations += "<li>📈 <b>Позитивний тренд:</b> Останні 10 боїв показують покращення - продовжуйте в тому ж напрямку!</li>";
            } else if (recentWinRate < profile.winRate() - 10) {
                recommendations += "<li>⚠️ <b>Негативний тренд:</b> Останні бої гірші за загальну статистику - перегляньте підхід.</li>";
            }
        }
    }

    if (profile.maxDamage > profile.avgDamage() * 2) {
        recommendations += "<li>💪 <b>Потенціал:</b> Ваш рекорд шкоди показує можливості для покращення середніх показників.</li>";
    }

    if (profile.avgExperience() < 500) {
        recommendations += "<li>⭐ <b>Досвід:</b> Працюйте над отриманням більшого досвіду за бій.</li>";
    }

    if (profile.avgCredits() < 20000) {
        recommendations += "<li>💰 <b>Економіка:</b> Покращте ефективність гри для отримання більшої кількості кредитів.</li>";
    }

    // Спеціальні рекомендації для танків 11-го рівня
    bool hasTier11Vehicles = false;
    for (auto it = profile.battlesByTier.constBegin(); it != profile.battlesByTier.constEnd(); ++it) {
        if (it.key() == "11") {
            hasTier11Vehicles = true;
            break;
        }
    }

    if (hasTier11Vehicles) {
        recommendations += "<li>🚀 <b>Танки 11-го рівня:</b> Використовуйте модульну систему покращень для максимальної ефективності.</li>";
        recommendations += "<li>⭐ <b>Статус 'Елітний':</b> Розблокуйте всі вузли покращень для отримання ексклюзивних елементів зовнішності.</li>";
        recommendations += "<li>🎯 <b>Особливі механіки:</b> Вивчіть унікальні можливості кожного танка 11-го рівня.</li>";
        
        // Спеціальні рекомендації для конкретних танків 11-го рівня
        QStringList tier11Vehicles = {"КР-1", "AMX 67 Imbattable", "FV4025 Contriver", "Taschenratte", "T803", "BZ-79", "Black Rock",
                                     "Объект 432У", "Leopard 120 Verbessert", "CS 67 Szakal", "AS-XX 40 t", "XM69 Hacker",
                                     "Hirschkäfer", "Strv 107-12", "AT-FV230 Breaker", "LeKpz Borkenkäfer"};
        
        for (const QString& vehicle : tier11Vehicles) {
            if (profile.battlesByVehicleType.contains(vehicle) && profile.battlesByVehicleType.value(vehicle) > 0) {
                if (vehicle == "Taschenratte") {
                    recommendations += "<li>🛡️ <b>Taschenratte:</b> Використовуйте систему додаткового озброєння для контролю позицій.</li>";
                } else if (vehicle == "Black Rock") {
                    recommendations += "<li>🏆 <b>Black Rock:</b> Нагорода за кампанію - використовуйте її переваги!</li>";
                } else if (vehicle.contains("Объект")) {
                    recommendations += "<li>⚡ <b>Объект 432У:</b> Розвивайте мобільність та точність стрільби.</li>";
                } else if (vehicle.contains("Leopard")) {
                    recommendations += "<li>🎯 <b>Leopard 120:</b> Покращуйте точність та далекобійність.</li>";
                }
            }
        }
    }

    recommendations += "</ul>";

    if (m_analysisText) m_analysisText->setHtml(analysisReport);
    if (m_recommendationsText) m_recommendationsText->setHtml(recommendations);
}