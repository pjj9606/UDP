/*
* UDP Client3
* ������ ���� �ϳ����� �ް�
* �߽��� ���� ������ �����ؼ� ����
*/

#include <cstdio>
#include <iostream>
#include <winsock2.h> // ���� ��� ���� 
#include "NETWORK_INFO.h"
#include <thread>
#include <queue>

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
    printf("RecvMsgThread ����\n");
    while (1) {
        while (!recvMessageQueue.empty()) {
            printf("Received\n");
            RecvMSG receiveMsg = recvMessageQueue.front();
            recvMessageQueue.pop();
            // TODO: ���ڵ� �� �� �޽��� ó��
            ///fortest            
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

bool BindSocket(SOCKET socket, SOCKADDR_IN sockaddr_in)
{
    if (bind(socket, (struct sockaddr*)&sockaddr_in, sizeof(sockaddr_in)) == SOCKET_ERROR) {  //���ε� ���Ͽ� �������� �ο�    
        cout << "���ε带 �� �� �����ϴ�." << endl;
        closesocket(socket);
        WSACleanup();
        exit(0);
    }
}

void RegistClient(SOCKET socket, SOCKADDR_IN fromServer)
{
    printf("RegistClient ����\n");
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
        // ���� �Ǹ� ���� �޽��� ť�� Ǫ��
        RecvMSG recvMSG;
        memcpy(recvMSG.buffer, buffer, sizeof(buffer));
        recvMessageQueue.push(recvMSG);
    }
}

void Network()
{
    LoadWSA();

    SOCKET serverSocket;        // ���� ����
    SOCKADDR_IN serverInfo;     // ���� �ּ����� ����ü
    SOCKADDR_IN fromClient;     // Ŭ���̾�Ʈ���� �޴� �ּ����� ����ü
    int fromClientSize;        // Ŭ���̾�Ʈ�κ��� �޴� �޽��� ũ��
    int recvSize, sendSize;
    char sendBuffer[BUFFER_SIZE];

    memset(&serverInfo, 0, sizeof(serverInfo)); memset(&fromClient, 0, sizeof(fromClient)); memset(sendBuffer, 0, BUFFER_SIZE);

    serverInfo.sin_family = AF_INET;                     // IPv4 �ּ�ü�� ��� 
    serverInfo.sin_addr.s_addr = inet_addr(SERVER_IP);   // ������ IP
    serverInfo.sin_port = htons(SERVER_PORT);            // ��Ʈ��ȣ

    // ���� ����
    serverSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP); // udp�� ���� ����. SOCK_DGRAM : UDP ���

    if (serverSocket == INVALID_SOCKET)
    {
        cout << "������ �����Ҽ� �����ϴ�." << endl;
        closesocket(serverSocket);
        WSACleanup();
        exit(0);
    }

    // bind - ���� ���� Ŭ���̾�Ʈ�� ���� welcome ����    
    BindSocket(serverSocket, serverInfo);
    // ���    
    thread registClientThread(RegistClient, serverSocket, fromClient);
    Sleep(100);
    printf("��ϿϷ�\n");

    /****************FOR TEST******************/
    thread sendMSGThread(SendMsgThread);
    Sleep(100);

    thread recvMSGThread(RecvMsgThread);    // ���� ���� ���ϸ��� ������ ������ ����, �Ű������� ���� ������ ��
    Sleep(100);

    // ���� �޽����� ���� ������ ť�� ������ sendMessageQueue Thread ���� ó����
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
