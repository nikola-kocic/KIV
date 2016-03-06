#include "init_test_data.h"

#include <QDir>
#include <QProcess>
#include <QDebug>


const QString InitTestData::getDateSetCommand(const QString &path, const QDateTime &dt) const
{
    const QString date_set_command = QString("touch -d %1 \"%2\"")
            .arg(dt.toLocalTime().toString("yyyy-MM-ddThh:mm:ss"), path);
    return date_set_command;
}


const QString InitTestData::getImageCreateCommand(const QString &path) const
{
    const QString text = QFileInfo(path).fileName().split(' ')[0];
    const QString image_create_command = QString(
                "convert -gravity south \
                -font -schumacher-clean-bold-r-normal--0-0-75-75-c-0-iso646.1991-irv \
                label:\"%1\" \"%2\"")
            .arg(text, path);
    return image_create_command;
}

const QString InitTestData::getImageCompressCommand(const QString &path) const
{
    const QString image_compress_command = QString("optipng -quiet -strip all \"%1\"")
            .arg(path);
    return image_compress_command;
}


bool InitTestData::generateFolderContentFile(const QString &path) const
{
    const QString out_path = QDir::cleanPath(
                path + QDir::separator() + "content.txt");
    const QString folder_content_command = QString(
                "tree -N -D --timefmt=%FT%T%z .");
    QProcess p;
    p.setWorkingDirectory(path);
    p.start(folder_content_command);
    p.waitForFinished();
    QFile f(out_path);
    bool success = f.open(QIODevice::WriteOnly);
    if (success)
    {
        success = f.write(p.readAllStandardOutput()) != -1;
    }
    return success;
}

void InitTestCommon::executeCommand(const QString &command, const QString &workingDir) const
{
    QProcess p;
    //qDebug() << command;
    p.setWorkingDirectory(workingDir);
    p.start(command);
    p.waitForFinished();
    QByteArray stderr = p.readAllStandardError();
    QByteArray stdout = p.readAllStandardOutput();
    if (!stderr.isEmpty() || !stdout.isEmpty())
    {
        qDebug() << command;
        qDebug() << stderr;
        qDebug() << stdout;
    }
}
void InitTestCommon::executeCommands(const QStringList &commands, const QString &workingDir) const
{
    for (const QString &command : commands)
    {
        executeCommand(command, workingDir);
    }
}

void InitTestData::createFiles(
        const std::vector<ArchiveFileInfo> &folders
        , const std::vector<ArchiveFileInfo> &files
        , std::function<QString(ArchiveFileInfo)> fFilePath
        , std::function<QString()> fWorkingDir
        ) const
{
    QStringList image_create_commands;
    QStringList date_set_commands;
    for (const ArchiveFileInfo &afi : folders)
    {
        const QString path = fFilePath(afi);
        Q_ASSERT_X(QDir().mkpath(path),
                   QString("makeTestData folders").toUtf8(),
                   path.toUtf8());
        date_set_commands.append(getDateSetCommand(path, afi.m_dateTime));
    }

    for (const ArchiveFileInfo &afi : files)
    {
        const QString path = fFilePath(afi);
        if (!afi.m_name.endsWith(".txt"))
        {
            image_create_commands.append(getImageCreateCommand(path));
            image_create_commands.append(getImageCompressCommand(path));
        }
        date_set_commands.append(getDateSetCommand(path, afi.m_dateTime));
    }
    const QString extracted_dir = fWorkingDir();
    executeCommands(image_create_commands, extracted_dir);
    executeCommands(date_set_commands, extracted_dir);
    generateFolderContentFile(extracted_dir);
    executeCommands(date_set_commands, extracted_dir);
}

InitTestArchives InitTestData::makeTestData(const DirStructureFixture &dsf) const
{
    const std::vector<ArchiveFileInfo> folders = dsf.getDirs();
    std::vector<ArchiveFileInfo> files = dsf.getFiles();
    const QString &dir = dsf.getPath();
    createFiles(folders,
             files,
             [&dir](ArchiveFileInfo afi) { return QDir::cleanPath(dir + QDir::separator() + afi.m_name); },
             [&dir]() { return dir; }
    );
    return InitTestArchives(dir);
}



const QStringList InitTestArchives::getArchiveCommandsZip() const
{
    return QStringList()
            << "zip --quiet -9 -r -Z store \"../ZIP zip store 😼.zip\" ."
            << "zip --quiet -9 -r -Z bzip2 \"../ZIP zip bzip2 😺.zip\" ."
            << "zip --quiet -9 -r -Z deflate \"../ZIP zip deflate 😻.zip\" ."
            << "7za a -mx=9 -r -tzip \"../ZIP 7za deflate 😹.zip\" ."
            << "rar a -r -m5 -ma5 \"../RAR rar5 😸.rar\" ."
               ;
}

const QStringList InitTestArchives::getArchiveCommandsRar4() const
{
    return QStringList()
            << "rar a -r -m5 -ma4 \"../RAR rar4 😷.rar\" ."
               ;
}

void InitTestArchives::createZip() const
{
    executeCommands(getArchiveCommandsZip(), m_src_dir);
}

void InitTestArchives::createRar4() const
{
    executeCommands(getArchiveCommandsRar4(), m_src_dir);
}
