#ifndef REPLAYANALYZERPAGE_H
#define REPLAYANALYZERPAGE_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>
#include <QVariantMap>
#include <QFutureWatcher>

#include "DatabaseManager.h"
#include "ReplayParser.h"
#include <QtConcurrent/QtConcurrent>

class ReplayAnalyzerPage : public QWidget {
    Q_OBJECT

public:
    explicit ReplayAnalyzerPage(DatabaseManager *dbManager, QWidget *parent = nullptr);
    ~ReplayAnalyzerPage();

private slots:
    void onSelectFileButtonClicked();
    void onReplayListItemClicked(QListWidgetItem *item);
    void handleAnalysisFinished();

public slots:
    void analyzeReplay(const QString &filePath);

private:
    void displayResults(const QString& results);
    void loadCachedReplays();

    // 🔹 Новий метод для структурованого звіту
    void displayStructuredResults(const QVariantMap &data);

    QPushButton *m_selectFileButton;
    QListWidget *m_replayList;
    QTextEdit *m_resultsTextEdit;
    QProgressBar *m_progressBar;
    DatabaseManager *m_dbManager;
    ReplayParser *m_replayParser;
    QFutureWatcher<QVariantMap> m_watcher;
};

#endif // REPLAYANALYZERPAGE_H
