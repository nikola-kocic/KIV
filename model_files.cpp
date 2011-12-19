#include "model_files.h"
#include "helper.h"
#include "settings.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include <QFile>
#include <QDir>
#include <QtGui/QStyle>

//#define DEBUG_MODEL_FILES
#ifdef DEBUG_MODEL_FILES
#include <QDebug>
#endif

FilesModel::FilesModel(QObject *parent)
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

void FilesModel::setPath(const FileInfo &path)
{
#ifdef DEBUG_MODEL_FILES
    qDebug() << QDateTime::currentDateTime() << "FilesModel::setPath" << path.getFilePath();
#endif
    this->clear();

    this->setHorizontalHeaderLabels(QStringList() << tr("Name"));
    QFile zipFile(path.containerPath);
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
        for (int j = 0; j < file_path_parts.size(); ++j)
        {
            if (file_path_parts.at(j).size() > 0)
            {
                if (j < file_path_parts.size() - 1)
                {
                    node = AddNode(node, file_path_parts.at(j), TYPE_ARCHIVE_DIR);
                }
                else
                {
                    QFileInfo fi(archive_files.at(i));
                    if (isImage(fi))
                    {
                        node = AddNode(node, file_path_parts.at(j), TYPE_ARCHIVE_FILE);
                    }
                }
            }
        }
    }

    this->invisibleRootItem()->appendRow(root);

}

QStandardItem* FilesModel::AddNode(QStandardItem *node, const QString &name, int type)
{
    for (int i = 0; i < node->rowCount(); ++i)
    {
        if (node->child(i)->text() == name)
        {
            return node->child(i);
        }
    }

    QStandardItem *ntvi = new QStandardItem();
    ntvi->setData(type, ROLE_TYPE);
    ntvi->setText(name);
    ntvi->setToolTip(name);
    if (type == TYPE_ARCHIVE_DIR)
    {
        ntvi->setIcon(m_icon_dir);
        indexToInsertByName(node, name);
        node->insertRow(indexToInsertByName(node, name), ntvi);
    }
    else
    {
        ntvi->setIcon(m_icon_file);
        node->appendRow(ntvi);
    }
    return ntvi;
}

int FilesModel::indexToInsertByName(QStandardItem *parent, const QString &name)
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

QModelIndex FilesModel::getDirectory(const QString &path)
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

QModelIndex FilesModel::findIndexChild(const QString &text, const QModelIndex &root)
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

QModelIndex FilesModel::findRootIndexChild(const QString &text)
{
    for (int i = 0; i < this->rowCount(); ++i)
    {
        if (index(i, 0).data() == text)
        {
            return index(i, 0);
        }
    }
    return QModelIndex();
}
