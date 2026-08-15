#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QListWidgetItem>
#include <QModelIndex>
#include <QResizeEvent>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QHash>

#include "playlistmanager.h"
#include "playlistdurationcalculator.h"
#include "settingsmanager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class Shortcuts;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void togglePlayback();
    void nextTrack();
    void previousTrack();
    void toggleMute();
    void seekBackward();
    void seekForward();
    void volumeUp();
    void volumeDown();
    void shortcutStop();

private:
    void on_openButton_clicked();
    void on_pauseButton_clicked();
    void on_stopButton_clicked();
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void closeEvent(QCloseEvent *event) override;
    void refreshCurrentFolder();
    void updatePlaylistCount();
    PlaylistDurationCalculator *durationCalculator;
    void openFolder();
    QHBoxLayout *breadcrumbLayout = nullptr;
    void showFolder(const QString &folder);
    void on_volumeSlider_valueChanged(int value);
    void on_playlistItemDoubleClicked(QListWidgetItem *item);
    void on_filesDropped(const QStringList &files);
    void updateBreadcrumbs(const QString &folder);
    void on_mediaStatusChanged(QMediaPlayer::MediaStatus status);
    void on_playlistContextMenu(const QPoint &position);
    void on_playlistRowsMoved(const QModelIndex &parent, int start, int end,
                              const QModelIndex &destination, int row);

    void on_positionChanged(qint64 position);
    void on_durationChanged(qint64 duration);
    void on_progressClicked(double percentage);
    bool restoringPlayback = false;
    qint64 savedPosition = 0;
    QString formatTime(qint64 milliseconds);
    QString currentFolder;
    void updateMetadata();
    void playCurrentTrack();
    void addFiles(const QStringList &files);
    bool eventFilter(QObject *object, QEvent *event) override;

    void updateUiScale();
    void resizeEvent(QResizeEvent *event) override;

    qreal uiScale = 1.0;
    QSize baseCentralSize;
    QHash<QWidget *, QRect> baseGeometries;
    QHash<QWidget *, QFont> baseFonts;
    Ui::MainWindow *ui;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    SettingsManager *settingsManager;
    PlaylistManager *playlistManager;
    qint64 duration = 0;
    Shortcuts *shortcuts;

};

#endif
