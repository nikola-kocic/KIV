#include "location_widget.h"

#include <QDebug>
#include <QKeyEvent>

LocationWidget::LocationWidget(QAbstractItemModel *model, QWidget *parent)
    : QLineEdit(parent)
    , m_model(model)
    , m_completer(new QCompleter(m_model, this))
    , m_current_fileinfo(FileInfo(""))
{
    setCompleter(m_completer);
    connect(this, SIGNAL(returnPressed()), this, SLOT(on_returnPressed()));
}

void LocationWidget::focusOutEvent(QFocusEvent *event)
{
    setText(m_current_fileinfo.getPath());
    clearFocus();
    return QLineEdit::focusOutEvent(event);
}

void LocationWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        clearFocus();
    }
    return QLineEdit::keyPressEvent(event);
}

void LocationWidget::on_returnPressed()
{
    const FileInfo fileinfo = FileInfo(text());
    if (fileinfo.isValid())
    {
        m_current_fileinfo = fileinfo;
        emit locationChanged(fileinfo);
    }
    else
    {
        setText(m_current_fileinfo.getPath());
    }
}

void LocationWidget::setText(const QString &text)
{
    m_current_fileinfo = FileInfo(text);
    return QLineEdit::setText(text);
}

void LocationWidget::setFileInfo(const FileInfo &fileinfo)
{
    m_current_fileinfo = fileinfo;
    setText(fileinfo.getPath());
}
