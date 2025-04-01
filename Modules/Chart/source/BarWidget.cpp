#include <BarWidget.h>
#include <CustomChartView.h>

#include <QBarcategoryaxis>
#include <QBarseries>
#include <QBarset>
#include <QBoxlayout>
#include <QValueaxis>
#include <QScrollBar>
#include <QMimeData>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QLabel>


static quint64 MaxVal{};

BarWidget::BarWidget(QWidget *parent) : QWidget(parent) , _details(new QLabel)
{
    setAcceptDrops(true);

    _details->setWindowFlags(Qt::ToolTip | Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    _details->setAttribute(Qt::WA_ShowWithoutActivating, true);
    _details->setAttribute(Qt::WA_TransparentForMouseEvents, true);
    _details->setStyleSheet("background-color : rgba(40,40,40,255);");
    _details->hide();
}

void BarWidget::drawChart(QChart* chart , const QVector<EventData>& events)
{
    auto set = new QBarSet("Events");
    set->setColor(Qt::darkCyan);

    for (int i = 0 , total = events.count(); i < total ; ++i)
    {
        const auto duration = events.at(i).duration;
        *set << duration;

        if(MaxVal < duration)
        {
            MaxVal = duration;
        }
    }

    chart->removeAllSeries();
    QBarSeries* series = new QBarSeries;

    QObject::connect(series , &QBarSeries::hovered , this , &BarWidget::onBarSeriesHovered);

    series->append(set);
    chart->addSeries(series);

    QStringList categories;
    for (int i = 0 , total = events.count() ; i < total ; ++i)
    {
        categories.append(events.at(i).name);
    }

    _axisX = new QBarCategoryAxis;
    _axisX->append(categories);
    _axisX->setLabelsBrush(Qt::gray);
    _axisX->setTitleBrush(Qt::gray);
    _axisX->setLinePenColor(Qt::darkGray);
    _axisX->setGridLineColor(Qt::darkGray);

    chart->addAxis(_axisX, Qt::AlignBottom);

    series->attachAxis(_axisX);

    _axisY = new QValueAxis;
    _axisY->setLabelsBrush(Qt::gray);
    _axisY->setTitleBrush(Qt::gray);
    _axisY->setLinePenColor(Qt::darkGray);
    _axisY->setGridLineColor(Qt::darkGray);

    resetChartRange();
    chart->addAxis(_axisY, Qt::AlignLeft);
    series->attachAxis(_axisY);
}

void BarWidget::resetChartRange()
{
    if(_axisY)
    {
        _axisY->setRange(0, MaxVal + MaxVal / 4);
    }
}

void BarWidget::onOpenFileClicked(const QString& filePath)
{
    if(filePath.isEmpty() == false)
    {
        presentChart(processJsonFile(filePath));
    }
}

QVector<EventData> BarWidget::sort(SortType sortType)
{
    QVector<EventData> eventsData;
    for(auto& eventData : _chartIndexToEventData)
    {
        eventsData.append(eventData);
    }

    _chartIndexToEventData.clear();

    switch (sortType)
    {
        case SortType::FromLongestDuration:
        {
            std::sort(eventsData.begin() , eventsData.end() , [](const EventData &first, const EventData &second)
                      {
                          return first.duration > second.duration;
                      });
            break;
        }
        case SortType::FromShortestDuration:
        {
            std::sort(eventsData.begin() , eventsData.end() , [](const EventData &first, const EventData &second)
                      {
                          return first.duration < second.duration;
                      });
            break;
        }
        case SortType::ByName:
        {
            QStringList strList;
            for(const auto& eventData : eventsData)
            {
                strList.append(eventData.name);
            }

            strList.sort();

            QVector<EventData> temp;
            for(quint32 i = 0 , total = strList.count(); i < total ; ++i)
            {
                for(quint32 j = 0 ; j < eventsData.count() ; ++j)
                {
                    if(strList.at(i) == eventsData.at(j).name)
                    {
                        temp.append(eventsData.takeAt(j));
                        break;
                    }
                }
            }

            eventsData = std::move(temp);

            break;
        }
        case SortType::FromEarliestFinished:
        {
            std::sort(eventsData.begin() , eventsData.end() , [](const EventData &first, const EventData &second)
                      {
                          return first.endTimePoint < second.endTimePoint;
                      });

            break;
        }
        case SortType::FromLatestFinished:
        {
            std::sort(eventsData.begin() , eventsData.end() , [](const EventData &first, const EventData &second)
                      {
                          return first.endTimePoint > second.endTimePoint;
                      });
            break;
        }
        case SortType::FromEarliestStarted:
        {
            std::sort(eventsData.begin() , eventsData.end() , [](const EventData &first, const EventData &second)
                      {
                          return first.beginTimePoint < second.beginTimePoint;
                      });

            break;
        }
        case SortType::FromLatestStarted:
        {
            std::sort(eventsData.begin() , eventsData.end() , [](const EventData &first, const EventData &second)
                      {
                          return first.beginTimePoint > second.beginTimePoint;
                      });
            break;
        }
    }

    for(quint32 i = 0 , total = eventsData.count(); i < total ; ++i)
    {
        _chartIndexToEventData.insert(i , eventsData.at(i));
    }

    return eventsData;
}

void BarWidget::onBarSeriesHovered(bool status, int index, QBarSet *barset)
{
    if(status)
    {
        _details->setText("Event name : " + _chartIndexToEventData.value(index).name +
                          "\nDuration   : " + QLocale(QLocale::English).toString(_chartIndexToEventData.value(index).duration) + " micro seconds" +
                          "\nThread id  : " + QString::number(_chartIndexToEventData.value(index).threadID));
        _details->setMargin(10);
        _details->adjustSize();

        QPoint cursorPos = QCursor::pos();

        int x = cursorPos.x() + 1;
        int y = cursorPos.y() + 1;

        _details->move(x, y);

        _details->show();
    }
    else
    {
        _details->close();
    }
}

void BarWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void BarWidget::dropEvent(QDropEvent *event)
{
    const QMimeData* mimeData = event->mimeData();

    if (mimeData->hasUrls())
    {
        const auto urls = mimeData->urls();
        QString filePath = urls.first().toLocalFile();

        presentChart(processJsonFile(filePath));

        event->acceptProposedAction();
    }
}

QVector<EventData> BarWidget::processJsonFile(const QString &filePath)
{
    QFile f(filePath);

    f.open(QIODevice::OpenModeFlag::ReadOnly);
    const auto jsonContent = f.readAll();
    f.close();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonContent);

    const auto rootObject = jsonDoc.object();
    const auto traceEvents = rootObject["traceEvents"].toArray();

    QVector<EventData> eventsData;
    for(int i = 0 , total = traceEvents.count(); i < total ; ++i)
    {
        QJsonObject event = traceEvents.at(i).toObject();

        QString name           = event["name"].toString();
        quint64 duration       = event["dur"].toVariant().toLongLong();
        quint64 threadId       = event["tid"].toVariant().toLongLong();
        quint64 beginTimePoint = event["beginTimePoint"].toVariant().toLongLong();
        quint64 endTimePoint   = event["endTimePoint"].toVariant().toLongLong();

        const auto eventData = EventData{std::move(name) , threadId, duration , beginTimePoint , endTimePoint};
        eventsData.append(eventData);
    }

    for(int i = 0 , total = eventsData.count(); i < total ; ++i)
    {
        quint32 count{1};
        bool repetitive{false};
        for(int j = i + 1 ; j < total ; ++j)
        {
            if(eventsData.at(i).name == eventsData.at(j).name)
            {
                eventsData[j].name += " - Call " + QString::number(++count);
                repetitive = true;
            }
        }

        if(repetitive)
        {
            eventsData[i].name += " - Call " + QString::number(1);
        }

        _chartIndexToEventData.insert(i , eventsData.at(i));
    }

    return sort(SortType::FromEarliestStarted);
}

