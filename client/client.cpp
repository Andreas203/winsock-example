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

        SOCKET clientSocket = INVALID_SOCKET;
        clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (clientSocket == INVALID_SOCKET) {
            std::cout << "ERROR at socket(): " << WSAGetLastError() << std::endl;
            WSACleanup();
            return 0;
        }
        else {

            // Connect to server socket

            std::cout << "socket() is OK!" << std::endl;
            sockaddr_in service;
            service.sin_family = AF_INET;
            InetPton(AF_INET, TEXT("127.0.0.1"), &service.sin_addr.s_addr);
            service.sin_port = htons(3000);

            if (connect(clientSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
                std::cout << "Client: connect() failed" << std::endl;
                WSACleanup();
                return 0;
            }
            else {

                // Send some data to server

                std::cout << "Connected! can start sending data" << std::endl;
                const char* sendbuf = "this is a test";
                char recvbuf[DEFAULT_BUFLEN];
                int iResult;
                int recvbuflen = DEFAULT_BUFLEN;
                iResult = send(clientSocket, sendbuf, (int)strlen(sendbuf), 0);

                if (iResult == SOCKET_ERROR) {
                    printf("send failed with error: %d\n", WSAGetLastError());
                    closesocket(clientSocket);
                    WSACleanup();
                    return 1;
                }

                printf("Bytes Sent: %ld\n", iResult);

                iResult = shutdown(clientSocket, SD_SEND);
                if (iResult == SOCKET_ERROR) {
                    printf("shutdown failed with error: %d\n", WSAGetLastError());
                    closesocket(clientSocket);
                    WSACleanup();
                    return 1;
                }


                // Receive some data from server

                do {

                    iResult = recv(clientSocket, recvbuf, recvbuflen, 0);
                    if (iResult > 0) {
                        printf("Bytes received: %d\n", iResult);
                        std::cout << "buffer: " << recvbuf << std::endl;
                    }

                    else if (iResult == 0)
                        printf("Connection closed\n");
                    else
                        printf("recv failed with error: %d\n", WSAGetLastError());

                } while (iResult > 0);
            }

            
        }

        // Cleanup

        closesocket(clientSocket);
        WSACleanup();
    }

    return 0;
}
