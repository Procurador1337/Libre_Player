#ifndef PLAYLISTDURATIONCALCULATOR_H
#define PLAYLISTDURATIONCALCULATOR_H

#include <QObject>
#include <QStringList>

class QMediaPlayer;

class PlaylistDurationCalculator : public QObject
{
    Q_OBJECT

public:
    explicit PlaylistDurationCalculator(QObject *parent = nullptr);

    void calculate(const QStringList &files);

signals:
    void durationCalculated(qint64 duration);

private:
    QStringList m_files;
    int m_currentIndex = 0;
    qint64 m_totalDuration = 0;

    QMediaPlayer *m_player = nullptr;

    void calculateNext();
};

#endif