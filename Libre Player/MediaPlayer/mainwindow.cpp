#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "clickableprogresslabel.h"
#include "shortcuts.h"
#include "playlistdurationcalculator.h"

#include <QFileDialog>
#include <QPushButton>
#include <QUrl>
#include <QSlider>
#include <QMediaMetaData>
#include <QImage>
#include <QFileInfo>
#include <QPixmap>
#include <QMenu>
#include <QKeyEvent>
#include <QEvent>
#include <QAbstractItemModel>
#include <QMessageBox>
#include <QDir>
#include <QFont>
#include <QResizeEvent>
#include <QDirIterator>
#include <QPushButton>
#include <QHBoxLayout>
#include <QDir>
#include <QFileDialog>
#include <QMediaPlayer>

//Window constructor, use this to connect buttons to code
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
    player(new QMediaPlayer(this)), audioOutput(new QAudioOutput(this)),
    playlistManager(new PlaylistManager(this)),
    settingsManager(new SettingsManager(this)),
    durationCalculator(new PlaylistDurationCalculator(this))
{
    ui->setupUi(this);

    setWindowTitle("Libre Player");

    baseCentralSize = ui->centralwidget->size();

    const QList<QWidget *> scalingWidgets =
        ui->centralwidget->findChildren<QWidget *>(
            QString(),
            Qt::FindDirectChildrenOnly
            );

    for (QWidget *widget : scalingWidgets) {
        baseGeometries.insert(
            widget,
            widget->geometry()
            );

        baseFonts.insert(
            widget,
            widget->font()
            );
    }

    // Restores the state of previous user
    audioOutput->setVolume(settingsManager->volume());
    audioOutput->setMuted(settingsManager->muted());

    ui->volumeSlider->setValue(
        static_cast<int>(settingsManager->volume() * 100)
        );

    playlistManager->setShuffle(
        settingsManager->shuffle()
        );

    playlistManager->setRepeatMode(
        static_cast<RepeatMode>(
            settingsManager->repeatMode()
            )
        );

    ui->muteButton->setText(
        audioOutput->isMuted()
            ? "Unmute"
            : "Mute"
        );

    ui->shuffleButton->setText(
        playlistManager->shuffle()
            ? "Shuffle On"
            : "Shuffle"
        );

    ui->repeatButton->setText(
        playlistManager->repeatMode() == RepeatMode::Off
            ? "Repeat Off"
            : playlistManager->repeatMode() == RepeatMode::All
                  ? "Repeat All"
                  : "Repeat One"
        );

    if (!settingsManager->windowSize().isEmpty())
        resize(settingsManager->windowSize());

    if (!settingsManager->windowPosition().isNull())
        move(settingsManager->windowPosition());


    // This avoids a memory leak somehow
    breadcrumbLayout = new QHBoxLayout;

    breadcrumbLayout->setContentsMargins(0, 0, 0, 0);
    breadcrumbLayout->setSpacing(4);

    ui->breadcrumbWidget->setLayout(breadcrumbLayout);


    connect(durationCalculator,
            &PlaylistDurationCalculator::durationCalculated,
            this,
            [this](qint64 duration)
            {
                const qint64 seconds = duration / 1000;
                const qint64 minutes = seconds / 60;
                const qint64 remainingSeconds = seconds % 60;

                ui->playlistCountLabel->setText(
                    QString("%1 tracks • %2:%3")
                        .arg(playlistManager->tracks().size())
                        .arg(minutes)
                        .arg(remainingSeconds, 2, 10, QChar('0'))
                    );
            });

    connect(playlistManager,
            &PlaylistManager::playlistChanged,
            this,
            [this]()
            {
                durationCalculator->calculate(
                    playlistManager->tracks()
                    );
            });

    shortcuts = new Shortcuts(this);
    player->setAudioOutput(audioOutput);

    connect(ui->openButton, &QPushButton::clicked,
            this, &MainWindow::on_openButton_clicked);

    connect(ui->pauseButton, &QPushButton::clicked,
            this, &MainWindow::on_pauseButton_clicked);

    connect(ui->stopButton, &QPushButton::clicked,
            this, &MainWindow::on_stopButton_clicked);

    connect(ui->volumeSlider, &QSlider::valueChanged,
            this, &MainWindow::on_volumeSlider_valueChanged);

    connect(player, &QMediaPlayer::positionChanged,
            this, &MainWindow::on_positionChanged);

    connect(player, &QMediaPlayer::durationChanged,
            this, &MainWindow::on_durationChanged);

    connect(player, &QMediaPlayer::metaDataChanged,
            this, &MainWindow::updateMetadata);

    connect(ui->previousButton, &QPushButton::clicked,
            this, &MainWindow::on_previousButton_clicked);

    connect(ui->nextButton, &QPushButton::clicked,
            this, &MainWindow::on_nextButton_clicked);

    connect(ui->progressLabel, &ClickableProgressLabel::clickedAt,
            this, &MainWindow::on_progressClicked);

    connect(ui->playlistWidget, &QListWidget::itemDoubleClicked,
            this, &MainWindow::on_playlistItemDoubleClicked);

    connect(player, &QMediaPlayer::mediaStatusChanged,
            this, &MainWindow::on_mediaStatusChanged);

    connect(ui->playlistWidget, &DropPlaylistWidget::filesDropped,
            this, &MainWindow::on_filesDropped);

    connect(ui->playlistWidget->model(),
            &QAbstractItemModel::rowsMoved,
            this,
            &MainWindow::on_playlistRowsMoved);

    ui->playlistWidget->setContextMenuPolicy(
        Qt::CustomContextMenu
        );

    connect(ui->playlistWidget,
            &QListWidget::customContextMenuRequested,
            this,
            &MainWindow::on_playlistContextMenu);

    ui->playlistWidget->installEventFilter(this);

    connect(ui->openFolderButton,
            &QPushButton::clicked,
            this,
            &MainWindow::openFolder);

    connect(ui->muteButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                audioOutput->setMuted(
                    !audioOutput->isMuted()
                    );

                ui->muteButton->setText(
                    audioOutput->isMuted()
                        ? "Unmute"
                        : "Mute"
                    );
            });

    connect(ui->shuffleButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                playlistManager->setShuffle(
                    !playlistManager->shuffle()
                    );

                ui->shuffleButton->setText(
                    playlistManager->shuffle()
                        ? "Shuffle On"
                        : "Shuffle"
                    );
            });

    connect(ui->repeatButton,
            &QPushButton::clicked,
            this,
            [this]()
            {
                RepeatMode mode =
                    playlistManager->repeatMode();

                if (mode == RepeatMode::Off) {
                    mode = RepeatMode::All;
                    ui->repeatButton->setText("Repeat All");
                }
                else if (mode == RepeatMode::All) {
                    mode = RepeatMode::One;
                    ui->repeatButton->setText("Repeat One");
                }
                else {
                    mode = RepeatMode::Off;
                    ui->repeatButton->setText("Repeat Off");
                }

                playlistManager->setRepeatMode(mode);
            });

    connect(ui->playlistSearch,
            &QLineEdit::textChanged,
            this,
            [this](const QString &text)
            {
                for (int i = 0;
                     i < ui->playlistWidget->count();
                     ++i) {

                    QListWidgetItem *item =
                        ui->playlistWidget->item(i);

                    item->setHidden(
                        !item->text().contains(
                            text,
                            Qt::CaseInsensitive
                            )
                        );
                }
            });


    // This block always has to come at the end for settings to be saved
    const QString savedFolder =
        settingsManager->folder();

    if (!savedFolder.isEmpty() &&
        QDir(savedFolder).exists()) {

        showFolder(savedFolder);

        const int track =
            settingsManager->currentTrack();

        if (track >= 0 &&
            track < playlistManager->tracks().size()) {

            playlistManager->setCurrentTrack(track);

            ui->playlistWidget->setCurrentRow(track);

            savedPosition =
                settingsManager->position();

            restoringPlayback = true;

            player->setSource(
                QUrl::fromLocalFile(
                    playlistManager->currentFile()
                    )
                );
        }
    }

    //Fixes minor visual bug when openning again the player

    //Fixes and sets up properly the search bar
    ui->playlistSearch->setText(
        settingsManager->searchText()
        );

    //Another fix
    if (settingsManager->maximized())
        showMaximized();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addFiles(const QStringList &files)
{
    if (files.isEmpty()) return;

    const bool wasEmpty = playlistManager->tracks().isEmpty();
    playlistManager->addFiles(files);

    for (const QString &file : files)
        ui->playlistWidget->addItem(QFileInfo(file).fileName());

    if (wasEmpty) {
        playlistManager->setCurrentTrack(0);
        playCurrentTrack();
    }

    updatePlaylistCount();
}

