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
#pragma comment (lib,"ws2_32.lib") // 윈속 라이브러리 링크
using namespace std;

NetworkFramework NF;


void Network()
{        
    NF.LoadWSA();

    SOCKET sendSocket = NF.NewSocket();                                       // 서버측 소켓 생성(me)
    SOCKET recvSocket = NF.NewSocket();
    SOCKADDR_IN tccInfo;
    SOCKADDR_IN atsInfo;
    memset(&tccInfo, 0, sizeof(tccInfo));
    memset(&atsInfo, 0, sizeof(atsInfo));
    tccInfo = NF.NewSocketAddrIn(SERVER_IP, Server_port::TCC);    // 서버 주소정보 구조체  
    atsInfo = NF.NewSocketAddrIn(SERVER_IP, Server_port::ATS);     // 클라이언트에서 받는 주소정보를 recv떄 저장해 둘 구조체, 사실 여기서는 별 필요 없는듯
//    memset(&clientInfo, 0, sizeof(clientInfo));
        

    // bind - 새로 오는 클라이언트를 받을 welcome 소켓    
    NF.BindSocket(recvSocket, tccInfo);
    // 등록
    // 클래스 멤버함수를 스레드로 실행시키기 위함인데 모양새가 영...    
    thread registRecvThread = NF.GetRegistRecvSockThread(recvSocket, tccInfo);        
    Sleep(100);

    /****************FOR TEST******************/
    thread sendMSGThread = NF.GetSendMsgQueueThread();
    Sleep(100);

    thread recvMSGThread = NF.GetRecvMsgQueueThread();    
    Sleep(100);

    // 보낼 메시지가 생    길 때마다 큐에 넣으면 sendMessageQueue Thread 에서 처리함    

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

    // 현재는 10초 지나면 그냥 연결 되도록
    // TODO: 모의기들끼리 처음에 연결 확인 메시지를 주고받으면서 setConnect 처리 후 메시지 전송 되도록
    Sleep(10000);
    NF.SetConnect(simulator::ats);

    networkThread.join();

    return 0;
}
