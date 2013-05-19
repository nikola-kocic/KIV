#include "location_widget.h"

#include <QDir>
#include <QKeyEvent>

//#define DEBUG_LOCATION
#ifdef DEBUG_LOCATION
#include "helper.h"
#endif

#include "fileinfo.h"

LocationWidget::LocationWidget(QAbstractItemModel *model, const QUrl &url, QWidget *parent)
    : QLineEdit(parent)
    , m_model(model)
    , m_completer(new QCompleter(m_model, this))
{
    setLocationUrl(url);
    setCompleter(m_completer);
    connect(this, SIGNAL(returnPressed()), this, SLOT(on_returnPressed()));
}

void LocationWidget::setLocationUrl(const QUrl &url)
{
    return setLocationUrlInternal(url);
}

void LocationWidget::focusOutEvent(QFocusEvent *event)
{
    setLocationUrlInternal(m_currentUrl);
    clearFocus();
    return QLineEdit::focusOutEvent(event);
}

void LocationWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        clearFocus();
    }
    else
    {
        return QLineEdit::keyPressEvent(event);
    }
}

void LocationWidget::setLocationUrlInternal(const QUrl &url)
{
    m_currentUrl = url;
    if (m_currentUrl.isLocalFile())
    {
        const QString path = QDir::toNativeSeparators(m_currentUrl.toLocalFile());
        setText(path);
    }
    else
    {
        setText(m_currentUrl.toString(QUrl::RemovePassword));
    }
}

void LocationWidget::on_returnPressed()
{
    const FileInfo fileinfo = FileInfo(text());
    if (fileinfo.isValid())
    {
        const QString path = fileinfo.getPath();
        // TODO: Add check if URL is local file
        const QUrl url = QUrl::fromLocalFile(path);
        setLocationUrl(url);
        emit urlChanged(url);
    }
    else
    {
        setLocationUrlInternal(m_currentUrl);
    }
}
