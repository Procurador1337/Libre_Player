#include "playlistdurationcalculator.h"
#include <QMediaPlayer>
#include <QUrl>

PlaylistDurationCalculator::PlaylistDurationCalculator(QObject *parent)
    : QObject(parent),
    m_player(new QMediaPlayer(this))
{
    connect(m_player,
            &QMediaPlayer::mediaStatusChanged,
            this,
            [this](QMediaPlayer::MediaStatus status)
            {
                if (status == QMediaPlayer::LoadedMedia) {
                    m_totalDuration += m_player->duration();
                    calculateNext();
                }
                else if (status == QMediaPlayer::InvalidMedia) {
                    calculateNext();
                }
            });
}

void PlaylistDurationCalculator::calculate(
    const QStringList &files)
{
    m_files = files;
    m_currentIndex = 0;
    m_totalDuration = 0;

    if (m_files.isEmpty()) {
        emit durationCalculated(0);
        return;
    }

    calculateNext();
}

void PlaylistDurationCalculator::calculateNext()
{
    if (m_currentIndex >= m_files.size()) {
        emit durationCalculated(m_totalDuration);
        return;
    }

    const QString file =
        m_files.at(m_currentIndex);

    ++m_currentIndex;

    m_player->setSource(
        QUrl::fromLocalFile(file)
        );
}