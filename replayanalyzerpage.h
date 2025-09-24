#ifndef REPLAYANALYZERPAGE_H
#define REPLAYANALYZERPAGE_H

#include <QWidget>
#include <QFutureWatcher>
#include <QVariantMap>

#include "ReplayParser.h"
#include "DatabaseManager.h"
#include <QListWidget>

class QPushButton;
class QProgressBar;
class QListWidget;
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
    void onClearReplaysButtonClicked(); // 🔹 Додано оголошення нового слоту

private:
    void analyzeReplay(const QString &filePath);
    void loadCachedReplays();
    void displayStructuredResults(const QVariantMap &data);
    void displayResults(const QString& results);

    QPushButton *m_selectFileButton;
    QPushButton *m_clearReplaysButton; // 🔹 Додано оголошення нової кнопки
    QProgressBar *m_progressBar;
    QListWidget *m_replayList;
    QTextEdit *m_resultsTextEdit;
    ReplayParser *m_replayParser;
    DatabaseManager *m_dbManager;
    QFutureWatcher<QVariantMap> m_watcher;
};

#endif // REPLAYANALYZERPAGE_H
