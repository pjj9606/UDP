/*
UDP_Client
/// TODO:
    /// 인코딩, 디코딩
    /// 메세지 큐
* 메시지 발신
- 소켓 정보를 큐에 넣어 한 스레드에서 발신 주소를 지정하고 UDP 발신

* 다수의 소켓에서 RECV :
-  소켓별로 수신 스레드를 생성하여 UDP 수신

*/
#include "NETWORK_INFO.h"
#include <iostream>
#include <cstdio>
#include <winsock2.h> // 윈속 헤더 포함 
#include <windows.h>
#include <thread>
#include <queue>s
#include <cstring>

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
    while (1) {
        while (!recvMessageQueue.empty()) {
            printf("Received\n");
            RecvMSG receiveMsg = recvMessageQueue.front();
            recvMessageQueue.pop();
            // 디코딩
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

void RegistClient(SOCKET socket, SOCKADDR_IN fromServer)
{
    printf("RecvMsgThread 시작\n");
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
        printf("Received\n");
        /// 수신 되면 수신 메시지 큐에 넣기
        RecvMSG recvMSG;
        memcpy(recvMSG.buffer, buffer, sizeof(buffer));
        recvMessageQueue.push(recvMSG);
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

void Network()
{
    LoadWSA();

    SOCKADDR_IN fromServer;   // 서버에서 받는 주소정보 구조체    
    SOCKADDR_IN toServer;   // 서버로 보내는 주소정보 구조체정보
    SOCKET clientSocket;
    int fromServerSize, recvSize;
    char sendBuffer[BUFFER_SIZE];

    memset(sendBuffer, 0, sizeof(sendBuffer));  memset(&toServer, 0, sizeof(toServer)); memset(&fromServer, 0, sizeof(fromServer));

    toServer.sin_family = AF_INET;
    toServer.sin_addr.s_addr = inet_addr(SERVER_IP);
    toServer.sin_port = htons(SERVER_PORT); // 포트번호

    clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (clientSocket == INVALID_SOCKET) {
        cout << "소켓을 생성할수 없습니다." << endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(0);
    }

    /* _____________________For Test________________________*/
    // 보낼 메시지가 생길 때마다 큐에 넣으면 sendMessageQueue Thread 에서 처리함
    SendMSG sendMsg = { clientSocket,toServer };
    memcpy(sendMsg.buffer, sendBuffer, sizeof(sendBuffer));

    sendMessageQueue.push(sendMsg);

    // 발신에서는 SockAddrIn이 필요하지 왜냐 1:다수의 소켓으로 보내니까 어디로 보낼지 지정을 해줘야 함    
    thread sendMSGThread(SendMsgThread);
    Sleep(100);
    // 등록
    RegistClient(clientSocket, fromServer);
    thread recvMSGThread(RecvMsgThread);    // 수신 받을 소켓마다 스레드 돌려서 실행, 매개변수로 소켓 넣으면 됨
    Sleep(100);
    recvMSGThread.join();
    sendMSGThread.join();

    closesocket(clientSocket);
    WSACleanup();
    /*________________________________________________________*/
}

// 송신할 메시지를 바이너리 형태로 만들어서 버퍼에 넣기
void EncodeMSG()
{

}
// 수신한 바이너리 형태의 메시지를 디코딩
void DecodeMSG()
{

}



int main()
{
    thread networkThread(Network);

    networkThread.join();

    return 0;
}
