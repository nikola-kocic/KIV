#ifndef MYLINEEDIT_H
#define MYLINEEDIT_H

#include <QLineEdit>

class MyLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit MyLineEdit(QWidget *parent = 0);
protected:
    void focusOutEvent(QFocusEvent * event);
signals:
    void focusLost();
public slots:
    
};

#endif // MYLINEEDIT_H
