#pragma once

#include <QWidget>
#include <QtCharts>
#include "model.hpp"

class QComboBox;
class QChartView;
class QLabel;
class QTextEdit;
class QDateEdit; 

class POPsPage : public QWidget
{
    Q_OBJECT

public:
    explicit POPsPage(PollutantModel* model, QWidget* parent = nullptr);
    void updatePollutantList();

private slots:
    void updateChart();
    void handleHovered(const QPointF &point, bool state);
    void handleDateRangeChanged(); 
    void updateInfoPanel();

private:
    void setupUI();
    void createPollutantSelector();
    void createLocationSelector();
    void createDateRangeSelector();
    void createChart();
    void createInfoPanel();
    QColor getComplianceColor(double value);
    QString getComplianceStatus(double value);
    bool isPOP(const QString& determinandLabel, const QString& definition);
    QString getRiskInfo(const QString& pollutant);

    PollutantModel* model;
    QComboBox* pollutantSelector;
    QComboBox* locationSelector;
    QDateEdit* startDateEdit;
    QDateEdit* endDateEdit;
    QChartView* chartView;
    QLabel* tooltipLabel;
    QTextEdit* infoPanel;
    
    struct Stats {
        double average;
        double maxValue;
        double minValue;
        int sampleCount;
    };
    Stats calculateStats(const QString& pollutant, const QString& location);
};