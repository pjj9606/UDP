/*
* UDP Client3
* ������ ���� �ϳ����� �ް�
* �߽��� ���� ������ �����ؼ� ����
*/

#include <cstdio>
#include <iostream>
#include <winsock2.h> // ���� ��� ���� 
#include "NETWORK_INFO.h"
#include "../NetworkFramework/NetworkFramework.h"
#include <queue>


#pragma comment (lib,"ws2_32.lib") // ���� ���̺귯�� ��ũ

using namespace std;

void Network()
{
    NetworkFramework NF;
    NF.LoadWSA();

    SOCKET serverSocket = NF.NewSocket();                                       // ������ ���� ����(me)
    SOCKADDR_IN serverInfo = NF.NewSocketAddrIn(SERVER_IP, Server_port::TCC);    // ���� �ּ����� ����ü
    
    SOCKADDR_IN fromClient;     // Ŭ���̾�Ʈ���� �޴� �ּ������� recv�� ������ �� ����ü, ��� ���⼭�� �� �ʿ� ���µ�
    memset(&fromClient, 0, sizeof(fromClient));
        

    // bind - ���� ���� Ŭ���̾�Ʈ�� ���� welcome ����    
    NF.BindSocket(serverSocket, serverInfo);

    // ���
    // Ŭ���� ����Լ��� ������� �����Ű�� �����ε� ������ ��...    
    thread registRecvThread = NF.GetRegistRecvSockThread(serverSocket, fromClient);    
    Sleep(100);
    printf("��ϿϷ�\n");

    /****************FOR TEST******************/
    thread sendMSGThread = NF.GetSendMsgQueueThread();
    Sleep(100);

    thread recvMSGThread = NF.GetRecvMsgQueueThread();    // ���� ���� ���ϸ��� ������ ������ ����, �Ű������� ���� ������ ��
    Sleep(100);

    // ���� �޽����� ���� ������ ť�� ������ sendMessageQueue Thread ���� ó����
    /*SendMSG sendMsg = { serverSocket, fromClient };
    memcpy(sendMsg.buffer, sendBuffer, sizeof(sendBuffer));

    sendMessageQueue.push(sendMsg);*/

    registRecvThread.join();
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
