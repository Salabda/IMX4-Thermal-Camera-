#include <iostream>
#include <vector>
#include <thread>
#include <cstring>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

class SocketServer {
public:
    const int PORT = 8888;
    std::vector<SOCKET> clientSockets;
    void broadcast(const std::string message) {
        // printf("Helloooooooo\n");
        for (SOCKET clientSocket : clientSockets) {
            //printf("send message: %s\n", message);
            try {
                int bytes_sent = send(clientSocket, message.c_str(), message.size(), 0);
                if (bytes_sent == SOCKET_ERROR) {
                    // Get the specific error code
                    int error_code = WSAGetLastError();

                    // Handle the error based on the code
                    switch (error_code) {
                    case WSAECONNRESET:
                        // Connection was reset by the peer
                        std::cerr << "Connection to client reset while sending message: " << message << std::endl;
                        break;
                    case WSAENOTSOCK:
                        // Invalid socket handle
                        std::cerr << "Invalid socket handle used for sending message: " << message << std::endl;
                        break;
                    default:
                        // Generic error
                        std::cerr << "Failed to send message: " << message << " with error code: " << error_code << std::endl;
                    }
                }
                else {
                    // Message successfully sent
                    std::cout << "Sent message to client: " << message << std::endl;
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Failed to send message: " << e.what() << std::endl;
            }
        }
    }


    int runServer() {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed" << std::endl;
            return -1;
        }

        SOCKET serverSocket;
        struct sockaddr_in address;

        if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            std::cerr << "Socket creation error" << std::endl;
            return -1;
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(PORT);

        if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
            std::cerr << "Bind failed" << std::endl;
            return -1;
        }

        if (listen(serverSocket, 5) == SOCKET_ERROR) {
            std::cerr << "Listen failed" << std::endl;
            return -1;
        }

        std::cout << "Server is listening on port " << PORT << "..." << std::endl;

         /*std::thread sendThread(sendMessages);
         sendThread.detach();*/

        while (true) {
            SOCKET clientSocket;
            struct sockaddr_in clientAddr;
            int addrlen = sizeof(clientAddr);

            if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrlen)) == INVALID_SOCKET) {
                std::cerr << "Accept failed" << std::endl;
                return -1;
            }

            // Add client socket to the list
            char clientIP[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
            clientSockets.push_back(clientSocket);
            std::cout << "Accepted connection from " << clientIP << std::endl;


           /* std::thread clientthread(handleClient, clientSocket);
            clientthread.detach();*/
        } 

        WSACleanup();
        return 0;
    }
private:
    void handleClient(SOCKET clientSocket) {
        bool errorOccurred = false; // Flag to track errors
        try {
            while (true) {
                char buffer[1024] = { 0 };
                int valread = recv(clientSocket, buffer, 1024, 0);
                if (valread <= 0)
                    break;

                // Handle the received message here
                std::cout << "Received message from client: " << buffer << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Error occurred: " << e.what() << std::endl;
            errorOccurred = true; // Set the error flag
        }

        // Close the client connection in the 'finally' equivalent block
        // using RAII principles (destructors)
        if (!errorOccurred) {
            auto it = std::find(clientSockets.begin(), clientSockets.end(), clientSocket);
            if (it != clientSockets.end()) {
                clientSockets.erase(it);
            }
            closesocket(clientSocket);
        }
        else {
            // Handle error case if needed
            // In this example, no specific error handling is shown
        }
    }
    void sendMessages() {
        while (true) {
            Sleep(5000); // Set to send a message every 5 seconds here
            const char* message = "hello";
            broadcast(message);
            std::cout << "Sending message: " << message << std::endl;
        }
    }

};

//int main2(){
//    SocketServer server;
//    int state = server.runServer();
//    return state;
//}