#include "ComplianceDashboard.hpp"
#include <QtWidgets>

ComplianceDashboard::ComplianceDashboard(PollutantModel *dataModel, QWidget *parent)
    : QWidget(parent), model(dataModel)
{
    setupInterface();
}

// This page takes a lot of load time. This function delays loading the data of the page, 
// so that the main window can be displayed quickly if user does not navigate to this page.
void ComplianceDashboard::showEvent(QShowEvent* event)
{
    if (!isInitialized && model && model->hasData()) {
        // create a loading dialog
        loadingDialog = new QProgressDialog("Loading compliance data...", QString(), 0, 0, this);
        loadingDialog->setWindowModality(Qt::WindowModal); 
        loadingDialog->setAutoClose(true);  
        loadingDialog->setCancelButton(nullptr);  
        loadingDialog->setWindowTitle("Please Wait");
        loadingDialog->setMinimumDuration(0); 
        loadingDialog->show();

        QTimer::singleShot(0, this, [this]() {
            updatePollutantList();
            isInitialized = true;
            
            // close the loading dialog
            if (loadingDialog) {
                loadingDialog->close();
                delete loadingDialog;
                loadingDialog = nullptr;
            }
        });
    }
    QWidget::showEvent(event);
}

void ComplianceDashboard::setupInterface()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    // Title
    QLabel *titleLabel = new QLabel(tr("Compliance Dashboard"));
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    mainLayout->addWidget(titleLabel);

    // Filters
    QGridLayout *filterLayout = new QGridLayout();
    createFilters();

    filterLayout->addWidget(new QLabel(tr("Location:")), 0, 0);
    filterLayout->addWidget(locationSelector, 0, 1);
    filterLayout->addWidget(new QLabel(tr("From:")), 0, 2);
    filterLayout->addWidget(startDateEdit, 0, 3);
    filterLayout->addWidget(new QLabel(tr("To:")), 1, 0);
    filterLayout->addWidget(endDateEdit, 1, 1);
    filterLayout->addWidget(new QLabel(tr("Status:")), 1, 2);
    filterLayout->addWidget(complianceFilter, 1, 3);

    mainLayout->addLayout(filterLayout);

    // stats panel
    rightPanelStats = new QLabel();
    rightPanelStats->setWordWrap(true);
    rightPanelStats->setStyleSheet("QLabel { font-size: 12px; padding: 10px; }");
    mainLayout->addWidget(rightPanelStats);

    // table
    createTable();
    mainLayout->addWidget(dataTable, 1);
}

void ComplianceDashboard::createFilters()
{
    // location selector
    locationSelector = new QComboBox();
    locationSelector->addItem("All Locations");
    connect(locationSelector, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &ComplianceDashboard::handleLocationChanged);

    // date range
    startDateEdit = new QDateEdit(QDate::currentDate().addDays(-30));
    endDateEdit = new QDateEdit(QDate::currentDate());
    startDateEdit->setCalendarPopup(true);
    endDateEdit->setCalendarPopup(true);
    connect(startDateEdit, &QDateEdit::dateChanged, this, &ComplianceDashboard::handleDateRangeChanged);
    connect(endDateEdit, &QDateEdit::dateChanged, this, &ComplianceDashboard::handleDateRangeChanged);

    // compliance filter
    complianceFilter = new QComboBox();
    complianceFilter->addItems({"All", "Compliant", "Warning", "Non-Compliant"});
    connect(complianceFilter, QOverload<const QString &>::of(&QComboBox::currentTextChanged),
            this, &ComplianceDashboard::handleComplianceFilterChanged);
}

void ComplianceDashboard::createTable()
{
    dataTable = new QTableWidget(this);
    dataTable->setColumnCount(5);
    dataTable->setHorizontalHeaderLabels({"Pollutant",
                                          "Average value",
                                          "Unit",
                                          "Compliant locations",
                                          "Status"});
    dataTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    dataTable->setAlternatingRowColors(true);
    dataTable->setEditTriggers(QTableWidget::NoEditTriggers);
}

