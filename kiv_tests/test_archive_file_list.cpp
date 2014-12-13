#include "test_archive_file_list.h"
#include <QtTest>

namespace QTest {
    template<>
    char *toString(const ArchiveFileInfo &afi)
    {
        QByteArray ba = "ArchiveFileInfo(";
        ba += afi.name.toUtf8() + ", ";
        ba += QByteArray::number(afi.uncompressedSize) + ", ";
        ba += toString(afi.dateTime);
        ba += ")";
        return qstrdup(ba.data());
    }
}


TestArchiveFileList::TestArchiveFileList(
        const IArchiveExtractor * const archive_extractor,
        const DirStructureFixture &tds,
        const QString &archives_dir,
        QObject *parent)
    : QObject(parent)
    , m_archive_extractor(archive_extractor)
    , m_archives_dir(archives_dir)
    , m_tds(tds)
{

}
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args)
{
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

void TestArchiveFileList::commonCheck(const QString &archiveName, const DirStructureFixture &tds)
{
    std::vector<std::unique_ptr<const ArchiveFileInfo> > list;
    const QString archivePath = QDir::cleanPath(
                m_archives_dir + QDir::separator() + archiveName);
    int success = m_archive_extractor->getFileInfoList(archivePath, list);
    QCOMPARE(success, 0);

    std::vector<std::unique_ptr<const std::vector<std::unique_ptr<const ArchiveFileInfo> > > > dirsAndFiles;
    dirsAndFiles.push_back(make_unique<const std::vector<std::unique_ptr<const ArchiveFileInfo> > >(tds.getFiles()));
    dirsAndFiles.push_back(make_unique<const std::vector<std::unique_ptr<const ArchiveFileInfo> > >(tds.getDirs()));

    for (const std::unique_ptr<const std::vector<std::unique_ptr<const ArchiveFileInfo> > > &c : dirsAndFiles)
    {
        for (const std::unique_ptr<const ArchiveFileInfo> &e : *c)
        {
            const auto aIt = std::find_if(
                        list.cbegin(),
                        list.cend(),
                        [&e] (const std::unique_ptr<const ArchiveFileInfo> &c){ return c->name == e->name; }
            );
            QVERIFY2(aIt != list.end(), e->name.toUtf8());
            QCOMPARE((*aIt)->dateTime, e->dateTime);
            QCOMPARE(*(*aIt), *e);
        }
    }
}

void TestArchiveFileListRar4::testRar4()
{
    commonCheck(QString::fromUtf8(u8"RAR rar4 😷.rar"), m_tds);
}

void TestArchiveFileListRar5::testRar5()
{
    commonCheck(QString::fromUtf8(u8"RAR rar5 😸.rar"), m_tds);
}

void TestArchiveFileListZip::testZip7zaDeflate()
{
    commonCheck(QString::fromUtf8(u8"ZIP 7za deflate 😹.zip"), m_tds);
}

void TestArchiveFileListZip::testZipZipBzip2()
{
    commonCheck(QString::fromUtf8(u8"ZIP zip bzip2 😺.zip"), m_tds);
}

void TestArchiveFileListZip::testZipZipDeflate()
{
    commonCheck(QString::fromUtf8(u8"ZIP zip deflate 😻.zip"), m_tds);
}

void TestArchiveFileListZip::testZipZipStore()
{
    commonCheck(QString::fromUtf8(u8"ZIP zip store 😼.zip"), m_tds);
}
