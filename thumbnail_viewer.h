#ifndef THUMBNAILVIEWER_H
#define THUMBNAILVIEWER_H

#include "komicviewer_enums.h"
#include "thumbnail_generator.h"

#include <QtGui/qlistwidget.h>
#include <QtCore/qthread.h>

class ThumbnailViewer : public QListWidget
{
    Q_OBJECT

public:
    ThumbnailViewer (QWidget * parent = 0);
    void startShowingThumbnails(const QString& path, const QStringList &filters_archive, const QStringList &filters_image);

private:
    void showThumbnails();
    QThread *threadThumbnails;
    int thumbCount;
    QString path;
    ThumbnailGenerator* gt;
    int thumbSize;

private slots:
    void onThreadThumbsFinished();
    void onThumbnailFinished(QIcon);

};

#endif // THUMBNAILVIEWER_H
