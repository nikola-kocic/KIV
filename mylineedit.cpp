#include "mylineedit.h"

MyLineEdit::MyLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
}

void MyLineEdit::focusOutEvent(QFocusEvent *event)
{
    QLineEdit::focusOutEvent(event);
    emit focusLost();
}
