// Copyright (C) 2023 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <MainWidget.h>

#include <QApplication>
#include <QHBoxLayout>
#include <QListView>
#include <QModelIndex>
#include <QStringListModel>
#include <QVBoxLayout>
#include <QMimedata>
#include <QMenuBar>
#include <QFileDialog>


MainWidget::MainWidget(QWidget *parent)
    : QMainWindow(parent) , _contentArea(new QWidget(this))
{
    resize(1200, 600);

    _contentArea->installEventFilter(this);
    setCentralWidget(_contentArea);

    _barWidget = new BarWidget(_contentArea);
    _barWidget->resize(_contentArea->size());
    _barWidget->setVisible(true);

    createActions();

    setMouseTracking(true);

    qApp->setApplicationDisplayName("Profiler");
}

bool MainWidget::eventFilter(QObject *object, QEvent *event)
{
    if (event->type() == QEvent::Resize && object == _contentArea && _barWidget)
    {
        _barWidget->resize(_contentArea->size());
    }

    return QObject::eventFilter(object, event);
}

void MainWidget::onOpenFileClicked()
{
    _barWidget->onOpenFileClicked(QFileDialog::getOpenFileName(this , "Open"));
}

void MainWidget::sortAndPresentChart(SortType sortType)
{
    _barWidget->presentChart(_barWidget->sort(sortType) , sortType);
}

void MainWidget::createActions()
{
    QMenu*   fileMenu = menuBar()->addMenu("File");
    QAction* newAction = fileMenu->addAction("Open");
    QAction* exitAction = fileMenu->addAction("Exit");

    connect(exitAction, &QAction::triggered, this, &QMainWindow::close);
    connect(newAction, &QAction::triggered, this, &MainWidget::onOpenFileClicked);

    //

    QMenu*   sortMenu = menuBar()->addMenu("Sort");
    const auto fromLongestDuration = sortMenu->addAction("From longest duration");
    const auto fromShortestDuration = sortMenu->addAction("From shortest duration");
    const auto fromEarliestStarted = sortMenu->addAction("From earliest started");
    const auto fromLatestStarted = sortMenu->addAction("From latest started");
    const auto fromEarliestFinished = sortMenu->addAction("From earliest finished");
    const auto fromLatestFinished = sortMenu->addAction("From latest finished");
    const auto byName = sortMenu->addAction("By name");

    connect(fromLongestDuration, &QAction::triggered, this, [this]()
            {
                sortAndPresentChart(SortType::FromLongestDuration);
            });

    connect(fromShortestDuration, &QAction::triggered, this, [this]()
            {
                sortAndPresentChart(SortType::FromShortestDuration);
            });

    connect(fromEarliestStarted , &QAction::triggered, this, [this]()
            {
                sortAndPresentChart(SortType::FromEarliestStarted);
            });

    connect(fromLatestStarted , &QAction::triggered, this, [this]()
            {
                sortAndPresentChart(SortType::FromLatestStarted);
            });

    connect(fromEarliestFinished , &QAction::triggered, this, [this]()
            {
                sortAndPresentChart(SortType::FromEarliestFinished);
            });

    connect(fromLatestFinished , &QAction::triggered, this, [this]()
            {
                sortAndPresentChart(SortType::FromLatestFinished);
            });

    connect(byName , &QAction::triggered, this, [this]()
            {
                sortAndPresentChart(SortType::ByName);
            });
}
