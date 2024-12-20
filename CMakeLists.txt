cmake_minimum_required(VERSION 3.16)

project(cwk2 VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Charts)

qt_standard_project_setup()

qt_add_executable(wqapp
    main.cpp
    dataset.cpp
    model.cpp
    window.cpp
    PollutantOverview.cpp
    POPsPage.cpp
    ComplianceDashboard.cpp
    EnvironmentalLitterIndicators.cpp
    CardWidget.cpp
)

target_link_libraries(wqapp PRIVATE Qt6::Widgets Qt6::Charts)

set_target_properties(wqapp PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE OFF
)