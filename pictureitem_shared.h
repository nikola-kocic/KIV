#ifndef PICTUREITEMSHARED_H
#define PICTUREITEMSHARED_H

#include "komicviewer_enums.h"

#include <QtCore/qtimer.h>
#include <QtGui/qwidget.h>

class PictureItemShared : public QObject
{
    Q_OBJECT

public:
    PictureItemShared();
    QPixmap getPixmap();
    void setZoom(qreal z);
    qreal getZoom();
    void setPixmap(const QPixmap &p);
    void setRotation(qreal r);
    qreal getRotation();
    void setLockMode(LockMode::Mode);
    QVector<qreal> getDefaultZoomSizes();
    LockMode::Mode getLockMode();

    void processWheelEvent( QWheelEvent* );
    bool processKeyPressEvent(int key);
    void processMousePressEvent(QMouseEvent *ev);

    QPoint pointToOrigin(int width, int height);
    void avoidOutOfScreen();
    void drag(const QPoint &pt);
    void beginDrag(const QPoint &pt);
    void endDrag();
    void updateLockMode();
    bool dragging;
    bool isPixmapNull();
    QSize widgetSize;
    QRectF boundingRect;
    void afterPixmapLoad();

private:
    void start_timerScrollPage();
    void ScrollPageHorizontal(int value);
    void ScrollPageVertical(int value);
    QPixmap bmp;
    qreal zoom;
    qreal rotation;
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

signals:
    void pageNext();
    void pagePrevious();
    void toggleFullscreen();
    void zoomChanged(qreal current, qreal previous);
    void updateCursor(Qt::CursorShape);
    void pixmapChanged();
    void update();

public slots:
    void zoomIn();
    void zoomOut();
    void fitToScreen();
    void fitWidth();
    void fitHeight();
private slots:
    void on_timerScrollPage_timeout();

};

#endif // PICTUREITEMSHARED_H
