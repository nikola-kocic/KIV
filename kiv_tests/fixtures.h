#ifndef FIXTURES_H
#define FIXTURES_H

#include <memory>
#include <vector>

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



class DirStructureFixturePathsUnicodeOutsideBMP
{
public:
    static QString getPath(DirStructureFixtureElement element);
    static QString getDirName();
};


class DirStructureFixturePathsUnicodeInsideBMP
{
public:
    static QString getPath(DirStructureFixtureElement element);
    static QString getDirName();
};



class DirStructureFixtureDateTimes
{
public:
    static QDateTime getDateTime(DirStructureFixtureElement element);
};

class DirStructureFixtureDateTimesLocal
{
public:
    static QDateTime getDateTime(DirStructureFixtureElement element);
};


class DirStructureFixtureSize
{
public:
    static quint32 getSize(DirStructureFixtureElement element);
};

class DirStructureFixtureSizeUnicodeInsideBMP
{
public:
    static quint32 getSize(DirStructureFixtureElement element);
};


class DirStructureFixture
{
public:
    DirStructureFixture(
            std::function<QString(DirStructureFixtureElement)> pathGiver
            , std::function<QString()> dirNameGiver
            , std::function<QDateTime(DirStructureFixtureElement)> dateGiver
            , std::function<quint32(DirStructureFixtureElement)> sizeGiver
            , const QString &baseDir
            );
    std::vector<ArchiveFileInfo> getDirs() const;
    std::vector<ArchiveFileInfo> getFiles() const;
    QString getPath() const;
    QString getBaseDir() const { return m_baseDir; }

protected:
    const std::function<QString(DirStructureFixtureElement)> m_pathGiver;
    const std::function<QString()> m_dirNameGiver;
    const std::function<QDateTime(DirStructureFixtureElement)> m_dateGiver;
    const std::function<quint32(DirStructureFixtureElement)> m_sizeGiver;
    const QString m_baseDir;
    ArchiveFileInfo get(DirStructureFixtureElement element) const;
};


#endif // FIXTURES_H
