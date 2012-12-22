#include <QtGui>

#include "archive_item.h"
#include "archive_model.h"
#include "helper.h"

#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include <QFileIconProvider>
#include <QFileSystemModel>
#include <QStyle>

//#define DEBUG_MODEL_FILES
#ifdef DEBUG_MODEL_FILES
#include <QDebug>
#endif

#include "unrar/archive_rar.h"

ArchiveModel::ArchiveModel(const QString &path, QObject *parent)
    : QAbstractItemModel(parent)
    , rootItem(new ArchiveItem("", QDateTime(), 0, "", ArchiveItem::TYPE_ARCHIVE))
    , m_icon_dir(QApplication::style()->standardIcon(QStyle::SP_DirIcon))
    , m_icon_file(QIcon::fromTheme("image-x-generic"))
    , m_type(ArchiveType::None)
{
#ifdef DEBUG_MODEL_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ArchiveModel::ArchiveModel" << path;
#endif
    if (m_icon_file.isNull())
    {
        m_icon_file = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
    }
    QFile archiveFile(path);
    QFileInfo archive_info(archiveFile);
    QFileIconProvider fip;

    // Try to open as ZIP
    QuaZip zip(&archiveFile);
    if (zip.open(QuaZip::mdUnzip))
    {
        zip.setFileNameCodec("UTF-8");

        QList<QuaZipFileInfo> archive_files = zip.getFileInfoList();
        //    QStringList archive_files = zip.getFileNameList();

        zip.close();
        if (zip.getZipError() != UNZ_OK)
        {
            qWarning("testRead(): zip.close(): %d", zip.getZipError());
            return;
        }

        /* Populate model */

        ArchiveItem *rootArchiveItem = new ArchiveItem(archive_info.fileName(),
                                                       archive_info.lastModified(),
                                                       archive_info.size(),
                                                       archive_info.absoluteFilePath(),
                                                       ArchiveItem::TYPE_ARCHIVE,
                                                       fip.icon(archive_info),
                                                       rootItem);
        rootItem->appendChild(rootArchiveItem);

        for (int i = 0; i < archive_files.size(); ++i)
        {
            ArchiveItem *node = rootArchiveItem;
            const QStringList file_path_parts = archive_files.at(i).name.split('/');
            QString folderPath = path + "/";
            for (int j = 0; j < file_path_parts.size(); ++j)
            {
                if (file_path_parts.at(j).size() > 0)
                {
                    folderPath.append(file_path_parts.at(j) + "/");
                    if (j < file_path_parts.size() - 1)
                    {
                        node = AddNode(file_path_parts.at(j),
                                       archive_files.at(i).dateTime,
                                       0,
                                       folderPath,
                                       ArchiveItem::TYPE_ARCHIVE_DIR,
                                       node);
                    }
                    else
                    {
                        const QFileInfo fi(archive_files.at(i).name);
                        if (Helper::isImageFile(fi))
                        {
                            const QString nodeFilePath = path + "/" + archive_files.at(i).name;
                            node = AddNode(file_path_parts.at(j),
                                           archive_files.at(i).dateTime,
                                           archive_files.at(i).uncompressedSize,
                                           nodeFilePath,
                                           ArchiveItem::TYPE_ARCHIVE_FILE,
                                           node);
                        }
                    }
                }
            }
        }
#ifdef DEBUG_MODEL_FILES
        qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ArchiveModel::ArchiveModel" << "ZIP";
#endif
        m_type = ArchiveType::Zip;
        return;
    }

    // Try to open as RAR
    if (ArchiveRar::loadlib())
    {

        struct RAROpenArchiveDataEx OpenArchiveData;
        memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));

        wchar_t* ArcNameW = new wchar_t[path.length() + 1];
        int sl = path.toWCharArray(ArcNameW);
        ArcNameW[sl] = 0;

        OpenArchiveData.ArcNameW = ArcNameW;
        OpenArchiveData.CmtBufSize = 0;
        OpenArchiveData.OpenMode = RAR_OM_LIST;
        OpenArchiveData.Callback = NULL;

        Qt::HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

        if (OpenArchiveData.OpenResult == 0)
        {
            int RHCode, PFCode;
            struct RARHeaderDataEx HeaderData;

            HeaderData.CmtBuf = NULL;
            memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

            ArchiveItem *rootArchiveItem = new ArchiveItem(archive_info.fileName(),
                                                           archive_info.lastModified(),
                                                           archive_info.size(),
                                                           archive_info.absoluteFilePath(),
                                                           ArchiveItem::TYPE_ARCHIVE,
                                                           fip.icon(archive_info),
                                                           rootItem);
            rootItem->appendChild(rootArchiveItem);

            while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0)
            {
                qint64 UnpSize = HeaderData.UnpSize + (((qint64)HeaderData.UnpSizeHigh) << 32);

                ArchiveItem *node = rootArchiveItem;
                const QString fileName = QString::fromWCharArray(HeaderData.FileNameW);

                const QStringList file_path_parts = fileName.split(QDir::separator());
                QString folderPath = path + "/";
                for (int j = 0; j < file_path_parts.size(); ++j)
                {
                    if (file_path_parts.at(j).size() > 0)
                    {
                        folderPath.append(file_path_parts.at(j) + "/");
                        if (j < file_path_parts.size() - 1)
                        {
                            node = AddNode(file_path_parts.at(j),
                                           dateFromDos(HeaderData.FileTime),
                                           0,
                                           folderPath,
                                           ArchiveItem::TYPE_ARCHIVE_DIR,
                                           node);
                        }
                        else
                        {
                            if (ArchiveRar::isDir(HeaderData))
                            {
                                node = AddNode(file_path_parts.at(j),
                                               dateFromDos(HeaderData.FileTime),
                                               0,
                                               folderPath,
                                               ArchiveItem::TYPE_ARCHIVE_DIR,
                                               node,
                                               true);
                            }
                            else
                            {
                                const QFileInfo fi(fileName);
                                if (Helper::isImageFile(fi))
                                {
                                    const QString nodeFilePath = path + "/" + fileName;
                                    node = AddNode(file_path_parts.at(j),
                                                   dateFromDos(HeaderData.FileTime),
                                                   UnpSize,
                                                   nodeFilePath,
                                                   ArchiveItem::TYPE_ARCHIVE_FILE,
                                                   node);
                                }
                            }
                        }
                    }
                }

                if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, NULL, NULL)) != 0)
                {
                    qWarning("%d", PFCode);
                    break;
                }
            }

            if (RHCode == ERAR_BAD_DATA)
            {
                qDebug("\nFile header broken");
            }

            RARCloseArchive(hArcData);

