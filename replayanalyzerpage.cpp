#include "ReplayAnalyzerPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

ReplayAnalyzerPage::ReplayAnalyzerPage(QWidget *parent) : QWidget(parent) {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QHBoxLayout *topLayout = new QHBoxLayout();

    m_selectFileButton = new QPushButton("Вибрати реплей...", this);
    m_selectFileButton->setCursor(Qt::PointingHandCursor); // Змінюємо курсор

    m_replayList = new QListWidget(this);
    m_replayList->addItem("Завантажені реплеї:");
    m_resultsTextEdit = new QTextEdit(this);
    m_resultsTextEdit->setReadOnly(true);
    m_resultsTextEdit->setPlainText("Результати аналізу будуть відображатися тут.");
    m_resultsTextEdit->setStyleSheet("background-color: #2a2a2a; border: 1px solid #4a4a4a; padding: 10px;");

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);
    m_progressBar->setAlignment(Qt::AlignCenter); // Вирівнюємо текст по центру
    m_progressBar->setStyleSheet(
        "QProgressBar { border: 1px solid #5577aa; border-radius: 5px; background-color: #3a3a3a; }"
        "QProgressBar::chunk { background-color: #5577aa; }"
        );

    topLayout->addWidget(m_selectFileButton);
    topLayout->addStretch();

    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(m_progressBar);
    mainLayout->addWidget(m_replayList);
    mainLayout->addWidget(m_resultsTextEdit);

    connect(m_selectFileButton, &QPushButton::clicked, [this]() {
        QString filePath = QFileDialog::getOpenFileName(this, "Вибрати реплей", "", "World of Tanks Replays (*.wotreplay)");
        if (!filePath.isEmpty()) {
            QFileInfo fileInfo(filePath);
            QListWidgetItem *item = new QListWidgetItem(fileInfo.fileName());
            item->setData(Qt::UserRole, filePath);
            m_replayList->addItem(item);
            analyzeReplay(filePath);
        }
    });
    connect(m_replayList, &QListWidget::itemClicked, [this](QListWidgetItem *item) {
        if (!item->data(Qt::UserRole).toString().isEmpty()) {
            analyzeReplay(item->data(Qt::UserRole).toString());
        }
    });
}

void ReplayAnalyzerPage::analyzeReplay(const QString &filePath) {
    m_progressBar->setValue(0);
    m_resultsTextEdit->setPlainText("Початок аналізу файлу: " + filePath + "\n\n");

    // Імітація процесу аналізу
    for (int i = 0; i <= 100; ++i) {
        m_progressBar->setValue(i);
        // Тут буде реальна логіка парсингу та аналізу
    }

    QString dummyResults = R"(
<h2>Результати аналізу реплею</h2>

<h3>1. Метрики ефективності</h3>
<ul>
    <li><b>Завдано шкоди:</b> 1500</li>
    <li><b>Знищено:</b> 3 танки</li>
    <li><b>Виявлено:</b> 5 танків</li>
    <li><b>Точність:</b> 75%</li>
    <li><b>Час виживання:</b> 5:30 хв</li>
</ul>

<h3>2. Аналіз поведінки</h3>
<p>Ви демонструєте агресивний стиль гри, часто вступаючи в прямі зіткнення. Ваша сильна сторона — швидке фокусування на пріоритетних цілях. Слабкість — схильність до ризику без достатньої підтримки команди.</p>

<h3>3. Персоналізовані рекомендації</h3>
<p>На основі аналізу, рекомендуємо:</p>
<ul>
    <li>Більш ефективно використовувати укриття під час перезарядки.</li>
    <li>Спробуйте іноді займати позицію підтримки, а не завжди бути на передовій.</li>
    <li>Приділіть увагу пробиттю вразливих зон, а не тільки корпусу та башти.</li>
</ul>
    )";

    displayResults(dummyResults);
}

void ReplayAnalyzerPage::displayResults(const QString& results) {
    m_resultsTextEdit->clear();
    m_resultsTextEdit->setHtml(results);
}
