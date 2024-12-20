# Project Overview

This application is designed to monitor and analyse water quality data, with a focus on pollutant tracking and compliance with safety standards. It was developed using **C++/Qt6** with a modular and user-friendly interface that adapts to changes in locale for basic internationalisation.

## Getting Started

### Dependencies

- **C++17** or higher
- **CSV Parser**
- **Qt6** libraries:
  - **QtWidgets**
  - **QtCharts**

### Setup Instructions

**1 - Compilation**

1. Run `mkdir build && cd build && cmake ..` in the `/1 - Application` directory to create & navigate to the build folder and to create cmake configuration files.

   ```bash
   mkdir build && cd build
   cmake ..
   ```

2. Compile the project by running the `make` command.

   ```bash
   make
   ```

**2 - Running the Application**

1. Navigate to the `/build` directory created previously.
2. Run the application using the `./wqapp` command in terminal, or by running the `wqapp.exe` executable file

   ```bash
   ./wqapp
   ```

### Usage Instructions

1. Load CSV file by clicking on the "Load CSV" button, and selecting the file containing the dataset.
2. Navigate to different tabs to select different views.
3. Filter or search the data using the search bar and drop-down menus.
4. Data sheet can be found at: https://environment.data.gov.uk/water-quality/view/download

### Dashboard

The dashboard page is the first page of the application. It provides quick access to all the pages and shows basic overview.

### Data page

The data page shows a basic overview of all the data in table format.

### Pollutant overview page

This page shows the commonn pollutants and represents data in line chart and provides compliance details.

### POP page

This page shows the commonn POP elements and represents data in line chart and provides compliance details on the side panel.

### Environmental Litter Indicators Page

This page shows the commonn physical pollutant elements and represents data in bar chart.

### Compliance Page

This page shows the overall compliance in a table format.
