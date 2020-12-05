#include "Rigi_Server.hpp"
#include "Rigi_Setting.hpp"

using namespace Rigitaeda;
class TCP_Session : public Rigitaeda::Rigi_TCPSession
{
public:
	TCP_Session() 
	{ 
	}
	virtual ~TCP_Session() { }

	void OnEvent_Receive(	__in char *_pData,
							__in size_t _nData_len )
	{

		Task_Filter();

		Task_Output(_pData, _nData_len);

		// 응답 주기
		std::string strPacket = "[{'cmd':'run'},{'data': 'test pakcet'}]";
		Send( strPacket.c_str(), strPacket.length() );
	}

	void OnEvent_Close()
	{
		std::cout << "[TCP_Session::OnClose] >> " << GetIP_Remote() << std::endl;
	}

	void Task_Filter()
	{

	}

	void Task_Output(	__in char *_pData,
						__in size_t _nData_len )
	{
		_OUTPUT_TYPE type = GetType_Output();

		switch((int)type)
		{
			case (int)_OUTPUT_TYPE::LOG:
				WriteLog_Packet( _pData, _nData_len );
			break;
			case (int)_OUTPUT_TYPE::FILE:
			break;
			case (int)_OUTPUT_TYPE::TCP:
			break;
		}
	}

	void WriteLog_Packet( 	__in char *_pData,
							__in size_t _nData_len )
	{
		std::cout << "[TCP_Session::Task_Output] >> " << _pData << std::endl;
		LOG(INFO) << _pData << std::endl;
	}
};