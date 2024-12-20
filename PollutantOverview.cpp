#include "PollutantOverview.hpp"
#include <QtWidgets>

PollutantOverview::PollutantOverview(PollutantModel* dataModel, QWidget* parent)
    : QWidget(parent)
    , model(dataModel)
{
    setupUI();
    
    tooltipLabel = new QLabel(this);
    tooltipLabel->setStyleSheet(
        "QLabel { background-color: white; color: black; border: 1px solid gray; "
        "padding: 5px; border-radius: 3px; }"
    );
    tooltipLabel->hide();
}

void PollutantOverview::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(25, 25, 25, 25);
    mainLayout->setSpacing(30);

    // Title panel
    QWidget* titlePanel = new QWidget;
    QHBoxLayout* titleLayout = new QHBoxLayout(titlePanel);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* titleLabel = new QLabel(tr("Pollutant Overview"));
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    mainLayout->addWidget(titlePanel);

    // Control panel
    QWidget* controlPanel = new QWidget;
    QHBoxLayout* controlsLayout = new QHBoxLayout(controlPanel);
    controlsLayout->setContentsMargins(0, 0, 0, 0);
    
    // // Search box
    createSearchBox();
    controlsLayout->addWidget(new QLabel(tr("Search:")));
    controlsLayout->addWidget(searchBox);

    // Dropdown Menu
    createPollutantSelector();
    controlsLayout->addWidget(new QLabel(tr("Or select:")));
    controlsLayout->addWidget(pollutantSelector);
    
    createDateRangeSelector();

    // date picker
    controlsLayout->addWidget(new QLabel(tr("From:")));
    controlsLayout->addWidget(startDateEdit);
    controlsLayout->addWidget(new QLabel(tr("To:")));
    controlsLayout->addWidget(endDateEdit);
    
    controlsLayout->addStretch();
    mainLayout->addWidget(controlPanel);

    // Chart
    createChart();
    mainLayout->addWidget(chartView, 1);

    setMinimumSize(800, 600);
}

void PollutantOverview::createSearchBox()
{
    searchBox = new QLineEdit();
    searchBox->setPlaceholderText("Search pollutant...");
    searchBox->setClearButtonEnabled(true);
    connect(searchBox, &QLineEdit::returnPressed, this, &PollutantOverview::handleSearch);
}

void PollutantOverview::createDateRangeSelector()
{
    startDateEdit = new QDateEdit(QDate::currentDate().addDays(-5));
    endDateEdit = new QDateEdit(QDate::currentDate());

    startDateEdit->setCalendarPopup(true);
    endDateEdit->setCalendarPopup(true);

    connect(startDateEdit, &QDateEdit::dateChanged, this, &PollutantOverview::handleDateRangeChanged);
    connect(endDateEdit, &QDateEdit::dateChanged, this, &PollutantOverview::handleDateRangeChanged);
}

void PollutantOverview::createPollutantSelector()
{
    pollutantSelector = new QComboBox();
    pollutantSelector->setMinimumWidth(150);
    connect(pollutantSelector, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &PollutantOverview::updateChart);
}

void PollutantOverview::createChart()
{
    QChart* chart = new QChart();
    chart->setTitle("Pollutant Concentration Over Time");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(400);
}

void PollutantOverview::handleSearch()
{
    QString searchText = searchBox->text().trimmed();
    if (searchText.isEmpty()) return;

    // match the search text with the pollutant names
    int index = pollutantSelector->findText(searchText, Qt::MatchContains);
    if (index >= 0) {
        pollutantSelector->setCurrentIndex(index);
    } else {
        QMessageBox::information(this, "Search Result", 
            "No pollutant found matching '" + searchText + "'");
    }
}

QColor PollutantOverview::getComplianceColor(double value)
{
    if (value > 10) return QColor(255, 0, 0);      // Red - Danger
    if (value > 1) return QColor(255, 165, 0);     // Orange - Warning
    return QColor(0, 128, 0);                      // Green - Safe
}

QString PollutantOverview::getComplianceStatus(double value)
{
    if (value > 10) return "Exceeding safe levels";
    if (value > 1) return "Caution level";
    return "Safe level";
}

void PollutantOverview::handleHovered(const QPointF &point, bool state)
{
    if (state) {
        QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(point.x());
        
        QString tooltipText = QString(
            "Date: %1\n"
            "Value: %2\n"
            "Status: %3"
        ).arg(dateTime.toString("yyyy-MM-dd"))
         .arg(point.y(), 0, 'f', 2)
         .arg(getComplianceStatus(point.y()));

        QPoint pos = chartView->mapFromGlobal(QCursor::pos());
        tooltipLabel->setText(tooltipText);
        tooltipLabel->adjustSize();
        tooltipLabel->move(pos.x() + 10, pos.y() - tooltipLabel->height() / 2);
        tooltipLabel->show();
    } else {
        tooltipLabel->hide();
    }
}

