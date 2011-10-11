#ifndef THUMBNAILVIEWER_H
#define THUMBNAILVIEWER_H

#include "komicviewer_enums.h"
#include "pixmap_loader.h"

#include <QtGui/qlistwidget.h>
#include <QtCore/qthread.h>
//#include <QtCore/QTime>

class ThumbnailViewer : public QListWidget
{
    Q_OBJECT

public:
    ThumbnailViewer( QWidget * parent = 0 );
    void startShowingThumbnails(const QString& path, bool isZip);
    void setViewMode(ViewMode mode);
    bool folderChangedFlag;
    void addItem(QListWidgetItem *item);

private:
    void populateList();
    void showThumbnail();
    QThread *threadThumbnails;
    int thumbCount;
    QString path;
    bool isZip;
    QString zipFileName;
    PixmapLoader* pl;
    int thumbSize;

private slots:
    void onThreadThumbsFinished();
    void onThumbnailFinished(QIcon);

};

#endif // THUMBNAILVIEWER_H
