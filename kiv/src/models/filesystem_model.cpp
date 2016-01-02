#include "kiv/src/models/filesystem_model.h"

#include <QDateTime>
#include <QFileInfo>

#include "kiv/src/helper.h"

FileSystemModel::FileSystemModel(QObject *parent)
    : QFileSystemModel(parent)
{
    /* Start modelFilesystem */
    QStringList filters;
    const QStringList filtersArchive = Helper::filtersArchive;
    for (const QString &filterArchive : filtersArchive)
    {
        filters.append("*." + filterArchive);
    }
    const QStringList filtersImage = Helper::getFiltersImage();
    for (const QString &filterImage : filtersImage)
    {
        filters.append("*." + filterImage);
    }

    setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    setNameFilterDisables(false);
    setNameFilters(filters);
    setRootPath("");
}

QVariant FileSystemModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::TextAlignmentRole:
    {
        if (index.column() == 1)
        {
            return int(Qt::AlignRight | Qt::AlignVCenter);
        }
        break;
    }
    case Qt::ToolTipRole:
    {
        const QFileInfo fi = this->fileInfo(index);
        QString tooltip = (QFileSystemModel::tr("Name") + ": " + fi.fileName()
                           + "\n" + QFileSystemModel::tr("Date Modified") + ": "
                           + fi.lastModified().toString(
                               Qt::SystemLocaleShortDate));
        if (!this->isDir(index))
        {
            tooltip.append("\n" + QFileSystemModel::tr("Size") + ": "
                           + Helper::size(fi.size()));
        }
        return tooltip;
    }
    default:
        break;
    }
    return QFileSystemModel::data(index, role);
}
