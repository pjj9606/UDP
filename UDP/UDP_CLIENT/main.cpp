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
#include "../UDP_SERVER/NETWORK_INFO.h"
#include <iostream>
#include <cstdio>
#include <winsock2.h> // ���� ��� ���� 
#include <windows.h>
#include <thread>
#include <queue>s
#include <cstring>
#include "../NetworkFramework/NetworkFramework.h"
#pragma comment (lib,"ws2_32.lib") // ���� ���̺귯�� ��ũ
using namespace std;

NetworkFramework NF;

void Network()
{
        
    NF.LoadWSA();

    SOCKET sendSocket = NF.NewSocket();   // �� ����(Ŭ���̾�Ʈ)
    SOCKET recvSocket = NF.NewSocket();   // �� ����(Ŭ���̾�Ʈ)
    SOCKADDR_IN tccInfo;
    SOCKADDR_IN atsInfo;
    memset(&tccInfo, 0, sizeof(tccInfo));
    memset(&atsInfo, 0, sizeof(atsInfo));
    tccInfo = NF.NewSocketAddrIn(SERVER_IP, Server_port::TCC);   // ������ ������ �ּ����� ����ü����
    atsInfo = NF.NewSocketAddrIn(SERVER_IP, Server_port::ATS);   // �� ���� addr
    //memset(&fromServer, 0, sizeof(fromServer));

    /* _____________________For Test________________________*/
    // ���� �޽����� ���� ������ ť�� ������ sendMessageQueue Thread ���� ó����
    char sendBuffer[BUFFER_SIZE] = "hi I'm client";
    NF.SendMsg(sendSocket, tccInfo, sendBuffer);     // ���� ����, ���� ��ġ, �޽ä�    
    NF.SendMsg(sendSocket, tccInfo, sendBuffer);     // ���� ����, ���� ��ġ, �޽ä�    
    NF.SendMsg(sendSocket, tccInfo, sendBuffer);     // ���� ����, ���� ��ġ, �޽ä�    
    
    // ���    
    NF.BindSocket(recvSocket, atsInfo);
    thread registClientThread = NF.GetRegistRecvSockThread(recvSocket, atsInfo);    
       
    // �߽ſ����� SockAddrIn�� �ʿ����� �ֳ� 1:�ټ��� �������� �����ϱ� ���� ������ ������ ����� ��    
    thread sendMSGThread = NF.GetSendMsgQueueThread();
    Sleep(100);
    thread recvMSGThread = NF.GetRecvMsgQueueThread();    // ���� ���� ���ϸ��� ������ ������ ����, �Ű������� ���� ������ ��
    Sleep(100);

    registClientThread.join();
    recvMSGThread.join();
    sendMSGThread.join();

    closesocket(sendSocket);
    closesocket(recvSocket);
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
