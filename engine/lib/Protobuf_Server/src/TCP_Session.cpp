// regex 헤더 안에 이미 __in, __out 정의 되어있다
// 해서 Rigi_Def.hpp 보다 먼저 선언을 해야 한다
#include <regex>
#include "TCP_Session.hpp"
#include "TCP_ServerMgr.hpp"
#include "MsgLog_Q.hpp"
#include "Policy.hpp"

#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/tokenizer.h>
#include <google/protobuf/message.h>

using namespace google::protobuf;
using namespace google::protobuf::io;
using namespace google::protobuf::compiler;

TCP_Session::TCP_Session() 
{ 
	m_pLogQ = nullptr;
	m_Event_RecvFilter = nullptr;
}

TCP_Session::~TCP_Session() 
{ 
}

void TCP_Session::Split_Protobuf( 	__in const char *_pData, 
									__in size_t _nData_len,
									__out std::string &_strTemp )
{
	bool bRet = false;
	if(nullptr == m_Event_RecvFilter)
		bRet = true;

	std::string strSendData = "";
	strSendData += _pData[0];
	//printf("[ 0] %02X => %c\n", _pData[0], _pData[0]);
	for(int i=1; i<(int)_nData_len; i++)
	{
		strSendData += _pData[i];

		//printf("[%2d] %02X => %c\n", i, _pData[i], _pData[i]);

		// 패킷을 다 받았다면 문장의 끝은 \n\n(0x1b 0x1b) 이다 
		if( 0x17 == _pData[i] && 0x17 == _pData[i-1] )
		{
			if(nullptr != m_Event_RecvFilter)
				bRet = m_Event_RecvFilter(strSendData);

			if(true == bRet)
			{
				//std::cout << "[SPLIT 1][" << strSendData << "]" << std::endl;

				std::string *pstrSendData = new std::string();
				pstrSendData->swap(strSendData);
				strSendData = "";

				m_pLogQ->Push_back( pstrSendData );
			}
		}
	}

	if( 1 < (int)strSendData.length() )
	{
		// 패킷을 다 받았다면 문장의 끝은 0x1b 0x1b 이다 
		if( (0x17 == strSendData[ strSendData.length()-1 ] ) &&
			(0x17 == strSendData[ strSendData.length()-2 ] ) )
		{
			if(nullptr != m_Event_RecvFilter)
				bRet = m_Event_RecvFilter(strSendData);

			if(true == bRet)
			{
				//std::cout << "[SPLIT 2][" << strSendData << "]" << std::endl;

				std::string *pstrSendData = new std::string();
				pstrSendData->swap(strSendData);
				strSendData = "";

				m_pLogQ->Push_back( pstrSendData );
			}
		}
		else
		{
			_strTemp.swap( strSendData );
		}
	}
	else
	{
		_strTemp.swap( strSendData );
	}
}

void TCP_Session::OnEvent_Receive(	__in char *_pData,
									__in size_t _nData_len )
{
	if( 1 > _nData_len )
	{
		std::cout << "[RECV] << recv size is 0" << std::endl;
		return;
	}

	if(2 < _nData_len)
	{
		if(true == m_strTempBuff.empty())
		{
			Split_Protobuf(_pData, _nData_len, m_strTempBuff);
		}
		else
		{
			std::string strTemp;
			strTemp.swap(m_strTempBuff);
			strTemp += _pData;
			m_strTempBuff = "";

			Split_Protobuf(strTemp.c_str(), strTemp.length(), m_strTempBuff);
		}
	}
	else
	{
		std::cout << "[NOT COMPLETE DATA !!!]" << std::endl;
	}
}

void TCP_Session::OnEvent_Close()
{
	std::cout << "[TCP_Session::OnClose] >> " << std::endl;
}

bool TCP_Session::OnEvent_Init()
{
	TCP_ServerMgr<TCP_Session> *pMgr = (TCP_ServerMgr<TCP_Session> *)Get_TCPMgr();

	m_pLogQ = pMgr->Get_LogQ();
	m_pPolicy = pMgr->Get_Policy();

	m_strIP_Port = Get_SessionIP();
	m_strIP_Port += ":";
	m_strIP_Port += std::to_string( pMgr->Get_Port() );

	std::cout << "[TCP_Session::OnEvent_Init] IP_PORT = " << m_strIP_Port << std::endl;

	return true;
}

void TCP_Session::Set_LogQ( __in MsgLog_Q *_pLogQ )
{
	m_pLogQ = _pLogQ;
}