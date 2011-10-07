#ifndef THUMBNAILVIEWER_H
#define THUMBNAILVIEWER_H

#include "komicviewer_enums.h"
#include "generatethumbnail.h"

#include <QtGui/qlistwidget.h>
#include <QtCore/qthread.h>

class ThumbnailViewer : public QListWidget
{
    Q_OBJECT

public:
    ThumbnailViewer (QWidget * parent = 0);
    void startShowingThumbnails(const QString& path);

private:
    void showThumbnails();
    QThread *threadThumbnails;
    int thumbCount;
    QString path;

private slots:
    void onThreadThumbsFinished();
    void onThumbnailFinished(QIcon);

};

#endif // THUMBNAILVIEWER_H
