#ifndef INIT_TEST_DATA_H
#define INIT_TEST_DATA_H

#include <functional>
#include <memory>
#include <vector>
#include <QString>

#include "fixtures.h"


class InitTestArchives
{
    friend class InitTestData;
protected:
    InitTestArchives(const QString &src_dir)
        :m_src_dir(src_dir)
    {}

    QString m_src_dir;
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
