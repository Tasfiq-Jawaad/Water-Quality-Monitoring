#include <QtWidgets>
#include <stdexcept>
#include "window.hpp"

static const int MIN_WIDTH = 800;

WaterQualityWindow::WaterQualityWindow() : QMainWindow()
{
    createMainWidget();
    createButtons();
    createToolBar();
    createStatusBar();
    addFileMenu();
    addHelpMenu();

    setMinimumWidth(MIN_WIDTH);
    setWindowTitle("Water Quality Monitor");
}

void WaterQualityWindow::createMainWidget()
{
    tabWidget = new QTabWidget(this);

    // Dashboard Page
    dashboardPage = new QWidget();
    QGridLayout *dashboardLayout = new QGridLayout(dashboardPage);

    // Title Label
    QLabel *dashboardLabel = new QLabel(tr("Water Quality Monitor - Dashboard"));
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    dashboardLabel->setFont(titleFont);
    dashboardLabel->setContentsMargins(0, 0, 0, 0);
    dashboardLabel->setFixedHeight(30);

    dashboardLayout->setContentsMargins(25, 25, 25, 25);
    dashboardLayout->setSpacing(25);
    dashboardLayout->addWidget(dashboardLabel, 0, 0, 1, -1, Qt::AlignHCenter); // Horizontal centering only

    QStringList tabTitles = { tr("Data"), tr("Pollutant Overview"), tr("POPs"), tr("Environmental Litter Indicators"), tr("Compliance")};

    int row = 1, col = 0;
    const int columns = 3;

    for (int i = 0; i < tabTitles.size(); ++i) {
        CardWidget *card = new CardWidget(tabTitles[i], dashboardPage);

        // Connect each card to open the corresponding tab
        connect(card, &CardWidget::cardClicked, this, [this, i]() {
            tabWidget->setCurrentIndex(i + 1); // +1 to account for the Dashboard tab
        });

        dashboardLayout->addWidget(card, row, col);

        col++;
        if (col == columns) {
            col = 0;
            row++;
        }
    }

    tabWidget->addTab(dashboardPage, tr("Dashboard"));

    // Data Page
    dataPage = new QWidget();
    QVBoxLayout *dataLayout = new QVBoxLayout(dataPage);
    int dataPageIndex = tabWidget->addTab(dataPage, tr("Data"));
    table = new QTableView();
    table->setModel(&model);
    table->setFont(QFontDatabase::systemFont(QFontDatabase::FixedFont));
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    table->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    dataLayout->addWidget(table);
    tabWidget->addTab(dataPage, tr("Data"));

    
    // Pollutant Overview Page
    overviewPage = new PollutantOverview(&model);
    tabWidget->addTab(overviewPage, tr("Pollutant Overview"));

    // POPs Page
    popsPage = new POPsPage(&model);
    tabWidget->addTab(popsPage, tr("POPs"));

    // Environmental Litter Indicators page
    litterIndicatorPage = new EnvironmentalLitterIndicators(&model);
    tabWidget->addTab(litterIndicatorPage, tr("Environmental Litter Indicators"));

    // Compliance Dashboard Page
    compliancePage = new ComplianceDashboard(&model);
    tabWidget->addTab(compliancePage, tr("Compliance"));

    setCentralWidget(tabWidget);
}

void WaterQualityWindow::createButtons()
{
    loadButton = new QPushButton(tr("Load CSV"));
    connect(loadButton, &QPushButton::clicked, this, &WaterQualityWindow::openCSV);
}

void WaterQualityWindow::createToolBar()
{
    QToolBar *toolBar = new QToolBar();

    toolBar->addSeparator();
    toolBar->addWidget(loadButton);

    addToolBar(Qt::TopToolBarArea, toolBar);
}

void WaterQualityWindow::createStatusBar()
{
    fileInfo = new QLabel("Current file: <none>");
    QStatusBar *status = statusBar();
    status->addWidget(fileInfo);
}

void WaterQualityWindow::addFileMenu()
{
    QAction *closeAction = new QAction("Quit", this);
    closeAction->setShortcut(QKeySequence::Close);
    connect(closeAction, SIGNAL(triggered()), this, SLOT(close()));

    QMenu *fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(closeAction);
}

void WaterQualityWindow::addHelpMenu()
{
    QAction *aboutAction = new QAction("&About", this);
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));

    QAction *aboutQtAction = new QAction("About &Qt", this);
    connect(aboutQtAction, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    QMenu *helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(aboutAction);
    helpMenu->addAction(aboutQtAction);
}

void WaterQualityWindow::openCSV()
{
    QString filename = QFileDialog::getOpenFileName(this, tr("Open CSV"), ".", tr("CSV Files (*.csv)"));
    if (filename.isEmpty())
        return;

    dataFilePath = filename;
    try
    {
        model.updateFromFile(dataFilePath);
    }
    catch (const std::exception &error)
    {
        QMessageBox::critical(this, "CSV File Error", error.what());
        return;
    }

    fileInfo->setText(QString("Current file: <kbd>%1</kbd>").arg(filename));
    table->resizeColumnsToContents();

    // update pollutantSelector's option
    // pollutantSelector->clear();
    // pollutantSelector->addItem("All");
    // for (auto &p : model.uniquePollutants())
    // {
    //     pollutantSelector->addItem(p);
    // }

    // // update overviewPage's option
    overviewPage->updatePollutantList();

    // // update popsPage's option
    popsPage->updatePollutantList();

    // // update litterPage's option
    litterIndicatorPage->updateFromModel();
}

void WaterQualityWindow::about()
{
    QMessageBox::about(this, "About Water Quality Monitor",
                       "This application visualises water quality data.\n\n"
                       "(c) 2024 Group 13");
}