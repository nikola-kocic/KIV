#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include "komicviewer_enums.h"

#include <QtCore/qtimer.h>
#include <QtGui/qwidget.h>

class PictureItemRaster : public QWidget
{
    Q_OBJECT

public:
    PictureItemRaster( QWidget * parent = 0, Qt::WindowFlags f = 0 );
    QPixmap getPixmap();
    void setZoom(qreal z);
    qreal getZoom();
    void setPixmap(const QPixmap &p);
    void setRotation(qreal r);
    qreal getRotation();
    void setLockMode(LockMode::Mode);
    QVector<qreal> getDefaultZoomSizes();
    LockMode::Mode getLockMode();

private:
    QPoint pointToOrigin(int width, int height);
    void avoidOutOfScreen();
    void drag(const QPoint &pt);
    void beginDrag(const QPoint &pt);
    void endDrag();
    void updateLockMode();
    void ScrollPageHorizontal(int value);
    void ScrollPageVertical(int value);
    void start_timerScrollPage();

    QPixmap bmp;
    QPixmap pixmap_edited;
    qreal zoom;
    qreal rotation;
    QRect boundingRect;
    bool dragging;
    bool rotating;
    bool flagJumpToEnd;
    QPoint dragPoint;
    QPoint translatePoint;
    QVector<qreal> defaultZoomSizes;
    LockMode::Mode lockMode;
    QTimer *timerScrollPage;
    //    MiddleClick::Action middleClickAction;
    //    Wheel::Action wheelAction;
//    bool LockDrag;

protected:
    void paintEvent(QPaintEvent *event);
//    QSize sizeHint();
    void wheelEvent( QWheelEvent* );
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void resizeEvent(QResizeEvent *);
    void keyPressEvent(QKeyEvent *ev);

signals:
    void pageNext();
    void pagePrevious();
    void toggleFullscreen();
    void zoomChanged();

public slots:
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void fitWidth();
    void fitHeight();
private slots:
    void on_timerScrollPage_timeout();

};

#endif // PICTUREITEM_H
