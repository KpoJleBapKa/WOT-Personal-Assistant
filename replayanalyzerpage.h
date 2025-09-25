#ifndef REPLAYANALYZERPAGE_H
#define REPLAYANALYZERPAGE_H

#include <QWidget>
#include <QFutureWatcher>
#include <QVariantMap>
#include <QListWidget>

#include "ReplayParser.h"
#include "DatabaseManager.h"
#include "MetricsCalculator.h"
#include "BehaviorAnalyzer.h"
#include "RecommenderSystem.h"

class QPushButton;
class QProgressBar;
class QTextEdit;
class QListWidgetItem;

class ReplayAnalyzerPage : public QWidget
{
    Q_OBJECT

public:
    explicit ReplayAnalyzerPage(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~ReplayAnalyzerPage();

private slots:
    void onSelectFileButtonClicked();
    void onReplayListItemClicked(QListWidgetItem *item);
    void handleAnalysisFinished();
    void onClearReplaysButtonClicked();

    // Нові слоти
    void onDeleteSelectedClicked();
    void onExportPdfClicked();

private:
    void analyzeReplay(const QString &filePath);
    void loadCachedReplays();
    void displayStructuredResults(const QVariantMap &data);
    void displayResults(const QString& results);
    QString cleanVehicleName(const QString &technicalName);

    // UI
    QPushButton *m_selectFileButton;
    QPushButton *m_clearReplaysButton;
    QPushButton *m_deleteSelectedButton;   // кнопка видалення одного елементу
    QPushButton *m_exportPdfButton;        // кнопка експорту в PDF
    QProgressBar *m_progressBar;
    QListWidget *m_replayList;
    QTextEdit *m_resultsTextEdit;

    // Аналітичні модулі
    ReplayParser *m_replayParser;
    DatabaseManager *m_dbManager;
    MetricsCalculator *m_metricsCalculator;
    BehaviorAnalyzer *m_behaviorAnalyzer;
    RecommenderSystem *m_recommenderSystem;

    QFutureWatcher<QVariantMap> m_watcher;
};

#endif // REPLAYANALYZERPAGE_H
