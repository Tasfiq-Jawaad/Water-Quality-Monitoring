#include "EnvironmentalLitterIndicators.hpp"

EnvironmentalLitterIndicators::EnvironmentalLitterIndicators(PollutantModel *dataModel, QWidget *parent)
    : QWidget(parent), model(dataModel)
{
    initializeInterface();
}

void EnvironmentalLitterIndicators::initializeInterface()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    QWidget* titlePanel = new QWidget;
    QHBoxLayout* titleLayout = new QHBoxLayout(titlePanel);
    titleLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* titleLabel = new QLabel(tr("Environmental Litter Indicators"));
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();

    mainLayout->addWidget(titlePanel);

    // Create filter panel
    createFilterPanel();
    QWidget* filterPannel = new QWidget;

    QHBoxLayout *filterLayout = new QHBoxLayout(filterPannel);

    filterLayout->setContentsMargins(0, 0, 0, 0);
    filterLayout->setSpacing(0);

    filterLayout->addWidget(new QLabel(tr("Material: ")));
    filterLayout->addWidget(waterType);

    filterLayout->addWidget(new QLabel(tr("Location: ")));
    filterLayout->addWidget(locationFilter);

    filterLayout->addWidget(new QLabel(tr("Pollutants: ")));
    filterLayout->addWidget(pollutants);

    mainLayout->addWidget(filterPannel);

    createComparisonChart();

    mainLayout->addWidget(litterComparisonChartView);

    setLayout(mainLayout);
    setMinimumSize(800, 600);
}

void EnvironmentalLitterIndicators::createFilterPanel()
{
    locationFilter = new QComboBox();
    locationFilter->addItem("All Locations");

    waterType = new QComboBox();
    waterType->addItem("All Types");

    pollutants = new QComboBox();
    pollutants->addItem("All Pollutants");

    
    connect(locationFilter, &QComboBox::currentTextChanged, this, &EnvironmentalLitterIndicators::handleLocationChanged);
    connect(waterType, &QComboBox::currentTextChanged, this, &EnvironmentalLitterIndicators::handleTypeChanged);
    connect(pollutants, &QComboBox::currentTextChanged, this, &EnvironmentalLitterIndicators::handlePollutantChanged);
}

void EnvironmentalLitterIndicators::createComparisonChart()
{
    QChart *chart = new QChart();
    chart->setTitle("Pollutant Levels Across Locations");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    litterComparisonChartView = new QChartView(chart);
    litterComparisonChartView->setRenderHint(QPainter::Antialiasing);
    litterComparisonChartView->setMinimumHeight(400);
}

