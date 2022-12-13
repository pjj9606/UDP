#include <queue>
#include <winsock2.h> // ���� ��� ���� 
#include <windows.h>
#include <iostream>
#include <thread>
#include <future>
#include <iostream>
#pragma comment (lib,"ws2_32.lib") // ���� ���̺귯�� ��ũ
#define BUFFER_SIZE 1024 // ���� ������

using namespace std;


typedef struct RecvMSG {        
    char buffer[BUFFER_SIZE];    
} RecvMSG;

typedef struct SendMSG {    
    SOCKET socket;
    SOCKADDR_IN sockAddr; // ���� ���� ��   
    char buffer[BUFFER_SIZE];
} SendMSG;

class NetworkFramework
{
private:
    queue<SendMSG> sendMsgQueue;                                  // �۽� �޽��� ť         
    queue<RecvMSG> recvMsgQueue;                                  // ���� �޽��� ť         
    bool connect[3] = { false, false, false };                    // TCC, ATS, MSS
public:        
    bool IsConnected(int simulator);
    void SetConnect(int simulator);
    SOCKET NewSocket();
    SOCKADDR_IN NewSocketAddrIn(const char* server_ip, int server_port);
    bool LoadWSA();
    void RegistRecvSocket(SOCKET recvSocket, SOCKADDR_IN recvSocketInfo);
    void RegistSendSocket(SOCKET sendSocket, SOCKADDR_IN sendSocketInfo);    
    // Ŭ���� ��������� ������� �����Ű�� ����
    // https://stackoverflow.com/questions/10673585/start-thread-with-member-function
    thread GetRegistRecvSockThread(SOCKET recvSocket, SOCKADDR_IN recvSockInfo);       
    thread GetRecvMsgQueueThread();
    thread GetSendMsgQueueThread();    
    void RunRecvMsgQueue();
    void RunSendMsgQueue();
    bool SendMsg(SOCKET sendSocket, SOCKADDR_IN sendSockInfo, char* msg);    // ����, ���� ������, �޽���
    void EncodeMsg(char* msg);
    void DecodeMsg(char* msg);   // ������ ���̳ʸ� ������ �޽����� ���ڵ�    
    template <typename T>
    void AddDataToBuffer(char** buffer, T Data, int* pivot);
    bool BindSocket(SOCKET socket, SOCKADDR_IN sockaddr_in);                  // Server ������ ó���� Bind ����� ��

};