void MainWindow::playCurrentTrack()
{
    const QString file = playlistManager->currentFile();

    if (file.isEmpty())
        return;

    player->setSource(QUrl::fromLocalFile(file));
    player->play();

    ui->pauseButton->setText("Pause");

    ui->playlistWidget->setCurrentRow(
        playlistManager->currentTrack()
        );
}

void MainWindow::on_openButton_clicked()
{
    addFiles(QFileDialog::getOpenFileNames(
        this, "Open Audio Files", QString(),
        "Audio Files (*.mp3 *.wav *.flac *.ogg *.m4a *.aac)"));
}

void MainWindow::on_filesDropped(const QStringList &files)
{
    addFiles(files);
}

void MainWindow::on_pauseButton_clicked()
{
    if (player->source().isEmpty())
        return;

    if (player->playbackState() == QMediaPlayer::PlayingState) {
        player->pause();
        ui->pauseButton->setText("Play");
    } else {
        player->play();
        ui->pauseButton->setText("Pause");
    }
}

void MainWindow::on_stopButton_clicked()
{
    player->stop();
    ui->pauseButton->setText("Play");
}

void MainWindow::on_volumeSlider_valueChanged(int value)
{
    audioOutput->setVolume(value / 100.0);
}

void MainWindow::on_durationChanged(qint64 value)
{
    duration = value;
    ui->totalTimeLabel->setText(formatTime(value));
}

