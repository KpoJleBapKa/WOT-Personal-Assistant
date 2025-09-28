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
    //if (lowerTechName.contains("t95_fv4201_chieftain")) return "Chieftain T95/FV4201";

    if (techName.contains("_")) {
        QStringList parts = techName.split("_");
        if (parts.size() > 1) {
            // Беремо останню частину, яка зазвичай є назвою
            return parts.last();
        }
    }
    return techName; // Повертаємо як є, якщо формат невідомий
}


inline const QMap<QString, VehicleInfo> vehicleMap = {
    // ======================================================================
    // === USSR (РОЗШИРЕНО) ==================================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"Object 277", {10, "heavyTank", "Object 277"}},
    {"IS-7", {10, "heavyTank", "IS-7"}},
    {"ST-II", {10, "heavyTank", "ST-II"}},
    {"Object 705A", {10, "heavyTank", "Object 705A"}},
    {"IS-4", {10, "heavyTank", "IS-4"}},
    {"Object 260", {10, "heavyTank", "Object 260"}},
    {"Object 279 (e)", {10, "heavyTank", "Object 279 (e)"}},
    {"Object 780", {10, "heavyTank", "Object 780"}},
    {"Object 257", {9, "heavyTank", "Object 257"}},
    {"T-10", {9, "heavyTank", "T-10"}},
    {"Object 705", {9, "heavyTank", "Object 705"}},
    {"ST-I", {9, "heavyTank", "ST-I"}},
    {"IS-3-II", {9, "heavyTank", "IS-3-II"}},
    {"Object 777", {9, "heavyTank", "Object 777 Version II"}},
    {"IS-3", {8, "heavyTank", "IS-3"}},
    {"Object 252U", {8, "heavyTank", "Object 252U Defender"}},
    {"KV-5", {8, "heavyTank", "KV-5"}},
    {"IS-6", {8, "heavyTank", "IS-6"}},
    {"IS-5", {8, "heavyTank", "IS-5 (Object 730)"}},
    {"KV-4", {8, "heavyTank", "KV-4"}},
    {"IS-2-II", {8, "heavyTank", "IS-2-II"}},
    {"Object 703 II", {8, "heavyTank", "Object 703 Version II"}},
    {"IS", {7, "heavyTank", "IS"}},
    {"KV-3", {7, "heavyTank", "KV-3"}},
    {"IS-2", {7, "heavyTank", "IS-2"}},
    {"KV-85", {6, "heavyTank", "KV-85"}},
    {"KV-2", {6, "heavyTank", "KV-2"}},
    {"T-150", {6, "heavyTank", "T-150"}},
    {"KV-1S", {5, "heavyTank", "KV-1S"}},
    {"KV-1", {5, "heavyTank", "KV-1"}},

    // --- Medium Tanks ---
    {"Object 140", {10, "mediumTank", "Object 140"}},
    {"T-62A", {10, "mediumTank", "T-62A"}},
    {"Object 430U", {10, "mediumTank", "Object 430U"}},
    {"K-91", {10, "mediumTank", "K-91"}},
    {"Object 907", {10, "mediumTank", "Object 907"}},
    {"T-22 medium", {10, "mediumTank", "T-22 medium"}},
    {"Object 430", {9, "mediumTank", "Object 430"}},
    {"T-54", {9, "mediumTank", "T-54"}},
    {"T-44", {8, "mediumTank", "T-44"}},
    {"Object 416", {8, "mediumTank", "Object 416"}},
    {"T-44-100", {8, "mediumTank", "T-44-100"}},
    {"T-43", {7, "mediumTank", "T-43"}},
    {"KV-13", {7, "mediumTank", "KV-13"}},
    {"T-34-85", {6, "mediumTank", "T-34-85"}},
    {"T-34-85M", {6, "mediumTank", "T-34-85M"}},
    {"T-34", {5, "mediumTank", "T-34"}},

    // --- Light Tanks ---
    {"T-100 LT", {10, "lightTank", "T-100 LT"}},
    {"T-54 ltwt.", {9, "lightTank", "T-54 ltwt."}},
    {"LTTB", {8, "lightTank", "LTTB"}},
    {"LT-432", {8, "lightTank", "LT-432"}},
    {"LTG", {7, "lightTank", "LTG"}},
    {"MT-25", {6, "lightTank", "MT-25"}},

    // --- AT-SPG ---
    {"Object 268/4", {10, "AT-SPG", "Object 268/4"}},
    {"Object 268", {10, "AT-SPG", "Object 268"}},
    {"Object 704", {9, "AT-SPG", "Object 704"}},
    {"SU-122-54", {9, "AT-SPG", "SU-122-54"}},
    {"ISU-152", {8, "AT-SPG", "ISU-152"}},
    {"ISU-152K", {8, "AT-SPG", "ISU-152K"}},
    {"SU-101", {8, "AT-SPG", "SU-101"}},
    {"SU-152", {7, "AT-SPG", "SU-152"}},
    {"SU-100", {6, "AT-SPG", "SU-100"}},
    {"SU-85", {5, "AT-SPG", "SU-85"}},

    // --- SPG ---
    {"Object 261", {10, "SPG", "Object 261"}},
    {"212A", {9, "SPG", "212A"}},
    {"SU-14-2", {8, "SPG", "SU-14-2"}},

    // ======================================================================
    // === Germany (РОЗШИРЕНО) =================================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"Maus", {10, "heavyTank", "Maus"}},
    {"E 100", {10, "heavyTank", "E 100"}},
    {"Pz.Kpfw. VII", {10, "heavyTank", "Pz.Kpfw. VII"}},
    {"VK 72.01 (K)", {10, "heavyTank", "VK 72.01 (K)"}},
    {"Mäuschen", {9, "heavyTank", "Mäuschen"}},
    {"E 75", {9, "heavyTank", "E 75"}},
    {"VK 45.02 (P) Ausf. B", {9, "heavyTank", "VK 45.02 B"}},
    {"Typ 205", {9, "heavyTank", "Typ 205"}},
    {"Tiger II", {8, "heavyTank", "Tiger II"}},
    {"VK 100.01 (P)", {8, "heavyTank", "VK 100.01 P"}},
    {"Löwe", {8, "heavyTank", "Löwe"}},
    {"E 75 TS", {8, "heavyTank", "E 75 TS"}},
    {"VK 168.01 (P)", {8, "heavyTank", "VK 168.01 (P)"}},
    {"Tiger I", {7, "heavyTank", "Tiger I"}},
    {"Tiger (P)", {7, "heavyTank", "Tiger (P)"}},
    {"VK 45.03", {7, "heavyTank", "VK 45.03"}},
    {"VK 36.01 (H)", {6, "heavyTank", "VK 36.01 (H)"}},

    // --- Medium Tanks ---
    {"E 50 Ausf. M", {10, "mediumTank", "E 50 Ausf. M"}},
    {"Leopard 1", {10, "mediumTank", "Leopard 1"}},
    {"Kunze Panzer", {10, "mediumTank", "Kunze Panzer"}},
    {"Kampfpanzer 50 t", {9, "mediumTank", "Kpz 50 t"}},
    {"E 50", {9, "mediumTank", "E 50"}},
    {"Leopard Prototyp A", {9, "mediumTank", "Leopard PT A"}},
    {"Indien-Panzer", {8, "mediumTank", "Indien-Panzer"}},
    {"Panther II", {8, "mediumTank", "Panther II"}},
    {"Panther mit 8,8 cm", {8, "mediumTank", "Panther 8,8"}},
    {"Pz. 58 Mutz", {8, "mediumTank", "Pz. 58 Mutz"}},
    {"Panther", {7, "mediumTank", "Panther"}},
    {"VK 30.02 (D)", {7, "mediumTank", "VK 30.02 (D)"}},
    {"Pz.Kpfw. IV Ausf. H", {5, "mediumTank", "Pz. IV H"}},

    // --- Light Tanks ---
    {"Rheinmetall Panzerwagen", {10, "lightTank", "Rheinmetall Panzerwagen"}},
    {"RU 251", {9, "lightTank", "Spähpanzer Ru 251"}},
    {"HWK 12", {8, "lightTank", "HWK 12"}},
    {"leKpz M 41 90 mm", {8, "lightTank", "leKpz M 41 90 mm"}},
    {"SP I C", {7, "lightTank", "Spähpanzer SP I C"}},
    {"VK 28.01", {6, "lightTank", "VK 28.01"}},
    {"Pz.Kpfw. II Luchs", {4, "lightTank", "Luchs"}},

    // --- Tank Destroyers (AT-SPG) ---
    {"Grille 15", {10, "AT-SPG", "Grille 15"}},
    {"JagdPz E 100", {10, "AT-SPG", "Jagdpanzer E 100"}},
    {"Waffenträger auf E 100", {10, "AT-SPG", "Waffenträger auf E 100"}},
    {"Jagdtiger", {9, "AT-SPG", "Jagdtiger"}},
    {"Waffenträger auf Pz. IV", {9, "AT-SPG", "Waffenträger auf Pz. IV"}},
    {"Rhm.-Borsig Waffenträger", {8, "AT-SPG", "Rhm.-Borsig"}},
    {"Ferdinand", {8, "AT-SPG", "Ferdinand"}},
    {"Jagdpanther II", {8, "AT-SPG", "Jagdpanther II"}},
    {"Skorpion G", {8, "AT-SPG", "Skorpion G"}},
    {"Jagdpanther", {7, "AT-SPG", "Jagdpanther"}},
    {"Sturer Emil", {7, "AT-SPG", "Sturer Emil"}},
    {"Nashorn", {6, "AT-SPG", "Nashorn"}},
    {"StuG III Ausf. G", {5, "AT-SPG", "StuG III G"}},
    {"Marder 38T", {4, "AT-SPG", "Marder 38T"}},

    // --- Artillery (SPG) ---
    {"G.W. E 100", {10, "SPG", "G.W. E 100"}},
    {"G.W. Tiger", {9, "SPG", "G.W. Tiger"}},
    {"G.W. Tiger (P)", {8, "SPG", "G.W. Tiger (P)"}},
    {"Hummel", {6, "SPG", "Hummel"}},
    {"Grille", {5, "SPG", "Grille"}},

    // ======================================================================
    // === USA (РОЗШИРЕНО) ===================================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"T110E5", {10, "heavyTank", "T110E5"}},
    {"T57 Heavy", {10, "heavyTank", "T57 Heavy Tank"}},
    {"M-V-Y", {10, "heavyTank", "M-V-Y"}},
    {"Concept 1B", {10, "heavyTank", "Concept 1B"}},
    {"M103", {9, "heavyTank", "M103"}},
    {"AE Phase I", {9, "heavyTank", "AE Phase I"}},
    {"T54E1", {9, "mediumTank", "T54E1"}}, // Класифікується як СТ, але веде до ТТ
    {"T32", {8, "heavyTank", "T32"}},
    {"T26E5", {8, "heavyTank", "T26E5 Patriot"}},
    {"Chrysler K", {8, "heavyTank", "Chrysler K"}},
    {"T832", {8, "heavyTank", "T832"}},
    {"T34", {8, "heavyTank", "T34"}},
    {"T77", {8, "heavyTank", "T77"}},
    {"T29", {7, "heavyTank", "T29"}},
    {"M-II-Y", {7, "heavyTank", "M-II-Y"}},
    {"M6", {6, "heavyTank", "M6"}},
    {"T1 Heavy", {5, "heavyTank", "T1 Heavy Tank"}},

    // --- Medium Tanks ---
    {"M48A5 Patton", {10, "mediumTank", "M48A5 Patton"}},
    {"M60", {10, "mediumTank", "M60"}},
    {"Patton the Tank", {10, "mediumTank", "Patton the Tank"}},
    {"M46 Patton", {9, "mediumTank", "M46 Patton"}},
    {"T95E6", {10, "mediumTank", "T95E6"}},
    {"M26 Pershing", {8, "mediumTank", "M26 Pershing"}},
    {"T69", {8, "mediumTank", "T69"}},
    {"T26E4 SuperPershing", {8, "mediumTank", "T26E4 SuperPershing"}},
    {"M46 Patton KR", {8, "mediumTank", "M46 Patton KR"}},
    {"T25 Pilot 1", {8, "mediumTank", "T25 Pilot Number 1"}},
    {"T20", {7, "mediumTank", "T20"}},
    {"M4A3E8 Sherman", {6, "mediumTank", "M4A3E8 Sherman"}},
    {"M4A3E2 Sherman Jumbo", {6, "mediumTank", "M4A3E2 Sherman Jumbo"}},
    {"M4 Sherman", {5, "mediumTank", "M4 Sherman"}},
    {"M7", {5, "mediumTank", "M7"}},

    // --- Light Tanks ---
    {"XM551 Sheridan", {10, "lightTank", "XM551 Sheridan"}},
    {"T49", {9, "lightTank", "T49"}},
    {"M41 Bulldog", {8, "lightTank", "M41 Walker Bulldog"}},
    {"T92", {8, "lightTank", "T92"}},
    {"T71 DA", {7, "lightTank", "T71 DA"}},
    {"T21", {6, "lightTank", "T21"}},
    {"T37", {6, "lightTank", "T37"}},
    {"M24 Chaffee", {5, "lightTank", "M24 Chaffee"}},

    // --- Tank Destroyers (AT-SPG) ---
    {"T110E3", {10, "AT-SPG", "T110E3"}},
    {"T110E4", {10, "AT-SPG", "T110E4"}},
    //{"T95", {9, "AT-SPG", "T95"}},
    {"T30", {9, "AT-SPG", "T30"}},
    {"T28", {8, "AT-SPG", "T28"}},
    {"T28 Prototype", {8, "AT-SPG", "T28 Prototype"}},
    {"TS-5", {8, "AT-SPG", "TS-5"}},
    {"T25 AT", {7, "AT-SPG", "T25 AT"}},
    {"T25/2", {7, "AT-SPG", "T25/2"}},
    {"M18 Hellcat", {6, "AT-SPG", "M18 Hellcat"}},
    {"M36 Jackson", {6, "AT-SPG", "M36 Jackson"}},
    {"T67", {5, "AT-SPG", "T67"}},
    {"M10 Wolverine", {5, "AT-SPG", "M10 Wolverine"}},

    // --- Artillery (SPG) ---
    {"T92 HMC", {10, "SPG", "T92 HMC"}},
    {"M53/M55", {9, "SPG", "M53/M55"}},
    {"M40/M43", {8, "SPG", "M40/M43"}},
    {"M12", {7, "SPG", "M12"}},
    {"M44", {6, "SPG", "M44"}},
    {"M41 HMC", {5, "SPG", "M41 HMC"}},

    // ======================================================================
    // === France (РОЗШИРЕНО) ==================================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"AMX M4 mle. 54", {10, "heavyTank", "AMX M4 mle. 54"}},
    {"AMX 50 B", {10, "heavyTank", "AMX 50 B"}},
    {"AMX M4 mle. 51", {9, "heavyTank", "AMX M4 mle. 51"}},
    {"AMX 50 120", {9, "heavyTank", "AMX 50 120"}},
    {"AMX 65 t", {8, "heavyTank", "AMX 65 t"}},
    {"AMX 50 100", {8, "heavyTank", "AMX 50 100"}},
    {"Somua SM", {8, "heavyTank", "Somua SM"}},
    {"AMX M4 mle. 49", {8, "heavyTank", "AMX M4 mle. 49 Liberté"}},
    {"AMX M4 mle. 45", {7, "heavyTank", "AMX M4 mle. 45"}},
    {"ARL 44", {6, "heavyTank", "ARL 44"}},
    {"BDR G1 B", {5, "heavyTank", "BDR G1 B"}},
    {"B1", {4, "heavyTank", "Char B1"}},

    // --- Medium Tanks ---
    {"Bat.-Châtillon 25 t", {10, "mediumTank", "Bat.-Châtillon 25 t"}},
    {"AMX 30 B", {10, "mediumTank", "AMX 30 B"}},
    {"Bat.-Châtillon 25 t AP", {9, "mediumTank", "Bat.-Châtillon 25 t AP"}},
    {"AMX 30", {9, "mediumTank", "AMX 30 1er prototype"}},
    {"Bat.-Châtillon Bourrasque", {8, "mediumTank", "Bat.-Châtillon Bourrasque"}},
    {"Lorraine 40 t", {8, "mediumTank", "Lorraine 40 t"}},
    {"AMX Chasseur de chars", {8, "mediumTank", "AMX CDC"}},
    {"M4A1 Revalorisé", {8, "mediumTank", "M4A1 Revalorisé"}},

    // --- Light Tanks (Tracked & Wheeled) ---
    {"AMX 13 105", {10, "lightTank", "AMX 13 105"}},
    {"Panhard EBR 105", {10, "lightTank", "Panhard EBR 105"}},
    {"AMX 13 90", {9, "lightTank", "AMX 13 90"}},
    {"Bat.-Châtillon 12 t", {9, "lightTank", "Bat.-Châtillon 12 t"}},
    {"Panhard EBR 90", {9, "lightTank", "Panhard EBR 90"}},
    {"ELC EVEN 90", {8, "lightTank", "ELC EVEN 90"}},
    {"Panhard EBR 75 (FL 10)", {8, "lightTank", "Panhard EBR 75"}},
    {"AMX 13 75", {7, "lightTank", "AMX 13 75"}},
    {"AMX 12 t", {6, "lightTank", "AMX 12 t"}},
    {"ELC AMX bis", {5, "lightTank", "ELC AMX bis"}},

    // --- Tank Destroyers (AT-SPG) ---
    {"AMX 50 Foch B", {10, "AT-SPG", "AMX 50 Foch B"}},
    {"AMX 50 Foch (155)", {10, "AT-SPG", "AMX 50 Foch (155)"}},
    {"AMX 50 Foch", {9, "AT-SPG", "AMX 50 Foch"}},
    {"AMX AC mle. 48", {8, "AT-SPG", "AMX AC mle. 48"}},
    {"AMX AC mle. 46", {7, "AT-SPG", "AMX AC mle. 46"}},
    {"ARL V39", {6, "AT-SPG", "ARL V39"}},
    {"S35 CA", {5, "AT-SPG", "S35 CA"}},

    // --- Artillery (SPG) ---
    {"Bat.-Châtillon 155 58", {10, "SPG", "Bat.-Châtillon 155 58"}},
    {"Bat.-Châtillon 155 55", {9, "SPG", "Bat.-Châtillon 155 55"}},
    {"Lorraine 155 mle. 51", {8, "SPG", "Lorraine 155 mle. 51"}},
    {"Lorraine 155 mle. 50", {7, "SPG", "Lorraine 155 mle. 50"}},
    {"AMX 13 F3 AM", {6, "SPG", "AMX 13 F3 AM"}},
    {"105 leFH18B2", {5, "SPG", "105 leFH18B2"}},

    // ======================================================================
    // === UK (РОЗШИРЕНО) ======================================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"Super Conqueror", {10, "heavyTank", "Super Conqueror"}},
    {"T95/FV4201 Chieftain", {10, "heavyTank", "Chieftain T95/FV4201"}},
    {"FV215b", {10, "heavyTank", "FV215b"}},
    {"Conqueror", {9, "heavyTank", "Conqueror"}},
    {"Concept No. 5", {9, "heavyTank", "Concept No. 5"}},
    {"Caernarvon", {8, "heavyTank", "Caernarvon"}},
    {"Caernarvon Action X", {8, "heavyTank", "Caernarvon Action X"}},
    {"FV201 (A45)", {7, "heavyTank", "FV201 (A45)"}},
    {"Black Prince", {7, "heavyTank", "Black Prince"}},
    {"Churchill VII", {6, "heavyTank", "Churchill VII"}},
    {"TOG II*", {6, "heavyTank", "TOG II*"}},
    {"Churchill I", {5, "heavyTank", "Churchill I"}},

    // --- Medium Tanks ---
    {"Centurion Action X", {10, "mediumTank", "Centurion Action X"}},
    {"FV4202", {10, "mediumTank", "FV4202"}},
    {"Centurion Mk. 7/1", {9, "mediumTank", "Centurion Mk. 7/1"}},
    {"Cobra", {9, "mediumTank", "Cobra"}},
    {"Centurion Mk. I", {8, "mediumTank", "Centurion Mk. I"}},
    {"Centurion Mk. 5/1 RAAC", {8, "mediumTank", "Centurion Mk. 5/1 RAAC"}},
    {"Charioteer", {8, "AT-SPG", "Charioteer"}}, // Часто грається як СТ
    {"Comet", {7, "mediumTank", "Comet"}},
    {"Cromwell", {6, "mediumTank", "Cromwell"}},
    {"Sherman Firefly", {6, "mediumTank", "Sherman Firefly"}},
    {"Cavalier", {5, "mediumTank", "Cavalier"}},
    {"Crusader", {5, "mediumTank", "Crusader"}},

    // --- Light Tanks ---
    {"Manticore", {10, "lightTank", "Manticore"}},
    {"GSOR3301 AVR FS", {9, "lightTank", "GSOR 3301"}},
    {"LHMTV", {8, "lightTank", "LHMTV"}},
    {"GSR 3301 Setter", {7, "lightTank", "Setter"}},

    // --- Tank Destroyers (AT-SPG) ---
    {"FV217 Badger", {10, "AT-SPG", "FV217 Badger"}},
    {"FV4005 Stage II", {10, "AT-SPG", "FV4005 Stage II"}},
    {"FV215b (183)", {10, "AT-SPG", "FV215b (183)"}},
    {"Tortoise", {9, "AT-SPG", "Tortoise"}},
    {"Conway", {9, "AT-SPG", "Conway"}},
    {"AT 15", {8, "AT-SPG", "AT 15"}},
    {"GSOR 1008", {8, "AT-SPG", "GSOR 1008"}},
    {"AT 7", {7, "AT-SPG", "AT 7"}},
    {"Challenger", {7, "AT-SPG", "Challenger"}},
    {"AT 8", {6, "AT-SPG", "AT 8"}},
    {"Churchill Gun Carrier", {6, "AT-SPG", "Churchill Gun Carrier"}},
    {"Achilles", {6, "AT-SPG", "Achilles"}},
    {"AT 2", {5, "AT-SPG", "AT 2"}},
    {"Archer", {5, "AT-SPG", "Archer"}},

    // --- Artillery (SPG) ---
    {"Conqueror Gun Carriage", {10, "SPG", "ConquerorGC"}},
    {"FV3805", {9, "SPG", "FV3805"}},
    {"FV207", {8, "SPG", "FV207"}},
    {"Crusader SP", {7, "SPG", "Crusader SP"}},
    {"FV304", {6, "SPG", "FV304"}},
    {"Bishop", {5, "SPG", "Bishop"}},

    // ======================================================================
    // === China (РОЗШИРЕНО) ===================================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"WZ-111 5A", {10, "heavyTank", "WZ-111 model 5A"}},
    {"113", {10, "heavyTank", "113"}},
    {"114 SP2", {10, "heavyTank", "114 SP2"}},
    {"116-F3", {10, "heavyTank", "116-F3"}},
    {"WZ-111 model 1-4", {9, "heavyTank", "WZ-111 model 1-4"}},
    {"WZ-114", {9, "heavyTank", "WZ-114"}},
    {"110", {8, "heavyTank", "110"}},
    {"WZ-111", {8, "heavyTank", "WZ-111"}},
    {"112", {8, "heavyTank", "112"}},
    {"WZ-112-2", {8, "heavyTank", "WZ-112-2"}},
    {"IS-2", {7, "heavyTank", "IS-2"}},

    // --- Medium Tanks ---
    {"121", {10, "mediumTank", "121"}},
    {"121B", {10, "mediumTank", "121B"}},
    {"WZ-120", {9, "mediumTank", "WZ-120"}},
    {"T-34-2", {8, "mediumTank", "T-34-2"}},
    {"T-34-3", {8, "mediumTank", "T-34-3"}},
    {"Type 59", {8, "mediumTank", "Type 59"}},
    {"122 TM", {8, "mediumTank", "122 TM"}},
    {"T-34-1", {7, "mediumTank", "T-34-1"}},
    {"Type 58", {6, "mediumTank", "Type 58"}},

    // --- Light Tanks ---
    {"WZ-132-1", {10, "lightTank", "WZ-132-1"}},
    {"WZ-132A", {9, "lightTank", "WZ-132A"}},
    {"WZ-132", {8, "lightTank", "WZ-132"}},
    {"Type 62", {7, "lightTank", "Type 62"}},
    {"WZ-131", {7, "lightTank", "WZ-131"}},
    {"59-16", {6, "lightTank", "59-16"}},
    {"Type 64", {6, "lightTank", "Type 64"}},

    // --- Tank Destroyers (AT-SPG) ---
    {"WZ-113G FT", {10, "AT-SPG", "WZ-113G FT"}},
    {"WZ-111G FT", {9, "AT-SPG", "WZ-111G FT"}},
    {"WZ-111-1G FT", {8, "AT-SPG", "WZ-111-1G FT"}},
    {"T-34-2G FT", {7, "AT-SPG", "T-34-2G FT"}},
    {"WZ-131G FT", {6, "AT-SPG", "WZ-131G FT"}},

    // ======================================================================
    // === Japan (РОЗШИРЕНО) ===================================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"Type 5 Heavy", {10, "heavyTank", "Type 5 Heavy"}},
    {"Type 4 Heavy", {9, "heavyTank", "Type 4 Heavy"}},
    {"O-Ho", {8, "heavyTank", "O-Ho"}},
    {"Type 63", {8, "heavyTank", "Type 63"}},
    {"O-Ni", {7, "heavyTank", "O-Ni"}},
    {"O-I", {6, "heavyTank", "O-I"}},
    {"O-I Experimental", {5, "heavyTank", "O-I Experimental"}},

    // --- Medium Tanks ---
    {"STB-1", {10, "mediumTank", "STB-1"}},
    {"Type 61", {9, "mediumTank", "Type 61"}},
    {"STA-1", {8, "mediumTank", "STA-1"}},
    {"STA-2", {8, "mediumTank", "STA-2"}},
    {"Chi-Ri", {7, "mediumTank", "Chi-Ri"}},
    {"Chi-To", {6, "mediumTank", "Chi-To"}},
    {"Chi-Nu", {5, "mediumTank", "Chi-Nu"}},
    {"Chi-Nu Kai", {5, "mediumTank", "Chi-Nu Kai"}},

    // --- Tank Destroyers (AT-SPG) ---
    {"Ho-Ri 3", {10, "AT-SPG", "Ho-Ri 3"}},
    {"Ho-Ri 2", {9, "AT-SPG", "Ho-Ri 2"}},
    {"Ho-Ri 1", {8, "AT-SPG", "Ho-Ri 1"}},
    {"Chi-To SPG", {7, "AT-SPG", "Chi-To SPG"}},
    {"Ji-Ro", {6, "AT-SPG", "Ji-Ro"}},
    {"Ho-Ni III", {5, "AT-SPG", "Ho-Ni III"}},

    // ======================================================================
    // === Czechoslovakia (РОЗШИРЕНО) ==========================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"Vz. 55", {10, "heavyTank", "Vz. 55"}},
    {"TNH Vz. 51", {9, "heavyTank", "TNH Vz. 51"}},
    {"TNH 105/1000", {8, "heavyTank", "TNH 105/1000"}},
    {"Škoda T 56", {8, "heavyTank", "Škoda T 56"}},
    {"Vz. 44-1", {7, "heavyTank", "Vz. 44-1"}},

    // --- Medium Tanks ---
    {"TVP T 50/51", {10, "mediumTank", "TVP T 50/51"}},
    {"Škoda T 50", {9, "mediumTank", "Škoda T 50"}},
    {"TVP VTU Koncept", {8, "mediumTank", "TVP VTU Koncept"}},
    {"Škoda T 27", {8, "mediumTank", "Škoda T 27"}},
    {"T-34/100", {7, "mediumTank", "T-34/100"}},
    {"Škoda T 25", {6, "mediumTank", "Škoda T 25"}},

    // --- Tank Destroyers (AT-SPG) ---
    {"Vz. 71", {10, "AT-SPG", "Vz. 71"}},
    {"Vz. 68 ver. 4", {9, "AT-SPG", "Vz. 68 ver. 4"}},
    {"Vz. 66", {8, "AT-SPG", "Vz. 66"}},
    {"ShPTK-TVP 100", {8, "AT-SPG", "ShPTK-TVP 100"}},
    {"Vz. 48", {7, "AT-SPG", "Vz. 48"}},
    {"Vz. 46", {6, "AT-SPG", "Vz. 46"}},
    {"Vz. 43", {5, "AT-SPG", "Vz. 43"}},


    // ======================================================================
    // === Poland (РОЗШИРЕНО) ==================================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"60TP Lewandowskiego", {10, "heavyTank", "60TP Lewandowskiego"}},
    {"50TP Tyszkiewicza", {9, "heavyTank", "50TP Tyszkiewicza"}},
    {"50TP prototyp", {8, "heavyTank", "50TP prototyp"}},
    {"56TP", {8, "heavyTank", "56TP"}},
    {"45TP Habicha", {7, "heavyTank", "45TP Habicha"}},
    {"40TP Habicha", {6, "heavyTank", "40TP Habicha"}},

    // --- Medium Tanks ---
    {"CS-63", {10, "mediumTank", "CS-63"}},
    {"CS-59", {9, "mediumTank", "CS-59"}},
    {"CS-52 LIS", {8, "mediumTank", "CS-52 LIS"}},
    {"CS-53", {8, "mediumTank", "CS-53"}},
    {"CS-44", {7, "mediumTank", "CS-44"}},
    {"BUGI", {6, "mediumTank", "BUGI"}},
    {"DS PZInż", {5, "mediumTank", "DS PZInż"}},


    // ======================================================================
    // === Sweden (РОЗШИРЕНО) ==================================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"Kranvagn", {10, "heavyTank", "Kranvagn"}},
    {"Emil II", {9, "heavyTank", "Emil II"}},
    {"Emil I", {8, "heavyTank", "Emil I"}},
    {"Bofors Tornvagn", {8, "heavyTank", "Bofors Tornvagn"}},

    // --- Medium Tanks ---
    {"UDES 15/16", {10, "mediumTank", "UDES 15/16"}},
    {"UDES 16", {9, "mediumTank", "UDES 16"}},
    {"UDES 14 Alt 5", {8, "mediumTank", "UDES 14 Alt 5"}},
    {"Lansen C", {8, "mediumTank", "Lansen C"}},
    {"Primo Victoria", {8, "mediumTank", "Primo Victoria"}},
    {"Leo", {7, "mediumTank", "Leo"}},
    {"Strv 74", {6, "mediumTank", "Strv 74"}},
    {"Strv m/42", {5, "mediumTank", "Strv m/42"}},

    // --- Tank Destroyers (AT-SPG) ---
    {"Strv 103B", {10, "AT-SPG", "Strv 103B"}},
    {"Strv 103-0", {9, "AT-SPG", "Strv 103-0"}},
    {"UDES 03", {8, "AT-SPG", "UDES 03"}},
    {"Strv S1", {8, "AT-SPG", "Strv S1"}},
    {"Ikv 90 Typ B", {7, "AT-SPG", "Ikv 90 Typ B"}},
    {"Ikv 65 Alt II", {6, "AT-SPG", "Ikv 65 Alt II"}},
    {"Ikv 103", {5, "AT-SPG", "Ikv 103"}},


    // ======================================================================
    // === Italy (РОЗШИРЕНО) ===================================================
    // ======================================================================
    // --- Heavy Tanks ---
    {"Rinoceronte", {10, "heavyTank", "Rinoceronte"}},
    {"Progetto C50 Mod. 66", {9, "heavyTank", "Progetto C50 Mod. 66"}},
    {"Progetto C45 mod. 71", {8, "heavyTank", "Progetto C45 mod. 71"}},
    {"Bisonte C45", {8, "heavyTank", "Bisonte C45"}},
    {"Carro d'assalto P.88", {7, "heavyTank", "Carro d'assalto P.88"}},

    // --- Medium Tanks ---
    {"Progetto 65", {10, "mediumTank", "Progetto M40 mod. 65"}},
    {"Carro 45t", {10, "mediumTank", "Carro da Combattimento 45 t"}},
    {"Prototipo Standard B", {9, "mediumTank", "Prototipo Standard B"}},
    {"P.44 Pantera", {8, "mediumTank", "P.44 Pantera"}},
    {"Progetto M35 mod. 46", {8, "mediumTank", "Progetto M35 mod. 46"}},
    {"P.43 ter", {7, "mediumTank", "P.43 ter"}},
    {"P.43 bis", {6, "mediumTank", "P.43 bis"}},
    {"P.43", {5, "mediumTank", "P.43"}},

    // --- Tank Destroyers (AT-SPG) ---
    {"Controcarro 3 Minotauro", {10, "AT-SPG", "Controcarro 3 Minotauro"}},
    {"Controcarro 1 Mk. 2", {9, "AT-SPG", "Controcarro 1 Mk. 2"}},
    {"Controcarro 0 Mk. 2", {8, "AT-SPG", "Controcarro 0 Mk. 2"}},
    {"SMV CC-64 Vipera", {8, "AT-SPG", "SMV CC-64 Vipera"}},
    {"SMV CC-67", {7, "AT-SPG", "SMV CC-67"}},
    {"Semovente M43 Bassotto", {6, "AT-SPG", "Semovente M43 Bassotto"}},
    {"Semovente M41", {5, "AT-SPG", "Semovente M41"}},
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
