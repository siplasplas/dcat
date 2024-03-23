cmake_minimum_required(VERSION 3.27)
project(dcat)

set(CMAKE_CXX_STANDARD 17)

add_definitions("-Werror=return-type -Werror=unused-result")

add_executable(${PROJECT_NAME} main.cpp
        DirectoryReader.cpp DirectoryReader.h
        AbstractDBWriter.h
        DBWriter.cpp DBWriter.h
        DBReader.cpp DBReader.h
        DirEntry.cpp DirEntry.h
        endian_serial.cpp endian_serial.h
)

target_link_libraries(${PROJECT_NAME} tkrzw)

add_executable(gtest_dcat gtest_serialization.cpp
        DirEntry.cpp DirEntry.h
        endian_serial.cpp endian_serial.h)

target_link_libraries(gtest_dcat gtest gtest_main)
