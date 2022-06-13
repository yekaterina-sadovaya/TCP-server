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

private:
    SOCKET listening_socket_;
    SOCKET client_socket_;
    sockaddr_in client_address_;
    void check_socket(SOCKET& socket);
};