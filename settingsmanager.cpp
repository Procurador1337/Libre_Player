#include "settingsmanager.h"
#include <QSettings>

SettingsManager::SettingsManager(QObject *parent)
    : QObject(parent),
    settings(new QSettings(
        "MediaPlayer",
        "MediaPlayer",
        this))
{
}

void SettingsManager::saveVolume(float volume)
{
    settings->setValue("volume", volume);
}

void SettingsManager::saveMaximized(
    bool maximized)
{
    settings->setValue(
        "maximized",
        maximized
        );
}

bool SettingsManager::maximized() const
{
    return settings->value(
       "maximized",
        false
        ).toBool();
}

float SettingsManager::volume() const
{
    return settings->value("volume", 1.0).toFloat();
}

void SettingsManager::saveMuted(bool muted)
{
    settings->setValue("muted", muted);
}

bool SettingsManager::muted() const
{
    return settings->value("muted", false).toBool();
}

void SettingsManager::saveShuffle(bool enabled)
{
    settings->setValue("shuffle", enabled);
}

bool SettingsManager::shuffle() const
{
    return settings->value("shuffle", false).toBool();
}

void SettingsManager::saveRepeatMode(int mode)
{
    settings->setValue("repeatMode", mode);
}

int SettingsManager::repeatMode() const
{
    return settings->value("repeatMode", 0).toInt();
}

void SettingsManager::saveFolder(const QString &folder)
{
    settings->setValue("folder", folder);
}

QString SettingsManager::folder() const
{
    return settings->value("folder").toString();
}

void SettingsManager::saveCurrentTrack(int track)
{
    settings->setValue("currentTrack", track);
}

int SettingsManager::currentTrack() const
{
    return settings->value("currentTrack", 0).toInt();
}

void SettingsManager::savePosition(qint64 position)
{
    settings->setValue("position", position);
}

qint64 SettingsManager::position() const
{
    return settings->value("position", 0).toLongLong();
}

void SettingsManager::saveWindowGeometry(
    const QPoint &position,
    const QSize &size)
{
    settings->setValue("windowPosition", position);
    settings->setValue("windowSize", size);
}

QPoint SettingsManager::windowPosition() const
{
    return settings->value("windowPosition").toPoint();
}

QSize SettingsManager::windowSize() const
{
    return settings->value("windowSize").toSize();
}

void SettingsManager::saveSearchText(
    const QString &text)
{
    settings->setValue(
        "searchText",
        text
        );
}

QString SettingsManager::searchText() const
{
    return settings->value(
                       "searchText"
                       ).toString();
}