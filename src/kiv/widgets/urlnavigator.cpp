#include "urlnavigator.h"

#include <QDir>
#include <QKeyEvent>

//#define DEBUG_LOCATION
#ifdef DEBUG_LOCATION
#include "helper.h"
#endif

#include "fileinfo.h"

UrlNavigator::UrlNavigator(QAbstractItemModel *model, const QUrl &url, QWidget *parent)
    : QLineEdit(parent)
    , m_model(model)
    , m_completer(new QCompleter(m_model, this))
    , m_currentHistoryIndex(0)
    , m_historyMax(30)
{
    setLocationUrl(url);
    setCompleter(m_completer);
    connect(this, SIGNAL(returnPressed()), this, SLOT(on_returnPressed()));
}

void UrlNavigator::setLocationUrl(const QUrl &url)
{
#ifdef DEBUG_LOCATION
    DEBUGOUT << "START; Current history index: " << m_currentHistoryIndex << m_history;
#endif
    if (m_currentHistoryIndex + 1 < m_history.size())
    {
        QList<QUrl>::iterator begin = m_history.begin() + m_currentHistoryIndex + 1;
        QList<QUrl>::iterator end = m_history.end();
        m_history.erase(begin, end);
    }
    if (m_history.isEmpty() || (!m_history.isEmpty() && m_history.last() != url))
    {
        m_history.append(url);
    }
    if (m_history.size() > m_historyMax)
    {
        m_history.removeAt(0);
    }
    m_currentHistoryIndex = m_history.size() - 1;
#ifdef DEBUG_LOCATION
    DEBUGOUT << "END; Current history index: " << m_currentHistoryIndex << m_history;
#endif

    return setLocationUrlInternal(url);
}

void UrlNavigator::focusOutEvent(QFocusEvent *event)
{
    setLocationUrlInternal(m_currentUrl);
    clearFocus();
    return QLineEdit::focusOutEvent(event);
}

void UrlNavigator::keyPressEvent(QKeyEvent *event)
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

void UrlNavigator::setLocationUrlInternal(const QUrl &url)
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

void UrlNavigator::updateContent()
{
#ifdef DEBUG_LOCATION
    DEBUGOUT << "Current history index: " << m_currentHistoryIndex << m_history;
#endif
    const QUrl url = m_history.at(m_currentHistoryIndex);
    emit urlChanged(url);
    return setLocationUrlInternal(url);
}

void UrlNavigator::on_returnPressed()
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

bool UrlNavigator::goBack()
{
    m_currentHistoryIndex--;
    if (m_currentHistoryIndex < 0)
    {
        m_currentHistoryIndex = 0;
    }
    updateContent();

    return !(m_currentHistoryIndex == 0);
}

bool UrlNavigator::goForward()
{
    m_currentHistoryIndex++;
    if (m_currentHistoryIndex > m_history.size() - 1)
    {
        m_currentHistoryIndex = m_history.size() - 1;
    }
    updateContent();

    return !(m_currentHistoryIndex = m_history.size() - 1);
}
