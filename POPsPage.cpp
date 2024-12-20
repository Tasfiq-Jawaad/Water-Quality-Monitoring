#include "POPsPage.hpp"
#include <QtWidgets>


POPsPage::POPsPage(PollutantModel *dataModel, QWidget *parent)
    : QWidget(parent), model(dataModel)
{
    setupUI();

    tooltipLabel = new QLabel(this);
    tooltipLabel->setStyleSheet(
        "QLabel { background-color: white; color: black; border: 1px solid gray; "
        "padding: 5px; border-radius: 3px; }");
    tooltipLabel->hide();
}

void POPsPage::setupUI()
{
    QHBoxLayout *mainHLayout = new QHBoxLayout(this);
    mainHLayout->setContentsMargins(10, 10, 10, 10);
    mainHLayout->setSpacing(10);

    // leftside main content
    QWidget *mainContent = new QWidget;
    QVBoxLayout *mainLayout = new QVBoxLayout(mainContent);
    mainLayout->setSpacing(10);

    // titles
    QLabel *titleLabel = new QLabel(tr("Persistent Organic Pollutants (POPs)"));
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // control panels
    QGridLayout *controlsLayout = new QGridLayout();

    createPollutantSelector();
    createLocationSelector();
    createDateRangeSelector();

    controlsLayout->addWidget(new QLabel(tr("Select POPs:")), 0, 0);
    controlsLayout->addWidget(pollutantSelector, 0, 1);
    controlsLayout->addWidget(new QLabel(tr("Location:")), 0, 2);
    controlsLayout->addWidget(locationSelector, 0, 3);
    controlsLayout->addWidget(new QLabel(tr("From:")), 1, 0);
    controlsLayout->addWidget(startDateEdit, 1, 1);
    controlsLayout->addWidget(new QLabel(tr("To:")), 1, 2);
    controlsLayout->addWidget(endDateEdit, 1, 3);

    mainLayout->addLayout(controlsLayout);

    // chart
    createChart();
    mainLayout->addWidget(chartView, 1);

    mainHLayout->addWidget(mainContent, 1);

    // rightside info panel
    createInfoPanel();
    mainHLayout->addWidget(infoPanel);

    setMinimumSize(1200, 600);
}

void POPsPage::createLocationSelector()
{
    locationSelector = new QComboBox();
    locationSelector->setMinimumWidth(200);
    connect(locationSelector, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &POPsPage::updateChart);
}

void POPsPage::createPollutantSelector()
{
    pollutantSelector = new QComboBox();
    pollutantSelector->setMinimumWidth(200);
    connect(pollutantSelector, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &POPsPage::updateChart);
}

void POPsPage::createDateRangeSelector()
{
    startDateEdit = new QDateEdit(QDate::currentDate().addDays(-5));
    endDateEdit = new QDateEdit(QDate::currentDate());

    startDateEdit->setCalendarPopup(true);
    endDateEdit->setCalendarPopup(true);

    connect(startDateEdit, &QDateEdit::dateChanged, this, &POPsPage::handleDateRangeChanged);
    connect(endDateEdit, &QDateEdit::dateChanged, this, &POPsPage::handleDateRangeChanged);
}

void POPsPage::createChart()
{
    QChart *chart = new QChart();
    chart->setTitle("POPs Concentration Over Time");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(400);
}

void POPsPage::createInfoPanel()
{
    infoPanel = new QTextEdit();
    infoPanel->setReadOnly(true);
    infoPanel->setMinimumWidth(300);
    infoPanel->setMaximumWidth(400);
}

// Helper functions

QColor POPsPage::getComplianceColor(double value)
{
    if (value > 10)
        return QColor(255, 0, 0); // Red - Danger
    if (value > 1)
        return QColor(255, 165, 0); // Orange - Warning
    return QColor(0, 128, 0);       // Green - Safe
}

QString POPsPage::getComplianceStatus(double value)
{
    if (value > 10)
        return "Exceeding safe levels";
    if (value > 1)
        return "Caution level";
    return "Safe level";
}

