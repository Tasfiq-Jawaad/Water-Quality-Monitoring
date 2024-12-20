#pragma once

#include <QWidget>
#include "model.hpp"
#include <QProgressDialog>

class QComboBox;
class QLabel;
class QDateEdit;
class QTableWidget;

// to store overall statistics
struct OverallStats {
    int totalPollutants = 0;
    int compliantCount = 0;
    int warningCount = 0;
    int nonCompliantCount = 0;
    double complianceRate = 0.0;
};

// to store compliance status
struct ComplianceStatus {
    QString pollutant;
    double averageValue;
    QString unit;
    int totalSites;
    int nonCompliantSites;
    QVector<QString> nonCompliantLocations;
    QString status;
};

class ComplianceDashboard : public QWidget
{
    Q_OBJECT

    public:
        explicit ComplianceDashboard(PollutantModel* model, QWidget* parent = nullptr);
        void updatePollutantList();

    protected:
        void showEvent(QShowEvent* event) override;

    private slots:
        void handleLocationChanged(const QString& location);
        void handleDateRangeChanged();
        void handleComplianceFilterChanged(const QString& status);
        void updateDashboard();

    private:
        void setupInterface();
        void createFilters();
        void createTable();
        void updateLocationSelector();
        void updateTable();
        void updateStatsPanel();
        ComplianceStatus calculateStatus(const QString& pollutant);
        QString getComplianceStatus(double value);
        OverallStats calculateOverallStats();

        // UI components
        PollutantModel* model;
        QComboBox* locationSelector;
        QComboBox* complianceFilter;
        QDateEdit* startDateEdit;
        QDateEdit* endDateEdit;
        QLabel* statsLabel;
        QTableWidget* dataTable;

        // Right panel for detailed statistics
        QWidget* rightPanel;
        QLabel* rightPanelTitle;
        QLabel* rightPanelStats;
        QLabel* rightPanelInfo;
        QProgressDialog* loadingDialog = nullptr;

        // showEvent() flag
        bool isInitialized = false;  
};