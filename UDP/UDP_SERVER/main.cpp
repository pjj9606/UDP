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
#pragma comment (lib,"ws2_32.lib") // ���� ���̺귯�� ��ũ
using namespace std;

NetworkFramework NF;


void Network()
{        
    NF.LoadWSA();

    SOCKET sendSocket = NF.NewSocket();                                       // ������ ���� ����(me)
    SOCKET recvSocket = NF.NewSocket();
    SOCKADDR_IN tccInfo;
    SOCKADDR_IN atsInfo;
    memset(&tccInfo, 0, sizeof(tccInfo));
    memset(&atsInfo, 0, sizeof(atsInfo));
    tccInfo = NF.NewSocketAddrIn(SERVER_IP, Server_port::TCC);    // ���� �ּ����� ����ü  
    atsInfo = NF.NewSocketAddrIn(SERVER_IP, Server_port::ATS);     // Ŭ���̾�Ʈ���� �޴� �ּ������� recv�� ������ �� ����ü, ��� ���⼭�� �� �ʿ� ���µ�
//    memset(&clientInfo, 0, sizeof(clientInfo));
        

    // bind - ���� ���� Ŭ���̾�Ʈ�� ���� welcome ����    
    NF.BindSocket(recvSocket, tccInfo);
    // ���
    // Ŭ���� ����Լ��� ������� �����Ű�� �����ε� ������ ��...    
    thread registRecvThread = NF.GetRegistRecvSockThread(recvSocket, tccInfo);        
    Sleep(100);

    /****************FOR TEST******************/
    thread sendMSGThread = NF.GetSendMsgQueueThread();
    Sleep(100);

    thread recvMSGThread = NF.GetRecvMsgQueueThread();    
    Sleep(100);

    // ���� �޽����� ��    �� ������ ť�� ������ sendMessageQueue Thread ���� ó����    

    //Sleep(5000) ;    
    //NF.SendMsg(serverSocket, clientInfo, sendBuffer);

    char sendBuffer[BUFFER_SIZE] = "hi I'm server";
    while(!NF.IsConnected(simulator::ats)){
        continue;
    }
    NF.SendMsg(sendSocket, atsInfo, sendBuffer);
    registRecvThread.join();
    recvMSGThread.join();
    sendMSGThread.join();
    /******************************************/

    closesocket(sendSocket);
    closesocket(recvSocket);
    WSACleanup();
}


int main()
{
    thread networkThread(Network);    

    // ����� 10�� ������ �׳� ���� �ǵ���
    // TODO: ���Ǳ�鳢�� ó���� ���� Ȯ�� �޽����� �ְ�����鼭 setConnect ó�� �� �޽��� ���� �ǵ���
    Sleep(10000);
    NF.SetConnect(simulator::ats);

    networkThread.join();

    return 0;
}