void POPsPage::handleHovered(const QPointF &point, bool state)
{
    if (state)
    {
        QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(point.x());

        QString tooltipText = QString(
                                  "Date: %1\n"
                                  "Value: %.2f\n"
                                  "Status: %2")
                                  .arg(dateTime.toString("yyyy-MM-dd"))
                                  .arg(point.y())
                                  .arg(getComplianceStatus(point.y()));

        QPoint pos = chartView->mapFromGlobal(QCursor::pos());
        tooltipLabel->setText(tooltipText);
        tooltipLabel->adjustSize();
        tooltipLabel->move(pos.x() + 10, pos.y() - tooltipLabel->height() / 2);
        tooltipLabel->show();
    }
    else
    {
        tooltipLabel->hide();
    }
}

void POPsPage::handleDateRangeChanged()
{
    if (startDateEdit->date() > endDateEdit->date())
    {
        endDateEdit->setDate(startDateEdit->date().addDays(5));
    }
    updateChart();
}

bool POPsPage::isPOP(const QString &determinandLabel, const QString &definition)
{
    QString label = determinandLabel.toLower();
    QString def = definition.toLower();

    // dataset contains keywords for POPs
    // to match against the label and definition
    QStringList keywords = {
        "organic", "org", "carbon",
        "pcb", "dioxin", "furan",
        "chlorinated", "pesticide",
        "herbicide", "pollutant"};

    for (const QString &keyword : keywords)
    {
        if (label.contains(keyword) || def.contains(keyword))
        {
            return true;
        }
    }

    return false;
}

QString POPsPage::getRiskInfo(const QString &pollutant)
{
    return tr("Persistent Organic Pollutants can accumulate in the environment "
              "and pose risks to human health and ecosystems.\n\n"
              "Health Risks:\n"
              "- Potential carcinogenic effects\n"
              "- Endocrine system disruption\n"
              "- Immune system impacts\n"
              "- Reproductive health concerns\n\n"
              "Environmental Persistence:\n"
              "- Long-lasting in the environment\n"
              "- Bio-accumulative in food chains\n"
              "- Potential for long-range transport");
}

POPsPage::Stats POPsPage::calculateStats(const QString& pollutant, const QString& location)
{
    Stats stats = {0, 0, 0, 0};  // initialize all values to 0
    double sum = 0;
    bool hasData = false;  // check if there is a vaild data point
    
    const auto& data = model->getPollutantData(pollutant);
    for (const auto& record : data) {
        if (location != "All Locations" && record.location != location) {
            continue;
        }
        
        if (!hasData) {
            // first valid data point
            stats.minValue = record.result;
            stats.maxValue = record.result;
            hasData = true;
        } else {
            stats.maxValue = std::max(stats.maxValue, record.result);
            stats.minValue = std::min(stats.minValue, record.result);
        }
        
        sum += record.result;
        stats.sampleCount++;
    }
    
    if (stats.sampleCount > 0) {
        stats.average = sum / stats.sampleCount;
    }
    
    return stats;
}

// Update Functions

void POPsPage::updatePollutantList()
{
    if (!model)
        return;

    QString currentPollutant = pollutantSelector->currentText();
    QString currentLocation = locationSelector->currentText();

    pollutantSelector->clear();
    locationSelector->clear();

    // update location selector
    QSet<QString> locations;
    for (const auto &record : model->getAllData())
    {
        locations.insert(record.location);
    }
    locationSelector->addItem(tr("All Locations"));
    
    for (const auto &location : locations)
    {
        locationSelector->addItem(location);
    }

    locationSelector->model()->sort(0, Qt::AscendingOrder); // default Qt::AscendingOrder

    // update pollutant selector
    for (const auto &pollutant : model->uniquePollutants())
    {
        QString definition = model->getPollutantDefinition(pollutant);
        if (isPOP(pollutant, definition))
        {
            pollutantSelector->addItem(pollutant);
        }
    }

    // restore previous selection
    int pollutantIndex = pollutantSelector->findText(currentPollutant);
    if (pollutantIndex >= 0)
    {
        pollutantSelector->setCurrentIndex(pollutantIndex);
    }

    int locationIndex = locationSelector->findText(currentLocation);
    if (locationIndex >= 0)
    {
        locationSelector->setCurrentIndex(locationIndex);
    }

    updateChart();
    updateInfoPanel();
}

