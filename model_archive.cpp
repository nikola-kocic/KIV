#include "model_archive.h"
#include "helper.h"
#include "settings.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include <QFile>
#include <QDir>
#include <QtGui/QStyle>
#include <QFileSystemModel>
#include <QLocale>

//#define DEBUG_MODEL_FILES
#ifdef DEBUG_MODEL_FILES
#include <QDebug>
#endif

ArchiveFilesModel::ArchiveFilesModel(QObject *parent)
    : QStandardItemModel(parent)
    , m_icon_dir(QApplication::style()->standardIcon(QStyle::SP_DirIcon))
    , m_icon_file(QApplication::style()->standardIcon(QStyle::SP_FileIcon))
{
}

QString ArchiveFilesModel::size(qint64 bytes)
{
    // According to the Si standard KB is 1000 bytes, KiB is 1024
    // but on windows sizes are calculated by dividing by 1024 so we do what they do.
    const qint64 kb = 1024;
    const qint64 mb = 1024 * kb;
    const qint64 gb = 1024 * mb;
    const qint64 tb = 1024 * gb;
    if (bytes >= tb)
        return QFileSystemModel::tr("%1 TB").arg(QLocale().toString(qreal(bytes) / tb, 'f', 3));
    if (bytes >= gb)
        return QFileSystemModel::tr("%1 GB").arg(QLocale().toString(qreal(bytes) / gb, 'f', 2));
    if (bytes >= mb)
        return QFileSystemModel::tr("%1 MB").arg(QLocale().toString(qreal(bytes) / mb, 'f', 1));
    if (bytes >= kb)
        return QFileSystemModel::tr("%1 KB").arg(QLocale().toString(bytes / kb));
    return QFileSystemModel::tr("%1 bytes").arg(QLocale().toString(bytes));
}

void ArchiveFilesModel::setPath(const FileInfo &info)
{
#ifdef DEBUG_MODEL_FILES
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "FilesModel::setPath" << path.getFilePath();
#endif
    this->clear();

    this->setHorizontalHeaderLabels(QStringList() << QFileSystemModel::tr("Name") << QFileSystemModel::tr("Size") << QFileSystemModel::tr("Date Modified"));
    QFile zipFile(info.container.canonicalFilePath());
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


    /* Populate treeViewFile */

    QFileIconProvider fip;
    QStandardItem *root = new QStandardItem();
//    root->setData(TYPE_ARCHIVE);
    QFileInfo zip_info(zipFile);
    root->setIcon(fip.icon(zip_info));
    root->setText(zip_info.fileName());

    QStandardItem *node = root;

    for (int i = 0; i < archive_files.size(); ++i)
    {
        node = root;
        QStringList file_path_parts = archive_files.at(i).name.split('/');
        QString folderPath = info.container.canonicalFilePath() + "/";
        for (int j = 0; j < file_path_parts.size(); ++j)
        {
            if (file_path_parts.at(j).size() > 0)
            {
                folderPath.append(file_path_parts.at(j) + "/");
                if (j < file_path_parts.size() - 1)
                {
                    node = AddNode(node, file_path_parts.at(j), TYPE_ARCHIVE_DIR, archive_files.at(i).dateTime);
                    node->setData(folderPath, QFileSystemModel::FilePathRole);
                }
                else
                {
                    QFileInfo fi(archive_files.at(i).name);
                    if (isImageFile(fi))
                    {
                        QString nodeFilePath = info.container.canonicalFilePath() + "/" + archive_files.at(i).name;
                        node = AddNode(node, file_path_parts.at(j), TYPE_ARCHIVE_FILE, archive_files.at(i).dateTime, archive_files.at(i).uncompressedSize);
                        node->setData(nodeFilePath, QFileSystemModel::FilePathRole);
                    }
                }
            }
        }
    }

    this->invisibleRootItem()->appendRow(root);

}

QStandardItem* ArchiveFilesModel::AddNode(QStandardItem *parent, const QString &name, const int type, const QDateTime &date, const quint64 bytes)
{
    for (int i = 0; i < parent->rowCount(); ++i)
    {
        if (parent->child(i)->text() == name)
        {
            return parent->child(i);
        }
    }

    QStandardItem *ntvi = new QStandardItem(name);
    ntvi->setData(type, ROLE_TYPE);
    ntvi->setToolTip(name);


    QStandardItem *date_item = new QStandardItem();
    date_item->setData(date, Qt::DisplayRole);
    ntvi->setData(date, ROLE_FILE_DATE);


    QString tooltip = QFileSystemModel::tr("Name") + ": " + ntvi->text() + "\n" +
            QFileSystemModel::tr("Date Modified") + ": " + date.toString(Qt::SystemLocaleShortDate);

    if (type == TYPE_ARCHIVE_DIR)
    {
        ntvi->setIcon(m_icon_dir);
        indexToInsertByName(parent, name);
        parent->insertRow(indexToInsertByName(parent, name), QList<QStandardItem *>() << ntvi << new QStandardItem() << date_item);
    }
    else
    {
        QStandardItem *size_item = new QStandardItem(size(bytes));
        size_item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);

        tooltip += "\n" + QFileSystemModel::tr("Size") + ": " + size_item->text();

        size_item->setToolTip(tooltip);

        ntvi->setIcon(m_icon_file);

        parent->appendRow(QList<QStandardItem *>() << ntvi << size_item << date_item);
    }

    ntvi->setToolTip(tooltip);
    date_item->setToolTip(tooltip);
    return ntvi;
}

int ArchiveFilesModel::indexToInsertByName(QStandardItem *parent, const QString &name)
{
    int lastFolderIndex = 0;
    for (int i = 0; i < parent->rowCount(); ++i)
    {
        int currentItemType = parent->child(lastFolderIndex)->data(ROLE_TYPE).toInt();
        if (currentItemType == TYPE_ARCHIVE_DIR)
        {
            if (parent->child(lastFolderIndex)->data(Qt::DisplayRole).toString().compare(name) > 0)
            {
                break;
            }

            ++lastFolderIndex;
        }
    }
    return lastFolderIndex;
}

QModelIndex ArchiveFilesModel::getDirectory(const QString &path)
{
    QModelIndex cri = invisibleRootItem()->child(0)->index();

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

QModelIndex ArchiveFilesModel::findIndexChild(const QString &text, const QModelIndex &root)
{
    if (!root.isValid()) return QModelIndex();
    for (int i = 0; root.child(i, 0).isValid(); ++i)
    {
        if (root.child(i, 0).data() == text)
        {
            return root.child(i, 0);
        }
    }
    return QModelIndex();
}