void MainWindow::on_positionChanged(qint64 position)
{
    if (duration <= 0) return;

    const int filled = static_cast<int>((double(position) / duration) * 30);
    ui->progressLabel->setText(
        QString("/").repeated(filled) +
        QString(":").repeated(30 - filled)
        );
    ui->currentTimeLabel->setText(formatTime(position));
}

QString MainWindow::formatTime(qint64 milliseconds)
{
    const qint64 seconds = milliseconds / 1000;
    return QString("%1:%2")
        .arg(seconds / 60)
        .arg(seconds % 60, 2, 10, QChar('0'));
}

void MainWindow::updateMetadata()
{
    const QMediaMetaData metadata = player->metaData();

    const QString title =
        metadata.stringValue(QMediaMetaData::Title).isEmpty()
            ? "Unknown Title"
            : metadata.stringValue(QMediaMetaData::Title);

    const QString artist =
        metadata.stringValue(QMediaMetaData::ContributingArtist).isEmpty()
            ? "Unknown Artist"
            : metadata.stringValue(QMediaMetaData::ContributingArtist);

    const QString album =
        metadata.stringValue(QMediaMetaData::AlbumTitle).isEmpty()
            ? "Unknown Album"
            : metadata.stringValue(QMediaMetaData::AlbumTitle);

    ui->titleLabel->setText(title);
    ui->artistLabel->setText(artist);
    ui->albumLabel->setText(album);

    QImage image =
        metadata.value(QMediaMetaData::CoverArtImage).value<QImage>();

    if (image.isNull())
        image = metadata.value(QMediaMetaData::ThumbnailImage).value<QImage>();

    if (!image.isNull()) {
        ui->albumArtLabel->setPixmap(
            QPixmap::fromImage(image).scaled(
                ui->albumArtLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                ));
    } else {
        ui->albumArtLabel->clear();
        ui->albumArtLabel->setText("No Artwork");
    }
}

void MainWindow::on_progressClicked(double percentage)
{
    if (duration > 0)
        player->setPosition(
            static_cast<qint64>(duration * percentage)
            );
}

void MainWindow::on_playlistItemDoubleClicked(QListWidgetItem *item)
{
    if (!item)
        return;

    const QString path =
        item->data(Qt::UserRole).toString();

    const QString type =
        item->data(Qt::UserRole + 1).toString();

    if (type == "folder") {
        showFolder(path);
        return;
    }

    if (type == "file") {
        const int index =
            playlistManager->tracks().indexOf(path);

        if (index < 0)
            return;

        playlistManager->setCurrentTrack(index);
        playCurrentTrack();
    }
}

