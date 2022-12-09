/*
* UDP Client3
* 수신은 소켓 하나에서 받고
* 발신은 보낼 소켓을 선택해서 보냄
*/

#include <cstdio>
#include <iostream>
#include <winsock2.h> // 윈속 헤더 포함 
#include "NETWORK_INFO.h"
#include <thread>
#include <queue>

#pragma comment (lib,"ws2_32.lib") // 윈속 라이브러리 링크
#define BUFFER_SIZE 1024 // 버퍼 사이즈

using namespace std;

typedef struct RecvMSG {
    char buffer[1024];
} RecvMSG;

typedef struct SendMSG {
    SOCKET socket;
    SOCKADDR_IN sockAddr; // 어디로 보낼 지   
    char buffer[1024];
} SendMSG;

queue<SendMSG> sendMessageQueue;        // 송신 메시지 큐
queue<RecvMSG> recvMessageQueue;        // 수신 메시지 큐

// 수신 큐 스레드
void RecvMsgThread()
{
    printf("RecvMsgThread 시작\n");
    while (1) {
        while (!recvMessageQueue.empty()) {
            printf("Received\n");
            RecvMSG receiveMsg = recvMessageQueue.front();
            recvMessageQueue.pop();
            // TODO: 디코딩 한 후 메시지 처리
            ///fortest            
        }
    }
}

// 발신 큐 스레드
void SendMsgThread()
{
    printf("SendMsgThread 시작\n");
    while (1) {
        while (!sendMessageQueue.empty()) {
            SendMSG sendMsg = sendMessageQueue.front();
            sendMessageQueue.pop();
            // 전송
            int Send_Size = sendto(sendMsg.socket, sendMsg.buffer, BUFFER_SIZE, 0, (struct sockaddr*)&sendMsg.sockAddr, sizeof(sendMsg.sockAddr));
            printf("Send Packet!\n");
            printf("%s\n", sendMsg.buffer);

            if (Send_Size != BUFFER_SIZE) {
                cout << "sendto() error!" << endl;
                exit(0);
            }
        }
    }
}

bool LoadWSA()
{
    WSADATA wsaData;        // 윈속 데이터 구조체
    if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
        cout << "WinSock 초기화부분에서 문제 발생 " << endl;
        WSACleanup();
        exit(0);
    }
    return true;
}

bool BindSocket(SOCKET socket, SOCKADDR_IN sockaddr_in)
{
    if (bind(socket, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == SOCKET_ERROR) {  //바인드 소켓에 서버정보 부여    
        cout << "바인드를 할 수 없습니다." << endl;
        closesocket(socket);
        WSACleanup();
        exit(0);
    }
}

void RegistClient(SOCKET socket, SOCKADDR_IN fromServer)
{
    printf("RegistClient 시작\n");
    char buffer[BUFFER_SIZE];
    memset(&buffer, 0, sizeof(buffer));
    int fromServerSize = sizeof(fromServer);

    // 수신 등록 & 대기   
    while (1) {
        int recvSize = recvfrom(socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&fromServer, &fromServerSize);
        // 수신 패킷 오류
        if (recvSize < 0) {
            cout << "recvfrom() error!" << endl;
            exit(0);
        }
        // 수신 되면 수신 메시지 큐에 푸시
        RecvMSG recvMSG;
        memcpy(recvMSG.buffer, buffer, sizeof(buffer));
        recvMessageQueue.push(recvMSG);
    }
}

void Network()
{
    LoadWSA();

    SOCKET serverSocket;        // 소켓 선언
    SOCKADDR_IN serverInfo;     // 서버 주소정보 구조체
    SOCKADDR_IN fromClient;     // 클라이언트에서 받는 주소정보 구조체
    int fromClientSize;        // 클라이언트로부터 받는 메시지 크기
    int recvSize, sendSize;
    char sendBuffer[BUFFER_SIZE];

    memset(&serverInfo, 0, sizeof(serverInfo)); memset(&fromClient, 0, sizeof(fromClient)); memset(sendBuffer, 0, BUFFER_SIZE);

    serverInfo.sin_family = AF_INET;                     // IPv4 주소체계 사용 
    serverInfo.sin_addr.s_addr = inet_addr(SERVER_IP);   // 루프백 IP
    serverInfo.sin_port = htons(SERVER_PORT);            // 포트번호

    // 소켓 생성
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // udp용 소켓 생성. SOCK_DGRAM : UDP 사용

    if (serverSocket == INVALID_SOCKET)
    {
        cout << "소켓을 생성할수 없습니다." << endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(0);
    }

    // bind - 새로 오는 클라이언트를 받을 welcome 소켓    
    BindSocket(serverSocket, serverInfo);
    // 등록    
    thread registClientThread(RegistClient, serverSocket, fromClient);
    Sleep(100);
    printf("등록완료\n");

    /****************FOR TEST******************/
    thread sendMSGThread(SendMsgThread);
    Sleep(100);

    thread recvMSGThread(RecvMsgThread);    // 수신 받을 소켓마다 스레드 돌려서 실행, 매개변수로 소켓 넣으면 됨
    Sleep(100);

    // 보낼 메시지가 생길 때마다 큐에 넣으면 sendMessageQueue Thread 에서 처리함
    /*SendMSG sendMsg = { serverSocket, fromClient };
    memcpy(sendMsg.buffer, sendBuffer, sizeof(sendBuffer));

    sendMessageQueue.push(sendMsg);*/

    registClientThread.join();
    recvMSGThread.join();
    sendMSGThread.join();
    /******************************************/

    closesocket(serverSocket);
    WSACleanup();
}


int main()
{
    thread networkThread(Network);

    networkThread.join();

    return 0;
}
