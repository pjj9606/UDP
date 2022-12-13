#include "NetworkFramework.h"

// 반드시 스레드로 실행시켜야 함
void NetworkFramework::RunRecvMsgQueue()
{
	printf("RecvMsgThread 시작\n");
	while (1) {
		while (!recvMsgQueue.empty()) {			
			printf("Received: %s\n", recvMsgQueue.front().buffer);
			RecvMSG receiveMsg = recvMsgQueue.front();	
			// portNum보고 연결 확인 처리			
			//printf("%x\n", ntohs(receiveMsg.sockAddr.sin_port));
			//printf("%d\n", ntohs(receiveMsg.sockAddr.sin_port));
			//printf("%x\n", htons(receiveMsg.sockAddr.sin_port));
			//printf("%d\n", htons(receiveMsg.sockAddr.sin_port));
			//
			//if (receiveMsg.sockAddr.sin_port >= 7000 && receiveMsg.sockAddr.sin_port <= 7002) {
			//	printf("Wrong PortNum\n");
			//	break;
			//}
			//if (!connect[receiveMsg.sockAddr.sin_port - 7000]) {				
			//	connect[receiveMsg.sockAddr.sin_port] = true;
			//	if (receiveMsg.sockAddr.sin_port == 7000)
			//		printf("TCC Connected!\n");
			//	else if (receiveMsg.sockAddr.sin_port == 7001)
			//		printf("ATS Connected!\n");
			//	if (receiveMsg.sockAddr.sin_port == 7002)
			//		printf("MSS Connected!\n");
			//}
				
			recvMsgQueue.pop();
			// 디코딩
		}
	}
}

// 반드시 스레드로 실행시켜야 함
void NetworkFramework::RunSendMsgQueue()
{
	printf("SendMsgThread 시작\n");
	while (1) {
		while (!sendMsgQueue.empty()) {
			SendMSG sendMsg = sendMsgQueue.front();
			sendMsgQueue.pop();
			// 전송
			int Send_Size = sendto(sendMsg.socket, sendMsg.buffer, BUFFER_SIZE, 0, (struct sockaddr*)&sendMsg.sockAddr, sizeof(sendMsg.sockAddr));
			printf("Send Packet!: %s\n", sendMsg.buffer);			

			if (Send_Size != BUFFER_SIZE) {
				cout << "sendto() error!" << endl;
				exit(0);
			}
		}
	}
}

bool NetworkFramework::LoadWSA()
{
	WSADATA wsaData;        // 윈속 데이터 구조체
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
		cout << "WinSock 초기화부분에서 문제 발생 " << endl;
		WSACleanup();
		exit(0);
	}
	return true;
}

bool NetworkFramework::IsConnected(int simulator)
{	
	return connect[simulator];
}


void NetworkFramework::SetConnect(int simulator)
{
	connect[simulator] = true;
}

SOCKET NetworkFramework::NewSocket()
{
	SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (s == INVALID_SOCKET) {
		cout << "소켓을 생성할수 없습니다." << endl;
		closesocket(s);
		WSACleanup();
		exit(0);
	}
	return s;
}

SOCKADDR_IN NetworkFramework::NewSocketAddrIn(const char* server_ip, int server_port)
{
	SOCKADDR_IN sockaddr_in;
	memset(&sockaddr_in, 0, sizeof(sockaddr_in));
	sockaddr_in.sin_family = AF_INET;
	sockaddr_in.sin_addr.s_addr = inet_addr(server_ip);
	sockaddr_in.sin_port = htons(server_port); // 포트번호

	return sockaddr_in;
}

// 수신 등록
// 반드시 스레드로 실행시켜야 함
void NetworkFramework::RegistRecvSocket(SOCKET recvSocket, SOCKADDR_IN recvSockInfo)
{
	memset(&recvSockInfo, 0, sizeof(recvSockInfo));

	printf("ReigstRecvSocket....\n");
	char buffer[BUFFER_SIZE];
	memset(&buffer, 0, sizeof(buffer));
	int fromServerSize = sizeof(recvSockInfo);
	printf("ReigstRecvSocket 완료\n");
	
	printf("수신 대기 시작\n");
	// 수신 등록 & 대기
	while (1) {
		int recvSize = recvfrom(recvSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&recvSockInfo, &fromServerSize);			
		// 수신 패킷 오류		
		if (recvSize < 0) {
			cout << "recvfrom() error!" << endl;
			exit(0);
		}				
		/// 수신 되면 수신 메시지 큐에 넣기	
		RecvMSG recvMSG;
		//recvSockInfo.sin_port = 7000;
		//recvMSG.sockAddr = recvSockInfo;		
		//// 버퍼 앞부분 지시 메시지를 까서 누가 보냈는지 처리하기?
		//unsigned int commandMsg;
		//char decodeBuffer[BUFFER_SIZE];
		//int msgPivot = 0;
		//memset(decodeBuffer, 0, sizeof(decodeBuffer));
		//// 지시 메시지
		//for (int i = 0; i < 4; ++i) {							// 바이트 만큼 버퍼에 쌓고
		//	decodeBuffer[i] = buffer[i];
		//	msgPivot++;	// msg 어디까지 읽었나 pivot에 기록
		//}
		//memcpy(&commandMsg, decodeBuffer, sizeof(commandMsg));	// 메모리 복사
		//printf("CommandMsg:%d\n", commandMsg);


		memcpy(recvMSG.buffer, buffer, sizeof(buffer));
		recvMsgQueue.push(recvMSG);
	}
}

