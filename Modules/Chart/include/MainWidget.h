#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QMainWindow>
#include <BarWidget.h>

class QListView;
class QStringListModel;

class MainWidget : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWidget(QWidget* parent = nullptr);

protected:
    bool eventFilter(QObject *object, QEvent *event) override;

private slots:
    void onOpenFileClicked();

private:
    void createActions();

    QWidget*   _contentArea = nullptr;
    BarWidget* _barWidget = nullptr;
    void sortAndPresentChart(SortType sortType);
    void fromLatestStarted();
};

#endif
