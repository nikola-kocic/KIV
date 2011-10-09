#ifndef THUMBNAILVIEWER_H
#define THUMBNAILVIEWER_H

#include "komicviewer_enums.h"
#include "pixmap_loader.h"

#include <QtGui/qlistwidget.h>
#include <QtCore/qthread.h>
#include <QtCore/QTime>

class ThumbnailViewer : public QListWidget
{
    Q_OBJECT

public:
    ThumbnailViewer( const QStringList &filters_archive, const QStringList &filters_image, QWidget * parent = 0 );
    void startShowingThumbnails(const QString& path);
    bool folderChangedFlag;

private:
    void populateList();
    void showThumbnails();
    QThread *threadThumbnails;
    int thumbCount;
    QString path;
    PixmapLoader* pl;
    int thumbSize;
    QStringList filters_archive;
    QStringList filters_image;
    QTime timer;

private slots:
    void onThreadThumbsFinished();
    void onThumbnailFinished(QIcon);

};

#endif // THUMBNAILVIEWER_H
