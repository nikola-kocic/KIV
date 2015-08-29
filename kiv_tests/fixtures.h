#ifndef FIXTURES_H
#define FIXTURES_H

#include <memory>

#include "../kiv/src/enums.h"


enum class DirStructureFixtureElement
{
    // rf = root folder
    rf1,
    rf2,
    rf3,

    // sf = subfolder
    sf1,
    sf2,
    sf3,

    // f = file
    f1,
    f2,
    f3,
    f4,
    f5,
    f6,
    f7,
    f8,
    f9,
    f10,
    fcontent
};



class IDirStructureFixturePaths
{
public:
    virtual QString getPath(DirStructureFixtureElement element) const = 0;
    virtual QString getDirName() const = 0;
};

class DirStructureFixturePathsUnicodeOutsideBMP : public IDirStructureFixturePaths
{
public:
    QString getPath(DirStructureFixtureElement element) const override final;
    QString getDirName() const override final;
};


class DirStructureFixturePathsUnicodeInsideBMP : public IDirStructureFixturePaths
{
public:
    QString getPath(DirStructureFixtureElement element) const override final;
    QString getDirName() const override final;
};




class IDirStructureFixtureDateTimes
{
public:
    virtual QDateTime getDateTime(DirStructureFixtureElement element) const = 0;
};

class DirStructureFixtureDateTimes : public IDirStructureFixtureDateTimes
{
public:
    QDateTime getDateTime(DirStructureFixtureElement element) const override;
};

class DirStructureFixtureDateTimesLocal : public DirStructureFixtureDateTimes
{
public:
    QDateTime getDateTime(DirStructureFixtureElement element) const override final;
};



class IDirStructureFixtureSize
{
public:
    virtual quint32 getSize(DirStructureFixtureElement element) const = 0;
};

class DirStructureFixtureSize : public IDirStructureFixtureSize
{
public:
    quint32 getSize(DirStructureFixtureElement element) const override;
};

class DirStructureFixtureSizeUnicodeInsideBMP : public DirStructureFixtureSize
{
public:
    quint32 getSize(DirStructureFixtureElement element) const override final;
};


class DirStructureFixture
{
public:
    DirStructureFixture(
            std::unique_ptr<IDirStructureFixturePaths> pathGiver
            , std::unique_ptr<IDirStructureFixtureDateTimes> dateGiver
            , std::unique_ptr<IDirStructureFixtureSize> sizeGiver
            , const QString &baseDir
            );
    std::vector<ArchiveFileInfo> getDirs() const;
    std::vector<ArchiveFileInfo> getFiles() const;
    QString getPath() const;
    QString getBaseDir() const { return m_baseDir; }

protected:
    const std::unique_ptr<IDirStructureFixturePaths> m_pathGiver;
    const std::unique_ptr<IDirStructureFixtureDateTimes> m_dateGiver;
    const std::unique_ptr<IDirStructureFixtureSize> m_sizeGiver;
    const QString m_baseDir;
    ArchiveFileInfo get(DirStructureFixtureElement element) const;
};


#endif // FIXTURES_H
