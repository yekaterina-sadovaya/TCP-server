#include <iostream>
#include <WS2tcpip.h>

using namespace std;
#include "TcpServer.h"

#pragma comment(lib, "ws2_32.lib")      // this is to link the WS2_32 library

int main() {

    std::shared_ptr<TcpServer> tcp_server(new TcpServer);

    int tcp_socket_port_number = 54000;

    // Create a TCP socket and assign the specified port
    tcp_server->createSocket(tcp_socket_port_number);
    // Switch on the listening mode
    tcp_server->socketListen();
    
    // Close the listening socket after we got the client
    tcp_server->closeListSocket();

    // Send echo back to the client
    tcp_server->sendEcho();

    // Close the socket 
    tcp_server->closeClientSocket();

    // Clean windows socket
    WSACleanup();

    
}