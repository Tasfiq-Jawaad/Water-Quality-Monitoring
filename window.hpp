#pragma once

#include <QMainWindow>
#include "model.hpp"
#include "PollutantOverview.hpp"
#include "POPsPage.hpp"
#include "ComplianceDashboard.hpp"
#include "EnvironmentalLitterIndicators.hpp"
#include "CardWidget.hpp"

class QComboBox;
class QLabel;
class QPushButton;
class QTableView;
class QTabWidget;

class WaterQualityWindow: public QMainWindow
{
    Q_OBJECT

public:
    WaterQualityWindow();

private:
    void createMainWidget();
    void createButtons();
    void createToolBar();
    void createStatusBar();
    void addFileMenu();
    void addHelpMenu();
    void createCardsWidget();

    PollutantModel model;      
    QString dataFilePath;      
    QTabWidget* tabWidget;     

    // Pages
    QWidget* dashboardPage;
    PollutantOverview* overviewPage;
    POPsPage* popsPage;
    ComplianceDashboard* compliancePage;
    QWidget* dataPage;
    EnvironmentalLitterIndicators* litterIndicatorPage;
    
    // Controls
    QTableView* table;         
    QLabel* fileInfo;          
    QComboBox* pollutantSelector; 
    QPushButton* loadButton;   

private slots:
    void openCSV();
    void about();
};