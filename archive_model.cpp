#include <QtGui>

#include "archive_item.h"
#include "archive_model.h"
#include "helper.h"

#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#include <QFileIconProvider>

//#define DEBUG_MODEL_FILES
#ifdef DEBUG_MODEL_FILES
#include <QDebug>
#endif


ArchiveModel::ArchiveModel(const QString &path, QObject *parent)
    : QAbstractItemModel(parent)
    , rootItem(new ArchiveItem("", QDateTime(), 0, "", Helper::TYPE_ARCHIVE))
    , rootArchiveItem(new ArchiveItem("", QDateTime(), 0, "", Helper::TYPE_ARCHIVE, QIcon(), rootItem))
    , m_icon_dir(QApplication::style()->standardIcon(QStyle::SP_DirIcon))
    , m_icon_file(QIcon::fromTheme("image-x-generic"))
{
#ifdef DEBUG_MODEL_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "FilesModel::setPath" << path.getFilePath();
#endif

    QFile zipFile(path);
    QuaZip zip(&zipFile);
    if (!zip.open(QuaZip::mdUnzip))
    {
        qWarning("testRead(): zip.open(): %d", zip.getZipError());
        return;
    }
    zip.setFileNameCodec("UTF-8");

    QList<QuaZipFileInfo> archive_files = zip.getFileInfoList();
    //    QStringList archive_files = zip.getFileNameList();

    zip.close();
    if (zip.getZipError() != UNZ_OK) {
        qWarning("testRead(): zip.close(): %d", zip.getZipError());
        return;
    }

    /* Populate model */

    QFileInfo zip_info(zipFile);
    QFileIconProvider fip;
    rootArchiveItem = new ArchiveItem(zip_info.fileName(), zip_info.lastModified(), zip_info.size(), zip_info.absoluteFilePath(), Helper::TYPE_ARCHIVE, fip.icon(zip_info), rootItem);
    rootItem->appendChild(rootArchiveItem);

    for (int i = 0; i < archive_files.size(); ++i)
    {
        ArchiveItem *node = rootArchiveItem;
        QStringList file_path_parts = archive_files.at(i).name.split('/');
        QString folderPath = path + "/";
        for (int j = 0; j < file_path_parts.size(); ++j)
        {
            if (file_path_parts.at(j).size() > 0)
            {
                folderPath.append(file_path_parts.at(j) + "/");
                if (j < file_path_parts.size() - 1)
                {
                    node = AddNode(file_path_parts.at(j), archive_files.at(i).dateTime, 0, folderPath, Helper::TYPE_ARCHIVE_DIR, node);
                }
                else
                {
                    QFileInfo fi(archive_files.at(i).name);
                    if (Helper::isImageFile(fi))
                    {
                        QString nodeFilePath = path + "/" + archive_files.at(i).name;
                        node = AddNode(file_path_parts.at(j), archive_files.at(i).dateTime, archive_files.at(i).uncompressedSize, nodeFilePath, Helper::TYPE_ARCHIVE_FILE, node);
                    }
                }
            }
        }
    }
}

ArchiveModel::~ArchiveModel()
{
    delete rootItem;
}

int ArchiveModel::columnCount(const QModelIndex & /* parent */) const
{
    return rootItem->columnCount();
}


QVariant ArchiveModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ArchiveItem *item = getItem(index);

    return item->data(role, index.column());
}

Qt::ItemFlags ArchiveModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

ArchiveItem *ArchiveModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        ArchiveItem *item = static_cast<ArchiveItem*>(index.internalPointer());
        if (item) return item;
    }
    return rootItem;
}

QVariant ArchiveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch (section)
        {
        case 0:
            return QFileSystemModel::tr("Name");
        case 1:
            return QFileSystemModel::tr("Size");
        case 2:
            return QFileSystemModel::tr("Date Modified");
        }
    }

    return QVariant();
}

QModelIndex ArchiveModel::index(int row, int column, const QModelIndex &parent) const
{
    if (column < 0 || row < 0)
        return QModelIndex();

    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    ArchiveItem *parentItem = getItem(parent);

    ArchiveItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ArchiveModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    if (rowCount() == 0)
        return QModelIndex();

    ArchiveItem *childItem = getItem(index);
    ArchiveItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int ArchiveModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    ArchiveItem *parentItem = getItem(parent);

    return parentItem->childCount();
}

ArchiveItem *ArchiveModel::AddNode(const QString &name, const QDateTime &date, const quint64 &bytes, const QString &path, int type, ArchiveItem *parent)
{
    for (int i = 0; i < parent->childCount(); ++i)
    {
        if (parent->child(i)->data(Qt::EditRole, ArchiveItem::col_name) == name)
        {
            return parent->child(i);
        }
    }

    ArchiveItem *ntvi = new ArchiveItem(name, date, bytes, path, type, (type == Helper::TYPE_ARCHIVE_FILE ? m_icon_file : m_icon_dir), parent);
    parent->appendChild(ntvi);

    return ntvi;
}

QModelIndex ArchiveModel::getDirectory(const QString &path)
{
    QModelIndex cri = this->index(0, 0);

    QStringList file_path_parts = path.split('/', QString::SkipEmptyParts);
    for (int j = 0; j < file_path_parts.size(); ++j)
    {
        cri = findIndexChild(file_path_parts.at(j), cri);
        if (!cri.isValid())
        {
            return QModelIndex();
        }
    }

    return cri;
}

QModelIndex ArchiveModel::findIndexChild(const QString &text, const QModelIndex &root)
{
    if (!root.isValid()) return QModelIndex();
    for (int i = 0; root.child(i, 0).isValid(); ++i)
    {
        if (root.child(i, 0).data(Qt::EditRole) == text)
        {
            return root.child(i, 0);
        }
    }
    return QModelIndex();
}
