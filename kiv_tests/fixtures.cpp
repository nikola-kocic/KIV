#include "fixtures.h"

#include <QDir>


DirStructureFixture::DirStructureFixture(
        std::unique_ptr<IDirStructureFixturePaths> pathGiver
        , std::unique_ptr<IDirStructureFixtureDateTimes> dateGiver
        , std::unique_ptr<IDirStructureFixtureSize> sizeGiver
        , const QString &baseDir
        )
    : m_pathGiver(std::move(pathGiver))
    , m_dateGiver(std::move(dateGiver))
    , m_sizeGiver(std::move(sizeGiver))
    , m_baseDir(baseDir)
{

}

QList<ArchiveFileInfo> DirStructureFixture::getDirs() const {
    return QList<ArchiveFileInfo>()
            << get(DirStructureFixtureElement::rf1)
            << get(DirStructureFixtureElement::rf2)
            << get(DirStructureFixtureElement::rf3)
            << get(DirStructureFixtureElement::sf1)
            << get(DirStructureFixtureElement::sf2)
            << get(DirStructureFixtureElement::sf3)
               ;
}

QList<ArchiveFileInfo> DirStructureFixture::getFiles() const {
    return QList<ArchiveFileInfo>()
            << get(DirStructureFixtureElement::f1)
            << get(DirStructureFixtureElement::f2)
            << get(DirStructureFixtureElement::f3)
            << get(DirStructureFixtureElement::f4)
            << get(DirStructureFixtureElement::f5)
            << get(DirStructureFixtureElement::f6)
            << get(DirStructureFixtureElement::f7)
            << get(DirStructureFixtureElement::f8)
            << get(DirStructureFixtureElement::f9)
            << get(DirStructureFixtureElement::f10)
            << get(DirStructureFixtureElement::fcontent)
               ;
}

QString DirStructureFixture::getPath() const
{
    return QDir::cleanPath(m_baseDir + QDir::separator() + m_pathGiver->getDirName());
}

ArchiveFileInfo DirStructureFixture::get(DirStructureFixtureElement element) const
{
    return ArchiveFileInfo(
                m_pathGiver->getPath(element),
                m_dateGiver->getDateTime(element),
                m_sizeGiver->getSize(element));
}


QString DirStructureFixturePathsUnicodeOutsideBMP::getPath(DirStructureFixtureElement element) const
{
    switch (element)
    {
    case DirStructureFixtureElement::rf1:
        return QString::fromUtf8(u8"root folder 1 新增文字文件 😀/");
    case DirStructureFixtureElement::rf2:
        return QString::fromUtf8(u8"root folder 2 จงฝ่าฟันพัฒนาวิชาการ 😃/");
    case DirStructureFixtureElement::rf3:
        return QString::fromUtf8(u8"root folder 3 דג סקרן 😆/");
    case DirStructureFixtureElement::sf1:
        return QString::fromUtf8(u8"root folder 1 新增文字文件 😀/sub folder 1 香薰浴 😁/");
    case DirStructureFixtureElement::sf2:
        return QString::fromUtf8(u8"root folder 3 דג סקרן 😆/sub folder 1 şoföre 😇/");
    case DirStructureFixtureElement::sf3:
        return QString::fromUtf8(u8"root folder 3 דג סקרן 😆/sub folder 2 いろはにほへとちりぬるを 😉/");
    case DirStructureFixtureElement::f1:
        return QString::fromUtf8(u8"root folder 1 新增文字文件 😀/1 未命名 😂.png");
    case DirStructureFixtureElement::f2:
        return QString::fromUtf8(u8"root folder 1 新增文字文件 😀/sub folder 1 香薰浴 😁/2 ÄÖÜ äöü ß 😏.png");
    case DirStructureFixtureElement::f3:
        return QString::fromUtf8(u8"root folder 3 דג סקרן 😆/3 côté 😌.png");
    case DirStructureFixtureElement::f4:
        return QString::fromUtf8(u8"root folder 3 דג סקרן 😆/sub folder 1 şoföre 😇/4 イロハニホヘト 😈.png");
    case DirStructureFixtureElement::f5:
        return QString::fromUtf8(u8"root folder 3 דג סקרן 😆/sub folder 2 いろはにほへとちりぬるを 😉/5 þjófum 😊.png");
    case DirStructureFixtureElement::f6:
        return QString::fromUtf8(u8"root folder 3 דג סקרן 😆/sub folder 2 いろはにほへとちりぬるを 😉/6 tükörfúrógép 😋.png");
    case DirStructureFixtureElement::f7:
        return QString::fromUtf8(u8"root folder 2 จงฝ่าฟันพัฒนาวิชาการ 😃/7 экземпляр 😄.png");
    case DirStructureFixtureElement::f8:
        return QString::fromUtf8(u8"root folder 2 จงฝ่าฟันพัฒนาวิชาการ 😃/8 Pchnąć 😅.png");
    case DirStructureFixtureElement::f9:
        return QString::fromUtf8(u8"9 Ξεσκεπάζω 😍.png");
    case DirStructureFixtureElement::f10:
        return QString::fromUtf8(u8"10 Heizölrückstoßabdämpfung 😎.png");
    case DirStructureFixtureElement::fcontent:
        return QString::fromUtf8(u8"content.txt");
    }
    return QString();
}

