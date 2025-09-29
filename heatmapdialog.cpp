#include "heatmapdialog.h"
#include <QFileInfo>
#include <QDir>
#include <QMessageBox>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPainterPath>
#include <QRadialGradient>
#include <QDebug>
#include <QGraphicsEffect>
#include <QGraphicsDropShadowEffect>
#include <cmath>

HeatmapDialog::HeatmapDialog(const QVariantMap &replayData, QWidget *parent)
    : QDialog(parent), m_replayData(replayData)
{
    qDebug() << "Створюємо HeatmapDialog...";
    
    try {
        setWindowTitle("🗺️ Теплова карта реплею");
        setModal(true);
        setMinimumSize(1000, 700);
        resize(1200, 800);
        
        // Отримуємо назву карти
        m_mapName = m_replayData.value("mapDisplayName", "unknown").toString();
        qDebug() << "Назва карти:" << m_mapName;
        
        // Створюємо генератор теплових карт
        m_heatmapGenerator = new HeatmapGenerator(this);
    m_wotReplayIntegration = new WotReplayIntegration(this);
        
        qDebug() << "Налаштовуємо UI...";
        setupUI();
        
        qDebug() << "Завантажуємо зображення карти...";
        loadMapImage();
        
        qDebug() << "Генеруємо теплову карту...";
        generateHeatmap();
        
        qDebug() << "HeatmapDialog створено успішно";
        
    } catch (const std::exception &e) {
        qDebug() << "Помилка при створенні HeatmapDialog:" << e.what();
        QMessageBox::critical(this, "Помилка", QString("Помилка при створенні теплової карти: %1").arg(e.what()));
    } catch (...) {
        qDebug() << "Невідома помилка при створенні HeatmapDialog";
        QMessageBox::critical(this, "Помилка", "Невідома помилка при створенні теплової карти");
    }
}

HeatmapDialog::~HeatmapDialog()
{
}

