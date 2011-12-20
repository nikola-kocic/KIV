#include "model_archive.h"
#include "helper.h"
#include "settings.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include <QFile>
#include <QDir>
#include <QtGui/QStyle>
#include <QFileSystemModel>

//#define DEBUG_MODEL_FILES
#ifdef DEBUG_MODEL_FILES
#include <QDebug>
#endif

ArchiveFilesModel::ArchiveFilesModel(QObject *parent)
    : QStandardItemModel(parent)
{
    m_icon_dir = QApplication::style()->standardIcon(QStyle::SP_DirIcon);
    m_icon_file = QApplication::style()->standardIcon(QStyle::SP_FileIcon);
}

QString bytesToSize(int bytes, int precision)
{
    int kilobyte = 1024;
    int megabyte = kilobyte * 1024;
    int gigabyte = megabyte * 1024;
    int terabyte = gigabyte * 1024;

    if ((bytes >= 0) && (bytes < kilobyte))
    {
        return QString::number(bytes) + " B";

    }
    else if ((bytes >= kilobyte) && (bytes < megabyte))
    {
        return QString::number(((float)bytes / kilobyte), 'f', precision) + " KiB";

    }
    else if ((bytes >= megabyte) && (bytes < gigabyte))
    {
        return QString::number(((float)bytes / megabyte), 'f', precision) + " MiB";

    }
    else if ((bytes >= gigabyte) && (bytes < terabyte))
    {
        return QString::number(((float)bytes / gigabyte), 'f', precision) + " GiB";

    }
    else if (bytes >= terabyte)
    {
        return QString::number(((float)bytes / terabyte), 'f', precision) + " TiB";
    }
    else
    {
        return QString::number(bytes) + " B";
    }
}

void ArchiveFilesModel::setPath(const FileInfo &info)
{
#ifdef DEBUG_MODEL_FILES
    qDebug() << QDateTime::currentDateTime() << "FilesModel::setPath" << path.getFilePath();
#endif
    this->clear();

    this->setHorizontalHeaderLabels(QStringList() << tr("Name"));
    QFile zipFile(info.container.canonicalFilePath());
    QuaZip zip(&zipFile);
    if (!zip.open(QuaZip::mdUnzip))
    {
        qWarning("testRead(): zip.open(): %d", zip.getZipError());
        return;
    }
    zip.setFileNameCodec("UTF-8");

    QStringList archive_files = zip.getFileNameList();

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
        QStringList file_path_parts = archive_files.at(i).split('/');
        QString folderPath = info.container.canonicalFilePath() + "/";
        for (int j = 0; j < file_path_parts.size(); ++j)
        {
            if (file_path_parts.at(j).size() > 0)
            {
                folderPath.append(file_path_parts.at(j) + "/");
                if (j < file_path_parts.size() - 1)
                {
                    node = AddNode(node, file_path_parts.at(j), TYPE_ARCHIVE_DIR);
                    node->setData(folderPath, QFileSystemModel::FilePathRole);
                }
                else
                {
                    QFileInfo fi(archive_files.at(i));
                    if (isImageFile(fi))
                    {
                        QString nodeFilePath = info.container.canonicalFilePath() + "/" + archive_files.at(i);
                        node = AddNode(node, file_path_parts.at(j), TYPE_ARCHIVE_FILE);
                        node->setData(nodeFilePath, QFileSystemModel::FilePathRole);
                    }
                }
            }
        }
    }

    this->invisibleRootItem()->appendRow(root);

}

QStandardItem* ArchiveFilesModel::AddNode(QStandardItem *parent, const QString &name, int type)
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
    if (type == TYPE_ARCHIVE_DIR)
    {
        ntvi->setIcon(m_icon_dir);
        indexToInsertByName(parent, name);
        parent->insertRow(indexToInsertByName(parent, name), ntvi);
    }
    else
    {
        ntvi->setIcon(m_icon_file);
        parent->appendRow(ntvi);
    }
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
