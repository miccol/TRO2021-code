# Copyright (C) 2020 Istituto Italiano di Tecnologia (IIT)
# michele.colledanchise@iit.it
# All rights reserved.
#
# This software may be modified and distributed under the terms of the
# BSD-3-Clause license. See the accompanying LICENSE file for details.

cmake_minimum_required(VERSION 3.5)
project(tro2020code)

add_definitions(-std=c++11)


find_package(BehaviorTreeV3 REQUIRED)

find_package(Qt5 COMPONENTS Core Gui Widgets Charts REQUIRED)
#find_package(Qt5Charts REQUIRED) #sudo apt install libqt5charts5 libqt5charts5-dev


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
    set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

include_directories(include/)
file(GLOB BTS_XML
  "xmls/*.xml"
)
file(COPY ${BTS_XML} DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

#setting the src files used
set(PROJECT_SRC
    "src/decorator_progress_sync.cpp"
    "src/decorator_resource_sync.cpp"
    "src/smooth_action.cpp"
    "src/barrier.cpp"
    "src/absolute_barrier.cpp"
    "src/relative_barrier.cpp"
    "src/sync_smooth_action.cpp"
    "src/resource_handler.cpp"
    )

set(PLOT_SRC
    "src/data_plotter.cpp"
)
add_executable(simple_demos simple_demos.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(simple_demos BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

add_executable(simple_demo_with_groot simple_demo_with_groot.cpp ${PROJECT_SRC})
target_link_libraries(simple_demo_with_groot BT::behaviortree_cpp_v3)

add_executable(Example1 examples/example1.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(Example1 BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

add_executable(Example2 examples/example2.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(Example2 BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

add_executable(Example5 examples/example5.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(Example5 BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

add_executable(Experiment1 experiments/analysis_absolute.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(Experiment1 BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

add_executable(Experiment2 experiments/analysis_relative.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(Experiment2 BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

add_executable(Experiment5 experiments/analysis_number_absolute.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(Experiment5 BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

add_executable(Experiment6 experiments/analysis_number_relative.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(Experiment6 BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

add_executable(Experiment7 experiments/analysis_predictability.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(Experiment7 BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

add_executable(Experiment8 examples/greedy_philosophers.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(Experiment8 BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

add_executable(Experiment9 examples/fair_philosophers.cpp ${PROJECT_SRC} ${PLOT_SRC})
target_link_libraries(Experiment9 BT::behaviortree_cpp_v3 Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Charts)

