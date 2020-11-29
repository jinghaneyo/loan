PROJECT(loan_buffer)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-m64")
add_definitions("-Wall -std=c++11")

#find_package(boost REQUIRED system thread asio)
#find_package (glog REQUIRED)
#find_package(gflags REQUIRED)

set(RIGITAEDA ../../lib/Rigitaeda/src)

INCLUDE_DIRECTORIES(/usr/include/boost/include)
LINK_DIRECTORIES(/usr/include/boost/lib)
INCLUDE_DIRECTORIES(/usr/include/glog/include)
LINK_DIRECTORIES(/usr/include/glog/lib)
INCLUDE_DIRECTORIES(/usr/include/gflags)
LINK_DIRECTORIES(/usr/lib64/)
INCLUDE_DIRECTORIES(${RIGITAEDA})

ADD_EXECUTABLE(loan_buffer
	src/main.cpp
	src/loan_Server.hpp
	${RIGITAEDA}/Rigi_Server.cpp
	${RIGITAEDA}/Rigi_TCPSession.cpp
	${RIGITAEDA}/Rigi_TCPMgr.hpp
	)

TARGET_LINK_LIBRARIES( loan_buffer gflags glog pthread boost_system boost_thread )