void PollutantOverview::handleDateRangeChanged()
{
    if (startDateEdit->date() > endDateEdit->date()) {
        endDateEdit->setDate(startDateEdit->date().addDays(5));
    }
    updateChart();
}

void PollutantOverview::updatePollutantList()
{
    if (!model) return;

    QString currentPollutant = pollutantSelector->currentText();
    QString searchText = searchBox->text().trimmed();
    
    pollutantSelector->clear();

    QStringList keywords = {
        "Lead", "Mercury", "Cadmium",
        "Chromium", "Nitrate", "Phosphate",
        "Chloroform", "1,1-Dichloro", "1,1,1,2 -TET",
        "1,3 -DICHLOR", "1122TetClEth", "11Cl-PF3OUdS",
        "11DClEthan", "12-DCA", "2,3,6-TBA",
        "2,4-D", "2,4-Xylenol", "2,4,6-T",
        "2,5-Xylenol", "245-T", "24Dichloropl",
        "2Phenoxyprop", "3:3 FTCA", "4-CAA",
        "4:2 FTSA", "4Cl3MePhenol", "4Phenoxbutyr",
        "5:3 FTCA", "6:2 FTSA", "7:3 FTCA",
        "8:2 FTSA", "9Cl-PF3ONS"
    };

    for (const auto &pollutant : model->uniquePollutants())
    {
        QString label = pollutant.toLower();

        for (const QString &keyword : keywords)
        {
            if (label.contains(keyword.toLower()))
            {
                pollutantSelector->addItem(pollutant);
                break;
            }
        }
    }


    if (!searchText.isEmpty()) {
        int index = pollutantSelector->findText(searchText, Qt::MatchContains);
        if (index >= 0) {
            pollutantSelector->setCurrentIndex(index);
        }
    } else if (!currentPollutant.isEmpty()) {
        int index = pollutantSelector->findText(currentPollutant);
        if (index >= 0) {
            pollutantSelector->setCurrentIndex(index);
        }
    }
    
    updateChart();
}

void PollutantOverview::updateChart()
{
    if (!model || !model->hasData()) return;

    QString selectedPollutant = pollutantSelector->currentText();
    if (selectedPollutant.isEmpty()) return;

    QLineSeries* series = new QLineSeries();
    series->setName(selectedPollutant);

    connect(series, &QLineSeries::hovered,
            this, &PollutantOverview::handleHovered);

    QDateTime startDate = startDateEdit->dateTime();
    QDateTime endDate = endDateEdit->dateTime().addDays(1);

    const auto& data = model->getPollutantData(selectedPollutant);
    QMap<QDateTime, QPair<double, int>> dailyData;

    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    for (const auto& record : data) {
        QDateTime recordTime = QDateTime::fromString(record.time, Qt::ISODate);
        if (recordTime >= startDate && recordTime <= endDate) {
            QPair<double, int>& dayData = dailyData[recordTime];
            dayData.first += record.result;
            dayData.second++;

            double value = record.result;
            minY = std::min(minY, value);
            maxY = std::max(maxY, value);
        }
    }

    double sum = 0;
    int count = 0;
    for (const auto& dayData : dailyData) {
        sum += dayData.first;
        count += dayData.second;
    }
    double periodAverage = count > 0 ? sum / count : 0;

    series->setPen(QPen(getComplianceColor(periodAverage), 2));

    QVector<QDateTime> timePoints = dailyData.keys();
    std::sort(timePoints.begin(), timePoints.end());

    for (const QDateTime& time : timePoints) {
        const QPair<double, int>& dayData = dailyData[time];
        double average = dayData.first / dayData.second;
        series->append(time.toMSecsSinceEpoch(), average);
    }

    QChart* chart = chartView->chart();
    chart->removeAllSeries();
    
    const auto oldAxes = chart->axes();
    for (auto axis : oldAxes) {
        chart->removeAxis(axis);
    }

    chart->addSeries(series);

    QDateTimeAxis* axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("Date");
    axisX->setGridLineVisible(true);
    axisX->setLabelsAngle(-45);
    axisX->setTickCount(8);

    QValueAxis* axisY = new QValueAxis();
    axisY->setTitleText("Concentration");
    axisY->setLabelFormat("%.2f");
    axisY->setGridLineVisible(true);

    double yMargin = (maxY - minY) * 0.1;
    if (yMargin == 0) yMargin = maxY * 0.1;
    axisY->setRange(std::max(0.0, minY - yMargin), maxY + yMargin);
    axisY->setTickCount(5);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chart->setMargins(QMargins(10, 10, 10, 10));
    chart->legend()->setAlignment(Qt::AlignTop);
}