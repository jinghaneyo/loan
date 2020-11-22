PROJECT(Rigitaeda)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-m64")
add_definitions("-Wall -std=c++14 -D_BIN")

find_package(boost REQUIRED system thread asio)
find_package (glog 0.4.0)
find_package(Gflags REQUIRED)
find_package(Glog REQUIRED)

include_directories(${GLOG_INCLUDE_DIRS} ${GFLAGS_INCLUDE_DIRS})

#INCLUDE_DIRECTORIES(/$ENV{HOME}/Dev/C++/ThirdParty/boost_1_74_0)
#LINK_DIRECTORIES(/$ENV{HOME}/Dev/C++/ThirdParty/boost_1_74_0/stage/gcc/lib)
INCLUDE_DIRECTORIES(/usr/local/Cellar/boost/1.74.0/include)
LINK_DIRECTORIES(/usr/local/Cellar/boost/1.74.0/lib)
# INCLUDE_DIRECTORIES(/usr/local/Cellar/glog/0.4.0/include)
# LINK_DIRECTORIES(/usr/local/Cellar/glog/0.4.0/lib)

ADD_EXECUTABLE(Rigitaeda 
	src/main.cpp
	src/Rigi_Server.cpp
	src/Rigi_Session.cpp)

TARGET_LINK_LIBRARIES( Rigitaeda glog )