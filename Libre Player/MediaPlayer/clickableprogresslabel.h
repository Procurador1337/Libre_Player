#ifndef CLICKABLEPROGRESSLABEL_H
#define CLICKABLEPROGRESSLABEL_H

#include <QLabel>
#include <QMouseEvent>

class ClickableProgressLabel : public QLabel
{
    Q_OBJECT

public:
    explicit ClickableProgressLabel(QWidget *parent = nullptr);

signals:
    void clickedAt(double percentage);

protected:
    void mousePressEvent(QMouseEvent *event) override;
};

#endif // CLICKABLEPROGRESSLABEL_H