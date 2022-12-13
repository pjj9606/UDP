#include <queue>
#include <winsock2.h> // 윈속 헤더 포함 
#include <windows.h>
#include <iostream>
#include <thread>
#include <future>
#include <iostream>
#pragma comment (lib,"ws2_32.lib") // 윈속 라이브러리 링크
#define BUFFER_SIZE 1024 // 버퍼 사이즈

using namespace std;


typedef struct RecvMSG {        
    char buffer[BUFFER_SIZE];    
} RecvMSG;

typedef struct SendMSG {    
    SOCKET socket;
    SOCKADDR_IN sockAddr; // 어디로 보낼 지   
    char buffer[BUFFER_SIZE];
} SendMSG;

class NetworkFramework
{
private:
    queue<SendMSG> sendMsgQueue;                                  // 송신 메시지 큐         
    queue<RecvMSG> recvMsgQueue;                                  // 수신 메시지 큐         
    bool connect[3] = { false, false, false };                    // TCC, ATS, MSS
public:        
    bool IsConnected(int simulator);
    void SetConnect(int simulator);
    SOCKET NewSocket();
    SOCKADDR_IN NewSocketAddrIn(const char* server_ip, int server_port);
    bool LoadWSA();
    void RegistRecvSocket(SOCKET recvSocket, SOCKADDR_IN recvSocketInfo);
    void RegistSendSocket(SOCKET sendSocket, SOCKADDR_IN sendSocketInfo);    
    // 클래스 멤버변수를 스레드로 실행시키기 위함
    // https://stackoverflow.com/questions/10673585/start-thread-with-member-function
    thread GetRegistRecvSockThread(SOCKET recvSocket, SOCKADDR_IN recvSockInfo);       
    thread GetRecvMsgQueueThread();
    thread GetSendMsgQueueThread();    
    void RunRecvMsgQueue();
    void RunSendMsgQueue();
    bool SendMsg(SOCKET sendSocket, SOCKADDR_IN sendSockInfo, char* msg);    // 소켓, 어디로 보낼지, 메시지
    void EncodeMsg(char* msg);
    void DecodeMsg(char* msg);   // 수신한 바이너리 형태의 메시지를 디코딩    
    template <typename T>
    void AddDataToBuffer(char** buffer, T Data, int* pivot);
    bool BindSocket(SOCKET socket, SOCKADDR_IN sockaddr_in);                  // Server 측에서 처음에 Bind 해줘야 함

};

