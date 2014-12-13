#ifndef TEST_ARCHIVE_FILES_H
#define TEST_ARCHIVE_FILES_H

#include <QObject>
#include <QHash>

#include "fixtures.h"
#include "kiv/include/IArchiveExtractor.h"
#include "kiv/src/archiveextractor.h"

class TestArchiveFiles : public QObject
{
    Q_OBJECT
public:
    explicit TestArchiveFiles(
            const IArchiveExtractor *const archive_extractor,
            const DirStructureFixture &tds,
            QObject *parent=nullptr);

protected:
    const IArchiveExtractor *const m_archive_extractor;
    const DirStructureFixture &m_tds;
    QHash<QString, QByteArray> m_file_content;
    void commonCheck(const QString &archiveName, const DirStructureFixture &tds);
    void readExtractedFiles();
};


class TestArchiveFilesZip : public TestArchiveFiles
{
    Q_OBJECT
    using TestArchiveFiles::TestArchiveFiles;

private Q_SLOTS:
    void initTestCase();
    void testRar5();
    void testZip7zaDeflate();
    void testZipZipBzip2();
    void testZipZipDeflate();
    void testZipZipStore();

};

class TestArchiveFilesRar4 : public TestArchiveFiles
{
    Q_OBJECT
    using TestArchiveFiles::TestArchiveFiles;

private Q_SLOTS:
    void initTestCase();
    void testRar4();

};
#endif // TEST_ARCHIVE_FILES_H
