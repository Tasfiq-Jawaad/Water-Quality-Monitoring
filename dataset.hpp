#pragma once

#include <string>
#include <vector>
#include <QString>

struct PollutantRecord {
   QString time;
   QString pollutant;
   double result;
   QString location;
   QString definition;  
   QString unit;        
   QString type;        
   bool isComplianceSample; 
};

class PollutantDataset
{
public:
   PollutantDataset() {}
   void loadData(const std::string& filename);

   int size() const { return (int)records.size(); }
   const PollutantRecord& operator[](int index) const { return records.at(index); }
   const std::vector<PollutantRecord>& data() const { return records; }

private:
   std::vector<PollutantRecord> records;
};