void POPsPage::updateChart()
{
    if (!model || !model->hasData())
        return;

    QString selectedPollutant = pollutantSelector->currentText();
    if (selectedPollutant.isEmpty())
        return;

    QLineSeries *series = new QLineSeries();
    series->setName(selectedPollutant);

    connect(series, &QLineSeries::hovered,
            this, &POPsPage::handleHovered);

    const auto &data = model->getPollutantData(selectedPollutant);
    QString selectedLocation = locationSelector->currentText();
    QDateTime startDate = startDateEdit->dateTime();
    QDateTime endDate = endDateEdit->dateTime().addDays(1);

    QMap<qint64, QPair<double, int>> timeData;

    // collect data points
    for (const auto &record : data)
    {
        if (selectedLocation != "All Locations" && record.location != selectedLocation)
        {
            continue;
        }

        QDateTime recordTime = QDateTime::fromString(record.time, Qt::ISODate);
        if (recordTime < startDate || recordTime > endDate)
        {
            continue;
        }

        qint64 timestamp = recordTime.toMSecsSinceEpoch();
        QPair<double, int> &point = timeData[timestamp];
        point.first += record.result;
        point.second++;
    }

    // calculate average values and add to series
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    for (auto it = timeData.begin(); it != timeData.end(); ++it)
    {
        double average = it.value().first / it.value().second;
        series->append(it.key(), average);

        minY = std::min(minY, average);
        maxY = std::max(maxY, average);
    }

    // get the chart and remove all old series
    QChart *chart = chartView->chart();
    chart->removeAllSeries();

    // clear old axes
    const auto oldAxes = chart->axes();
    for (auto axis : oldAxes)
    {
        chart->removeAxis(axis);
    }

    chart->addSeries(series);

    // create new axes
    QDateTimeAxis *axisX = new QDateTimeAxis();
    axisX->setFormat("yyyy-MM-dd");
    axisX->setTitleText("Date");
    axisX->setGridLineVisible(true);
    axisX->setLabelsAngle(-45);
    axisX->setTickCount(8);

    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Concentration");
    axisY->setLabelFormat("%.2f");
    axisY->setGridLineVisible(true);

    // set range for Y axis
    double margin = (maxY - minY) * 0.1;
    if (margin == 0)
        margin = maxY * 0.1;
    axisY->setRange(std::max(0.0, minY - margin), maxY + margin);
    axisY->setTickCount(5);

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);

    series->attachAxis(axisX);
    series->attachAxis(axisY);

    // update info panel
    updateInfoPanel();
}

void POPsPage::updateInfoPanel()
{
    QString selectedPollutant = pollutantSelector->currentText();
    QString selectedLocation = locationSelector->currentText();
    
    if (selectedPollutant.isEmpty()) {
        infoPanel->clear();
        return;
    }

    Stats stats = calculateStats(selectedPollutant, selectedLocation);
    QString complianceStatus = getComplianceStatus(stats.average);
    QColor statusColor = getComplianceColor(stats.average);
    
    // get determinand.unit.label
    QString unit;
    const auto& data = model->getPollutantData(selectedPollutant);
    if (!data.empty()) {
        unit = data[0].unit;
    }
    
    QString html = QString(
        "<div style='padding: 10px;'>"
        "<h3>%1</h3>"
        "<p><b>Location:</b> %2</p>"
        "<p><b>Current Status:</b> <span style='color: %3;'>%4</span></p>"
        "<h4>Statistics:</h4>"
        "<ul>"
        "<li>Average: %5 %9</li>"
        "<li>Maximum: %6 %9</li>"
        "<li>Minimum: %7 %9</li>"
        "<li>Sample Count: %8</li>"
        "</ul>"
        "<h4>Health and Environmental Information:</h4>"
        "<p>%10</p>"
        "</div>"
    ).arg(selectedPollutant)
     .arg(selectedLocation)
     .arg(statusColor.name())
     .arg(complianceStatus)
     .arg(QString::number(stats.average, 'f', 5))
     .arg(QString::number(stats.maxValue, 'f', 5))
     .arg(QString::number(stats.minValue, 'f', 5))
     .arg(stats.sampleCount)
     .arg(unit)
     .arg(getRiskInfo(selectedPollutant));

    infoPanel->setHtml(html);
}