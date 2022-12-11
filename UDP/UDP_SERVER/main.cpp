/*
* UDP Client3
* 수신은 소켓 하나에서 받고
* 발신은 보낼 소켓을 선택해서 보냄
*/

#include <cstdio>
#include <iostream>
#include <winsock2.h> // 윈속 헤더 포함 
#include "NETWORK_INFO.h"
#include "../NetworkFramework/NetworkFramework.h"
#include <queue>


#pragma comment (lib,"ws2_32.lib") // 윈속 라이브러리 링크

using namespace std;

void Network()
{
    NetworkFramework NF;
    NF.LoadWSA();

    SOCKET serverSocket = NF.NewSocket();                                       // 서버측 소켓 생성(me)
    SOCKADDR_IN serverInfo = NF.NewSocketAddrIn(SERVER_IP, Server_port::TCC);    // 서버 주소정보 구조체
    
    SOCKADDR_IN fromClient;     // 클라이언트에서 받는 주소정보를 recv떄 저장해 둘 구조체, 사실 여기서는 별 필요 없는듯
    memset(&fromClient, 0, sizeof(fromClient));
        

    // bind - 새로 오는 클라이언트를 받을 welcome 소켓    
    NF.BindSocket(serverSocket, serverInfo);

    // 등록
    // 클래스 멤버함수를 스레드로 실행시키기 위함인데 모양새가 영...    
    thread registRecvThread = NF.GetRegistRecvSockThread(serverSocket, fromClient);    
    Sleep(100);
    printf("등록완료\n");

    /****************FOR TEST******************/
    thread sendMSGThread = NF.GetSendMsgQueueThread();
    Sleep(100);

    thread recvMSGThread = NF.GetRecvMsgQueueThread();    // 수신 받을 소켓마다 스레드 돌려서 실행, 매개변수로 소켓 넣으면 됨
    Sleep(100);

    // 보낼 메시지가 생길 때마다 큐에 넣으면 sendMessageQueue Thread 에서 처리함
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
