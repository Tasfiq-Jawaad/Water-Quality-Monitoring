#pragma once

#include <QWidget>
#include <QtCharts>
#include "model.hpp"

class QComboBox;
class QDateEdit;
class QChartView;
class QLineEdit;

class PollutantOverview : public QWidget
{
    Q_OBJECT

public:
    explicit PollutantOverview(PollutantModel* model, QWidget* parent = nullptr);
    void updatePollutantList();

private slots:
    void updateChart();
    void handleDateRangeChanged();
    void handleHovered(const QPointF &point, bool state);
    void handleSearch();

private:
    void setupUI();
    void createDateRangeSelector();
    void createPollutantSelector();
    void createSearchBox();
    void createChart();
    QColor getComplianceColor(double value);
    QString getComplianceStatus(double value);

    PollutantModel* model;
    QComboBox* pollutantSelector;
    QDateEdit* startDateEdit;
    QDateEdit* endDateEdit;
    QChartView* chartView;
    QLabel* tooltipLabel;
    QLineEdit* searchBox;
};