void ComplianceDashboard::updateLocationSelector()
{
    if (!model || !model->hasData())
    {
        locationSelector->setEnabled(false);
        return;
    }

    QString currentLocation = locationSelector->currentText();
    locationSelector->clear();
    locationSelector->addItem("All Locations");

    QSet<QString> locations;
    const auto &allData = model->getAllData();
    for (const auto &record : allData)
    {
        if (!record.location.isEmpty())
        {
            locations.insert(record.location);
        }
    }

    QStringList sortedLocations = locations.values();
    sortedLocations.sort();
    locationSelector->addItems(sortedLocations);

    int index = locationSelector->findText(currentLocation);
    if (index >= 0)
    {
        locationSelector->setCurrentIndex(index);
    }

    locationSelector->setEnabled(true);
}

void ComplianceDashboard::handleLocationChanged(const QString &location)
{
    updateDashboard();
}

void ComplianceDashboard::handleDateRangeChanged()
{
    QDate startDate = startDateEdit->date();
    QDate endDate = endDateEdit->date();

    if (startDate > endDate)
    {
        endDateEdit->setDate(startDate.addDays(30));
    }
    updateDashboard();
}

void ComplianceDashboard::handleComplianceFilterChanged(const QString &status)
{
    updateDashboard();
}

void ComplianceDashboard::updatePollutantList()
{
    if (!model || !model->hasData()) {
        isInitialized = false;  // reset the flag of showEvent()
        return;
    }

    updateLocationSelector();
    updateDashboard();
}

void ComplianceDashboard::updateDashboard()
{
    if (!model || !model->hasData())
    {
        statsLabel->clear();
        rightPanelTitle->clear();
        rightPanelStats->clear();
        return;
    }

    updateStatsPanel();
    updateTable();
}

void ComplianceDashboard::updateTable()
{
    dataTable->setRowCount(0);

    const auto pollutants = model->uniquePollutants();
    QString filterStatus = complianceFilter->currentText();

    for (const auto &pollutant : pollutants) {
        ComplianceStatus status = calculateStatus(pollutant);
        if (status.totalSites == 0) continue;

        if (filterStatus != "All" && status.status != filterStatus) {
            continue;
        }

        int row = dataTable->rowCount();
        dataTable->insertRow(row);

        dataTable->setItem(row, 0, new QTableWidgetItem(status.pollutant));
        dataTable->setItem(row, 1, new QTableWidgetItem(QString::number(status.averageValue, 'f', 2)));
        dataTable->setItem(row, 2, new QTableWidgetItem(status.unit));
        dataTable->setItem(row, 3, new QTableWidgetItem(QString("%1/%2")
            .arg(status.totalSites - status.nonCompliantSites)
            .arg(status.totalSites)));
        
        QTableWidgetItem* statusItem = new QTableWidgetItem(status.status);
        if (status.status == "Compliant") {
            statusItem->setForeground(QColor(0, 128, 0));  // green
        } else if (status.status == "Warning") {
            statusItem->setForeground(QColor(255, 165, 0));  // amber
        } else {
            statusItem->setForeground(QColor(255, 0, 0));  // red
        }
        dataTable->setItem(row, 4, statusItem);
    }
}

void ComplianceDashboard::updateStatsPanel()
{
    if (!model || !model->hasData()) {
        rightPanelStats->clear();
        return;
    }

    OverallStats stats = calculateOverallStats();
    double nonCompliantRate = static_cast<double>(stats.nonCompliantCount) / stats.totalPollutants;
    
    QString complianceStatus;
    QString colorStyle;
    if (nonCompliantRate > 0.3) {
        complianceStatus = tr("Non-Compliant");
        colorStyle = "color: red";
    } else if (nonCompliantRate > 0.15) {
        complianceStatus = tr("Warning");
        colorStyle = "color: #FFA500"; // amber
    } else {
        complianceStatus = tr("Compliant");
        colorStyle = "color: green";
    }

    QString location = locationSelector->currentText();
    rightPanelStats->setText(QString(
        "<div style='font-size: 16px;'><b>%1</b></div>"
        "<div style='margin-top: 10px;'>Statistics:<br/></div>"
        "• Total samples: %2<br/>"
        "• Compliant: %3<br/>"
        "• Warning: %4<br/>"
        "• Non-Compliant: %5<br/>"
        "<div style='margin-top: 10px;'>Overall Status: <span style='%6'><b>%7</b></span></div>")
        .arg(location)
        .arg(stats.totalPollutants)
        .arg(stats.compliantCount)
        .arg(stats.warningCount)
        .arg(stats.nonCompliantCount)
        .arg(colorStyle)
        .arg(complianceStatus));

    rightPanelStats->setStyleSheet("font-size: 16px;");

}

