#ifndef VEHICLE_DATA_H
#define VEHICLE_DATA_H

#include <QMap>
#include <QString>
#include <QStringList>

// Структура для зберігання повної інформації про техніку
struct VehicleInfo {
    int tier;
    QString type; // heavyTank, mediumTank, lightTank, AT-SPG, SPG
    QString name;
};

// Створюємо простір імен, щоб уникнути конфліктів
namespace VehicleDatabase {

// 1. Централізована функція для очищення назви
inline QString cleanNameForLookup(const QString& techName) {
    QString lowerTechName = techName.toLower();

    // Спеціальні випадки, які обробляються неправильно стандартною логікою
    if (lowerTechName.contains("vk7201")) return "VK 72.01 (K)";
    if (lowerTechName.contains("lewandowskiego")) return "60TP Lewandowskiego";
    if (lowerTechName.contains("wz-111_5a")) return "WZ-111 5A";
    if (lowerTechName.contains("t95_fv4201_chieftain")) return "Chieftain T95/FV4201";

    if (techName.contains("_")) {
        QStringList parts = techName.split("_");
        if (parts.size() > 1) {
            // Беремо останню частину, яка зазвичай є назвою
            return parts.last();
        }
    }
    return techName; // Повертаємо як є, якщо формат невідомий
}


// 2. Оновлена база даних з "чистими" назвами як ключами
inline const QMap<QString, VehicleInfo> vehicleMap = {
    // --- USSR ---
    {"Object 277", {10, "heavyTank", "Object 277"}},
    {"IS-7", {10, "heavyTank", "IS-7"}},
    {"ST-II", {10, "heavyTank", "ST-II"}},
    {"Object 705A", {10, "heavyTank", "Object 705A"}},
    {"IS-4", {10, "heavyTank", "IS-4"}},
    {"Object 279 (e)", {10, "heavyTank", "Object 279 (e)"}},
    {"Object 140", {10, "mediumTank", "Object 140"}},
    {"T-62A", {10, "mediumTank", "T-62A"}},
    {"Object 430U", {10, "mediumTank", "Object 430U"}},
    {"K-91", {10, "mediumTank", "K-91"}},
    {"T-100 LT", {10, "lightTank", "T-100 LT"}},
    {"Object 268/4", {10, "AT-SPG", "Object 268/4"}},
    {"Object 268", {10, "AT-SPG", "Object 268"}},
    {"Object 261", {10, "SPG", "Object 261"}},
    {"Object 257", {9, "heavyTank", "Object 257"}},
    {"T-10", {9, "heavyTank", "T-10"}},
    {"Object 705", {9, "heavyTank", "Object 705"}},
    {"Object 430", {9, "mediumTank", "Object 430"}},
    {"IS-3", {8, "heavyTank", "IS-3"}},
    {"Object 252U", {8, "heavyTank", "Object 252U"}},
    {"KV-5", {8, "heavyTank", "KV-5"}},
    {"IS-6", {8, "heavyTank", "IS-6"}},

    // --- Germany ---
    {"VK 72.01 (K)", {10, "heavyTank", "VK 72.01 (K)"}},
    {"E 100", {10, "heavyTank", "E 100"}},
    {"Maus", {10, "heavyTank", "Maus"}},
    {"Pz.Kpfw. VII", {10, "heavyTank", "Pz.Kpfw. VII"}},
    {"E 50 Ausf. M", {10, "mediumTank", "E 50 Ausf. M"}},
    {"Leopard 1", {10, "mediumTank", "Leopard 1"}},
    {"Rheinmetall Panzerwagen", {10, "lightTank", "Rheinmetall Panzerwagen"}},
    {"Grille 15", {10, "AT-SPG", "Grille 15"}},
    {"JagdPz E 100", {10, "AT-SPG", "JagdPz E 100"}},
    {"Mäuschen", {9, "heavyTank", "Mäuschen"}},
    {"E 75", {9, "heavyTank", "E 75"}},
    {"E 50", {9, "mediumTank", "E 50"}},
    {"Löwe", {8, "heavyTank", "Löwe"}},

    // --- USA ---
    {"T110E5", {10, "heavyTank", "T110E5"}},
    {"T57 Heavy", {10, "heavyTank", "T57 Heavy"}},
    {"T832", {10, "heavyTank", "T832"}}, // Ваш танк
    {"M48A5 Patton", {10, "mediumTank", "M48A5 Patton"}},
    {"M60", {10, "mediumTank", "M60"}},
    {"XM551 Sheridan", {10, "lightTank", "XM551 Sheridan"}},
    {"T110E3", {10, "AT-SPG", "T110E3"}},
    {"T110E4", {10, "AT-SPG", "T110E4"}},
    {"M103", {9, "heavyTank", "M103"}},
    {"T32", {8, "heavyTank", "T32"}},
    {"Chrysler K", {8, "heavyTank", "Chrysler K"}},
    {"T34", {8, "heavyTank", "T34"}},

    // --- France ---
    {"AMX 50 B", {10, "heavyTank", "AMX 50 B"}},
    {"AMX M4 mle. 54", {10, "heavyTank", "AMX M4 mle. 54"}},
    {"Bat.-Châtillon 25 t", {10, "mediumTank", "Bat.-Châtillon 25 t"}},
    {"AMX 13 105", {10, "lightTank", "AMX 13 105"}},
    {"AMX 50 Foch B", {10, "AT-SPG", "AMX 50 Foch B"}},
    {"AMX 50 120", {9, "heavyTank", "AMX 50 120"}},
    {"Lorraine 40 t", {8, "mediumTank", "Lorraine 40 t"}},
    {"AMX 50 100", {8, "heavyTank", "AMX 50 100"}},

    // --- UK ---
    {"Super Conqueror", {10, "heavyTank", "Super Conqueror"}},
    {"Chieftain T95/FV4201", {10, "heavyTank", "Chieftain T95/FV4201"}},
    {"Centurion Action X", {10, "mediumTank", "Centurion Action X"}},
    {"Manticore", {10, "lightTank", "Manticore"}},
    {"FV217 Badger", {10, "AT-SPG", "FV217 Badger"}},
    {"FV4005", {10, "AT-SPG", "FV4005 Stage II"}},
    {"Conqueror", {9, "heavyTank", "Conqueror"}},
    {"Caernarvon", {8, "heavyTank", "Caernarvon"}},

    // --- China ---
    {"113", {10, "heavyTank", "113"}},
    {"WZ-111 5A", {10, "heavyTank", "WZ-111 5A"}},
    {"121", {10, "mediumTank", "121"}},
    {"WZ-132-1", {10, "lightTank", "WZ-132-1"}},
    {"WZ-111", {8, "heavyTank", "WZ-111"}},

    // --- Інші нації (додано для прикладу) ---
    {"Type 5 Heavy", {10, "heavyTank", "Type 5 Heavy"}},
    {"STB-1", {10, "mediumTank", "STB-1"}},
    {"TVP T 50/51", {10, "mediumTank", "TVP T 50/51"}},
    {"Kranvagn", {10, "heavyTank", "Kranvagn"}},
    {"60TP Lewandowskiego", {10, "heavyTank", "60TP Lewandowskiego"}},
    {"Progetto 65", {10, "mediumTank", "Progetto 65"}},
    {"Rinoceronte", {10, "heavyTank", "Rinoceronte"}}
};

// 3. Єдина функція для отримання даних за повною технічною назвою
inline VehicleInfo getInfoFromTechName(const QString& techName) {
    QString cleanName = cleanNameForLookup(techName);

    // Пошук за точною відповідністю
    if (vehicleMap.contains(cleanName)) {
        return vehicleMap.value(cleanName);
    }

    // Якщо точної відповідності немає, спробуємо знайти за частковою
    for (auto it = vehicleMap.constBegin(); it != vehicleMap.constEnd(); ++it) {
        if (cleanName.contains(it.key(), Qt::CaseInsensitive) || it.key().contains(cleanName, Qt::CaseInsensitive)) {
            return it.value();
        }
    }

    // Повертаємо "невідомий" об'єкт, якщо танка немає в базі
    return {0, "unknown", cleanName};
}

} // namespace VehicleDatabase

#endif // VEHICLE_DATA_H
