#include "model.hpp"

void PollutantModel::updateFromFile(const QString& filename)
{
    beginResetModel();
    dataset.loadData(filename.toStdString());
    endResetModel();

    applyFilter();
}

QVariant PollutantModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    if (role == Qt::DisplayRole) {
        const auto& record = filteredData.at(index.row());
        switch (index.column()) {
            case 0: return record.time; 
            case 1: return record.pollutant; 
            case 2: return record.result;   // concentration value
            case 3: return record.location; 
        }
    }

    return QVariant();
}

void PollutantModel::applyFilter()
{
    beginResetModel();
    filteredData.clear();
    for (const auto& r: dataset.data()) {
        if (currentFilter == "All" || r.pollutant == currentFilter) {
            filteredData.push_back(r);
        }
    }
    endResetModel();
}

void PollutantModel::setFilterPollutant(const QString& pollutant)
{
    currentFilter = pollutant;
    applyFilter();
}

std::vector<QString> PollutantModel::uniquePollutants() const
{
    std::set<QString> unique;
    for (auto& r: dataset.data()) {
        unique.insert(r.pollutant);
    }
    return std::vector<QString>(unique.begin(), unique.end());
}

std::vector<QString> PollutantModel::uniqueTypes() const
{
    std::set<QString> unique;
    for (auto& r: dataset.data()) {
        unique.insert(r.type);
    }
    return std::vector<QString>(unique.begin(), unique.end());
}

QString PollutantModel::getPollutantDefinition(const QString& pollutant) const
{
    for (const auto& record : dataset.data()) {
        if (record.pollutant == pollutant) {
            return record.definition;  
        }
    }
    return QString();
}
