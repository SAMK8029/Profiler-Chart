#pragma once

#include <QChartView>

class BarWidget;

class CustomChartView : public QChartView
{
    Q_OBJECT
public:
    explicit CustomChartView(QChart *chart, BarWidget* parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    void zoom(QEvent* event, qreal factor);

    BarWidget* const _parent;
    bool             _isPanning{};
    QPoint           _lastMousePos{};
};