void MainWindow::on_mediaStatusChanged(QMediaPlayer::MediaStatus status)
{
    if (status == QMediaPlayer::LoadedMedia &&
        restoringPlayback) {

        player->setPosition(savedPosition);
        restoringPlayback = false;
        return;
    }

    if (status == QMediaPlayer::InvalidMedia) {

        restoringPlayback = false;

        QMessageBox::warning(
            this,
            "Playback Error",
            "This file can not be played. Skipping to the next track."
            );

        if (!playlistManager->tracks().isEmpty() &&
            !playlistManager->nextFile().isEmpty()) {

            playCurrentTrack();
        }

        return;
    }

    if (status != QMediaPlayer::EndOfMedia)
        return;

    if (playlistManager->tracks().isEmpty())
        return;

    if (playlistManager->repeatMode() == RepeatMode::One) {
        playCurrentTrack();
        return;
    }

    if (playlistManager->repeatMode() == RepeatMode::All) {
        if (!playlistManager->nextFile().isEmpty())
            playCurrentTrack();

        return;
    }

    if (!playlistManager->nextFile().isEmpty())
        playCurrentTrack();
}

void MainWindow::on_nextButton_clicked()
{
    if (!playlistManager->nextFile().isEmpty())
        playCurrentTrack();
}

void MainWindow::on_previousButton_clicked()
{
    if (!playlistManager->previousFile().isEmpty())
        playCurrentTrack();
}

void MainWindow::on_playlistContextMenu(const QPoint &position)
{
    QListWidgetItem *item =
        ui->playlistWidget->itemAt(position);

    QMenu menu(this);

    QAction *playAction =
        item ? menu.addAction("Play") : nullptr;

    QAction *playNextAction =
        item ? menu.addAction("Play Next") : nullptr;

    QAction *propertiesAction =
        item ? menu.addAction("Properties") : nullptr;

    QAction *removeAction =
        item ? menu.addAction("Remove") : nullptr;

    QAction *refreshAction =
        menu.addAction("Refresh");

    if (item)
        menu.addSeparator();

    QAction *clearAction =
        menu.addAction("Clear Playlist");

    QAction *selectedAction =
        menu.exec(
            ui->playlistWidget->viewport()->mapToGlobal(position)
            );

    if (selectedAction == playAction) {

        const QString type =
            item->data(Qt::UserRole + 1).toString();

        // Folders are opened instead of being played
        if (type == "folder") {
            showFolder(
                item->data(Qt::UserRole).toString()
                );
            return;
        }

        if (type != "file")
            return;

        const int index =
            ui->playlistWidget->row(item);

        if (index < 0 ||
            index >= playlistManager->tracks().size())
            return;

        playlistManager->setCurrentTrack(index);
        playCurrentTrack();
    }

    else if (selectedAction == playNextAction) {

        const QString type =
            item->data(Qt::UserRole + 1).toString();

        if (type != "file")
            return;

        const int index =
            ui->playlistWidget->row(item);

        const int current =
            playlistManager->currentTrack();

        if (index < 0 ||
            index >= playlistManager->tracks().size())
            return;

        int destination = current + 1;

        if (destination > playlistManager->tracks().size())
            destination = playlistManager->tracks().size();

        if (index == current ||
            index == destination)
            return;

        playlistManager->moveTrack(
            index,
            destination
            );
    }

    //If you want to add more properties you can add them through here
    else if (selectedAction == propertiesAction) {

        const QString type =
            item->data(Qt::UserRole + 1).toString();

        if (type != "file")
            return;

        const QString file =
            item->data(Qt::UserRole).toString();

        QFileInfo info(file);

        if (!info.exists())
            return;

        QString details;

        details +=
            "Name: " +
            info.fileName() +
            "\n";

        details +=
            "Path: " +
            info.absoluteFilePath() +
            "\n";

        details +=
            "Size: " +
            QString::number(
                info.size() / 1024.0,
                'f',
                1
                ) +
            " KB\n";

        details +=
            "Format: " +
            info.suffix().toUpper() +
            "\n";

        QMediaPlayer *propertyPlayer =
            new QMediaPlayer(this);

        connect(
            propertyPlayer,
            &QMediaPlayer::mediaStatusChanged,
            this,
            [this, propertyPlayer, details](
                QMediaPlayer::MediaStatus status)
            {
                if (status !=
                    QMediaPlayer::LoadedMedia)
                    return;

                const QMediaMetaData metadata =
                    propertyPlayer->metaData();

                const QString title =
                    metadata.stringValue(
                        QMediaMetaData::Title
                        );

                const QString artist =
                    metadata.stringValue(
                        QMediaMetaData::ContributingArtist
                        );

                const QString album =
                    metadata.stringValue(
                        QMediaMetaData::AlbumTitle
                        );

                QString result = details;

                result +=
                    "\nTitle: " +
                    (title.isEmpty()
                         ? "Unknown"
                         : title);

                result +=
                    "\nArtist: " +
                    (artist.isEmpty()
                         ? "Unknown"
                         : artist);

                result +=
                    "\nAlbum: " +
                    (album.isEmpty()
                         ? "Unknown"
                         : album);

                result +=
                    "\nDuration: " +
                    formatTime(
                        propertyPlayer->duration()
                        );

                QMessageBox::information(
                    this,
                    "File Properties",
                    result
                    );

                propertyPlayer->deleteLater();
            });

        connect(
            propertyPlayer,
            &QMediaPlayer::errorOccurred,
            this,
            [propertyPlayer](
                QMediaPlayer::Error,
                const QString &)
            {
                propertyPlayer->deleteLater();
            });

        propertyPlayer->setSource(
            QUrl::fromLocalFile(file)
            );
    }


    else if (selectedAction == refreshAction) {
        refreshCurrentFolder();
    }


    else if (selectedAction == removeAction) {

        const QString type =
            item->data(Qt::UserRole + 1).toString();

        if (type != "file")
            return;

        const int index =
            ui->playlistWidget->row(item);

        if (index < 0 ||
            index >= playlistManager->tracks().size())
            return;

        const bool current =
            index == playlistManager->currentTrack();

        playlistManager->removeTrack(index);

        delete ui->playlistWidget->takeItem(index);

        if (playlistManager->tracks().isEmpty()) {

            player->stop();
            ui->pauseButton->setText("Play");
        }
        else if (current) {

            playCurrentTrack();
        }
        else {

            ui->playlistWidget->setCurrentRow(
                playlistManager->currentTrack()
                );
        }

        updatePlaylistCount();
    }

    // Clears the playlist
    else if (selectedAction == clearAction) {

        QMessageBox::StandardButton reply =
            QMessageBox::question(
                this,
                "Clear Playlist",
                "Are you sure you want to clear the playlist?",
                QMessageBox::Yes |
                    QMessageBox::No
                );

        if (reply == QMessageBox::Yes) {

            playlistManager->clear();
            ui->playlistWidget->clear();

            player->stop();

            ui->pauseButton->setText("Play");

            updatePlaylistCount();
        }
    }
}

