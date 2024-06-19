#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

SOCKET clientSocket;

void receiveMessages() {
    char buffer[1024];
    int result;
    while ((result = recv(clientSocket, buffer, 1024, 0)) > 0) {
        buffer[result] = '\0';
        cout << buffer << endl;
    }
    closesocket(clientSocket);
}

int main() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        cerr << "WSAStartup failed" << endl;
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET) {
        cerr << "Error creating socket" << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddr = { AF_INET, htons(12345), INADDR_ANY };
    inet_pton(AF_INET, "192.168.142.1", &serverAddr.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "Connect failed" << endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    string nickname;
    cout << "Choose username: ";
    getline(cin, nickname);

    thread(receiveMessages).detach();

    string message;
    while (getline(cin, message)) {
        message = nickname + ": " + message;
        send(clientSocket, message.c_str(), message.length(), 0);
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}