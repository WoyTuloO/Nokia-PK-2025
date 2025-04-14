project(UeApplication)
cmake_minimum_required(VERSION 3.12)

aux_source_directory(. SRC_LIST)
aux_source_directory(Ports SRC_LIST)
aux_source_directory(States SRC_LIST)

set(SRC_LIST ${SRC_LIST} SmsStorage.cpp)


set(APPLICATION_SOURCES
Application.cpp
SmsStorage.cpp
        Ports/BtsPort.cpp
        Ports/UserPort.cpp
        Ports/TimerPort.cpp

        States/BaseState.cpp
        States/ConnectedState.cpp
        States/ConnectingState.cpp
        States/NotConnectedState.cpp
        States/ViewingMessageListState.cpp
        States/ViewingMessageState.cpp





)

add_library(${PROJECT_NAME} ${APPLICATION_SOURCES})
target_link_libraries(${PROJECT_NAME} Common)
