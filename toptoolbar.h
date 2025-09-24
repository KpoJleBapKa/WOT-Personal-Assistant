#ifndef TOPTOOLBAR_H
#define TOPTOOLBAR_H

#include <QWidget>
#include <QPushButton>

class TopToolbar : public QWidget {
    Q_OBJECT

public:
    explicit TopToolbar(QWidget *parent = nullptr);

signals:
    void settingsClicked();
    void aboutClicked();

private:
    QPushButton *m_settingsButton;
    QPushButton *m_aboutButton;
};

#endif // TOPTOOLBAR_H
