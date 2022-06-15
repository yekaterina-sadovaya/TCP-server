#include <iostream>
using namespace std; 

class TcpServer {
public:
    TcpServer();
    ~TcpServer();

    void createSocket(const int& port_number);
    void socketListen();
    void printPortAddress();
    void closeListSocket();
    void closeClientSocket();
    void sendEcho();
    int sendFile(const std::string& fileName);

private:
    SOCKET listening_socket_;
    SOCKET client_socket_;
    sockaddr_in client_address_;
    void check_socket(SOCKET& socket);
    int getFileSize(const std::string& fileName);
    int sendBuffer(SOCKET s, const char* buffer, int bufferSize);
};