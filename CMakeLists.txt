cmake_minimum_required(VERSION 3.27)
project(dcat)

set(CMAKE_CXX_STANDARD 17)

add_definitions("-Werror=return-type -Werror=unused-result")

add_executable(${PROJECT_NAME} main.cpp
        DirectoryReader.cpp DirectoryReader.h
        AbstractDBWriter.h
        DBWriter.cpp DBWriter.h
        DBReader.cpp DBReader.h
)

target_link_libraries(${PROJECT_NAME} tkrzw)