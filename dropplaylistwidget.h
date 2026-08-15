#ifndef DROPPLAYLISTWIDGET_H
#define DROPPLAYLISTWIDGET_H

#include <QListWidget>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>

class DropPlaylistWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit DropPlaylistWidget(QWidget *parent = nullptr);

signals:
    void filesDropped(const QStringList &files);
    void itemsReordered(int from, int to);

protected:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // DROPPLAYLISTWIDGET_H