thread NetworkFramework::GetRegistRecvSockThread(SOCKET recvSocket, SOCKADDR_IN recvSockInfo) {
	return thread([=] {RegistRecvSocket(recvSocket, recvSockInfo);});	
}

thread NetworkFramework::GetRecvMsgQueueThread()
{
	return thread([=] {RunRecvMsgQueue();});
}

thread NetworkFramework::GetSendMsgQueueThread()
{
	return thread([=] {RunSendMsgQueue();});
}

// 보낼 때는 등록이 필요없네 -> 이 함수 필요없음
void NetworkFramework::RegistSendSocket(SOCKET sendSocket, SOCKADDR_IN sendSockInfo)
{
}


bool NetworkFramework::SendMsg(SOCKET sendSocket, SOCKADDR_IN sendSockInfo, char* msg)
{
	SendMSG sendMsg = {sendSocket, sendSockInfo };
	memcpy(sendMsg.buffer, msg, sizeof(sendMsg.buffer));

	sendMsgQueue.push(sendMsg);

	return false;
}

// 버퍼에 다 담아야 하는데 얶떢계 하지
// char* msg에 
void NetworkFramework::EncodeMsg(char* msg)
{


}

void NetworkFramework::DecodeMsg(char* msg)
{
	unsigned int commandMsg;

	char decodeBuffer[BUFFER_SIZE];
	int msgPivot = 0;
	memset(decodeBuffer, 0, sizeof(decodeBuffer));
	// 지시 메시지
	for (int i = 0; i < 4; ++i) {							// 바이트 만큼 버퍼에 쌓고
		decodeBuffer[i] = msg[i];
		msgPivot++;	// msg 어디까지 읽었나 pivot에 기록
	}
	memcpy(&commandMsg, decodeBuffer, sizeof(commandMsg));	// 메모리 복사
	memset(decodeBuffer, 0, sizeof(decodeBuffer));			// 버퍼 비우기

	
			/// enum처리 해야함
	if (commandMsg == 31 || commandMsg == 32 || commandMsg == 33 || commandMsg == 34 || commandMsg == 35) {
		connect[0] = true;
	}
	else if (commandMsg == 61 || commandMsg == 62 || commandMsg == 63 || commandMsg == 64 || commandMsg == 65) {
		connect[0] = true;
	}
	else if (commandMsg == 41 || commandMsg == 42 || commandMsg == 43 || commandMsg == 44 || commandMsg == 45) {
		connect[1] = true;
	}
	else if (commandMsg == 51 || commandMsg == 52 || commandMsg == 53 || commandMsg == 54 || commandMsg == 55) {
		connect[2] = true;
	}


	// II_001
	if (commandMsg == 31) {	/// TODO:이거 열거형으로 바꿔야 하는데
		
	}
	else if (commandMsg == 32) {
		int nowPivot = msgPivot;
		for (int i = 0; i < 36; ++i) {
			decodeBuffer[i] = msg[nowPivot + i];
			msgPivot++;			
		}
		/// TODO: 좌표 구조체에 바로 memcpy때리면 됨
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// 버퍼 비우기
	}
	//_____________ II_001 _____________
	// TCC -> ATS
	else if (commandMsg == 33) {
		// StartScenario()
	}
	else if (commandMsg == 34) {
		// InterCeptComplete()
	}
	else if (commandMsg == 35) {
		// InitScenario()
	}
	//_____________ II_002 _____________
	//TCC -> MSS
	else if (commandMsg == 61) {		
		// NOSTATE
	}	
	else if (commandMsg == 62) {
		// ShootFire	
		int nowPivot = msgPivot;
		for (int i = 0; i < 20; ++i) {
			decodeBuffer[i] = msg[nowPivot + i];
			msgPivot++;
		}
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// 버퍼 비우기
	}	
	else if (commandMsg == 63) {
		// IntereptComplete
	}	
	else if (commandMsg == 64) {
		// InitScenario
	}
	//_____________ II_003 _____________
	// ATS -> TCC
	else if(commandMsg ==51){
		// NOSTATE
	}
	else if (commandMsg == 52) {
		/// TODO: 해당하는 변수에 memcpy
		// MisslePosition
		int nowPivot = msgPivot;		
		for (int i = 0; i < 4; ++i) {
			decodeBuffer[i] = msg[nowPivot + i];
			msgPivot++;			
		}
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// 버퍼 비우기

		nowPivot = msgPivot;
		for (int i = 0; i < 16; ++i) {
			decodeBuffer[i] = msg[nowPivot + i];
			msgPivot++;
		}
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// 버퍼 비우기
	}

	//_____________ II_004 _____________
	// MSS -> TCC
	else if (commandMsg == 41) {
		// NOSTATE
	}
	else if (commandMsg == 42) {
		// MisslePosition
		int nowPivot = msgPivot;
		for (int i = 0; i < 4; ++i) {
			decodeBuffer[i] = msg[nowPivot + i];
			msgPivot++;
		}
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// 버퍼 비우기

		nowPivot = msgPivot;
		for (int i = 0; i < 16; ++i) {
			decodeBuffer[i] = msg[nowPivot + i];
			msgPivot++;
		}
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// 버퍼 비우기
	}	
}

bool NetworkFramework::BindSocket(SOCKET listenSocket, SOCKADDR_IN serverAddr_in)
{
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr_in, sizeof(serverAddr_in)) == SOCKET_ERROR) {  //바인드 소켓에 서버정보 부여    
		cout << "바인드를 할 수 없습니다." << endl;
		closesocket(listenSocket);
		WSACleanup();
		exit(0);
	}
	return true;
}

