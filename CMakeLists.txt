cmake_minimum_required(VERSION 3.27)
project(dcat)

set(CMAKE_CXX_STANDARD 17)

add_executable(dcat main.cpp
        DirectoryReader.cpp DirectoryReader.h
        AbstractDBWriter.h
        DBWriter.cpp DBWriter.h
        DBReader.cpp DBReader.h
)
