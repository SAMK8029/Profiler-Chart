#ifndef BARWIDGET_H
#define BARWIDGET_H

#include <QWidget>

class QBarSet;
class QBarSeries;
class QBarCategoryAxis;
class QValueAxis;
class QChart;
class CustomChartView;
class QLabel;

struct EventData
{
    QString name;
    quint64 threadID;
    quint64 duration;
    quint64 beginTimePoint;
    quint64 endTimePoint;
};

enum class SortType
{
    FromLongestDuration  ,
    FromShortestDuration ,
    FromEarliestFinished ,
    FromLatestFinished   ,
    FromEarliestStarted  ,
    FromLatestStarted    ,
    ByName
};

class BarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BarWidget(QWidget *parent = nullptr);

    void drawChart(QChart* chart , const QVector<EventData> &events);
    void resetChartRange();
    void onOpenFileClicked(const QString& filePath);
    QVector<EventData> sort(SortType sortType);
    void presentChart(const QVector<EventData>& events, SortType sortType = SortType::FromEarliestStarted);

private slots:
    void onBarSeriesHovered(bool status, int index, QBarSet *barset);

private:
    QVector<EventData> processJsonFile(const QString& filePath);
    QChart* createChart(const QString &chartTitle);
    void createChartView(QChart *chart);
    void deletePreviousChartView();
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void exportToPdf();

    QBarCategoryAxis* _axisX{};
    QValueAxis*       _axisY{};
    CustomChartView*  _chartView{};
    QLabel*           _details{};
    QHash<quint32 , EventData> _chartIndexToEventData;
};

#endif
