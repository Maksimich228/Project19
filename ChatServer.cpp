#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

vector<SOCKET> clients;

void broadcast(const string& message, SOCKET sender) {
    for (SOCKET client : clients) {
        if (client != sender) {
            send(client, message.c_str(), message.length(), 0);
        }
    }
}

void handleClient(SOCKET client) {
    char buffer[1024];
    int result;
    while ((result = recv(client, buffer, 1024, 0)) > 0) {
        buffer[result] = '\0';
        broadcast(buffer, client);
    }
    closesocket(client);
    clients.erase(remove(clients.begin(), clients.end(), client), clients.end());
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = { AF_INET, htons(12345), INADDR_ANY };
    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Bind failed" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        cerr << "Listen failed" << endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    cout << "Server is working and waiting for connections" << endl;

    while (true) {
        SOCKET clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == INVALID_SOCKET) {
            cerr << "Accept failed" << endl;
            closesocket(serverSocket);
            WSACleanup();
            return 1;
        }

        clients.push_back(clientSocket);
        thread(handleClient, clientSocket).detach();
    }

    closesocket(serverSocket);
    WSACleanup();
    return 0;
}