void MainWindow::on_playlistRowsMoved(
    const QModelIndex &parent,
    int start,
    int end,
    const QModelIndex &destination,
    int row)
{
    Q_UNUSED(parent);
    Q_UNUSED(end);
    Q_UNUSED(destination);

    if (start < 0 ||
        start >= ui->playlistWidget->count())
        return;

    QListWidgetItem *movedItem =
        ui->playlistWidget->item(start);

    if (!movedItem)
        return;

    const QString type =
        movedItem->data(Qt::UserRole + 1).toString();

    if (type != "file") {
        refreshCurrentFolder();
        return;
    }

    const int to =
        row > start ? row - 1 : row;

    if (to < 0 ||
        to >= playlistManager->tracks().size())
        return;

    playlistManager->moveTrack(start, to);

    ui->playlistWidget->setCurrentRow(
        playlistManager->currentTrack()
        );
}

bool MainWindow::eventFilter(QObject *object, QEvent *event)
{
    if (object == ui->playlistWidget &&
        event->type() == QEvent::KeyPress) {

        QKeyEvent *keyEvent =
            static_cast<QKeyEvent *>(event);

        if (keyEvent->key() == Qt::Key_Delete) {

            QListWidgetItem *item =
                ui->playlistWidget->currentItem();

            if (item) {

                const QString type =
                    item->data(Qt::UserRole + 1).toString();

                if (type != "file")
                    return true;

                const int index =
                    ui->playlistWidget->row(item);

                if (index >= 0 &&
                    index < playlistManager->tracks().size()) {

                    const bool current =
                        index == playlistManager->currentTrack();

                    playlistManager->removeTrack(index);

                    delete ui->playlistWidget->takeItem(index);

                    if (playlistManager->tracks().isEmpty()) {
                        player->stop();
                        ui->pauseButton->setText("Play");
                    }
                    else if (current) {
                        playCurrentTrack();
                    }
                    else {
                        ui->playlistWidget->setCurrentRow(
                            playlistManager->currentTrack()
                            );
                    }

                    updatePlaylistCount();
                }

                return true;
            }
        }
    }

    return QMainWindow::eventFilter(object, event);
}

