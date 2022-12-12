#include "NetworkFramework.h"

// �ݵ�� ������� ������Ѿ� ��
void NetworkFramework::RunRecvMsgQueue()
{
	printf("RecvMsgThread ����\n");
	while (1) {
		while (!recvMsgQueue.empty()) {
			printf("Received: %s\n", recvMsgQueue.front().buffer);
			RecvMSG receiveMsg = recvMsgQueue.front();			
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
	if (::bind(listenSocket, (struct sockaddr*)&serverAddr_in, sizeof(serverAddr_in)) == SOCKET_ERROR) {  //���ε� ���Ͽ� �������� �ο�    
		cout << "���ε带 �� �� �����ϴ�." << endl;
		closesocket(listenSocket);
		WSACleanup();
		exit(0);
	}
	return true;
}
