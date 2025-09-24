#ifndef REPLAYANALYZERPAGE_H
#define REPLAYANALYZERPAGE_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QListWidget>
#include <QProgressBar>

class ReplayAnalyzerPage : public QWidget {
    Q_OBJECT

public:
    explicit ReplayAnalyzerPage(QWidget *parent = nullptr);

public slots:
    // Слот для запуску аналізу реплею
    void analyzeReplay(const QString &filePath);

private:
    void displayResults(const QString& results);

    QPushButton *m_selectFileButton;
    QListWidget *m_replayList;
    QTextEdit *m_resultsTextEdit;
    QProgressBar *m_progressBar;
};

#endif // REPLAYANALYZERPAGE_H
