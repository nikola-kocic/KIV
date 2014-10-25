#include "kiv/src/widgets/urlnavigator.h"

#include <QDir>
#include <QKeyEvent>

//#define DEBUG_LOCATION
#ifdef DEBUG_LOCATION
#include "kiv/src/helper.h"
#endif

#include "kiv/src/fileinfo.h"

UrlNavigator::UrlNavigator(QAbstractItemModel *model, const QUrl &url,
                           QWidget *parent)
    : QLineEdit(parent)
    , m_model(model)
    , m_completer(new QCompleter(m_model, this))
    , m_historyIndex(0)
    , m_historyMax(30)
{
    setLocationUrl(url);
    setCompleter(m_completer);
    connect(this, SIGNAL(returnPressed()), this, SLOT(on_returnPressed()));
}

void UrlNavigator::setLocationUrl(const QUrl &url)
{
#ifdef DEBUG_LOCATION
    DEBUGOUT << "START; Current history index: " << m_currentHistoryIndex
             << m_history;
#endif
    if (m_historyIndex + 1 < m_history.size())
    {
        QList<QUrl>::iterator begin = m_history.begin() + m_historyIndex + 1;
        QList<QUrl>::iterator end = m_history.end();
        m_history.erase(begin, end);
    }
    if (m_history.isEmpty()
        || (!m_history.isEmpty() && m_history.last() != url))
    {
        m_history.append(url);
    }
    if (m_history.size() > m_historyMax)
    {
        m_history.removeAt(0);
    }
    m_historyIndex = m_history.size() - 1;
#ifdef DEBUG_LOCATION
    DEBUGOUT << "END; Current history index: " << m_currentHistoryIndex
             << m_history;
#endif

    setLocationUrlInternal(url);

    emit historyChanged();
}

void UrlNavigator::focus()
{
    this->setFocus();
    this->selectAll();
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
        const QString path =
                QDir::toNativeSeparators(m_currentUrl.toLocalFile());
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
    const QUrl url = m_history.at(m_historyIndex);
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

bool UrlNavigator::setHistoryIndex(int index)
{
    if (index < 0 || index > m_history.size() - 1)
    {
        return false;
    }
    if (index == m_historyIndex)
    {
        return true;
    }

    m_historyIndex = index;
    updateContent();
    emit historyChanged();

    return true;
}

/**
 * @brief Goes back one step in the URL history.
 * The signals UrlNavigator::urlChanged() and UrlNavigator::historyChanged()
 * are emitted if true is returned.
 * @return False is returned if the beginning of the history has already been
 * reached and hence going back was not possible.
 */
bool UrlNavigator::goBack()
{
    if (m_historyIndex == 0)
    {
        return false;
    }

    return setHistoryIndex(m_historyIndex - 1);
}

/**
 * @brief Goes forward one step in the URL history.
 * The signals UrlNavigator::urlChanged() and UrlNavigator::historyChanged()
 * are emitted if true is returned.
 * @return False is returned if the end of the history has already been reached
 * and hence going forward was not possible.
 */
bool UrlNavigator::goForward()
{
    if (m_historyIndex == m_history.size() - 1)
    {
        return false;
    }

    return setHistoryIndex(m_historyIndex + 1);
}

/**
 * @return Returns the history index of the current URL,
 * where 0 <= history index < UrlNavigator::historySize().
 */
int UrlNavigator::historyIndex() const
{
    return m_historyIndex;
}

/**
 * @return Returns the amount of items in the history.
 */
int UrlNavigator::historySize() const
{
    return m_history.size();
}

QList<QUrl> UrlNavigator::getHistory() const
{
    return m_history;
}
