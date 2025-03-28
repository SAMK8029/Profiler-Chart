#include <BarWidget.h>
#include <CustomChartView.h>


CustomChartView::CustomChartView(QChart *chart, BarWidget* parent) : _parent(parent), QChartView(chart, parent)
{
    setAcceptDrops(false);
}

void CustomChartView::wheelEvent(QWheelEvent* event)
{
    if (event->modifiers() & Qt::ControlModifier)
    {
        zoom(event , event->angleDelta().y() > 0 ? 1.1 : 0.9);
    }
    else
    {
        QChartView::wheelEvent(event);
    }
}

void CustomChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _isPanning = true;
        _lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
        event->accept();
    }

    QChartView::mousePressEvent(event);
}

void CustomChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (_isPanning)
    {
        QPoint delta = event->pos() - _lastMousePos;
        chart()->scroll(-delta.x(), 0);
        _lastMousePos = event->pos();
        event->accept();
    }

    QChartView::mouseMoveEvent(event);
}

void CustomChartView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        _isPanning = false;
        setCursor(Qt::ArrowCursor);
        event->accept();
    }

    QChartView::mouseReleaseEvent(event);
}

void CustomChartView::keyPressEvent(QKeyEvent* event)
{
    if(event->modifiers() & Qt::ControlModifier)
    {
        if(event->key() == Qt::Key::Key_Plus)
        {
            zoom(event , 1.1);
        }
        if(event->key() == Qt::Key::Key_Minus)
        {
            zoom(event , 0.9);event->accept();
        }
    }
}

void CustomChartView::zoom(QEvent* event, qreal factor)
{
    chart()->zoom(factor);

    event->accept();

    _parent->resetChartRange();
}
