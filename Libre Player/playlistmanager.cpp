#include "playlistmanager.h"
#include <QRandomGenerator>

PlaylistManager::PlaylistManager(QObject *parent)
    : QObject(parent)
{
}

void PlaylistManager::addFiles(const QStringList &files)
{
    for (const QString &file : files) {
        if (!m_playlist.contains(file))
            m_playlist.append(file);
    }

    emit playlistChanged();
}
void PlaylistManager::removeTrack(int index)
{
    if (index < 0 || index >= m_playlist.size()) {
        return;
    }

    m_playlist.removeAt(index);

    if (m_playlist.isEmpty()) {
        m_currentTrack = 0;
    } else if (index < m_currentTrack) {
        --m_currentTrack;
    } else if (m_currentTrack >= m_playlist.size()) {
        m_currentTrack = m_playlist.size() - 1;
    }

    emit playlistChanged();
    emit currentTrackChanged(m_currentTrack);
}

void PlaylistManager::clear()
{
    m_playlist.clear();
    m_currentTrack = 0;
    emit playlistChanged();
    emit currentTrackChanged(0);
}

void PlaylistManager::moveTrack(int from, int to)
{
    if (from < 0 || from >= m_playlist.size() ||
        to < 0 || to >= m_playlist.size() || from == to) {
        return;
    }

    m_playlist.move(from, to);

    if (m_currentTrack == from) {
        m_currentTrack = to;
    } else if (from < m_currentTrack && to >= m_currentTrack) {
        --m_currentTrack;
    } else if (from > m_currentTrack && to <= m_currentTrack) {
        ++m_currentTrack;
    }

    emit playlistChanged();
    emit currentTrackChanged(m_currentTrack);
}

const QStringList &PlaylistManager::tracks() const
{
    return m_playlist;
}

int PlaylistManager::currentTrack() const
{
    return m_currentTrack;
}

void PlaylistManager::setCurrentTrack(int index)
{
    if (index < 0 || index >= m_playlist.size()) {
        return;
    }

    m_currentTrack = index;
    emit currentTrackChanged(index);
}

QString PlaylistManager::currentFile() const
{
    return m_playlist.value(m_currentTrack);
}

QString PlaylistManager::nextFile()
{
    if (m_playlist.isEmpty())
        return QString();

    if (shuffleEnabled && m_playlist.size() > 1) {
        int next = m_currentTrack;

        while (next == m_currentTrack) {
            next = QRandomGenerator::global()->bounded(
                m_playlist.size()
                );
        }

        m_currentTrack = next;
        return m_playlist.at(m_currentTrack);
    }

    if (m_currentTrack + 1 >= m_playlist.size()) {
        if (currentRepeatMode == RepeatMode::All) {
            m_currentTrack = 0;
            return m_playlist.at(m_currentTrack);
        }

        return QString();
    }

    ++m_currentTrack;
    return m_playlist.at(m_currentTrack);
}

QString PlaylistManager::previousFile()
{
    if (m_currentTrack <= 0) {
        return {};
    }

    setCurrentTrack(m_currentTrack - 1);
    return currentFile();
}

bool PlaylistManager::shuffle() const
{
    return shuffleEnabled;
}

void PlaylistManager::setShuffle(bool enabled)
{
    shuffleEnabled = enabled;
}

RepeatMode PlaylistManager::repeatMode() const
{
    return currentRepeatMode;
}

void PlaylistManager::setRepeatMode(RepeatMode mode)
{
    currentRepeatMode = mode;
}