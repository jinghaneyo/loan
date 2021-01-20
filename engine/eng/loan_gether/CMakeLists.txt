set(APP_NAME loan_gether)
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
set(SENDTO_INCLUDE ../../lib/Send_to/include)
set(SENDTO_SRC ../../lib/Send_to/src)

INCLUDE_DIRECTORIES(/usr/include/boost/include)
LINK_DIRECTORIES(/usr/include/boost/lib)
INCLUDE_DIRECTORIES(/usr/include/glog/include)
LINK_DIRECTORIES(/usr/include/glog/lib)
INCLUDE_DIRECTORIES(/usr/include/gflags)
LINK_DIRECTORIES(/usr/lib64/)
INCLUDE_DIRECTORIES(/usr/local/include)
LINK_DIRECTORIES(/usr/local/lib64)

INCLUDE_DIRECTORIES(src)
INCLUDE_DIRECTORIES(${RIGITAEDA_INCLUDE})
INCLUDE_DIRECTORIES(${RIGITAEDA_SRC})
INCLUDE_DIRECTORIES(src/client)
INCLUDE_DIRECTORIES(src/data)
INCLUDE_DIRECTORIES(src/protobuf)
INCLUDE_DIRECTORIES(${SENDTO_INCLUDE})

ADD_EXECUTABLE(${APP_NAME}
	src/main.cpp
	src/Conf_Yaml.cpp
	src/data/Data_Policy.hpp
	src/data/MsgLog_Q.hpp
	src/client/TCP_Client.hpp
	src/client/TCP_ClientMgr.cpp
	src/protobuf/loan.pb.cc
	${RIGITAEDA_INCLUDE}/Rigi_ClientTCP.hpp
	${RIGITAEDA_SRC}/Rigi_SessionPool.cpp
	${RIGITAEDA_SRC}/Rigi_TCPSession.cpp
	${SENDTO_INCLUDE}/Session_Pool.hpp
	${SENDTO_SRC}/Session_RoundRobin.cpp
	${SENDTO_SRC}/Session_FailOver.cpp
	${SENDTO_SRC}/Session_FailBack.cpp
	)

TARGET_LINK_LIBRARIES( ${APP_NAME} gflags boost_system pthread boost_system protobuf glog yaml-cpp)
