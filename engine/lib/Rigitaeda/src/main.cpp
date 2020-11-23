#ifdef _BIN
#include <string>
#include <glog/logging.h>
#include "Rigi_Server.hpp"

int main()
{
	google::InitGoogleLogging("DUMP");                // 구글 로그 초기화
 	google::SetLogDestination( google::GLOG_INFO, "./DUMP." );      // 출력 로그 레벨 설정, 출력 위치 Test. <= 파일 저장시 접두사로 사용됨
	//google::EnableLogCleaner(3);

	LOG(INFO) << "Found " << 1 << " cookies";

	Rigitaeda::Rigi_Server server;
	server.Run(3333, 100);

	return 1;
}
#endif
