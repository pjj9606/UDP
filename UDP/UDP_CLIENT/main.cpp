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
#include "../UDP_SERVER/NETWORK_INFO.h"
#include <iostream>
#include <cstdio>
#include <winsock2.h> // 윈속 헤더 포함 
#include <windows.h>
#include <thread>
#include <queue>s
#include <cstring>
#include "../NetworkFramework/NetworkFramework.h"
#pragma comment (lib,"ws2_32.lib") // 윈속 라이브러리 링크
using namespace std;

NetworkFramework NF;

void Network()
{
        
    NF.LoadWSA();

    SOCKET sendSocket = NF.NewSocket();   // 내 소켓(클라이언트)
    SOCKET recvSocket = NF.NewSocket();   // 내 소켓(클라이언트)
    SOCKADDR_IN tccInfo;
    SOCKADDR_IN atsInfo;
    memset(&tccInfo, 0, sizeof(tccInfo));
    memset(&atsInfo, 0, sizeof(atsInfo));
    tccInfo = NF.NewSocketAddrIn(SERVER_IP, Server_port::TCC);   // 서버로 보내는 주소정보 구조체정보
    atsInfo = NF.NewSocketAddrIn(SERVER_IP, Server_port::ATS);   // 내 수신 addr
    //memset(&fromServer, 0, sizeof(fromServer));

    /* _____________________For Test________________________*/
    // 보낼 메시지가 생길 때마다 큐에 넣으면 sendMessageQueue Thread 에서 처리함
    char sendBuffer[BUFFER_SIZE] = "hi I'm client";
    NF.SendMsg(sendSocket, tccInfo, sendBuffer);     // 담을 소켓, 도달 위치, 메시ㅈ    
    NF.SendMsg(sendSocket, tccInfo, sendBuffer);     // 담을 소켓, 도달 위치, 메시ㅈ    
    NF.SendMsg(sendSocket, tccInfo, sendBuffer);     // 담을 소켓, 도달 위치, 메시ㅈ    
    
    // 등록    
    NF.BindSocket(recvSocket, atsInfo);
    thread registClientThread = NF.GetRegistRecvSockThread(recvSocket, atsInfo);    
       
    // 발신에서는 SockAddrIn이 필요하지 왜냐 1:다수의 소켓으로 보내니까 어디로 보낼지 지정을 해줘야 함    
    thread sendMSGThread = NF.GetSendMsgQueueThread();
    Sleep(100);
    thread recvMSGThread = NF.GetRecvMsgQueueThread();    // 수신 받을 소켓마다 스레드 돌려서 실행, 매개변수로 소켓 넣으면 됨
    Sleep(100);

    registClientThread.join();
    recvMSGThread.join();
    sendMSGThread.join();

    closesocket(sendSocket);
    closesocket(recvSocket);
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
