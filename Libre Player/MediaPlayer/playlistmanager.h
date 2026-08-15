#ifndef PLAYLISTMANAGER_H
#define PLAYLISTMANAGER_H

#include <QObject>
#include <QStringList>

enum class RepeatMode
{
    Off,
    All,
    One
};

class PlaylistManager : public QObject
{
    Q_OBJECT

public:
    explicit PlaylistManager(QObject *parent = nullptr);

    void addFiles(const QStringList &files);
    void removeTrack(int index);
    void clear();
    void moveTrack(int from, int to);

    const QStringList &tracks() const;
    int currentTrack() const;
    void setCurrentTrack(int index);
    QString currentFile() const;
    QString nextFile();
    QString previousFile();

    bool shuffle() const;
    void setShuffle(bool enabled);

    RepeatMode repeatMode() const;
    void setRepeatMode(RepeatMode mode);

signals:
    void playlistChanged();
    void currentTrackChanged(int index);

private:
    QStringList m_playlist;
    int m_currentTrack = 0;

    bool shuffleEnabled = false;
    RepeatMode currentRepeatMode = RepeatMode::Off;
};

#endif // PLAYLISTMANAGER_H