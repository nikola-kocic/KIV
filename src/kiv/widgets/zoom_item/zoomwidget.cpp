#include "widgets/zoom_item/zoomwidget.h"

//#define DEBUG_ZOOM_WIDGET
#ifdef DEBUG_ZOOM_WIDGET
#include <QDebug>
#endif
#include <QKeyEvent>
#include <QLineEdit>

#include "helper.h"

ZoomWidget::ZoomWidget(QWidget *parent)
    : QComboBox(parent),
      m_zoomValue(1)
{
    //setEnabled(false);
    setInsertPolicy(QComboBox::NoInsert);
    setMaxVisibleItems(12);
    setMinimumContentsLength(8);
    setEditable(true);
    setFocusPolicy(Qt::StrongFocus);
    lineEdit()->setCompleter(0);

    for (int i = 0; i < Helper::defaultZoomSizes.size(); ++i)
    {
        const qreal &z = Helper::defaultZoomSizes.at(i);
        addItem(QString::number((z * 100), 'f', 0) + "%", z);
        if (qFuzzyCompare(z, 1) == true)
        {
            setCurrentIndex(count() - 1);
        }
    }

    connect(lineEdit(), SIGNAL(returnPressed()), this, SLOT(on_textChanged()));
}

void ZoomWidget::on_zoomChanged(qreal current, qreal previous)
{
    m_zoomValue = current;
    if (!Helper::defaultZoomSizes.contains(current))
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
        /* Add select current Zoom value from comboBox */

        for (int existingIndex = 0; existingIndex < count(); ++existingIndex)
        {
            if (Helper::FuzzyCompare(itemData(existingIndex).toReal(), current))
            {
                setCurrentIndex(existingIndex);
                break;
            }
        }
    }

    if (!Helper::defaultZoomSizes.contains(previous))
    {
        /* Remove previous Zoom value if it's not in default zoom sizes */

        for (int existingIndex = 0; existingIndex < count(); ++existingIndex)
        {
            if (Helper::FuzzyCompare(itemData(existingIndex).toReal(), previous))
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

void ZoomWidget::keyPressEvent(QKeyEvent *event)
{
#ifdef DEBUG_ZOOM_WIDGET
    Helper::debuglog(Q_FUNC_INFO, QString::number(event->key()));
#endif
    if (event->key() == Qt::Key_Escape)
    {
        on_zoomChanged(m_zoomValue, m_zoomValue);
        clearFocus();
        event->accept();
    }
    return QComboBox::keyPressEvent(event);
}

void ZoomWidget::focusOutEvent(QFocusEvent *event)
{
#ifdef DEBUG_ZOOM_WIDGET
    Helper::debuglog(Q_FUNC_INFO, lineEdit()->text());
#endif
    on_zoomChanged(m_zoomValue, m_zoomValue);
    return QComboBox::focusOutEvent(event);
}
