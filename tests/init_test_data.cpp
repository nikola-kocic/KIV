#include "init_test_data.h"

#include <QDebug>
#include <QDir>
#include <QProcess>

namespace {
class CommandData
{
public:
    CommandData(QString program, QStringList arguments)
        : program(std::move(program))
        , arguments(std::move(arguments))
    {}

    QString program;
    QStringList arguments;
};

const CommandData getDateSetCommand(const QString &path, const QDateTime &dt)
{
    return CommandData("touch", {"-d", dt.toLocalTime().toString("yyyy-MM-ddThh:mm:ss"), path});
}

const CommandData getImageCreateCommand(const QString &path)
{
    const QString text = QFileInfo(path).fileName().split(' ')[0];
    return CommandData(
        "convert",
        {"-gravity",
         "south",
         "-font",
         "-schumacher-clean-bold-r-normal--0-0-75-75-c-0-iso646.1991-irv",
         QString("label:\"%1\"").arg(text),
         path});
}

const CommandData getImageCompressCommand(const QString &path)
{
    return CommandData("optipng", {"-quiet", "-strip", "all", path});
}

bool generateFolderContentFile(const QDir &path)
{
    const QString out_path = path.filePath("content.txt");
    const QString folder_content_command = QString("tree");
    const QStringList folder_content_command_args = {"-N", "-D", "--timefmt=%FT%T%z", "."};
    auto p = QProcess{};
    p.setWorkingDirectory(path.absolutePath());
    p.start(folder_content_command, folder_content_command_args);
    p.waitForFinished();
    QFile f(out_path);
    bool success = f.open(QIODevice::WriteOnly);
    if (success) {
        success = f.write(p.readAllStandardOutput()) != -1;
    }
    return success;
}

bool executeCommand(const CommandData &command, const QDir &workingDir)
{
    bool success = false;
#ifndef WIN32
    QProcess p;
    //qDebug() << command;
    p.setWorkingDirectory(workingDir.absolutePath());
    p.start(command.program, command.arguments);
    success = p.waitForFinished();
    if (!success) {
        QProcess::ProcessError pe = p.error();
        qDebug() << "Error" << pe << "running \"" << command.program << " " << command.arguments
                 << "\"";
    }
    QByteArray stderr = p.readAllStandardError();
    QByteArray stdout = p.readAllStandardOutput();
    if (!stderr.isEmpty() || !stdout.isEmpty()) {
        qDebug() << command.program << " " << command.arguments;
        qDebug() << stderr;
        qDebug() << stdout;
    }
#endif
    return success;
}

bool executeCommands(const std::vector<CommandData> &commands, const QDir &workingDir)
{
    bool success = true;
    for (const CommandData &command : commands) {
        success = executeCommand(command, workingDir);
    }
    return success;
}

void createFiles(
    const std::vector<ArchiveFileInfo> &folders,
    const std::vector<ArchiveFileInfo> &files,
    std::function<QString(ArchiveFileInfo)> fFilePath,
    std::function<QDir()> fWorkingDir)
{
    std::vector<CommandData> image_create_commands;
    std::vector<CommandData> date_set_commands;
    for (const ArchiveFileInfo &afi : folders) {
        const QString path = fFilePath(afi);
        Q_ASSERT_X(QDir().mkpath(path), QString("makeTestData folders").toUtf8(), path.toUtf8());
        date_set_commands.push_back(getDateSetCommand(path, afi.m_dateTime));
    }

    for (const ArchiveFileInfo &afi : files) {
        const QString path = fFilePath(afi);
        if (!afi.m_name.endsWith(".txt")) {
            image_create_commands.push_back(getImageCreateCommand(path));
            image_create_commands.push_back(getImageCompressCommand(path));
        }
        date_set_commands.push_back(getDateSetCommand(path, afi.m_dateTime));
    }
    const auto extracted_dir = fWorkingDir();
    executeCommands(image_create_commands, extracted_dir);
    executeCommands(date_set_commands, extracted_dir);
    generateFolderContentFile(extracted_dir);
    executeCommands(date_set_commands, extracted_dir);
}

const std::vector<CommandData> getArchiveCommandsZip()
{
    return {
        CommandData{"zip", {"--quiet", "-9", "-r", "-Z", "store", "../ZIP zip store 😼.zip", "."}},
        CommandData{"zip", {"--quiet", "-9", "-r", "-Z", "bzip2", "../ZIP zip bzip2 😺.zip", "."}},
        CommandData{"zip", {"--quiet", "-9", "-r", "-Z", "deflate", "../ZIP zip deflate 😻.zip", "."}},
        CommandData{"7za", {"a", "-mx=9", "-r", "-tzip", "../ZIP 7za deflate 😹.zip", "."}},
        CommandData{"rar", {"a", "-r", "-m5", "-ma5", "../RAR rar5 😸.rar", "."}}};
}

const std::vector<CommandData> getArchiveCommandsRar4()
{
    return {CommandData{"rar", {"a", "-r", "-m5", "-ma4", "../RAR rar4 😷.rar", "."}}};
}

} // namespace

InitTestArchives InitTestData::makeTestData(const DirStructureFixture &dsf) const
{
    const auto folders = dsf.getDirs();
    const auto files = dsf.getFiles();
    const auto dir = dsf.getPath();
    createFiles(
        folders,
        files,
        [&dir](ArchiveFileInfo afi) { return dir.filePath(afi.m_name); },
        [&dir]() { return dir; });
    return InitTestArchives(dir);
}

void InitTestArchives::createZip() const
{
    executeCommands(getArchiveCommandsZip(), m_src_dir);
}

void InitTestArchives::createRar4() const
{
    executeCommands(getArchiveCommandsRar4(), m_src_dir);
}
