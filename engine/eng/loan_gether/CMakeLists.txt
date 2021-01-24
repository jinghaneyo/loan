set(APP_NAME loan_gether)
PROJECT(${APP_NAME})

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_COMPILER g++)
set(CMAKE_CXX_FLAGS "-m64")
add_definitions("-Wall -std=c++11")

#find_package(boost REQUIRED system thread asio)
#find_package (glog REQUIRED)
#find_package(gflags REQUIRED)

set(COMMON ../common)
set(LIB_COMM ../../lib/Common)
set(RIGITAEDA_INCLUDE ../../lib/Rigitaeda/include)
set(RIGITAEDA_SRC ../../lib/Rigitaeda/src)
set(SENDTO_INCLUDE ../../lib/Send_to/include)
set(SENDTO_SRC ../../lib/Send_to/src)
set(PROTOBUF_SERVER_INCLUDE ../../lib/Protobuf_Server/include)
set(PROTOBUF_SERVER_SRC ../../lib/Protobuf_Server/src)
set(PROTOBUF_SERVER_DATA ../../lib/Protobuf_Server/data)
set(PROTOBUF_CLIENT_INCLUDE ../../lib/Protobuf_Client/include)
set(PROTOBUF_CLIENT_SRC ../../lib/Protobuf_Client/src)

INCLUDE_DIRECTORIES(${COMMON})
INCLUDE_DIRECTORIES(${LIB_COMM})
INCLUDE_DIRECTORIES(${RIGITAEDA_INCLUDE})
INCLUDE_DIRECTORIES(${RIGITAEDA_SRC})
INCLUDE_DIRECTORIES(${SENDTO_INCLUDE})
INCLUDE_DIRECTORIES(${SENDTO_SRC})
INCLUDE_DIRECTORIES(${PROTOBUF_SERVER_INCLUDE})
INCLUDE_DIRECTORIES(${PROTOBUF_SERVER_SRC})
INCLUDE_DIRECTORIES(${PROTOBUF_SERVER_DATA})
INCLUDE_DIRECTORIES(${PROTOBUF_SERVER_DATA}/protbuf)
INCLUDE_DIRECTORIES(${PROTOBUF_CLIENT_INCLUDE})
INCLUDE_DIRECTORIES(${PROTOBUF_CLIENT_SRC})

ADD_EXECUTABLE(${APP_NAME}
	# ${RIGITAEDA_INCLUDE}/Rigi_Header.hpp
	# ${SENDTO_INCLUDE}/Session_Header.hpp
	src/main.cpp
	${COMMON}/Run.cpp
	${COMMON}/Data_Policy.hpp
	${COMMON}/Conf_Yaml.cpp
	${LIB_COMM}/protobuf/loan.pb.cc
	${LIB_COMM}/Policy.hpp
	${LIB_COMM}/MsgLog_Q.hpp
	${LIB_COMM}/loan_util.cpp
	${PROTOBUF_SERVER_INCLUDE}/TCP_ServerMgr.hpp
	${PROTOBUF_SERVER_SRC}/TCP_Session.cpp
	${PROTOBUF_CLIENT_SRC}/TCP_ClientMgr.cpp
	${PROTOBUF_CLIENT_INCLUDE}/TCP_Client.hpp
	${RIGITAEDA_INCLUDE}/Rigi_ClientTCP.hpp
	${RIGITAEDA_INCLUDE}/Rigi_TCPServerMgr.hpp
	${RIGITAEDA_SRC}/Rigi_Server.cpp
	${RIGITAEDA_SRC}/Rigi_SessionPool.cpp
	${RIGITAEDA_SRC}/Rigi_TCPSession.cpp
	${SENDTO_INCLUDE}/Session_Pool.hpp
	${SENDTO_SRC}/Session_RoundRobin.cpp
	${SENDTO_SRC}/Session_FailOver.cpp
	${SENDTO_SRC}/Session_FailBack.cpp
	)

TARGET_LINK_LIBRARIES( ${APP_NAME} gflags boost_system pthread boost_system protobuf glog yaml-cpp)
