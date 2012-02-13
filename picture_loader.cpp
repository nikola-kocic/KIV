#include "picture_loader.h"
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

#include <QFile>
#include <QBuffer>
#include <QImageReader>
#include <QPainter>

#ifdef WIN32
#include "windows.h"
#include "unrar/unrar.h"
#endif

//#define DEBUG_PICTURE_LOADER
#ifdef DEBUG_PICTURE_LOADER
#include <QDebug>
//#include <QTime>
#endif

QImage PictureLoader::getImage(const FileInfo &info)
{
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::getImage" << info.getPath();
#endif
    if (!info.fileExists())
    {
        return QImage(0,0);
    }
    else if (info.isInArchive())
    {
        return PictureLoader::getImageFromArchive(ThumbnailInfo(info, QSize(0, 0)));
    }
    else
    {
        return PictureLoader::getImageFromFile(ThumbnailInfo(info, QSize(0, 0)));
    }
    return QImage(0,0);
}

QImage PictureLoader::getThumbnail(const ThumbnailInfo &thumb_info)
{
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::getThumbnail" << thumb_info.getFileInfo().getPath() << thumb_info.getThumbSize();
#endif
    if (!thumb_info.getFileInfo().fileExists())
    {
        return QImage(0,0);
    }
    else if (thumb_info.getFileInfo().isInArchive())
    {
        return PictureLoader::styleThumbnail(PictureLoader::getImageFromArchive(thumb_info), thumb_info.getThumbSize());
    }
    else
    {
        return PictureLoader::styleThumbnail(PictureLoader::getImageFromFile(thumb_info), thumb_info.getThumbSize());
    }
    return QImage(0,0);
}

QImage PictureLoader::styleThumbnail(const QImage &img, const QSize &thumb_size)
{
    QImage thumb(thumb_size.width() + 2, thumb_size.height() + 2, QImage::Format_ARGB32);
    thumb.fill(qRgba(255, 255, 255, 0));
    QPainter painter(&thumb);
    QPoint imgPoint((thumb.width() - img.width()) / 2, (thumb.height() - img.height()) / 2);
    painter.setPen(Qt::lightGray);
    painter.drawImage(imgPoint, img);
    painter.drawRect(0, 0, thumb.width() - 1, thumb.height() - 1);
    painter.end();
    return thumb;
}

QImage PictureLoader::getImageFromFile(const ThumbnailInfo &thumb_info)
{
    QImageReader image_reader(thumb_info.getFileInfo().getPath());
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << thumb_info.getFileInfo().getPath() << image_reader.format() << image_reader.supportedImageFormats();
#endif
    if (!thumb_info.getThumbSize().isEmpty())
    {
        if (image_reader.size().height() > thumb_info.getThumbSize().height() || image_reader.size().width() > thumb_info.getThumbSize().width())
        {
            image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size(), thumb_info.getThumbSize()));
        }
    }
    return image_reader.read();
}

#ifdef WIN32
int CALLBACK CallbackProc(unsigned int msg, long myBufferPtr, long rarBuffer, long bytesProcessed)
{
    switch(msg)
    {
    case UCM_CHANGEVOLUME:
        return -1;

    case UCM_PROCESSDATA:
        memcpy(*(char**)myBufferPtr, (char*)rarBuffer, bytesProcessed);
        *(char**)myBufferPtr += bytesProcessed;
        return 1;

    case UCM_NEEDPASSWORD:
        return -1;
    }

    return 0;
}
#endif

