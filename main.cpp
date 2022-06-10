#include <iostream>
#include <WS2tcpip.h>

using namespace std;

#pragma comment(lib, "ws2_32.lib")      // this is to link the WS2_32 library

int main() {

    // include WS2tcpip to access network soccets for Windows
    // Windows socket initialization
    WSADATA data;   // This structure fills when the socket starts
    WORD version = MAKEWORD(2, 2);  // Request the version
    int socket_ok = WSAStartup(version, &data);
    if (socket_ok != 0) {
        std::cout << std::endl << "The windows socket cannot be initialized with the version requested" << std::endl;
        return EXIT_FAILURE;
    }

    // Create a socket
    SOCKET listening_socket = socket(AF_INET, SOCK_STREAM, 0);     // SOCK_STREAM is used to open a TCP socket
    if (listening_socket == INVALID_SOCKET) {
        std::cout << std::endl << "The TCP socket is invalid" << std::endl;
        return EXIT_FAILURE;
    }
    else if (listening_socket == EAGAIN) {
        std::cout << std::endl << "Cannot create a socket, try again" << std::endl;
        return EXIT_FAILURE;
    }
    else if (listening_socket == EINTR) {
        std::cout << std::endl << "The call was interrupted" << std::endl;
        return EXIT_FAILURE;
    }

    // Assign an IP address and port to the socket
    sockaddr_in soc_address;
    soc_address.sin_family = AF_INET;
    soc_address.sin_port = htons(54000);   // host to network short
    soc_address.sin_addr.S_un.S_addr = INADDR_ANY;

    // Then, the listening socket is binded to the address
    bind(listening_socket, (sockaddr*)&soc_address, sizeof(soc_address));

    // After the socket is created and has an address, it can be switched to the listening mode
    listen(listening_socket, SOMAXCONN);

    // First we assign the address and the size of the client
    sockaddr_in client_address; 
    int client_size = sizeof(client_address);

    // Then, the socket is switched to the listening mode
    SOCKET client_socket = accept(listening_socket, (sockaddr*)&client_address, &client_size);
    if (client_socket == INVALID_SOCKET) {
        std::cout << std::endl << "The TCP socket is invalid" << std::endl;
        return EXIT_FAILURE;
    }
    else if (client_socket == EAGAIN) {
        std::cout << std::endl << "Cannot create a socket, try again" << std::endl;
        return EXIT_FAILURE;
    }
    else if (client_socket == EINTR) {
        std::cout << std::endl << "The call was interrupted" << std::endl;
        return EXIT_FAILURE;
    }

    // Then we want to print some information and for this we want to get
    // some data from objects using inet
    char host[NI_MAXHOST];      // remote name of the client
    char service[NI_MAXHOST];   // the service port to which the client is connected to

    ZeroMemory(host, NI_MAXHOST);
    ZeroMemory(service, NI_MAXHOST);

    // Convert the network address into the string of characters
    std::cout << "The host address is " << host << std::endl;
    if (getnameinfo((sockaddr*)&client_address, client_size, host, NI_MAXHOST, service, NI_MAXSERV, 0) == 0) {
        std::cout << std::endl << "The host is conected to the PORT number: " << service << std::endl;
    }
    else {
        inet_ntop(AF_INET, &client_address.sin_addr, host, NI_MAXHOST);
        std::cout << std::endl << "The host is conected to the PORT number: " << ntohs(client_address.sin_port) << std::endl;
    }


    // Close the listening socket after we got the client
    closesocket(listening_socket);

    // Send echo back to the client
    char buf[4096];
    while (true) {
        ZeroMemory(buf, 4096);
        // Wait for the client to send the data
        int if_bytes_received = recv(client_socket, buf, 4096, 0);
        if (if_bytes_received == SOCKET_ERROR) {
            std::cout << std::endl << "Error in the received data" << std::endl;
            break;
        }
        else if (if_bytes_received == 0) {
            std::cout << std::endl << "The client is disconnected" << std::endl;
            break;
        }

        // Echo message back to the client
        send(client_socket, buf, if_bytes_received + 1, 0);      // this tells the number of bytes to send
    }

    // Close the socket 
    closesocket(client_socket);

    // Clean windows socket
    WSACleanup();

    
}