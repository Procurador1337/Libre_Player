#include "clickableprogresslabel.h"

ClickableProgressLabel::ClickableProgressLabel(QWidget *parent)
    : QLabel(parent)
{
}

void ClickableProgressLabel::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        double percentage =
            static_cast<double>(event->position().x()) / width();

        percentage = qBound(0.0, percentage, 1.0);

        emit clickedAt(percentage);
    }

    QLabel::mousePressEvent(event);
}