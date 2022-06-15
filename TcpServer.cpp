#include <WS2tcpip.h>
#include <fstream>

#include "TcpServer.h"

#pragma comment(lib, "ws2_32.lib")      // this is to link the WS2_32 library

TcpServer::TcpServer()
{
}

TcpServer::~TcpServer()
{
}


void TcpServer::createSocket(const int& port_number)
{

    // include WS2tcpip to access network soccets for Windows
    // Windows socket initialization
    WSADATA data;   // This structure fills when the socket starts
    WORD version = MAKEWORD(2, 2);  // Request the version
    int socket_ok = WSAStartup(version, &data);
    if (socket_ok != 0) {
        std::cout << std::endl << "The windows socket cannot be initialized with the version requested" << std::endl;
        exit(0);
    }
    else {
        std::cout << std::endl << "The socket was initialized" << std::endl;
    }

    // Create a socket
    listening_socket_ = socket(AF_INET, SOCK_STREAM, 0);     // SOCK_STREAM is used to open a TCP socket
    check_socket(listening_socket_);

    // Assign an IP address and port to the socket
    sockaddr_in soc_address;
    soc_address.sin_family = AF_INET;
    soc_address.sin_port = htons(port_number);   // host to network short
    soc_address.sin_addr.S_un.S_addr = INADDR_ANY;

    // Then, the listening socket is binded to the address
    bind(listening_socket_, (sockaddr*)&soc_address, sizeof(soc_address));

}

void TcpServer::socketListen()
{
    // The socket is switched to the listening mode
    listen(listening_socket_, SOMAXCONN);
    // Assign the address and the size of the client
    int client_size = sizeof(client_address_);
    client_socket_ = accept(listening_socket_, (sockaddr*)&client_address_, &client_size);
    check_socket(client_socket_);
}

void TcpServer::check_socket(SOCKET& socket)
{
    if (socket == INVALID_SOCKET) {
        std::cout << std::endl << "The TCP socket is invalid due to the following error: " << WSAGetLastError() << std::endl;
        exit(0);
    }
    else if (socket == EAGAIN) {
        std::cout << std::endl << "Cannot create a socket, try again" << std::endl;
        exit(0);
    }
    else if (socket == EINTR) {
        std::cout << std::endl << "The call was interrupted" << std::endl;
        exit(0);
    }
}

void TcpServer::closeListSocket()
{
    closesocket(listening_socket_);
}

void TcpServer::closeClientSocket()
{
    closesocket(client_socket_);
}


void TcpServer::sendEcho()
{
    char buf[4096];
    while (true) {
        ZeroMemory(buf, 4096);
        // Wait for the client to send the data
        int if_bytes_received = recv(client_socket_, buf, 4096, 0);
        if (if_bytes_received == SOCKET_ERROR) {
            std::cout << std::endl << "Error in the received data" << std::endl;
            break;
        }
        else if (if_bytes_received == 0) {
            std::cout << std::endl << "The client is disconnected" << std::endl;
            break;
        }

        // Echo message back to the client
        send(client_socket_, buf, if_bytes_received + 1, 0);      // this tells the number of bytes to send
    }
}


void TcpServer::printPortAddress()
{
    // Then we want to print some information and for this we want to get
    // some data from objects using inet
    int client_size = sizeof(client_address_);
    char host[NI_MAXHOST];      // remote name of the client
    char service[NI_MAXHOST];   // the service port to which the client is connected to

    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service, NI_MAXHOST);

    // Convert the network address into the string of characters
    if (getnameinfo((sockaddr*)&client_address_, client_size, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
        std::cout << std::endl << "The host is conected to the PORT number: " << service << std::endl;
    }
    else {
        inet_ntop(AF_INET, &client_address_.sin_addr, host, NI_MAXHOST);
        std::cout << std::endl << "The host is conected to the PORT number: " << ntohs(client_address_.sin_port) << std::endl;
    }
}

/* This method sends a file requested by a cliend and returns
the file size if succeded. If not, returns an error code:
-1 if cannot open the file for input
-2 if cannot send the length of the file
-3 if cannot send the file
*/
int TcpServer::sendFile(const std::string& fileName) {

    int chunkSize = 64 * 1024;
    const int fileSize = getFileSize(fileName);
    if (fileSize < 0) { 
        std::cout << "Cannot open this file for input";
        return -1; }

    std::ifstream file(fileName, std::ifstream::binary);
    if (file.fail()) { 
        std::cout << "Cannot open this file for input";
        return -1; }

    if (sendBuffer(client_socket_, reinterpret_cast<const char*>(&fileSize),
        sizeof(fileSize)) != sizeof(fileSize)) {
        std::cout << "Cannot send the length of the file";
        return -2;
    }

    char* buffer = new char[chunkSize];
    bool errored = false;
    int i = fileSize;
    while (i != 0) {
        const int ssize = __min(i, (int)chunkSize);
        if (!file.read(buffer, ssize)) { errored = true; break; }
        const int l = sendBuffer(client_socket_, buffer, (int)ssize);
        if (l < 0) { errored = true; break; }
        i -= l;
    }
    delete[] buffer;

    file.close();

    if (errored == true) {
        std::cout << "Cannot send the file";
        return -3;
    }
    else {
        return fileSize;
    }
}

/*
    Determines the size of the file
*/
int TcpServer::getFileSize(const std::string& fileName) {

    FILE* f;
    if (fopen_s(&f, fileName.c_str(), "rb") != 0) {
        return -1;
    }
    _fseeki64(f, 0, SEEK_END);
    const int len = _ftelli64(f);
    fclose(f);
    return len;
}

/*
    This method sends data in buffer until bufferSize value is achieved
*/
int TcpServer::sendBuffer(SOCKET s, const char* buffer, int bufferSize) {

    int chunkSize = 4 * 1024;
    int i = 0;
    while (i < bufferSize) {
        const int result = send(s, &buffer[i], __min(chunkSize, bufferSize - i), 0);
        if (result < 0) { 
            // if result < 0, this means an error occured
            return result; 
        } 
        i += result;
    }
    return i;
}