#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QObject>
#include <QString>
#include <QSize>
#include <QPoint>

class SettingsManager : public QObject
{
    Q_OBJECT

public:
    explicit SettingsManager(QObject *parent = nullptr);

    void saveVolume(float volume);
    float volume() const;

    void saveMuted(bool muted);
    bool muted() const;

    void saveShuffle(bool enabled);
    bool shuffle() const;

    void saveRepeatMode(int mode);
    int repeatMode() const;

    void saveMaximized(bool maximized);
    bool maximized() const;

    void saveFolder(const QString &folder);
    QString folder() const;

    void saveCurrentTrack(int track);
    int currentTrack() const;

    void savePosition(qint64 position);
    qint64 position() const;

    void saveSearchText(const QString &text);
    QString searchText() const;

    void saveWindowGeometry(const QPoint &position, const QSize &size);
    QPoint windowPosition() const;
    QSize windowSize() const;

private:
    class QSettings *settings;
};

#endif