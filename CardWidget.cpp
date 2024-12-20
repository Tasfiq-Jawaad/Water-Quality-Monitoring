#include "CardWidget.hpp"

CardWidget::CardWidget(const QString &title, QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);

    button = new QPushButton(this);
    QFont titleFont;
    titleFont.setPointSize(12);
    titleFont.setBold(true);
    
    // Create title and description labels
    QLabel *titleLabel = new QLabel(title);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    
    QString description;
    if(title == "Data") {
        description = "View raw data in tabular format";
    } else if(title == "Pollutant Overview") {
        description = "Visual overview of pollutant data"; 
    } else if(title == "POPs") {
        description = "Analysis of Persistent Organic Pollutants";
    } else if(title == "Environmental Litter Indicators") {
        description = "Statistical analysis of environmental litter indicators";
    } else if(title == "Compliance") {
        description = "Compliance analysis and monitoring dashboard";
    }

    QLabel *descLabel = new QLabel(description);
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignCenter);
    
    // Add labels to button layout
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(titleLabel);
    buttonLayout->addWidget(descLabel);
    button->setLayout(buttonLayout);

    button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    button->setStyleSheet(
        "QPushButton {"
        "  background-color: #32527b;"
        "  border: 1px solid #2c3e50;" 
        "  border-radius: 8px;"
        "  padding: 15px;"
        "  text-align: center;"
        "  color: #ffffff;"  
        "  font-weight: bold;"
        "  font: 20px;"          
        "}"
        "QPushButton:hover {"
        "  background-color: #2c3e50;"  
        "  border: 1px solid #32527b;"
        "}"
    );
    
    layout->addWidget(button);

    // Navigate when card is clicked
    connect(button, &QPushButton::clicked, this, &CardWidget::cardClicked);
}