#ifndef PICTUREITEM_H
#define PICTUREITEM_H

#include <QtGui/qlabel.h>
#include <QtCore/qtimer.h>

namespace LockMode
{
    enum Mode { None, Autofit, FitWidth, FitHeight, Zoom };
}


class PictureItem : public QLabel
{
    Q_OBJECT

public:
    PictureItem( QWidget * parent = 0, Qt::WindowFlags f = 0 );
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
    bool flagJumpToEnd;
    QPixmap pixmap_edited;
//    MiddleClick::Action middleClickAction;
//    Wheel::Action wheelAction;

    qreal zoom;
    qreal rotation;
    QRect boundingRect;
    QPixmap bmp;
    bool dragging;
    bool rotating;
    QPoint dragPoint;
    QPoint translatePoint;
    QVector<qreal> defaultZoomSizes;
    LockMode::Mode lockMode;
    QTimer *timerScrollPage;
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