void MainWindow::togglePlayback()
{
    on_pauseButton_clicked();
}

void MainWindow::nextTrack()
{
    on_nextButton_clicked();
}

void MainWindow::previousTrack()
{
    on_previousButton_clicked();
}

void MainWindow::toggleMute()
{
    audioOutput->setMuted(!audioOutput->isMuted());

    ui->muteButton->setText(
        audioOutput->isMuted()
            ? "Unmute"
            : "Mute"
        );
}

void MainWindow::updatePlaylistCount()
{
    const int count =
        playlistManager->tracks().size();

    ui->playlistCountLabel->setText(
        count == 1
            ? "1 track"
            : QString("%1 tracks").arg(count)
        );
}

void MainWindow::openFolder()
{
    const QString folder =
        QFileDialog::getExistingDirectory(
            this,
            "Select Music Folder"
            );

    if (folder.isEmpty())
        return;

    showFolder(folder);
}

void MainWindow::showFolder(const QString &folder)
{
    //This line avoids a memory leak
    currentFolder = folder;

    updateBreadcrumbs(folder);

    ui->playlistWidget->clear();
    playlistManager->clear();

    QDir dir(folder);

    const QFileInfoList folders =
        dir.entryInfoList(
            QDir::Dirs | QDir::NoDotAndDotDot,
            QDir::Name
            );

    for (const QFileInfo &folderInfo : folders) {
        QListWidgetItem *item =
            new QListWidgetItem(folderInfo.fileName());

        item->setData(Qt::UserRole, folderInfo.absoluteFilePath());
        item->setData(Qt::UserRole + 1, "folder");

        ui->playlistWidget->addItem(item);
    }

    const QStringList filters = {
        "*.mp3",
        "*.wav",
        "*.flac",
        "*.ogg",
        "*.m4a",
        "*.aac"
    };

    const QFileInfoList files =
        dir.entryInfoList(
            filters,
            QDir::Files,
            QDir::Name
            );

    QStringList audioFiles;

    for (const QFileInfo &fileInfo : files) {
        audioFiles.append(fileInfo.absoluteFilePath());
    }

    playlistManager->addFiles(audioFiles);

    for (const QString &file : audioFiles) {
        QListWidgetItem *item =
            new QListWidgetItem(QFileInfo(file).fileName());

        item->setData(Qt::UserRole, file);
        item->setData(Qt::UserRole + 1, "file");

        ui->playlistWidget->addItem(item);
    }

    updatePlaylistCount();
}

void MainWindow::refreshCurrentFolder()
{
    if (!currentFolder.isEmpty())
        showFolder(currentFolder);
}