ComplianceStatus ComplianceDashboard::calculateStatus(const QString &pollutant)
{
    ComplianceStatus status;
    status.pollutant = pollutant;
    status.totalSites = 0;
    status.nonCompliantSites = 0;
    status.averageValue = 0.0;

    const auto &data = model->getPollutantData(pollutant);
    if (data.empty()) return status;

    QMap<QString, QPair<double, bool>> siteData;
    // store the count of samples for each site
    QMap<QString, int> siteCount;

    QString location = locationSelector->currentText();
    QDateTime startDate = startDateEdit->dateTime();
    QDateTime endDate = endDateEdit->dateTime().addDays(1);

    // calculate the average value for each site
    for (const auto &record : data) {
        if (location != "All Locations" && record.location != location) {
            continue;
        }

        QDateTime recordTime = QDateTime::fromString(record.time, Qt::ISODate);
        if (!recordTime.isValid() || recordTime < startDate || recordTime > endDate) {
            continue;
        }

        // update the site data
        if (!siteData.contains(record.location)) {
            siteData[record.location] = QPair<double, bool>(0.0, record.isComplianceSample);
            siteCount[record.location] = 0;
        }
        siteData[record.location].first += record.result;
        siteCount[record.location]++;
        // set the site as non-compliant if any sample is non-compliant
        if (!record.isComplianceSample) {
            siteData[record.location].second = false;
        }
    }

    // calculate the total average value
    double totalSum = 0.0;
    int totalCount = 0;
    status.totalSites = siteData.size();

    for (auto it = siteData.begin(); it != siteData.end(); ++it) {
        const QString& site = it.key();
        double siteAverage = it.value().first / siteCount[site];
        bool isCompliant = it.value().second;

        totalSum += siteAverage;
        totalCount++;

        if (!isCompliant) {
            status.nonCompliantSites++;
            status.nonCompliantLocations.append(site);
        }
    }

    // set the status
    status.averageValue = totalCount > 0 ? totalSum / totalCount : 0.0;
    status.unit = !data.empty() ? data[0].unit : "";
    
    // set overall status
    if (status.nonCompliantSites == 0) {
        status.status = "Compliant";
    } else {
        status.status = "Non-Compliant";
    }

    return status;
}

QString ComplianceDashboard::getComplianceStatus(double value)
{
    if (value > 10.0)
        return "Non-Compliant";
    if (value > 1.0)
        return "Warning";
    return "Compliant";
}

OverallStats ComplianceDashboard::calculateOverallStats()
{
    OverallStats stats;
    QString selectedLocation = locationSelector->currentText();
    int totalSamples = 0;
    int compliantSamples = 0;

    for (const auto &pollutant : model->uniquePollutants())
    {
        ComplianceStatus status = calculateStatus(pollutant);
        if (status.totalSites == 0)
            continue;

        stats.totalPollutants++;
        totalSamples += status.totalSites;
        compliantSamples += (status.totalSites - status.nonCompliantSites);

        if (status.averageValue > 10.0)
        {
            stats.nonCompliantCount++;
        }
        else if (status.averageValue > 1.0)
        {
            stats.warningCount++;
        }
        else
        {
            stats.compliantCount++;
        }
    }

    stats.complianceRate = totalSamples > 0 ? (double)compliantSamples / totalSamples : 0.0;
    return stats;
}