#ifdef DEBUG_MODEL_FILES
            qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "ArchiveModel::ArchiveModel" << "RAR";
#endif
            m_type = ArchiveType::Rar;
            delete ArcNameW;
            return;
        }
        delete ArcNameW;
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

ArchiveItem *ArchiveModel::AddNode(const QString &name, const QDateTime &date, const quint64 &bytes, const QString &path, const int type, ArchiveItem *parent, bool updateDate)
{
    for (int i = 0; i < parent->childCount(); ++i)
    {
        if (parent->child(i)->data(Qt::EditRole, ArchiveItem::col_name) == name)
        {
            if (updateDate == true)
            {
                parent->child(i)->setDate(date);
            }
            return parent->child(i);
        }
    }

    ArchiveItem *ntvi = new ArchiveItem(name,
                                        date,
                                        bytes,
                                        path,
                                        type,
                                        (type == ArchiveItem::TYPE_ARCHIVE_FILE ? m_icon_file : m_icon_dir),
                                        parent);
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

QDateTime ArchiveModel::dateFromDos(const uint dosTime)
{
    const ushort hiWord = (ushort)((dosTime & 0xFFFF0000) >> 16);
    const ushort loWord = (ushort)(dosTime & 0xFFFF);
    const uint year = ((hiWord & 0xFE00) >> 9) + 1980;
    const uint month = (hiWord & 0x01E0) >> 5;
    const uint day = hiWord & 0x1F;
    const uint hour = (loWord & 0xF800) >> 11;
    const uint minute = (loWord & 0x07E0) >> 5;
    const uint second = (loWord & 0x1F) << 1;
    return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
}