//Function to add nested files on top of playlist
void MainWindow::updateBreadcrumbs(const QString &folder)
{
    while (breadcrumbLayout->count() > 0) {
        QLayoutItem *item = breadcrumbLayout->takeAt(0);

        if (item->widget())
            delete item->widget();

        delete item;
    }

    QDir dir(folder);

    QStringList parts =
        dir.absolutePath().split(
            '/',
            Qt::SkipEmptyParts
            );

    QString currentPath = "/";

    for (int i = 0; i < parts.size(); ++i) {

        if (i > 0)
            currentPath += "/";

        currentPath += parts.at(i);

        QPushButton *button =
            new QPushButton(parts.at(i).left(1));

        button->setFlat(true);
        button->setFixedSize(28, 28);
        button->setToolTip(parts.at(i));

        if (i == parts.size() - 1)
            button->setEnabled(false);

        const QString path = currentPath;

        connect(button,
                &QPushButton::clicked,
                this,
                [this, path]()
                {
                    showFolder(path);
                });

        breadcrumbLayout->addWidget(button);

        if (i < parts.size() - 1) {
            QLabel *separator =
                new QLabel(">");

            breadcrumbLayout->addWidget(separator);
        }
    }
    //This line avoids a memory leak
    breadcrumbLayout->addStretch();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settingsManager->saveVolume(
        audioOutput->volume()
        );

    settingsManager->saveMuted(
        audioOutput->isMuted()
        );

    settingsManager->saveShuffle(
        playlistManager->shuffle()
        );

    settingsManager->saveRepeatMode(
        static_cast<int>(
            playlistManager->repeatMode()
            )
        );

    settingsManager->saveFolder(
        currentFolder
        );

    settingsManager->saveCurrentTrack(
        playlistManager->currentTrack()
        );

    settingsManager->savePosition(
        player->position()
        );

    settingsManager->saveWindowGeometry(
        pos(),
        size()
        );

    settingsManager->saveSearchText(
        ui->playlistSearch->text()
        );

    settingsManager->saveMaximized(
        isMaximized()
        );

    event->accept();
}

void MainWindow::seekBackward()
{
    player->setPosition(
        qMax<qint64>(
            0,
            player->position() - 5000
            )
        );
}

void MainWindow::seekForward()
{
    player->setPosition(
        qMin<qint64>(
            player->duration(),
            player->position() + 5000
            )
        );
}

void MainWindow::volumeUp()
{
    const int value =
        ui->volumeSlider->value();

    ui->volumeSlider->setValue(
        qMin(100, value + 5)
        );
}

void MainWindow::volumeDown()
{
    const int value =
        ui->volumeSlider->value();

    ui->volumeSlider->setValue(
        qMax(0, value - 5)
        );
}

void MainWindow::shortcutStop()
{
    on_stopButton_clicked();
}

void MainWindow::updateUiScale()
{
    if (baseCentralSize.isEmpty())
        return;

    const qreal widthScale =
        static_cast<qreal>(ui->centralwidget->width()) /
        baseCentralSize.width();

    const qreal heightScale =
        static_cast<qreal>(ui->centralwidget->height()) /
        baseCentralSize.height();

    uiScale =
        qBound(0.75, qMin(widthScale, heightScale), 1.0);

    const int scaledWidth =
        qRound(baseCentralSize.width() * uiScale);

    const int scaledHeight =
        qRound(baseCentralSize.height() * uiScale);

    for (auto it = baseGeometries.cbegin();
         it != baseGeometries.cend();
         ++it) {

        QWidget *widget = it.key();
        const QRect baseGeometry = it.value();

        widget->setGeometry(
            qRound(baseGeometry.x() * uiScale),
            qRound(baseGeometry.y() * uiScale),
            qRound(baseGeometry.width() * uiScale),
            qRound(baseGeometry.height() * uiScale)
            );

        QFont font = baseFonts.value(widget);
        font.setPointSizeF(
            baseFonts.value(widget).pointSizeF() * uiScale
            );

        widget->setFont(font);
    }

    const QRect playlistBaseGeometry =
        baseGeometries.value(ui->playlistWidget);

    const int playlistX =
        qRound(playlistBaseGeometry.x() * uiScale);

    const int rightMargin =
        qRound((baseCentralSize.width() -
                playlistBaseGeometry.right() - 1) * uiScale);

    const int bottomMargin =
        qRound((baseCentralSize.height() -
                playlistBaseGeometry.bottom() - 1) * uiScale);

    const int playlistWidth =
        qMax(80, ui->centralwidget->width() -
                  playlistX - rightMargin);

    const int playlistHeight =
        qMax(80, ui->centralwidget->height() -
                  qRound(playlistBaseGeometry.y() * uiScale) -
                  bottomMargin);

    ui->playlistWidget->setGeometry(
        playlistX,
        qRound(playlistBaseGeometry.y() * uiScale),
        playlistWidth,
        playlistHeight
        );

    const QList<QPushButton *> breadcrumbButtons =
        ui->breadcrumbWidget->findChildren<QPushButton *>();

    for (QPushButton *button : breadcrumbButtons) {
        const int size =
            qMax(21, qRound(28 * uiScale));

        button->setFixedSize(size, size);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    updateUiScale();
}
