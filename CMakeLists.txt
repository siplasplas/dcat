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
        UTF.hpp
        VLQ.cpp VLQ.h
)

target_link_libraries(${PROJECT_NAME} tkrzw)

add_executable(gtest_dcat
        gtest_serial_big.cpp
        gtest_serial_vlq.cpp
        gtest_filestamp.cpp
        DirEntry.cpp DirEntry.h
        endian_serial.cpp endian_serial.h
        VLQ.cpp VLQ.h
        )

target_link_libraries(gtest_dcat gtest gtest_main)
