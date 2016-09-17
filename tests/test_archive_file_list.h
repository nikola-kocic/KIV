#ifndef TEST_ARCHIVE_FILE_LIST_H
#define TEST_ARCHIVE_FILE_LIST_H

#include <QObject>
#include <QStringList>

#include "fixtures.h"
#include "include/IArchiveExtractor.h"
#include "archiveextractor.h"



class TestArchiveFileList: public QObject
{
    Q_OBJECT

public:
    TestArchiveFileList(
            const IArchiveExtractor *const archive_extractor,
            const DirStructureFixture &tds,
            const QString &archives_dir,
            QObject *parent=nullptr);

protected:
    const IArchiveExtractor *const m_archive_extractor;
    const QString m_archives_dir;
    const DirStructureFixture &m_tds;

    void commonCheck(const QString &archiveName,
                     const DirStructureFixture &tds);
};

class TestArchiveFileListZip: public TestArchiveFileList
{
    Q_OBJECT
    using TestArchiveFileList::TestArchiveFileList;

private Q_SLOTS:
    void testZip7zaDeflate();
    void testZipZipBzip2();
    void testZipZipDeflate();
    void testZipZipStore();
};

class TestArchiveFileListRar4: public TestArchiveFileList
{
    Q_OBJECT
    using TestArchiveFileList::TestArchiveFileList;

private Q_SLOTS:
    void testRar4();
};

class TestArchiveFileListRar5: public TestArchiveFileList
{
    Q_OBJECT
    using TestArchiveFileList::TestArchiveFileList;

private Q_SLOTS:
    void testRar5();
};
#endif // TEST_ARCHIVE_FILE_LIST_H