void BarWidget::createChartView(QChart *chart)
{
    _chartView = new CustomChartView(chart, this);
    _chartView->setRenderHint(QPainter::Antialiasing);

    delete layout();
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_chartView);
    setLayout(layout);
}

void BarWidget::deletePreviousChartView()
{
    if(_chartView)
    {
        delete _chartView;
    }
}

QChart* BarWidget::createChart(const QString& chartTitle)
{
    const auto chart = new QChart;
    chart->setBackgroundBrush(QColor(20,20,20,255));
    chart->setTitleBrush(QBrush(Qt::white));
    chart->setTitle(chartTitle);
    chart->setAnimationOptions(QChart::NoAnimation);

    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(Qt::gray);

    return chart;
}

void BarWidget::presentChart(const QVector<EventData>& events , SortType sortType)
{
    QString chartTitle = "Profiler (MicroSecond) - Sort type : ";

    switch (sortType)
    {
        case SortType::FromLongestDuration:
        {
            chartTitle += "From longest duration";
            break;
        }
        case SortType::FromShortestDuration:
        {
            chartTitle += "From shortest duration";
            break;
        }
        case SortType::FromEarliestFinished:
        {
            chartTitle += "From earliest finished";
            break;
        }
        case SortType::FromLatestFinished:
        {
            chartTitle += "From latest finished";
            break;
        }
        case SortType::FromEarliestStarted:
        {
            chartTitle += "From earliest started";
            break;
        }
        case SortType::FromLatestStarted:
        {
            chartTitle += "From latest started";
            break;
        }
        case SortType::ByName:
        {
            chartTitle += "By name";
            break;
        }
    }

    deletePreviousChartView();
    const auto chart = createChart(chartTitle);
    createChartView(chart);
    drawChart(chart , events);
}
