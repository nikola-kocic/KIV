#ifndef KOMICVIEWERENUMS_H
#define KOMICVIEWERENUMS_H

namespace LockMode
{
    enum Mode { None, Autofit, FitWidth, FitHeight, Zoom };
}

namespace ViewMode
{
    enum Mode { None, Autofit, FitWidth, FitHeight, Zoom };
}

const int TYPE_FILE = 1001;
const int TYPE_DIR = 1002;
const int TYPE_ARCHIVE = 1003;


int getArchiveNumberFromTreewidget(int number);

int makeArchiveNumberForTreewidget(int number);

#endif // KOMICVIEWERENUMS_H
