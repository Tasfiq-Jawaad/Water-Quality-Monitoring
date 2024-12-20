#include "dataset.hpp"
#include "csv.hpp"

void PollutantDataset::loadData(const std::string& filename)
{
    csv::CSVReader reader(filename);
    records.clear();

    for (auto& row : reader) {
        try {
            QString time = QString::fromStdString(row["sample.sampleDateTime"].get<>());
            QString pollutant = QString::fromStdString(row["determinand.label"].get<>());
            QString location = QString::fromStdString(row["sample.samplingPoint.label"].get<>());
            QString definition = QString::fromStdString(row["determinand.definition"].get<>());
            QString unit = QString::fromStdString(row["determinand.unit.label"].get<>());
            QString type = QString::fromStdString(row["sample.sampledMaterialType.label"].get<>());
            
            // deal with the result field
            double concentration = 0.0;
            try {
                concentration = row["result"].get<double>();
            } catch (...) {
                // if failed, try to get the string and convert
                QString resultStr = QString::fromStdString(row["result"].get<std::string>());
                if (resultStr.startsWith("<")) {
                    // deal with small values
                    resultStr = resultStr.mid(1);
                    concentration = resultStr.toDouble();
                } else {
                    concentration = 0.0;  // default value
                }
            }

            // deal with the compliance field
            bool isCompliant = false;
            try {
                isCompliant = row["sample.isComplianceSample"].get<bool>();
            } catch (...) {
                try {
                    std::string complianceStr = row["sample.isComplianceSample"].get<std::string>();
                    std::transform(complianceStr.begin(), complianceStr.end(), 
                                 complianceStr.begin(), ::tolower);
                    isCompliant = (complianceStr == "true" || complianceStr == "1" || 
                                 complianceStr == "yes");
                } catch (...) {
                    isCompliant = false;
                }
            }

            PollutantRecord record {
                time,
                pollutant,
                concentration,
                location,
                definition,
                unit,
                type,
                isCompliant
            };
            records.push_back(record);
        } catch (const std::exception& e) {
            continue;  // if exception occurs, skip this record
        }
    }
}