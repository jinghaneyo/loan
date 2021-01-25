#include "loan_util.hpp"
#include <fstream>
#include <iostream>

std::string Replace_All(	__in const std::string &_strSource,
							__in const char *_pstrOld,
							__in const char *_pstrNew )
{
	std::string strResult = _strSource;
	std::string::size_type pos = 0;

	int nOldLen = strlen(_pstrOld);

	while ((pos = strResult.find(_pstrOld)) != std::string::npos)
	{
		strResult.replace(pos, nOldLen, _pstrNew);
	}

	return strResult;
}

std::string Get_LocalHostIP()
{
	boost::asio::io_service io_service;
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(boost::asio::ip::host_name(), "");
	boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve( query );
	boost::asio::ip::tcp::endpoint ep = *iter;

	std::string strLocalIP = ep.address().to_string();

	return strLocalIP;
}

std::string GetCurrentTime( __in const char *_pszFormat )
{
	if(nullptr == _pszFormat || 0 == strlen(_pszFormat) )
		return std::string("");

	std::string strResult;
	struct tm tstruct; // 기본 사용가능 
	char szTime[1024] = {0,}; 

	time_t t = time(NULL); 
	if (nullptr != localtime_r(&t, &tstruct)) 
	{ 
		strftime(szTime, sizeof(szTime), _pszFormat, &tstruct); 
		strResult = szTime;
	} 

	return strResult;
}

std::string Replace_Macro( __in std::string &_strSource )
{
	std::string strFilePath = Replace_All(_strSource, "[LOCAL_IP]", Get_LocalHostIP().c_str() );
	std::string::size_type pos_ip = _strSource.find("[LOCAL_IP]");
	while( std::string::npos != pos_ip )
	{
		strFilePath = Replace_All(strFilePath, "[LOCAL_IP]", Get_LocalHostIP().c_str() );

		pos_ip = strFilePath.find("[LOCAL_IP]");
	}

	std::string strTimeFormat;
	std::string::size_type pos_start = strFilePath.find("[CURRENT_TIME:");
	while( std::string::npos != pos_start )
	{
		std::string::size_type pos_end = strFilePath.find("]", pos_start );
		if( std::string::npos != pos_end )
		{
			strTimeFormat = "";
			strTimeFormat += strFilePath.substr(pos_start, pos_end);
			strTimeFormat += "]";

			strFilePath = Replace_All(strFilePath, strTimeFormat.c_str(), GetCurrentTime().c_str() );
		}

		pos_start = strFilePath.find("[CURRENT_TIME:");
	}

	return strFilePath;
}

bool OpenFile( 	__in const char *_pszFilePath, 
				__in const char *_pszLocale, 
				__in std::ofstream &_ofstream )
{
	try
	{
		if(false == _ofstream.is_open())
		{
			//_ofstream.imbue("ko_KR.UTF-8");
			//_ofstream.imbue( _pszLocale );
			_ofstream.open( _pszFilePath, std::ios::out | std::ios::ate );
			if(true == _ofstream.is_open())
				std::cout << "[OpenFile] Open file ret = " << "SUCC | Path = " << _pszFilePath << "\n";
			else
				std::cout << "[OpenFile] Open file ret = " << "FAIL | Path = " << _pszFilePath << "\n";
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "[Exception][OpenFile] Err = " << e.what() << "\n";
		return false;
	}
	catch(...)
	{
		std::cerr << "[Exception][OpenFile] Err = Unknown" << "\n";
		return false;
	}

	return _ofstream.is_open();
}