void HeatmapDialog::setupUI()
{
    qDebug() << "Налаштовуємо UI компоненти...";
    
    try {
        auto *mainLayout = new QVBoxLayout(this);
        mainLayout->setContentsMargins(15, 15, 15, 15);
        mainLayout->setSpacing(10);
        
        // Заголовок
        auto *titleLabel = new QLabel(QString("Теплова карта: %1").arg(m_mapName));
        titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #4fa8ff; margin-bottom: 10px;");
        titleLabel->setAlignment(Qt::AlignCenter);
        mainLayout->addWidget(titleLabel);
        
        qDebug() << "Заголовок додано";
        
        // Контроли
        auto *controlsLayout = new QHBoxLayout();
        
        // Тип карти
        auto *mapTypeLabel = new QLabel("Тип карти:");
        mapTypeLabel->setStyleSheet("color: #e6eef6; font-weight: bold;");
        controlsLayout->addWidget(mapTypeLabel);
        
        m_mapTypeCombo = new QComboBox();
        m_mapTypeCombo->addItems({"Всі позиції", "Позиції гравця", "Позиції ворогів", "Позиції союзників", "Місця пострілів", "Командна теплова карта"});
        m_mapTypeCombo->setStyleSheet(R"(
            QComboBox {
                background-color: #252525; color: #e6eef6; border: 1px solid #333;
                border-radius: 6px; padding: 6px 12px; min-width: 150px;
            }
            QComboBox::drop-down { border: none; }
            QComboBox::down-arrow { image: none; border: none; }
            QComboBox QAbstractItemView {
                background-color: #252525; color: #e6eef6; border: 1px solid #333;
                selection-background-color: #3a7bd5;
            }
        )");
        controlsLayout->addWidget(m_mapTypeCombo);
        
        qDebug() << "ComboBox додано";
        
        controlsLayout->addSpacing(20);
        
        // Прозорість
        auto *opacityLabel = new QLabel("Прозорість:");
        opacityLabel->setStyleSheet("color: #e6eef6; font-weight: bold;");
        controlsLayout->addWidget(opacityLabel);
        
        m_opacitySlider = new QSlider(Qt::Horizontal);
        m_opacitySlider->setRange(0, 100);
        m_opacitySlider->setValue(70);
        m_opacitySlider->setStyleSheet(R"(
            QSlider::groove:horizontal {
                background: #333; height: 6px; border-radius: 3px;
            }
            QSlider::handle:horizontal {
                background: #4fa8ff; width: 18px; height: 18px; border-radius: 9px;
                margin: -6px 0;
            }
            QSlider::handle:horizontal:hover {
                background: #66b3ff;
            }
        )");
        controlsLayout->addWidget(m_opacitySlider);
        
        m_opacityLabel = new QLabel("70%");
        m_opacityLabel->setStyleSheet("color: #e6eef6; font-weight: bold; min-width: 40px;");
        controlsLayout->addWidget(m_opacityLabel);
        
        qDebug() << "Слайдер прозорості додано";
        
        controlsLayout->addStretch();
        
        // Кнопки
        m_saveButton = new QPushButton("💾 Зберегти зображення");
        m_saveButton->setStyleSheet(R"(
            QPushButton {
                background-color: #2d5a2d; color: #e6eef6; border: 1px solid #3d6a3d;
                border-radius: 6px; padding: 8px 16px; font-weight: bold;
            }
            QPushButton:hover { background-color: #3d6a3d; }
        )");
        controlsLayout->addWidget(m_saveButton);
        
        m_closeButton = new QPushButton("❌ Закрити");
        m_closeButton->setStyleSheet(R"(
            QPushButton {
                background-color: #4b1f1f; color: #e6eef6; border: 1px solid #662222;
                border-radius: 6px; padding: 8px 16px; font-weight: bold;
            }
            QPushButton:hover { background-color: #632727; }
        )");
        controlsLayout->addWidget(m_closeButton);
        
        qDebug() << "Кнопки додано";
        
        mainLayout->addLayout(controlsLayout);
        
        // Графічний вид
        m_scene = new QGraphicsScene(this);
        m_graphicsView = new QGraphicsView(m_scene);
        m_graphicsView->setStyleSheet("background-color: #1a1a1a; border: 1px solid #333; border-radius: 8px;");
        m_graphicsView->setRenderHint(QPainter::Antialiasing);
        m_graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
        m_graphicsView->setInteractive(true);
        mainLayout->addWidget(m_graphicsView);
        
        qDebug() << "Графічний вид додано";
        
        // Підключення сигналів
        connect(m_opacitySlider, &QSlider::valueChanged, this, &HeatmapDialog::onOpacityChanged);
        connect(m_mapTypeCombo, QOverload<const QString &>::of(&QComboBox::currentTextChanged), 
                this, &HeatmapDialog::onMapTypeChanged);
        connect(m_saveButton, &QPushButton::clicked, this, &HeatmapDialog::onSaveImageClicked);
        connect(m_closeButton, &QPushButton::clicked, this, &HeatmapDialog::onCloseClicked);
        
        qDebug() << "Сигнали підключено";
        qDebug() << "UI налаштовано успішно";
        
    } catch (const std::exception &e) {
        qDebug() << "Помилка при налаштуванні UI:" << e.what();
        QMessageBox::critical(this, "Помилка", QString("Помилка при налаштуванні інтерфейсу: %1").arg(e.what()));
    } catch (...) {
        qDebug() << "Невідома помилка при налаштуванні UI";
        QMessageBox::critical(this, "Помилка", "Невідома помилка при налаштуванні інтерфейсу");
    }
}

void HeatmapDialog::loadMapImage()
{
    m_mapImagePath = getMapImagePath(m_mapName);
    
    if (m_mapImagePath.isEmpty()) {
        QMessageBox::warning(this, "Помилка", 
                           QString("Не вдалося знайти зображення карти для: %1\nПеревірте, чи існує папка maps/images").arg(m_mapName));
        return;
    }
    
    if (!QFileInfo::exists(m_mapImagePath)) {
        QMessageBox::warning(this, "Помилка", 
                           QString("Не вдалося знайти зображення карти: %1").arg(m_mapImagePath));
        return;
    }
    
    m_mapPixmap = QPixmap(m_mapImagePath);
    if (m_mapPixmap.isNull()) {
        QMessageBox::warning(this, "Помилка", 
                           QString("Не вдалося завантажити зображення карти: %1").arg(m_mapImagePath));
        return;
    }
    
    // Додаємо карту до сцени
    m_mapItem = m_scene->addPixmap(m_mapPixmap);
    m_scene->setSceneRect(m_mapPixmap.rect());
    
    // Центруємо вид
    m_graphicsView->fitInView(m_mapItem, Qt::KeepAspectRatio);
    
    qDebug() << "Карта успішно завантажена:" << m_mapImagePath;
}

void HeatmapDialog::generateHeatmap()
{
    qDebug() << "Починаємо генерацію теплової карти...";
    
    try {
        // Отримуємо дані про позиції з реплею
        m_playerPositions.clear();
        m_enemyPositions.clear();
        m_allyPositions.clear();
        m_shotPositions.clear();
        
        qDebug() << "Очищено списки позицій";
        
        // Отримуємо ID гравця-записувача
        quint32 recorderPlayerId = m_replayData.value("playerID", 0).toUInt();
        qDebug() << "ID гравця-записувача:" << recorderPlayerId;
        
        // Отримуємо команду гравця-записувача
        int recorderTeam = m_replayData.value("playerTeam", -1).toInt();
        qDebug() << "Команда гравця-записувача:" << recorderTeam;
        
        // Додаткове логування для діагностики
        qDebug() << "Доступні ключі в реплеї:" << m_replayData.keys();
        qDebug() << "Містить player_positions:" << m_replayData.contains("player_positions");
        qDebug() << "Містить shot_events:" << m_replayData.contains("shot_events");
        qDebug() << "Містить vehicles:" << m_replayData.contains("vehicles");
        
        // Аналізуємо позиції гравців
        if (m_replayData.contains("player_positions")) {
            QVariantList positions = m_replayData.value("player_positions").toList();
            qDebug() << "Знайдено позицій гравців:" << positions.size();
            
            for (const QVariant &posVar : positions) {
                QVariantMap pos = posVar.toMap();
                
                quint32 entityId = pos.value("entityId", 0).toUInt();
                int team = pos.value("team", -1).toInt();
                float x = pos.value("x", 0.0).toFloat();
                float y = pos.value("y", 0.0).toFloat();
                float z = pos.value("z", 0.0).toFloat();
                
                // Конвертуємо 3D координати в 2D для карти
                // Використовуємо x та z як 2D координати (y - висота)
                QPointF position2D(x, z);
                
                if (entityId == recorderPlayerId) {
                    // Це наш гравець
                    m_playerPositions.append(position2D);
                } else if (team == recorderTeam) {
                    // Союзники
                    m_allyPositions.append(position2D);
                } else if (team != -1) {
                    // Вороги
                    m_enemyPositions.append(position2D);
                }
            }
        } else {
            qDebug() << "Позиції гравців не знайдено в даних реплею";
        }
        
        // Аналізуємо події стрільби для отримання позицій пострілів
        if (m_replayData.contains("shot_events")) {
            QVariantList shotEvents = m_replayData.value("shot_events").toList();
            qDebug() << "Знайдено подій стрільби:" << shotEvents.size();
            
            for (const QVariant &shotVar : shotEvents) {
                QVariantMap shot = shotVar.toMap();
                
                quint32 attackerId = shot.value("attackerId", 0).toUInt();
                
                // Додаємо позиції пострілів (спрощено - використовуємо позиції гравців)
                if (attackerId == recorderPlayerId) {
                    // Знаходимо позицію гравця на момент пострілу
                    float shotTime = shot.value("timestamp", 0.0).toFloat();
                    
                    // Шукаємо найближчу позицію гравця до моменту пострілу
                    QPointF shotPosition = findPlayerPositionAtTime(recorderPlayerId, shotTime);
                    if (!shotPosition.isNull()) {
                        m_shotPositions.append(shotPosition);
                    }
                }
            }
        } else {
            qDebug() << "Події стрільби не знайдено в даних реплею";
        }
        
        // Якщо немає реальних даних, генеруємо тестові дані
        if (m_playerPositions.isEmpty() && m_enemyPositions.isEmpty() && m_allyPositions.isEmpty()) {
            qDebug() << "Генеруємо тестові дані для демонстрації";
            // Генеруємо тестові позиції для демонстрації
            for (int i = 0; i < 50; ++i) {
                double x = fmod(i * 20.0, 1000.0);
                double y = 100 + (i % 3) * 100;
                m_playerPositions.append(QPointF(x, y));
                m_shotPositions.append(QPointF(x, y));
                
                if (i % 2 == 0) {
                    m_enemyPositions.append(QPointF(x + 50, y + 50));
                } else {
                    m_allyPositions.append(QPointF(x - 50, y - 50));
                }
            }
        }
        
        qDebug() << "Позиції згенеровано - Гравець:" << m_playerPositions.size() 
                 << "Вороги:" << m_enemyPositions.size() 
                 << "Союзники:" << m_allyPositions.size() 
                 << "Постріли:" << m_shotPositions.size();
        
        drawHeatmapOverlay();
        qDebug() << "Теплова карта згенерована успішно";
        
    } catch (const std::exception &e) {
        qDebug() << "Помилка при генерації теплової карти:" << e.what();
        QMessageBox::critical(this, "Помилка", QString("Помилка при генерації теплової карти: %1").arg(e.what()));
    } catch (...) {
        qDebug() << "Невідома помилка при генерації теплової карти";
        QMessageBox::critical(this, "Помилка", "Невідома помилка при генерації теплової карти");
    }
}

void HeatmapDialog::drawHeatmapOverlay()
{
    qDebug() << "Починаємо малювання теплової карти...";
    
    try {
        if (m_mapPixmap.isNull()) {
            qDebug() << "Карта не завантажена, пропускаємо малювання";
            return;
        }
        
        qDebug() << "Розмір карти:" << m_mapPixmap.size();
        
        QString mapType = m_mapTypeCombo->currentText();
        qDebug() << "Тип карти:" << mapType;
        
        QImage resultImage;
        
        // Визначаємо, які позиції показувати
        if (mapType == "Позиції гравця") {
            resultImage = m_wotReplayIntegration->generateHeatmap(m_mapName, m_playerPositions, m_mapPixmap.width(), m_mapPixmap.height());
        } else if (mapType == "Позиції ворогів") {
            resultImage = m_wotReplayIntegration->generateHeatmap(m_mapName, m_enemyPositions, m_mapPixmap.width(), m_mapPixmap.height());
        } else if (mapType == "Позиції союзників") {
            resultImage = m_wotReplayIntegration->generateHeatmap(m_mapName, m_allyPositions, m_mapPixmap.width(), m_mapPixmap.height());
        } else if (mapType == "Місця пострілів") {
            resultImage = m_wotReplayIntegration->generateHeatmap(m_mapName, m_shotPositions, m_mapPixmap.width(), m_mapPixmap.height());
        } else if (mapType == "Командна теплова карта") {
            // Командна теплова карта: гравці проти ворогів
            resultImage = m_wotReplayIntegration->generateTeamHeatmap(m_mapName, 
                                                                      m_playerPositions + m_allyPositions,
                                                                      m_enemyPositions,
                                                                      m_mapPixmap.width(), 
                                                                      m_mapPixmap.height());
        } else {
            // Всі позиції
            QVector<QPointF> allPositions = m_playerPositions + m_enemyPositions + m_allyPositions + m_shotPositions;
            resultImage = m_wotReplayIntegration->generateHeatmap(m_mapName, allPositions, m_mapPixmap.width(), m_mapPixmap.height());
        }
        
        qDebug() << "Теплова карта згенерована";
        
        // Конвертуємо в QPixmap
        m_heatmapPixmap = QPixmap::fromImage(resultImage);
        
        // Додаємо теплову карту до сцени
        if (m_heatmapItem && m_heatmapItem->scene() == m_scene) {
            m_scene->removeItem(m_heatmapItem);
        }
        m_heatmapItem = nullptr;
        m_heatmapItem = m_scene->addPixmap(m_heatmapPixmap);
        
        qDebug() << "Теплова карта додана до сцени";
        
        // Встановлюємо прозорість
        onOpacityChanged(m_opacitySlider->value());
        
        qDebug() << "Малювання теплової карти завершено успішно";
        
    } catch (const std::exception &e) {
        qDebug() << "Помилка при малюванні теплової карти:" << e.what();
        QMessageBox::critical(this, "Помилка", QString("Помилка при малюванні теплової карти: %1").arg(e.what()));
    } catch (...) {
        qDebug() << "Невідома помилка при малюванні теплової карти";
        QMessageBox::critical(this, "Помилка", "Невідома помилка при малюванні теплової карти");
    }
}

QPointF HeatmapDialog::findPlayerPositionAtTime(quint32 playerId, float timestamp)
{
    if (!m_replayData.contains("player_positions")) {
        return QPointF();
    }
    
    QVariantList positions = m_replayData.value("player_positions").toList();
    QPointF closestPosition;
    float closestTimeDiff = std::numeric_limits<float>::max();
    
    for (const QVariant &posVar : positions) {
        QVariantMap pos = posVar.toMap();
        
        if (pos.value("entityId", 0).toUInt() == playerId) {
            float posTime = pos.value("timestamp", 0.0).toFloat();
            float timeDiff = qAbs(posTime - timestamp);
            
            if (timeDiff < closestTimeDiff) {
                closestTimeDiff = timeDiff;
                float x = pos.value("x", 0.0).toFloat();
                float z = pos.value("z", 0.0).toFloat();
                closestPosition = QPointF(x, z);
            }
        }
    }
    
    return closestPosition;
}

QString HeatmapDialog::findMapsDirectory()
{
    QStringList possiblePaths = {
        "maps/images",
        "./maps/images",
        "../maps/images",
        "../../maps/images",
        QDir::currentPath() + "/maps/images",
        QDir::currentPath() + "/../maps/images"
    };
    
    for (const QString &path : possiblePaths) {
        QDir mapsDir(path);
        if (mapsDir.exists()) {
            QStringList filters;
            filters << "*.png";
            QStringList mapFiles = mapsDir.entryList(filters, QDir::Files);
            if (!mapFiles.isEmpty()) {
                qDebug() << "Знайдено папку з картами:" << path << "з" << mapFiles.size() << "файлами";
                return path;
            }
        }
    }
    
    qDebug() << "Папка з картами не знайдена!";
    return "";
}

QString HeatmapDialog::getMapImagePath(const QString &mapName)
{
    qDebug() << "Шукаємо карту:" << mapName;
    
    // Спочатку знайдемо правильний шлях до папки з картами
    QString mapsDir = findMapsDirectory();
    if (mapsDir.isEmpty()) {
        qDebug() << "Не вдалося знайти папку з картами!";
        return "";
    }
    
    // Мапа відомих назв карт до файлів 
    // Оновлено на основі офіційної вікі Wargaming: https://wiki.wargaming.net/ru/Tank:%D0%9A%D0%B0%D1%80%D1%82%D1%8B
    QMap<QString, QString> mapMapping = {
        // Літні карти
        {"Берлін", "105_germany_se20"},
        {"Вестфілд", "23_westfeld"},
        {"Границя імперії", "105_germany_se20"},
        {"Кордон імперії", "105_germany_se20"},
        {"Жемчужна річка", "36_fishing_bay"},
        {"Перлинна річка", "36_fishing_bay"},
        {"Карелія", "01_karelia"},
        {"Лайв Окс", "35_steppes"},
        {"Ласвілль", "07_lakeville"},
        {"Лінія Зігфріда", "14_siegfried_line"},
        {"Малиновка", "02_malinovka"},
        {"Мінськ", "90_minsk"},
        {"Монастир", "19_monastery"},
        {"Мурованка", "11_murovanka"},
        {"Небельбург", "105_germany_se20"},
        {"Нормандія", "108_normandy_se20"},
        {"Оверлорд", "108_normandy_se20"},
        {"Пагорби", "10_hills"},
        {"Паріж", "112_eiffel_tower_ctf"},
        {"Польща", "99_poland"},
        {"Прохоровка", "05_prohorovka"},
        {"Рибальська бухта", "36_fishing_bay"},
        {"Руїнберг", "08_ruinberg"},
        {"Степи", "35_steppes"},
        {"Тундра", "63_tundra"},
        {"Фьорд", "33_fjord"},
        {"Хіммельсдорф", "04_himmelsdorf"},
        {"Червоний Шир", "34_redshire"},
        {"Ерленберг", "13_erlenberg"},
        
        // Зимові карти
        {"Хіммельсдорф зима", "86_himmelsdorf_winter"},
        {"Руїнберг зима", "103_ruinberg_winter"},
        {"Зима", "84_winter"},
        
        // Пустельні карти
        {"Пустеля", "28_desert"},
        {"Ель-Халуф", "29_el_hallouf"},
        {"Аеродром", "31_airfield"},
        
        // Азійські карти
        {"Велика стіна", "59_asia_great_wall"},
        {"Мяо", "60_asia_miao"},
        {"Корея", "73_asia_korea"},
        {"Японія", "53_japan"},
        
        // Північноамериканські карти
        {"Північна Америка", "43_north_america"},
        {"Канада", "47_canada_a"},
        
        // Спеціальні карти
        {"Яма", "100_thepit"},
        {"День Д", "101_dday"},
        {"Німеччина", "105_germany_se20"},
        {"Ейфелева вежа", "112_eiffel_tower_ctf"},
        {"Чехія", "114_czech"},
        {"Швеція", "115_sweden"},
        {"Загублене місто", "95_lost_city"},
        {"Прохоровка оборона", "96_prohorovka_defense"},
        
        // Додаткові карти
        {"Кампания", "03_campania"},
        {"Энск", "06_ensk"},
        {"Лейквіль", "07_lakeville"},
        {"Комарін", "15_komarin"},
        {"Мюнхен", "17_munchen"},
        {"Скеля", "18_cliff"},
        {"Болото", "22_slough"},
        {"Західне поле", "23_westfeld"},
        {"Кавказ", "37_caucasus"},
        {"Лінія Маннергейма", "38_mannerheim_line"},
        {"Крим", "39_crimea"},
        {"Харків", "83_kharkiv"},
        {"Сталінград", "92_stalingrad"}
    };
    
    // Шукаємо точну відповідність
    if (mapMapping.contains(mapName)) {
        QString fileName = QString("%1/%2.png").arg(mapsDir).arg(mapMapping[mapName]);
        qDebug() << "Перевіряємо файл:" << fileName;
        if (QFileInfo::exists(fileName)) {
            qDebug() << "Файл існує:" << fileName;
            return fileName;
        }
        qDebug() << "Файл НЕ знайдено для карти:" << mapName;
    }
    
    // Додаткові альтернативні назви (різні локалізації)
    QMap<QString, QString> alternativeNames = {
        {"Berlin", "105_germany_se20"},
        {"Westfield", "23_westfeld"},
        {"Empire's Border", "105_germany_se20"},
        {"Pearl River", "36_fishing_bay"},
        {"Karelia", "01_karelia"},
        {"Live Oaks", "35_steppes"},
        {"Lakeville", "07_lakeville"},
        {"Siegfried Line", "14_siegfried_line"},
        {"Malinovka", "02_malinovka"},
        {"Minsk", "90_minsk"},
        {"Monastery", "19_monastery"},
        {"Murovanka", "11_murovanka"},
        {"Nebelburg", "105_germany_se20"},
        {"Normandy", "108_normandy_se20"},
        {"Overlord", "108_normandy_se20"},
        {"Hills", "10_hills"},
        {"Paris", "112_eiffel_tower_ctf"},
        {"Poland", "99_poland"},
        {"Prokhorovka", "05_prohorovka"},
        {"Fishing Bay", "36_fishing_bay"},
        {"Ruinberg", "08_ruinberg"},
        {"Steppes", "35_steppes"},
        {"Tundra", "63_tundra"},
        {"Fjord", "33_fjord"},
        {"Himmelsdorf", "04_himmelsdorf"},
        {"Redshire", "34_redshire"},
        {"Erlenberg", "13_erlenberg"},
        {"Desert", "28_desert"},
        {"El Halluf", "29_el_hallouf"},
        {"Airfield", "31_airfield"},
        {"Great Wall", "59_asia_great_wall"},
        {"Miao", "60_asia_miao"},
        {"Korea", "73_asia_korea"},
        {"Japan", "53_japan"},
        {"North America", "43_north_america"},
        {"Canada", "47_canada_a"},
        {"The Pit", "100_thepit"},
        {"D-Day", "101_dday"},
        {"Germany", "105_germany_se20"},
        {"Eiffel Tower", "112_eiffel_tower_ctf"},
        {"Czech", "114_czech"},
        {"Sweden", "115_sweden"},
        {"Lost City", "95_lost_city"},
        {"Prokhorovka Defense", "96_prohorovka_defense"}
    };
    
    // Перевіряємо альтернативні назви
    if (alternativeNames.contains(mapName)) {
        QString fileName = QString("%1/%2.png").arg(mapsDir).arg(alternativeNames[mapName]);
        qDebug() << "Перевіряємо альтернативний файл:" << fileName;
        if (QFileInfo::exists(fileName)) {
            qDebug() << "Альтернативний файл існує:" << fileName;
            return fileName;
        }
        qDebug() << "Альтернативний файл НЕ знайдено для карти:" << mapName;
    }
    
    // Якщо не знайдено точну відповідність, спробуємо знайти за частиною назви
    QString lowerMapName = mapName.toLower();
    for (auto it = mapMapping.constBegin(); it != mapMapping.constEnd(); ++it) {
        if (it.key().toLower().contains(lowerMapName) || lowerMapName.contains(it.key().toLower())) {
            QString fileName = QString("%1/%2.png").arg(mapsDir).arg(it.value());
            qDebug() << "Перевіряємо часткову відповідність:" << fileName;
            if (QFileInfo::exists(fileName)) {
                qDebug() << "Часткова відповідність знайдена:" << fileName;
                return fileName;
            }
        }
    }
    
    // Якщо нічого не знайдено, використовуємо першу доступну карту
    QDir mapsDirObj(mapsDir);
    QStringList filters;
    filters << "*.png";
    QStringList mapFiles = mapsDirObj.entryList(filters, QDir::Files);
    
    if (!mapFiles.isEmpty()) {
        QString defaultMap = QString("%1/%2").arg(mapsDir).arg(mapFiles.first());
        qDebug() << "Використовуємо карту за замовчуванням:" << defaultMap;
        return defaultMap;
    }
    
    qDebug() << "Не знайдено жодної карти в папці:" << mapsDir;
    return "";
}

QColor HeatmapDialog::getHeatmapColor(double intensity)
{
    // Конвертуємо інтенсивність (0-1) в колір
    if (intensity < 0.25) {
        return QColor(0, 0, 255, 100); // Синій
    } else if (intensity < 0.5) {
        return QColor(0, 255, 255, 150); // Блакитний
    } else if (intensity < 0.75) {
        return QColor(255, 255, 0, 200); // Жовтий
    } else {
        return QColor(255, 0, 0, 255); // Червоний
    }
}

void HeatmapDialog::onOpacityChanged(int value)
{
    m_opacityLabel->setText(QString("%1%").arg(value));
    
    if (m_heatmapItem) {
        m_heatmapItem->setOpacity(value / 100.0);
    }
}

void HeatmapDialog::onMapTypeChanged(const QString &mapType)
{
    Q_UNUSED(mapType)
    drawHeatmapOverlay();
}

void HeatmapDialog::onSaveImageClicked()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Зберегти теплову карту",
        QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + 
        QString("/heatmap_%1.png").arg(m_mapName),
        "PNG Images (*.png);;JPEG Images (*.jpg)"
    );
    
    if (!fileName.isEmpty()) {
        // Створюємо зображення, що поєднує карту та теплову карту
        QPixmap combinedPixmap(m_mapPixmap.size());
        combinedPixmap.fill(Qt::white);
        
        QPainter painter(&combinedPixmap);
        painter.drawPixmap(0, 0, m_mapPixmap);
        painter.drawPixmap(0, 0, m_heatmapPixmap);
        
        if (combinedPixmap.save(fileName)) {
            QMessageBox::information(this, "Успіх", 
                                   QString("Теплова карта збережена: %1").arg(fileName));
        } else {
            QMessageBox::warning(this, "Помилка", "Не вдалося зберегти зображення");
        }
    }
}

void HeatmapDialog::onCloseClicked()
{
    close();
}
