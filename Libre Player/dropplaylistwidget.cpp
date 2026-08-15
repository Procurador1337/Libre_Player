#include "dropplaylistwidget.h"

#include <QMimeData>
#include <QUrl>

DropPlaylistWidget::DropPlaylistWidget(QWidget *parent)
    : QListWidget(parent)
{
    setAcceptDrops(true);

    viewport()->setAcceptDrops(true);
    viewport()->installEventFilter(this);

    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDefaultDropAction(Qt::MoveAction);
}

void DropPlaylistWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        return;
    }

    if (event->mimeData()->hasFormat(
            "application/x-qabstractitemmodeldatalist")) {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
}

void DropPlaylistWidget::dragMoveEvent(QDragMoveEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        event->acceptProposedAction();
        return;
    }

    if (event->mimeData()->hasFormat(
            "application/x-qabstractitemmodeldatalist")) {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
}

void DropPlaylistWidget::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {

        QStringList files;

        const QList<QUrl> urls = event->mimeData()->urls();

        for (const QUrl &url : urls) {
            if (url.isLocalFile()) {
                files.append(url.toLocalFile());
            }
        }

        if (!files.isEmpty()) {
            emit filesDropped(files);
            event->acceptProposedAction();
            return;
        }
    }

    QListWidget::dropEvent(event);
}