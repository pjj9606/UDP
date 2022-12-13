#include "NetworkFramework.h"

// �ݵ�� ������� ������Ѿ� ��
void NetworkFramework::RunRecvMsgQueue()
{
	printf("RecvMsgThread ����\n");
	while (1) {
		while (!recvMsgQueue.empty()) {			
			printf("Received: %s\n", recvMsgQueue.front().buffer);
			RecvMSG receiveMsg = recvMsgQueue.front();	
			// portNum���� ���� Ȯ�� ó��			
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
			// ���ڵ�
		}
	}
}

// �ݵ�� ������� ������Ѿ� ��
void NetworkFramework::RunSendMsgQueue()
{
	printf("SendMsgThread ����\n");
	while (1) {
		while (!sendMsgQueue.empty()) {
			SendMSG sendMsg = sendMsgQueue.front();
			sendMsgQueue.pop();
			// ����
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
	WSADATA wsaData;        // ���� ������ ����ü
	if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
		cout << "WinSock �ʱ�ȭ�κп��� ���� �߻� " << endl;
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
		cout << "������ �����Ҽ� �����ϴ�." << endl;
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
	sockaddr_in.sin_port = htons(server_port); // ��Ʈ��ȣ

	return sockaddr_in;
}

// ���� ���
// �ݵ�� ������� ������Ѿ� ��
void NetworkFramework::RegistRecvSocket(SOCKET recvSocket, SOCKADDR_IN recvSockInfo)
{
	memset(&recvSockInfo, 0, sizeof(recvSockInfo));

	printf("ReigstRecvSocket....\n");
	char buffer[BUFFER_SIZE];
	memset(&buffer, 0, sizeof(buffer));
	int fromServerSize = sizeof(recvSockInfo);
	printf("ReigstRecvSocket �Ϸ�\n");
	
	printf("���� ��� ����\n");
	// ���� ��� & ���
	while (1) {
		int recvSize = recvfrom(recvSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&recvSockInfo, &fromServerSize);			
		// ���� ��Ŷ ����		
		if (recvSize < 0) {
			cout << "recvfrom() error!" << endl;
			exit(0);
		}				
		/// ���� �Ǹ� ���� �޽��� ť�� �ֱ�	
		RecvMSG recvMSG;
		//recvSockInfo.sin_port = 7000;
		//recvMSG.sockAddr = recvSockInfo;		
		//// ���� �պκ� ���� �޽����� � ���� ���´��� ó���ϱ�?
		//unsigned int commandMsg;
		//char decodeBuffer[BUFFER_SIZE];
		//int msgPivot = 0;
		//memset(decodeBuffer, 0, sizeof(decodeBuffer));
		//// ���� �޽���
		//for (int i = 0; i < 4; ++i) {							// ����Ʈ ��ŭ ���ۿ� �װ�
		//	decodeBuffer[i] = buffer[i];
		//	msgPivot++;	// msg ������ �о��� pivot�� ���
		//}
		//memcpy(&commandMsg, decodeBuffer, sizeof(commandMsg));	// �޸� ����
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

// ���� ���� ����� �ʿ���� -> �� �Լ� �ʿ����
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

// ���ۿ� �� ��ƾ� �ϴµ� �Z���� ����
// char* msg�� 
void NetworkFramework::EncodeMsg(char* msg)
{


}

void NetworkFramework::DecodeMsg(char* msg)
{
	unsigned int commandMsg;

	char decodeBuffer[BUFFER_SIZE];
	int msgPivot = 0;
	memset(decodeBuffer, 0, sizeof(decodeBuffer));
	// ���� �޽���
	for (int i = 0; i < 4; ++i) {							// ����Ʈ ��ŭ ���ۿ� �װ�
		decodeBuffer[i] = msg[i];
		msgPivot++;	// msg ������ �о��� pivot�� ���
	}
	memcpy(&commandMsg, decodeBuffer, sizeof(commandMsg));	// �޸� ����
	memset(decodeBuffer, 0, sizeof(decodeBuffer));			// ���� ����

	
			/// enumó�� �ؾ���
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
	if (commandMsg == 31) {	/// TODO:�̰� ���������� �ٲ�� �ϴµ�
		
	}
	else if (commandMsg == 32) {
		int nowPivot = msgPivot;
		for (int i = 0; i < 36; ++i) {
			decodeBuffer[i] = msg[nowPivot + i];
			msgPivot++;			
		}
		/// TODO: ��ǥ ����ü�� �ٷ� memcpy������ ��
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// ���� ����
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
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// ���� ����
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
		/// TODO: �ش��ϴ� ������ memcpy
		// MisslePosition
		int nowPivot = msgPivot;		
		for (int i = 0; i < 4; ++i) {
			decodeBuffer[i] = msg[nowPivot + i];
			msgPivot++;			
		}
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// ���� ����

		nowPivot = msgPivot;
		for (int i = 0; i < 16; ++i) {
			decodeBuffer[i] = msg[nowPivot + i];
			msgPivot++;
		}
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// ���� ����
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
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// ���� ����

		nowPivot = msgPivot;
		for (int i = 0; i < 16; ++i) {
			decodeBuffer[i] = msg[nowPivot + i];
			msgPivot++;
		}
		memset(decodeBuffer, 0, sizeof(decodeBuffer));			// ���� ����
	}	
}

bool NetworkFramework::BindSocket(SOCKET listenSocket, SOCKADDR_IN serverAddr_in)
{
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr_in, sizeof(serverAddr_in)) == SOCKET_ERROR) {  //���ε� ���Ͽ� �������� �ο�    
		cout << "���ε带 �� �� �����ϴ�." << endl;
		closesocket(listenSocket);
		WSACleanup();
		exit(0);
	}
	return true;
}

