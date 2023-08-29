#include <ctime>
#include <iostream>
#include <string>


#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <stdio.h>

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "3000"

int main() {

    // Load winsock
    
    WSADATA wsaData;
    int wsaerr;
    WORD wVersionRequested = MAKEWORD(2, 2);
    wsaerr = WSAStartup(wVersionRequested, &wsaData);
    if (wsaerr != 0) {
        std::cout << "The Winsock dll not found" << std::endl;

    }
    else {
        std::cout << "The Winsock dll found!" << std::endl;
        std::cout << "The status: " << wsaData.szSystemStatus << std::endl;

        // Create socket

        SOCKET serverSocket = INVALID_SOCKET;
        serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (serverSocket == INVALID_SOCKET) {
            std::cout << "ERROR at socket(): " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 0;
        }
        else {

            // Bind socket address and port

            std::cout << "socket() is OK!" << std::endl;
            sockaddr_in service;
            service.sin_family = AF_INET;
            InetPton(AF_INET, TEXT("127.0.0.1"), &service.sin_addr.s_addr);
            service.sin_port = htons(3000);

            if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
                std::cout << "bind() failed: " << WSAGetLastError() << std::endl;
                closesocket(serverSocket);
                WSACleanup();
                return 0;
            }
            else {
                std::cout << "bind() is OK!" << std::endl;
                if (listen(serverSocket, 1) == SOCKET_ERROR) {
                    std::cout << "listen() failed: " << WSAGetLastError() << std::endl;

                }
                else {
                    // New socket to accept connections

                    std::cout << "listen() is OK, waiting for connections" << std::endl;
                    SOCKET acceptSocket =  accept(serverSocket, NULL, NULL);

                    if (acceptSocket == INVALID_SOCKET) {
                        std::cout << "accept failed: " << WSAGetLastError() << std::endl;
                        WSACleanup();
                    }
                    else {
                        char recvbuf[DEFAULT_BUFLEN];
                        int recvbuflen = DEFAULT_BUFLEN;
                        int iResult;
                        int iSendResult;

                        // Receive until the peer shuts down the connection
                        do {

                            iResult = recv(acceptSocket, recvbuf, recvbuflen, 0);
                            if (iResult > 0) {
                                printf("Bytes received: %d\n", iResult);
                                std::cout << "buffer: " << recvbuf << std::endl;

                                // Echo the buffer back to the sender
                                iSendResult = send(acceptSocket, recvbuf, iResult, 0);
                                if (iSendResult == SOCKET_ERROR) {
                                    printf("send failed with error: %d\n", WSAGetLastError());
                                    closesocket(acceptSocket);
                                    WSACleanup();
                                    return 1;
                                }
                                printf("Bytes sent: %d\n", iSendResult);
                            }
                            else if (iResult == 0)
                                printf("Connection closing...\n");
                            else {
                                printf("recv failed with error: %d\n", WSAGetLastError());
                                closesocket(acceptSocket);
                                WSACleanup();
                                return 1;
                            }

                        } while (iResult > 0);
                    }

                }
            }
        }

        // Cleanup

        closesocket(serverSocket);
        WSACleanup();
    }

	return 0;
}
