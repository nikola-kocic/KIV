#include "test_archive_files.h"

#include <QDir>
#include <QCoreApplication>
#include <QFile>
#include <QtTest>

TestArchiveFiles::TestArchiveFiles(const IArchiveExtractor *const archive_extractor,
        const DirStructureFixture &tds,
        QObject *parent)
    : QObject(parent)
    , m_archive_extractor(archive_extractor)
    , m_tds(tds)
{
}

void TestArchiveFiles::readExtractedFiles()
{
    for (ArchiveFileInfo &afi : m_tds.getFiles())
    {
        const QString path = m_tds.getPath() + QDir::separator() + afi.m_name;
        QFile f(path);
        QVERIFY2(f.open(QIODevice::ReadOnly), path.toUtf8());
        m_file_content[afi.m_name] = f.readAll();
        QVERIFY2(m_file_content[afi.m_name].size() > 0, path.toUtf8());
    }
}

void TestArchiveFiles::commonCheck(const QString &archiveName, const DirStructureFixture &tds)
{
    const QString archivePath = QDir::cleanPath(
                m_tds.getBaseDir() + QDir::separator() + archiveName);
    for(ArchiveFileInfo &afi : tds.getFiles())
    {
        QByteArray buffer;
        m_archive_extractor->readFile(archivePath, afi.m_name, buffer, -1);
        QString fileContentKey = afi.m_name;
        QVERIFY2(buffer.size() > 0, QString(archivePath + "\n\n" + afi.m_name).toUtf8());
        QVERIFY2(m_file_content[fileContentKey].size() > 0, QString(archivePath + "\n\n" + afi.m_name).toUtf8());
        if (buffer != m_file_content[fileContentKey])
        {
            qDebug() << afi.m_name;
            qDebug() << QString(buffer);
            qDebug() << QString(m_file_content[fileContentKey]);
        }
        //QVERIFY2(buffer == m_file_content[fileContentKey], afi.name.toUtf8());
    }
}

void TestArchiveFilesRar4::initTestCase()
{
    readExtractedFiles();
}

void TestArchiveFilesRar4::testRar4()
{
    commonCheck(QString::fromUtf8(u8"RAR rar4 😷.rar"), m_tds);
}

void TestArchiveFilesZip::initTestCase()
{
    readExtractedFiles();
}

void TestArchiveFilesZip::testRar5()
{
    commonCheck(QString::fromUtf8(u8"RAR rar5 😸.rar"), m_tds);
}

void TestArchiveFilesZip::testZip7zaDeflate()
{
    commonCheck(QString::fromUtf8(u8"ZIP 7za deflate 😹.zip"), m_tds);
}

void TestArchiveFilesZip::testZipZipBzip2()
{
//    commonCheck(QString::fromUtf8(u8"ZIP zip bzip2 😺.zip"), m_tds);
}

void TestArchiveFilesZip::testZipZipDeflate()
{
    commonCheck(QString::fromUtf8(u8"ZIP zip deflate 😻.zip"), m_tds);
}

void TestArchiveFilesZip::testZipZipStore()
{
    commonCheck(QString::fromUtf8(u8"ZIP zip store 😼.zip"), m_tds);
}