QImage PictureLoader::getImageFromArchive(const ThumbnailInfo &thumb_info)
{
    bool passed = false;
    QBuffer out;
    char c;

    QFile zipFile(thumb_info.getFileInfo().getContainerPath());
    QuaZip zip(&zipFile);
    if (zip.open(QuaZip::mdUnzip))
    {
        zip.setFileNameCodec("UTF-8");

        if (zip.setCurrentFile(thumb_info.getFileInfo().zipImagePath()))
        {
            QuaZipFile file(&zip);
            if (file.open(QIODevice::ReadOnly))
            {
                out.open(QIODevice::WriteOnly);
                char buf[4096];
                int len = 0;
                while (file.getChar(&c))
                {
                    buf[len++] = c;
                    if (len >= 4096)
                    {
                        out.write(buf, len);
                        len = 0;
                    }
                }
                if (len > 0)
                {
                    out.write(buf, len);
                }
                out.close();
                passed = true;
            }
            else
            {
                return QImage(0, 0);
            }
        }
        else
        {
            return QImage(0, 0);
        }
    }

#ifdef WIN32
    if (!passed)
    {
        const QString rarImagePath = thumb_info.getFileInfo().rarImagePath();


        wchar_t* ArcName = new wchar_t[thumb_info.getFileInfo().getContainerPath().length() + 1];
        int sl = thumb_info.getFileInfo().getContainerPath().toWCharArray(ArcName);
        ArcName[sl] = 0;

        char *callBackBuffer;

        struct RAROpenArchiveDataEx OpenArchiveData;
        memset(&OpenArchiveData, 0, sizeof(OpenArchiveData));
        OpenArchiveData.ArcNameW = ArcName;
        OpenArchiveData.CmtBufSize = 0;
        OpenArchiveData.OpenMode = RAR_OM_EXTRACT;
        OpenArchiveData.Callback = CallbackProc;
        OpenArchiveData.UserData = (long) &callBackBuffer;

        HANDLE hArcData = RAROpenArchiveEx(&OpenArchiveData);

        if (OpenArchiveData.OpenResult == 0)
        {
            int RHCode, PFCode;
            struct RARHeaderDataEx HeaderData;

            HeaderData.CmtBuf = NULL;
            memset(&OpenArchiveData.Reserved, 0, sizeof(OpenArchiveData.Reserved));

            while ((RHCode = RARReadHeaderEx(hArcData, &HeaderData)) == 0)
            {
                const QString fileName = QString::fromWCharArray(HeaderData.FileNameW);
                if (fileName == rarImagePath)
                {
                    qint64 UnpSize = HeaderData.UnpSize + (((qint64)HeaderData.UnpSizeHigh) << 32);
                    char *buffer = new char[UnpSize];
                    callBackBuffer = buffer;

                    PFCode = RARProcessFileW(hArcData, RAR_TEST, NULL, NULL);

                    out.open(QIODevice::WriteOnly);
                    out.write(buffer, UnpSize);
                    out.close();
                    delete[] buffer;
                    break;
                }
                else
                {
                    if ((PFCode = RARProcessFileW(hArcData, RAR_SKIP, NULL, NULL)) != 0)
                    {
                        qWarning("%d", PFCode);
                        break;
                    }
                }
            }

            if (RHCode == ERAR_BAD_DATA)
            {
                qDebug("\nFile header broken");
            }

            RARCloseArchive(hArcData);

            passed = true;
        }
    }
#endif
    if (!passed)
        return QImage(0, 0);


#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::getImageFromZip" << "finished reading from zip" << thumb_info.getFileInfo().getPath();
#endif
    QImageReader image_reader(&out);
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << image_reader.format() << image_reader.supportedImageFormats();
#endif
    if (!thumb_info.getThumbSize().isEmpty())
    {
        if (image_reader.size().height() > thumb_info.getThumbSize().height() || image_reader.size().width() > thumb_info.getThumbSize().width())
        {
            image_reader.setScaledSize(PictureLoader::ThumbnailImageSize(image_reader.size(), thumb_info.getThumbSize()));
        }
    }
    return image_reader.read();
}

QSize PictureLoader::ThumbnailImageSize(const QSize &image_size, const QSize &thumb_size)
{
    QSize result = image_size;
    result.scale(thumb_size, Qt::KeepAspectRatio);
#ifdef DEBUG_PICTURE_LOADER
    qDebug() << QDateTime::currentDateTime().toString(Qt::ISODate) << "PictureLoader::ThumbnailImageSize" << "scaling image from" << image_size << "to" << result;
#endif
    return result;
}
