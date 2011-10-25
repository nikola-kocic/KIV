#include "archive_model.h"
#include "komicviewer_enums.h"
#include "settings.h"
#include "system_icons.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

//#include <QtCore/qdebug.h>
#include <QtCore/qfile.h>
#include <QtGui/qimagereader.h>
#include <QtCore/qdir.h>

void ArchiveModel::setPath(const FileInfo &info)
{
    this->clear();

    if (info.zipPathToImage.isEmpty())
    {
        QDir dir(info.containerPath);

        QFileInfoList list = dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot, QDir::DirsFirst | QDir::Name);
        QFileIconProvider fip;

        for (int i = 0; i < list.count(); ++i)
        {
            QStandardItem *item = 0;

            QFileInfo info = list.at(i);
            if (info.isDir())
            {
                item = new QStandardItem();
                item->setData(TYPE_DIR, ROLE_TYPE);
            }
            else if (isArchive(info))
            {
                item = new QStandardItem();
                item->setData(TYPE_ARCHIVE, ROLE_TYPE);
            }
            else if (isImage(info))
            {
                item = new QStandardItem();
                item->setData(TYPE_FILE, ROLE_TYPE);
            }

            if (item != 0)
            {
                item->setText(info.fileName());
                item->setIcon(fip.icon(info));
                item->setToolTip(item->text());

                this->invisibleRootItem()->appendRow(item);
            }
        }
    }
    else
    {
        this->clear();

        QFile zipFile(info.containerPath);
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


        //Populate treeViewFile

        QFileIconProvider fip;
        QStandardItem *root = new QStandardItem();
        root->setData(TYPE_ARCHIVE);
        QFileInfo zip_info(zipFile);
        root->setIcon(fip.icon(zip_info));
        root->setText(zip_info.fileName());

        QStandardItem *node = root;

        for (int i = 0; i < archive_files.count(); ++i)
        {
            node = root;
            QStringList file_path_parts = archive_files.at(i).split('/');
            for (int j = 0; j < file_path_parts.count(); ++j)
            {
                if (file_path_parts.at(j).count() > 0)
                {
                    if (j < file_path_parts.count() - 1)
                    {
                        node = AddNode(node, file_path_parts.at(j), TYPE_ARCHIVE_DIR);
                    }
                    else
                        //if (j == file_path_parts.count() - 1)
                    {
                        QFileInfo fi(archive_files.at(i));
                        if (QImageReader::supportedImageFormats().contains(fi.suffix().toLower().toLocal8Bit()))
                        {
                            node = AddNode(node, file_path_parts.at(j), TYPE_ARCHIVE_FILE);
                        }
                    }
                }
            }
        }

        this->invisibleRootItem()->appendRow(root);

    }
}

QStandardItem* ArchiveModel::AddNode(QStandardItem *node, const QString &name, int type)
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
    if (type == TYPE_DIR || type == TYPE_ARCHIVE_DIR)
    {
        ntvi->setIcon(SystemIcons::getDirectoryIcon());
        int lastFolderIndex = 0;
        for (int i = 0; i < node->rowCount(); ++i)
        {
            int currentItemType = node->child(lastFolderIndex)->data(ROLE_TYPE).toInt();
            if (currentItemType == TYPE_DIR || currentItemType == TYPE_ARCHIVE_DIR)
            {
                if (node->child(lastFolderIndex)->data(Qt::DisplayRole).toString().compare(name) > 0)
                {
                    break;
                }

                ++lastFolderIndex;
            }
        }
        node->insertRow(lastFolderIndex, ntvi);
    }
    else
    {
        ntvi->setIcon(SystemIcons::getFileIcon());
        node->appendRow(ntvi);
    }
    return ntvi;
}