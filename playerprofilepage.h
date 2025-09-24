#ifndef PLAYERPROFILEPAGE_H
#define PLAYERPROFILEPAGE_H

#include <QWidget>
#include <QLabel>
#include <QTextEdit>

class PlayerProfilePage : public QWidget {
    Q_OBJECT

public:
    explicit PlayerProfilePage(QWidget *parent = nullptr);

private:
    QLabel *m_titleLabel;
    QTextEdit *m_statsDisplay;
};

#endif // PLAYERPROFILEPAGE_H
