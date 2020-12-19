cmake_minimum_required(VERSION 3.12)
project(myDB)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(myDB
        accessControl.cpp
        accessControl.h
        accessTest.cpp
        DB.cpp
        DB.h
        fileUtil.cpp
        fileUtil.h
        interpreter.cpp
        interpreter.h
        interpreterTest.cpp
        ui.cpp
        tableService.h
        tool.cpp
        tool.h
        view.cpp
        view.h
        viewOperation.cpp
        viewOperation.h tableService.cpp)
