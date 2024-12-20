#ifndef ENVIRONMENTALLITTERINDICATORS_HPP
#define ENVIRONMENTALLITTERINDICATORS_HPP

#include <QtWidgets>
#include <QtCharts>
#include "model.hpp"

class EnvironmentalLitterIndicators : public QWidget
{
    Q_OBJECT

public:
    explicit EnvironmentalLitterIndicators(PollutantModel *dataModel, QWidget *parent = nullptr);
    void updateFromModel();

private:
    // Data model
    PollutantModel *model;

    // UI components
    QChartView *litterComparisonChartView;
    QLabel *complianceLabel;
    QComboBox *locationFilter;
    QComboBox *waterType;
    QComboBox *pollutants;

    // Initialization methods
    void initializeInterface();
    void createFilterPanel();
    void createComparisonChart();
    void updateComparisonChart();
    void updateComplianceIndicator();

private slots:
    void handleLocationChanged();
    void handleTypeChanged();
    void handlePollutantChanged();
};

#endif // ENVIRONMENTALLITTERINDICATORS_HPP
