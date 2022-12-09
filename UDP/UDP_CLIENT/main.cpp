/*
UDP_Client
/// TODO:
    /// ���ڵ�, ���ڵ�
    /// �޼��� ť
* �޽��� �߽�
- ���� ������ ť�� �־� �� �����忡�� �߽� �ּҸ� �����ϰ� UDP �߽�

* �ټ��� ���Ͽ��� RECV :
-  ���Ϻ��� ���� �����带 �����Ͽ� UDP ����

*/
#include "NETWORK_INFO.h"
#include <iostream>
#include <cstdio>
#include <winsock2.h> // ���� ��� ���� 
#include <windows.h>
#include <thread>
#include <queue>s
#include <cstring>

#pragma comment (lib,"ws2_32.lib") // ���� ���̺귯�� ��ũ
#define BUFFER_SIZE 1024 // ���� ������

using namespace std;

typedef struct RecvMSG {
    char buffer[1024];
} RecvMSG;

typedef struct SendMSG {
    SOCKET socket;
    SOCKADDR_IN sockAddr; // ���� ���� ��   
    char buffer[1024];
} SendMSG;

queue<SendMSG> sendMessageQueue;        // �۽� �޽��� ť
queue<RecvMSG> recvMessageQueue;        // ���� �޽��� ť

// ���� ť ������
void RecvMsgThread()
{
    while (1) {
        while (!recvMessageQueue.empty()) {
            printf("Received\n");
            RecvMSG receiveMsg = recvMessageQueue.front();
            recvMessageQueue.pop();
            // ���ڵ�
        }
    }
}

// �߽� ť ������
void SendMsgThread()
{
    printf("SendMsgThread ����\n");
    while (1) {
        while (!sendMessageQueue.empty()) {
            SendMSG sendMsg = sendMessageQueue.front();
            sendMessageQueue.pop();
            // ����
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
    printf("RecvMsgThread ����\n");
    char buffer[BUFFER_SIZE];
    memset(&buffer, 0, sizeof(buffer));
    int fromServerSize = sizeof(fromServer);

    // ���� ��� & ���
    while (1) {
        int recvSize = recvfrom(socket, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&fromServer, &fromServerSize);

        // ���� ��Ŷ ����
        if (recvSize < 0) {
            cout << "recvfrom() error!" << endl;
            exit(0);
        }
        printf("Received\n");
        /// ���� �Ǹ� ���� �޽��� ť�� �ֱ�
        RecvMSG recvMSG;
        memcpy(recvMSG.buffer, buffer, sizeof(buffer));
        recvMessageQueue.push(recvMSG);
    }
}

bool LoadWSA()
{
    WSADATA wsaData;        // ���� ������ ����ü
    if (WSAStartup(0x202, &wsaData) == SOCKET_ERROR) {
        cout << "WinSock �ʱ�ȭ�κп��� ���� �߻� " << endl;
        WSACleanup();
        exit(0);
    }
    return true;
}

void Network()
{
    LoadWSA();

    SOCKADDR_IN fromServer;   // �������� �޴� �ּ����� ����ü    
    SOCKADDR_IN toServer;   // ������ ������ �ּ����� ����ü����
    SOCKET clientSocket;
    int fromServerSize, recvSize;
    char sendBuffer[BUFFER_SIZE];

    memset(sendBuffer, 0, sizeof(sendBuffer));  memset(&toServer, 0, sizeof(toServer)); memset(&fromServer, 0, sizeof(fromServer));

    toServer.sin_family = AF_INET;
    toServer.sin_addr.s_addr = inet_addr(SERVER_IP);
    toServer.sin_port = htons(SERVER_PORT); // ��Ʈ��ȣ

    clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (clientSocket == INVALID_SOCKET) {
        cout << "������ �����Ҽ� �����ϴ�." << endl;
        closesocket(clientSocket);
        WSACleanup();
        exit(0);
    }

    /* _____________________For Test________________________*/
    // ���� �޽����� ���� ������ ť�� ������ sendMessageQueue Thread ���� ó����
    SendMSG sendMsg = { clientSocket,toServer };
    memcpy(sendMsg.buffer, sendBuffer, sizeof(sendBuffer));

    sendMessageQueue.push(sendMsg);

    // �߽ſ����� SockAddrIn�� �ʿ����� �ֳ� 1:�ټ��� �������� �����ϱ� ���� ������ ������ ����� ��    
    thread sendMSGThread(SendMsgThread);
    Sleep(100);
    // ���
    RegistClient(clientSocket, fromServer);
    thread recvMSGThread(RecvMsgThread);    // ���� ���� ���ϸ��� ������ ������ ����, �Ű������� ���� ������ ��
    Sleep(100);
    recvMSGThread.join();
    sendMSGThread.join();

    closesocket(clientSocket);
    WSACleanup();
    /*________________________________________________________*/
}

// �۽��� �޽����� ���̳ʸ� ���·� ���� ���ۿ� �ֱ�
void EncodeMSG()
{

}
// ������ ���̳ʸ� ������ �޽����� ���ڵ�
void DecodeMSG()
{

}



int main()
{
    thread networkThread(Network);

    networkThread.join();

    return 0;
}
