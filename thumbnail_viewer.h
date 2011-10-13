#ifndef THUMBNAILVIEWER_H
#define THUMBNAILVIEWER_H

#include "komicviewer_enums.h"
#include "pixmap_loader.h"
#include "archive_model.h"

#include <QtGui/qlistview.h>
#include <QtCore/qthread.h>
#include <QtGui/qstandarditemmodel.h>
//#include <QtCore/QTime>

class ThumbnailViewer : public QListView
{
    Q_OBJECT

public:
    ThumbnailViewer(ArchiveModel *am, QWidget * parent = 0);
    void setViewMode(ViewMode mode);
    bool folderChangedFlag;
    void setCurrentDirectory(const QString &filePath, bool isZip = false, const QString &zipFileName = "");

private:
    void populateList();
    void showThumbnail();
    QThread *threadThumbnails;

    int thumbCount;
    QString path;
    bool isZip;
    ArchiveModel *am;
    QString zipFileName;
    PixmapLoader* pl;
    int thumbSize;
    QStandardItemModel *model;
    void startShowingThumbnails();


private slots:
    void onThreadThumbsFinished();
    void onThumbnailFinished(QIcon);
    void OnTreeViewArchiveDirsCurrentChanged ( const QModelIndex & current, const QModelIndex & previous );

};

#endif // THUMBNAILVIEWER_H
