#include "helper.h"

#include <QFileSystemModel>
#include <QLocale>
#include <QImageReader>

QStringList Helper::m_filters_archive = QStringList() << "zip" << "cbz";
QStringList Helper::m_filters_image = QStringList();

QStringList Helper::getFiltersArchive()
{
    return m_filters_archive;
}

QStringList Helper::getFiltersImage()
{
    if (m_filters_image.isEmpty())
    {
        for (int i = 0; i < QImageReader::supportedImageFormats().size(); ++i)
        {
            m_filters_image.append(QImageReader::supportedImageFormats().at(i));
        }
    }
    return m_filters_image;
}

QString Helper::size(const qint64 bytes)
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
