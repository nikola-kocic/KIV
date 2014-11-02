#include "kiv/src/widgets/zoom_widget.h"

#include <QKeyEvent>
#include <QLineEdit>
#include <QVector>

#include "kiv/src/helper.h"

//#define DEBUG_ZOOM_WIDGET
#ifdef DEBUG_ZOOM_WIDGET
#include "kiv/src/helper.h"
#endif

ZoomWidget::ZoomWidget(const QVector<qreal> defaultZoomSizes,
                       QWidget *parent)
    : QComboBox(parent)
    , m_zoomValue(1)
    , m_defaultZoomSizes(defaultZoomSizes)
{
    //setEnabled(false);
    setInsertPolicy(QComboBox::NoInsert);
    setMaxVisibleItems(12);
    setMinimumContentsLength(8);
    setEditable(true);
    setFocusPolicy(Qt::StrongFocus);
    lineEdit()->setCompleter(nullptr);

    for (const qreal &z : m_defaultZoomSizes)
    {
        addItem(QString::number((z * 100), 'f', 0) + "%", z);
        if (qFuzzyCompare(z, 1) == true)
        {
            setCurrentIndex(count() - 1);
        }
    }

    connect(lineEdit(), SIGNAL(returnPressed()), this, SLOT(on_textChanged()));
    connect(this, SIGNAL(activated(int)), this, SLOT(on_activated(int)));
}

void ZoomWidget::on_zoomChanged(qreal current, qreal previous)
{
    m_zoomValue = current;
    if (!m_defaultZoomSizes.contains(current))
    {
        /* Add current Zoom value to comboBox */

        const QString zoomText = QString::number((current * 100), 'f', 0) + "%";
        int insertIndex = 0;
        for (; insertIndex < count(); ++insertIndex)
        {
            if (itemData(insertIndex).toReal() > current)
            {
                break;
            }
        }

        insertItem(insertIndex, zoomText, current);
        setCurrentIndex(insertIndex);
    }
    else
    {
        /* Select current Zoom value from comboBox */

        for (int existingIndex = 0; existingIndex < count(); ++existingIndex)
        {
            if (Helper::FuzzyCompare(itemData(existingIndex).toReal(), current))
            {
                setCurrentIndex(existingIndex);
                break;
            }
        }
    }

    if (!m_defaultZoomSizes.contains(previous))
    {
        /* Remove previous Zoom value if it's not in default zoom sizes */

        for (int existingIndex = 0; existingIndex < count(); ++existingIndex)
        {
            if (Helper::FuzzyCompare(itemData(existingIndex).toReal(),
                                     previous))
            {
                removeItem(existingIndex);
                break;
            }
        }
    }
}

void ZoomWidget::on_textChanged()
{
    QString zoomvalue = lineEdit()->text();
    zoomvalue.remove('%');

    bool ok;
    const int dec = zoomvalue.toInt(&ok, 10);

    if (ok)
    {
        m_zoomValue = qreal(dec) / 100;
        emit zoomChanged(m_zoomValue);
    }
    else
    {
        on_zoomChanged(m_zoomValue, m_zoomValue);
    }
}

void ZoomWidget::on_activated(int index)
{
    setZoom(itemData(index).toReal());
}

void ZoomWidget::keyPressEvent(QKeyEvent *event)
{
#ifdef DEBUG_ZOOM_WIDGET
    DEBUGOUT << QString::number(event->key());
#endif
    if (event->key() == Qt::Key_Escape)
    {
        on_zoomChanged(m_zoomValue, m_zoomValue);
        clearFocus();
        event->accept();
    }
    else
    {
        return QComboBox::keyPressEvent(event);
    }
}

void ZoomWidget::focusOutEvent(QFocusEvent *event)
{
#ifdef DEBUG_ZOOM_WIDGET
    DEBUGOUT << lineEdit()->text();
#endif
    on_zoomChanged(m_zoomValue, m_zoomValue);
    return QComboBox::focusOutEvent(event);
}

void ZoomWidget::zoomIn()
{
    for (const qreal &z : m_defaultZoomSizes)
    {
        if (z > m_zoomValue)
        {
            setZoom(z);
            return;
        }
    }

    setZoom(m_zoomValue * 1.25);
}

void ZoomWidget::zoomOut()
{
    for (int i = 0; i < m_defaultZoomSizes.size(); ++i)
    {
        if (m_defaultZoomSizes.at(i) >= m_zoomValue)
        {
            if (i != 0)
            {
                setZoom(m_defaultZoomSizes.at(i - 1));
            }
            else
            {
                setZoom(m_zoomValue / 1.25);
            }
            return;
        }
    }

    setZoom(m_zoomValue / 1.25);
}

void ZoomWidget::setZoom(qreal value)
{
    emit zoomChanged(value);
    on_zoomChanged(value, m_zoomValue);
}

qreal ZoomWidget::getZoom()
{
    return m_zoomValue;
}
