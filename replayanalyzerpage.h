#ifndef REPLAYANALYZERPAGE_H
#define REPLAYANALYZERPAGE_H

#include <QWidget>
#include <QFutureWatcher>
#include <QVariantMap>

#include "ReplayParser.h"
#include "DatabaseManager.h"
#include "MetricsCalculator.h"    // 🔹 Додано
#include "BehaviorAnalyzer.h"     // 🔹 Додано
#include "RecommenderSystem.h"    // 🔹 Додано
#include <QListWidget>

class QPushButton;
class QProgressBar;
class QTextEdit;

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

private:
    void analyzeReplay(const QString &filePath);
    void loadCachedReplays();
    void displayStructuredResults(const QVariantMap &data);
    void displayResults(const QString& results);
    QString cleanVehicleName(const QString &technicalName);

    QPushButton *m_selectFileButton;
    QPushButton *m_clearReplaysButton;
    QProgressBar *m_progressBar;
    QListWidget *m_replayList;
    QTextEdit *m_resultsTextEdit;

    ReplayParser *m_replayParser;
    DatabaseManager *m_dbManager;
    MetricsCalculator *m_metricsCalculator;   // 🔹 Додано
    BehaviorAnalyzer *m_behaviorAnalyzer;     // 🔹 Додано
    RecommenderSystem *m_recommenderSystem;   // 🔹 Додано

    QFutureWatcher<QVariantMap> m_watcher;
};

#endif // REPLAYANALYZERPAGE_H