void EnvironmentalLitterIndicators::updateComparisonChart()
{
    QString currentLocation = locationFilter->currentText();
    QString currentType = waterType->currentText();
    QString currentPollutant = pollutants->currentText();

    // Dynamic data structure
    QMap<QString, double> compliantData;     
    QMap<QString, double> nonCompliantData; 

    // Initialize a variable to track the maximum value for y-axis
    double maxValue = 0.0;

    for (const auto &result : model->getAllData()) {
        if (result.location != currentLocation ||
            (result.type.toUpper() != currentType.toUpper() && currentType != "All Types") ||
            result.pollutant != currentPollutant) {
            continue;
        }

        QString time = result.time;
        if (result.result >= 2.0) { // arbitrary value set as threshold 
            nonCompliantData[time] += result.result; // non-compliant values
        } else {
            compliantData[time] += result.result;   // compliant values
        }

        // Update maxValue if the current result is greater
        maxValue = std::max(maxValue, result.result);
    }


    // Create a new chart
    QChart *newChart = new QChart();
    newChart->setTitle("Pollutant result for " + currentPollutant);
    newChart->setAnimationOptions(QChart::SeriesAnimations);

    if (compliantData.isEmpty() && nonCompliantData.isEmpty()) {
        newChart->setTitle("No Data Found for the Selected Filters");
        litterComparisonChartView->setChart(newChart);
        return;
    }

    QStringList times = compliantData.keys() + nonCompliantData.keys();
    std::sort(times.begin(), times.end());

    // Split the time labels
    QStringList formattedTimes;
    for (const QString &time : times) {
        // Extract components from the time string
        QStringList dateTimeParts = time.split('T'); // Split into date and time parts
        if (dateTimeParts.size() == 2) {
            QString datePart = dateTimeParts[0]; 
            QString timePart = dateTimeParts[1]; 

            // Further split the datePart into year, month, day
            QStringList dateParts = datePart.split('-');
            if (dateParts.size() == 3) {
                QString year = dateParts[0];
                QString formattedTime = QString("<b>%1</b><br>%2").arg(year, timePart.left(5));
                formattedTimes.append(formattedTime);
            } else {
                formattedTimes.append(time); // Fallback: Add raw time if parsing fails
            }
        } else {
            formattedTimes.append(time); // Fallback: Add raw time if parsing fails
        }
    }


    // Create bar sets for compliant and non-compliant data
    QBarSet *compliantSet = new QBarSet("Compliant");
    compliantSet->setColor(QColor(173, 216, 230)); // Light blue color (RGB)

    QBarSet *nonCompliantSet = new QBarSet("Non-Compliant");
    nonCompliantSet->setColor(QColor(255, 165, 0)); // Orange color (RGB)

    // Populate the bar sets with data for each time
    for (const QString &time : times) {
        compliantSet->append(compliantData.value(time, 0.0));
        nonCompliantSet->append(nonCompliantData.value(time, 0.0));
    }

    // Create the bar series
    QBarSeries *barSeries = new QBarSeries();
    barSeries->append(compliantSet);
    barSeries->append(nonCompliantSet);

    // Set up X-axis (Times)
    QBarCategoryAxis *axisX = new QBarCategoryAxis();
    axisX->append(formattedTimes);
    axisX->setTitleText("Time Period");

    // Set up Y-axis (Values)
    QValueAxis *axisY = new QValueAxis();
    axisY->setTitleText("Pollutant value");
    axisY->setRange(0, maxValue); // Use the calculated maxValue

    // Add axes to the chart
    newChart->addAxis(axisX, Qt::AlignBottom);
    newChart->addAxis(axisY, Qt::AlignLeft);

    // Attach the bar series to the axes
    barSeries->attachAxis(axisX);
    barSeries->attachAxis(axisY);

    // Add the bar series to the chart
    newChart->addSeries(barSeries);

    // Add legend
    newChart->legend()->setVisible(true);
    newChart->legend()->setAlignment(Qt::AlignBottom);

    // Update the view
    litterComparisonChartView->setChart(newChart);
    litterComparisonChartView->setRenderHint(QPainter::Antialiasing);
}

void EnvironmentalLitterIndicators::handleLocationChanged()
{
    QString currentType = waterType->currentText();
    QString currentLocation = locationFilter->currentText();

    pollutants->clear();

    QStringList keywords = {
    "BWP", "SewageDebris", "TarryResidus",
    };

    QSet<QString> uniquePollutants; // To keep track of unique pollutants

    for (const auto &result : model->getAllData()) {
        if (result.location == currentLocation && (result.type.toUpper() == currentType.toUpper() || currentType == "All Types")) {
            QString label = result.pollutant.toLower();

            for (const QString &keyword : keywords) {
                if (label.contains(keyword.toLower())) {
                    if (!uniquePollutants.contains(result.pollutant)) {
                        uniquePollutants.insert(result.pollutant); // Add to the set
                        pollutants->addItem(result.pollutant); // Add to the QComboBox
                    }
                    break; // Exit the loop since a match is found
                }
            }
        }
    }

    if (uniquePollutants.isEmpty()) {
        pollutants->addItem("No physical pollutant found");
    }
}

void EnvironmentalLitterIndicators::handleTypeChanged()
{
    QString currentType = waterType->currentText();
    QString currentLocation = locationFilter->currentText();

    locationFilter->clear();

    QSet<QString> locations;
    for (const auto &record : model->getAllData())
    {   
        if(currentType == "All Types" || record.type == currentType.toUpper())
            locations.insert(record.location);
    }

    for (const auto &location : locations)
    {
        locationFilter->addItem(location);
    }

    locationFilter->model()->sort(0, Qt::AscendingOrder);

    locationFilter->setCurrentIndex(0);
}

void EnvironmentalLitterIndicators::handlePollutantChanged()
{
    updateComparisonChart();
}

void EnvironmentalLitterIndicators::updateFromModel()
{
    if (!model)
        return;

    QString currentType = waterType->currentText();
    QString currentLocation = locationFilter->currentText();

    waterType->clear();
    locationFilter->clear();

    waterType->addItem("All Types");

    for (const auto &type : model->uniqueTypes())
    {
        QString label = type.toLower();

        for (int x = 0; x < label.length(); x++)
        {
            // If it's the first character of the string or after a space, capitalize it
            if (x == 0 || label[x - 1] == ' ')
            {
                label[x] = label[x].toUpper(); // Convert the character to uppercase
            }
        }

        waterType->addItem(label);
    }
}
