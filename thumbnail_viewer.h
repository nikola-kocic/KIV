#ifndef THUMBNAILVIEWER_H
#define THUMBNAILVIEWER_H

#include "komicviewer_enums.h"
#include "pixmap_loader.h"

#include <QtGui/qlistview.h>
#include <QtCore/qthread.h>
#include <QtGui/qstandarditemmodel.h>
#include <QtGui/qfilesystemmodel.h>
//#include <QtCore/QTime>

class ThumbnailViewer : public QListView
{
    Q_OBJECT

public:
    ThumbnailViewer(QWidget * parent = 0);
    void setViewMode(ViewMode mode);
    bool folderChangedFlag;
    void setCurrentDirectory(const QString &path, bool isZip = false, const QString &zipFileName = "");

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
    QFileSystemModel *fsm;

private slots:
    void onThreadThumbsFinished();
    void onThumbnailFinished(QIcon);

};

#endif // THUMBNAILVIEWER_H
