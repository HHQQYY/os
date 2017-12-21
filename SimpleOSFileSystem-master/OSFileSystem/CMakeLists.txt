cmake_minimum_required(VERSION 3.5)
project(OS)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES  main.cpp help.cpp cd.cpp startsys.cpp mkdir.cpp rmdir.cpp open.cpp close.cpp .idea/open_path.cpp open_path.cpp FileSubstr.cpp creat.cpp rm.cpp filewrite.cpp dowrite.cpp fileread.cpp doread.cpp exitsys.cpp cat.cpp)
add_executable(OS ${SOURCE_FILES})