#include "NetworkFramework.h"

// 반드시 스레드로 실행시켜야 함
void NetworkFramework::RunRecvMsgQueue()
{
	printf("RecvMsgThread 시작\n");
	while (1) {
		while (!recvMsgQueue.empty()) {
			printf("Received: %s\n", recvMsgQueue.front().buffer);
			RecvMSG receiveMsg = recvMsgQueue.front();			
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
	SendMSG sendMsg = { sendSocket, sendSockInfo };
	memcpy(sendMsg.buffer, msg, sizeof(sendMsg.buffer));

	sendMsgQueue.push(sendMsg);

	return false;
}

void NetworkFramework::EncodeMsg()
{
}

void NetworkFramework::DecodeMsg()
{
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
