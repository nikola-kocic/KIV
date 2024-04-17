#ifndef INIT_TEST_DATA_H
#define INIT_TEST_DATA_H

#include <QDir>
#include <QString>

#include "fixtures.h"

class InitTestArchives
{
    friend class InitTestData;

protected:
    InitTestArchives(const QDir &src_dir)
        : m_src_dir(src_dir)
    {}

    QDir m_src_dir;

public:
    void createZip() const;
    void createRar4() const;
};

class InitTestData
{
public:
    InitTestArchives makeTestData(const DirStructureFixture &dsf) const;
};

#endif // INIT_TEST_DATA_H