QString DirStructureFixturePathsUnicodeOutsideBMP::getDirName() const
{
    return QString::fromUtf8(u8"extracted 🙂");
}



QString DirStructureFixturePathsUnicodeInsideBMP::getPath(DirStructureFixtureElement element) const
{
    switch (element)
    {
    case DirStructureFixtureElement::rf1:
        return QString::fromUtf8(u8"root folder 1 新增文字文件/");
    case DirStructureFixtureElement::rf2:
        return QString::fromUtf8(u8"root folder 2 จงฝ่าฟันพัฒนาวิชาการ/");
    case DirStructureFixtureElement::rf3:
        return QString::fromUtf8(u8"root folder 3 דג סקרן/");
    case DirStructureFixtureElement::sf1:
        return QString::fromUtf8(u8"root folder 1 新增文字文件/sub folder 1 香薰浴/");
    case DirStructureFixtureElement::sf2:
        return QString::fromUtf8(u8"root folder 3 דג סקרן/sub folder 1 şoföre/");
    case DirStructureFixtureElement::sf3:
        return QString::fromUtf8(u8"root folder 3 דג סקרן/sub folder 2 いろはにほへとちりぬるを/");
    case DirStructureFixtureElement::f1:
        return QString::fromUtf8(u8"root folder 1 新增文字文件/1 未命名.png");
    case DirStructureFixtureElement::f2:
        return QString::fromUtf8(u8"root folder 1 新增文字文件/sub folder 1 香薰浴/2 ÄÖÜ äöü ß.png");
    case DirStructureFixtureElement::f3:
        return QString::fromUtf8(u8"root folder 3 דג סקרן/3 côté.png");
    case DirStructureFixtureElement::f4:
        return QString::fromUtf8(u8"root folder 3 דג סקרן/sub folder 1 şoföre/4 イロハニホヘト.png");
    case DirStructureFixtureElement::f5:
        return QString::fromUtf8(u8"root folder 3 דג סקרן/sub folder 2 いろはにほへとちりぬるを/5 þjófum.png");
    case DirStructureFixtureElement::f6:
        return QString::fromUtf8(u8"root folder 3 דג סקרן/sub folder 2 いろはにほへとちりぬるを/6 tükörfúrógép.png");
    case DirStructureFixtureElement::f7:
        return QString::fromUtf8(u8"root folder 2 จงฝ่าฟันพัฒนาวิชาการ/7 экземпляр.png");
    case DirStructureFixtureElement::f8:
        return QString::fromUtf8(u8"root folder 2 จงฝ่าฟันพัฒนาวิชาการ/8 Pchnąć.png");
    case DirStructureFixtureElement::f9:
        return QString::fromUtf8(u8"9 Ξεσκεπάζω.png");
    case DirStructureFixtureElement::f10:
        return QString::fromUtf8(u8"10 Heizölrückstoßabdämpfung.png");
    case DirStructureFixtureElement::fcontent:
        return QString::fromUtf8(u8"content.txt");
    }
    return QString();
}

