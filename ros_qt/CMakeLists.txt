cmake_minimum_required(VERSION 3.0.2)
project(ros_qt)


find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Network Widgets REQUIRED)



catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES ros_qt
#  CATKIN_DEPENDS roscpp std_msgs
#  DEPENDS system_lib
)



include_directories(
# include
  ${catkin_INCLUDE_DIRS}
)



add_executable(qt_ros_node src/main.cpp src/tcpclient.cpp src/tcpserver.cpp )

target_include_directories(qt_ros_node
    PRIVATE 
        ${PROJECT_SOURCE_DIR}/src
)

target_link_libraries(qt_ros_node ${catkin_LIBRARIES} Qt5::Core Qt5::Network Qt5::Widgets)


