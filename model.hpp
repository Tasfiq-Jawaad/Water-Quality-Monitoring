#pragma once

#include <QAbstractTableModel>
#include <QString>
#include <vector>
#include <set>
#include "dataset.hpp"

class PollutantModel: public QAbstractTableModel
{
   Q_OBJECT

public:
   PollutantModel(QObject* parent = nullptr): QAbstractTableModel(parent) {}
   void updateFromFile(const QString&);
   bool hasData() const { return dataset.size() > 0; }

   int rowCount(const QModelIndex&) const override { return (int)filteredData.size(); }
   int columnCount(const QModelIndex&) const override { return 4; }

   QVariant data(const QModelIndex&, int) const override;

   void setFilterPollutant(const QString& pollutant);
   std::vector<QString> uniquePollutants() const;
   std::vector<QString> uniqueTypes() const;
   
   std::vector<PollutantRecord> getPollutantData(const QString& pollutant) const {
       std::vector<PollutantRecord> result;
       for (const auto& record : dataset.data()) {
           if (pollutant.isEmpty() || record.pollutant == pollutant) {
               result.push_back(record);
           }
       }
       return result;
   }
   
   QString getPollutantDefinition(const QString& pollutant) const;
   const std::vector<PollutantRecord>& getAllData() const { return dataset.data(); }

private:
   PollutantDataset dataset;
   std::vector<PollutantRecord> filteredData; 
   QString currentFilter = "All";

   void applyFilter();
};
