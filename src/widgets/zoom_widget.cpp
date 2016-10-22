#include "widgets/zoom_widget.h"

#include <QKeyEvent>
#include <QLineEdit>
#include <QVector>

#include "helper.h"

//#define DEBUG_ZOOM_WIDGET
#ifdef DEBUG_ZOOM_WIDGET
#include "helper.h"
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

    connect(lineEdit(), &QLineEdit::returnPressed,
            this, &ZoomWidget::on_textChanged);
    connect(this, static_cast<void (QComboBox::*)(int)>(
                &QComboBox::activated),
            this, &ZoomWidget::on_activated);
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
        m_zoomValue = dec / 100.0;
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
    qreal zoomToApply = m_zoomValue * 1.25;
    if (!m_defaultZoomSizes.isEmpty()
            && zoomToApply > m_defaultZoomSizes.first()
            && m_zoomValue < m_defaultZoomSizes.last())
    {
        const auto it = std::find_if(
                    m_defaultZoomSizes.constBegin(),
                    m_defaultZoomSizes.constEnd(),
                    [this] (const qreal &z) { return z > m_zoomValue; } );
        if (it != m_defaultZoomSizes.end())
        {
            zoomToApply = *it;
        }
    }

    setZoom(zoomToApply);
}

void ZoomWidget::zoomOut()
{
    qreal zoomToApply = m_zoomValue / 1.25;
    if (!m_defaultZoomSizes.isEmpty()
            && m_zoomValue > m_defaultZoomSizes.first()
            && zoomToApply < m_defaultZoomSizes.last())
    {
        std::reverse_iterator<QVector<qreal>::const_iterator> rbegin(
                    m_defaultZoomSizes.constEnd());
        std::reverse_iterator<QVector<qreal>::const_iterator> rend(
                    m_defaultZoomSizes.constBegin());
        const auto it = std::find_if(
                    rbegin,
                    rend,
                    [this] (const qreal &z) { return z < m_zoomValue; } );
        if (it != rend)
        {
            zoomToApply = *it;
        }
    }
    setZoom(zoomToApply);
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
