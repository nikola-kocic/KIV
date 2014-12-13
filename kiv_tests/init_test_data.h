#ifndef INIT_TEST_DATA_H
#define INIT_TEST_DATA_H

#include <functional>
#include <memory>
#include <vector>
#include <QString>

#include "fixtures.h"


class InitTestCommon
{
protected:
    void executeCommands(const QStringList &commands, const QString &workingDir) const;
    void executeCommand(const QString &command, const QString &workingDir) const;
};

class InitTestArchives : public InitTestCommon
{
    friend class InitTestData;
protected:
    InitTestArchives(const QString &src_dir)
        :m_src_dir(src_dir)
    {}

    QString m_src_dir;
    const QStringList getArchiveCommandsZip() const;
    const QStringList getArchiveCommandsRar4() const;
public:

    void createZip() const;
    void createRar4() const;
};

class InitTestData : InitTestCommon
{
public:
    InitTestArchives makeTestData(const DirStructureFixture &dsf) const;

protected:
    const QStringList getDateSetCommands(const ArchiveFileInfo& afi, const QString &base_path) const;
    const QString getImageCreateCommand(const QString &path) const;
    bool generateFolderContentFile(const QString &path) const;
    const QString getImageCompressCommand(const QString &path) const;
    const QString getDateSetCommand(const QString &path, const QDateTime &dt) const;
    void createFiles(
            const std::vector<std::unique_ptr<const ArchiveFileInfo> > &folders
            , const std::vector<std::unique_ptr<const ArchiveFileInfo> > &files
            , std::function<QString(ArchiveFileInfo)> f
            , std::function<QString()> fWorkingDir) const;
};



#endif // INIT_TEST_DATA_H
