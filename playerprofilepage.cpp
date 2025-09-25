#include "playerprofilepage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLocale>

PlayerProfilePage::PlayerProfilePage(DatabaseManager *dbManager, QWidget *parent)
    : QWidget(parent), m_dbManager(dbManager)
{
    m_metricsCalculator = new MetricsCalculator(this);
    setupUI();

    // Підключаємось до сигналу від бази даних
    connect(m_dbManager, &DatabaseManager::dataChanged, this, &PlayerProfilePage::recalculateAndDisplay);
    // Підключаємось до вибору гравця в ComboBox
    connect(m_playerSelector, &QComboBox::currentTextChanged, this, &PlayerProfilePage::onPlayerSelected);

    // Первинний розрахунок при створенні сторінки
    recalculateAndDisplay();
}

void PlayerProfilePage::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QHBoxLayout *selectorLayout = new QHBoxLayout();
    selectorLayout->addWidget(new QLabel("Профіль гравця:"));
    m_playerSelector = new QComboBox(this);
    selectorLayout->addWidget(m_playerSelector);
    selectorLayout->addStretch();
    mainLayout->addLayout(selectorLayout);

    m_profileDisplay = new QTextEdit(this);
    m_profileDisplay->setReadOnly(true);
    mainLayout->addWidget(m_profileDisplay);
}

void PlayerProfilePage::recalculateAndDisplay()
{
    m_profiles.clear();
    QVariantList replays = m_dbManager->getReplays();

    for (const QVariant& replayVar : replays) {
        QVariantMap replayData = replayVar.toMap().value("analysisData").toMap();
        if (replayData.isEmpty()) continue;

        QString playerName = replayData.value("playerName").toString();
        if (playerName.isEmpty()) continue;

        PlayerProfile& profile = m_profiles[playerName];
        profile.playerName = playerName;
        profile.battleCount++;

        QVariantMap metrics = m_metricsCalculator->calculate(replayData);
        profile.totalDamage += metrics.value("totalDamageDealt").toDouble();
        profile.totalAssisted += metrics.value("damageAssisted").toDouble();
        profile.totalBlocked += metrics.value("damageBlockedByArmor").toDouble();
        profile.totalKills += metrics.value("kills").toDouble();

        QString vehicleType = metrics.value("vehicleType").toString();
        if (vehicleType != "unknown") {
            profile.battlesByVehicleType[vehicleType]++;
            profile.damageByVehicleType[vehicleType] += metrics.value("totalDamageDealt").toDouble();
        }
    }

    // Оновлюємо список гравців у ComboBox
    QString currentPlayer = m_playerSelector->currentText();
    m_playerSelector->clear();
    m_playerSelector->addItems(m_profiles.keys());

    int currentIndex = m_playerSelector->findText(currentPlayer);
    if (currentIndex != -1) {
        m_playerSelector->setCurrentIndex(currentIndex);
    }

    // Відображаємо профіль поточного або першого гравця
    onPlayerSelected(m_playerSelector->currentText());
}

void PlayerProfilePage::onPlayerSelected(const QString &playerName)
{
    if (playerName.isEmpty()) {
        m_profileDisplay->setHtml("<p>Немає даних для відображення. Завантажте та проаналізуйте реплеї.</p>");
        return;
    }

    if (m_profiles.contains(playerName)) {
        generateProfileReport(m_profiles.value(playerName));
    }
}

void PlayerProfilePage::refreshProfile()
{
    // Цей слот можна викликати, щоб примусово оновити дані, наприклад, при переході на вкладку
    recalculateAndDisplay();
}

void PlayerProfilePage::generateProfileReport(const PlayerProfile &profile)
{
    QLocale locale(QLocale::English);
    QString report;
    report += QString("<h1>Карта танкіста: %1</h1>").arg(profile.playerName);

    // --- ЗАГАЛЬНА СТАТИСТИКА ---
    report += "<h2>Загальна статистика</h2>";
    report += QString("<p><b>Проаналізовано боїв:</b> %1</p>").arg(profile.battleCount);
    report += QString("<p><b>Середня шкода за бій:</b> %1</p>").arg(locale.toString(qRound(profile.avgDamage())));
    double avgAssisted = profile.battleCount > 0 ? profile.totalAssisted / profile.battleCount : 0;
    double avgBlocked = profile.battleCount > 0 ? profile.totalBlocked / profile.battleCount : 0;
    double avgKills = profile.battleCount > 0 ? profile.totalKills / profile.battleCount : 0;
    report += QString("<p><b>Середня допомога за бій:</b> %1</p>").arg(locale.toString(qRound(avgAssisted)));
    report += QString("<p><b>Середній заблокований збиток:</b> %1</p>").arg(locale.toString(qRound(avgBlocked)));
    report += QString("<p><b>Середня кількість знищених:</b> %1</p>").arg(QString::number(avgKills, 'f', 2));

    // --- СТАТИСТИКА ЗА ТИПАМИ ТЕХНІКИ ---
    report += "<h2>Ефективність за класами техніки</h2>";
    report += "<table border='1' style='width:100%; border-collapse: collapse;'><tr><th>Клас</th><th>Боїв</th><th>Середня шкода</th></tr>";
    for(auto it = profile.battlesByVehicleType.constBegin(); it != profile.battlesByVehicleType.constEnd(); ++it) {
        QString type = it.key();
        int battles = it.value();
        double avgDmg = (battles > 0) ? profile.damageByVehicleType.value(type) / battles : 0;
        report += QString("<tr><td style='padding: 5px;'>%1</td><td style='padding: 5px;'>%2</td><td style='padding: 5px;'>%3</td></tr>")
                      .arg(type).arg(battles).arg(locale.toString(qRound(avgDmg)));
    }
    report += "</table>";

    // --- СИЛЬНІ ТА СЛАБКІ СТОРОНИ ---
    report += "<h2>Аналіз стилю гри</h2><ul>";
    if (avgBlocked > 1500 && profile.battlesByVehicleType.contains("heavyTank")) {
        report += "<li><b>Сильна сторона:</b> Ви впевнено використовуєте броню на важких танках.</li>";
    }
    if (avgAssisted > 1500 && (profile.battlesByVehicleType.contains("lightTank") || profile.battlesByVehicleType.contains("mediumTank"))) {
        report += "<li><b>Сильна сторона:</b> Ефективна гра від розвідки та підтримки союзників.</li>";
    }
    if (profile.avgDamage() > 2500) {
        report += "<li><b>Сильна сторона:</b> Висока реалізація шкоди.</li>";
    }

    if (avgAssisted < 500 && profile.battlesByVehicleType.value("lightTank", 0) > 3) {
        report += "<li><b>Точка росту:</b> На легких танках варто приділяти більше уваги розвідці.</li>";
    }
    if (avgBlocked < 500 && profile.battlesByVehicleType.value("heavyTank", 0) > 3) {
        report += "<li><b>Точка росту:</b> Броня важких танків використовується недостатньо. Спробуйте грати агресивніше на першій лінії.</li>";
    }
    report += "</ul>";

    m_profileDisplay->setHtml(report);
}