QString DirStructureFixturePathsUnicodeInsideBMP::getDirName() const
{
    return QString::fromUtf8(u8"extracted");
}


QDateTime DirStructureFixtureDateTimes::getDateTime(DirStructureFixtureElement element) const
{
    /*
     * (Used seconds divisible by 2 because unrar public functions expose only
     * MS-DOS timestamp format which has two second precision.)
     */
    switch (element)
    {
    case DirStructureFixtureElement::rf1:
        return QDateTime(QDate(2014, 11, 19), QTime(22, 58, 30), Qt::UTC);
    case DirStructureFixtureElement::rf2:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 2, 30), Qt::UTC);
    case DirStructureFixtureElement::rf3:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 5, 30), Qt::UTC);
    case DirStructureFixtureElement::sf1:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 0, 30), Qt::UTC);
    case DirStructureFixtureElement::sf2:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 7, 30), Qt::UTC);
    case DirStructureFixtureElement::sf3:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 9, 30), Qt::UTC);
    case DirStructureFixtureElement::f1:
        return QDateTime(QDate(2014, 11, 19), QTime(22, 59, 30), Qt::UTC);
    case DirStructureFixtureElement::f2:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 1, 30), Qt::UTC);
    case DirStructureFixtureElement::f3:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 6, 30), Qt::UTC);
    case DirStructureFixtureElement::f4:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 8, 30), Qt::UTC);
    case DirStructureFixtureElement::f5:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 10, 30), Qt::UTC);
    case DirStructureFixtureElement::f6:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 11, 30), Qt::UTC);
    case DirStructureFixtureElement::f7:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 3, 30), Qt::UTC);
    case DirStructureFixtureElement::f8:
        return QDateTime(QDate(2014, 11, 19), QTime(23, 4, 30), Qt::UTC);
    case DirStructureFixtureElement::f9:
        return QDateTime(QDate(2014, 11, 19), QTime(22, 56, 30), Qt::UTC);
    case DirStructureFixtureElement::f10:
        return QDateTime(QDate(2014, 11, 19), QTime(22, 55, 30), Qt::UTC);
    case DirStructureFixtureElement::fcontent:
        return QDateTime(QDate(2014, 11, 19), QTime(22, 57, 30), Qt::UTC);
    }
    return QDateTime();
}



quint32 DirStructureFixtureSize::getSize(DirStructureFixtureElement element) const
{

    switch (element)
    {
    case DirStructureFixtureElement::rf1:
    case DirStructureFixtureElement::rf2:
    case DirStructureFixtureElement::rf3:
    case DirStructureFixtureElement::sf1:
    case DirStructureFixtureElement::sf2:
    case DirStructureFixtureElement::sf3:
        return 0;
    case DirStructureFixtureElement::f1:
        return 77;
    case DirStructureFixtureElement::f2:
        return 84;
    case DirStructureFixtureElement::f3:
        return 84;
    case DirStructureFixtureElement::f4:
        return 85;
    case DirStructureFixtureElement::f5:
        return 84;
    case DirStructureFixtureElement::f6:
        return 82;
    case DirStructureFixtureElement::f7:
        return 83;
    case DirStructureFixtureElement::f8:
        return 79;
    case DirStructureFixtureElement::f9:
        return 83;
    case DirStructureFixtureElement::f10:
        return 84;
    case DirStructureFixtureElement::fcontent:
        return 1300;
    }
    return 0;
}


QDateTime DirStructureFixtureDateTimesLocal::getDateTime(DirStructureFixtureElement element) const
{
    const QDateTime dt = DirStructureFixtureDateTimes::getDateTime(element);
    QDateTime localDateTime= dt.addSecs(60 * 60); // Archives were created in UTC+1 timezone
    localDateTime.setTimeSpec(Qt::LocalTime);
    return localDateTime;
}


quint32 DirStructureFixtureSizeUnicodeInsideBMP::getSize(DirStructureFixtureElement element) const
{
    switch (element)
    {
    case DirStructureFixtureElement::fcontent:
        return 1220;
    default:
        return DirStructureFixtureSize::getSize(element);
    }
    return 0;
}
