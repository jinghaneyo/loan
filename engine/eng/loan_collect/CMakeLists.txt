set(APP_NAME loan_collect)
PROJECT(${APP_NAME})

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-m64")
add_definitions("-Wall -std=c++11")

#find_package(boost REQUIRED system thread asio)
#find_package (glog REQUIRED)
#find_package(gflags REQUIRED)

set(RIGITAEDA_INCLUDE ../../lib/Rigitaeda/include)
set(RIGITAEDA_SRC ../../lib/Rigitaeda/src)

INCLUDE_DIRECTORIES(${RIGITAEDA_INCLUDE})
INCLUDE_DIRECTORIES(${RIGITAEDA_SRC})
INCLUDE_DIRECTORIES(/usr/include/boost/include)
LINK_DIRECTORIES(/usr/include/boost/lib)
INCLUDE_DIRECTORIES(/usr/include/glog/include)
LINK_DIRECTORIES(/usr/include/glog/lib)
INCLUDE_DIRECTORIES(/usr/include/gflags)
LINK_DIRECTORIES(/usr/lib64/)

ADD_EXECUTABLE(${APP_NAME}
	src/main.cpp
	src/loan_Server.hpp
	${RIGITAEDA_SRC}/Rigi_Server.cpp
	${RIGITAEDA_SRC}/Rigi_SessionPool.cpp
	${RIGITAEDA_SRC}/Rigi_TCPSession.cpp
	${RIGITAEDA_INCLUDE}/Rigi_Setting.hpp
	${RIGITAEDA_INCLUDE}/Rigi_TCPMgr.hpp
	)

#TARGET_LINK_LIBRARIES( ${APP_NAME} gflags glog pthread boost_system boost_thread )
TARGET_LINK_LIBRARIES( ${APP_NAME} gflags pthread boost_system )
