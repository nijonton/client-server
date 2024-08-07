#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

int main() {
	WSADATA wsaData;
	int wsaerr;
	WORD wVersionRequested = MAKEWORD(2, 2);
	wsaerr = WSAStartup(wVersionRequested, &wsaData);
	if (wsaerr != 0) {
		cout << "The WinSock dll is not found!" << endl;
		return 0;
	}
	else {
		cout << "The WinSock dll found!" << endl;
		cout << "The status: " << wsaData.szSystemStatus << endl;
	}
	SOCKET clientSocket, recvSocket;
	int port = 55555;
		
	clientSocket = INVALID_SOCKET;
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
	else {
		cout << "socket() is OK!" << endl;
	}

	sockaddr_in clientService;
	clientService.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &clientService.sin_addr.s_addr);
	clientService.sin_port = htons(port);
	if (connect(clientSocket, (SOCKADDR*)&clientService, sizeof(clientService)) == SOCKET_ERROR) {
		cout << "Client: connect() - failed to connect." << endl;
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}
	else {
		cout << "CLient: connect() is OK!" << endl;
		cout << "Client: can start sending and receiving data..." << endl;
	}
	// receiving an invitation from the server

	char receiveBuffer[200];
	memset(receiveBuffer, 0, sizeof(receiveBuffer));
	recvSocket = recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0);
	if (recvSocket != INVALID_SOCKET) {
		cout << "Message received as an invitation: " << receiveBuffer << endl;
	}
	else {
		cout << "Having an error: " << WSAGetLastError << endl;
		closesocket(clientSocket);
		WSACleanup();
		return 0;
	}

	char buffer[200];
	char recbuffer[200];
	while (true) {
		cout << "Enter your message to send to the Server: ";
		cin.getline(buffer, 200);
		int byteCount = send(clientSocket, buffer, sizeof(buffer), 0);
		if (byteCount == SOCKET_ERROR) {
			cout << "Server send error: " << WSAGetLastError() << endl;
			closesocket(clientSocket);
			WSACleanup();
			return -1;
		}
		else if (strcmp("exit", buffer) == 0) {
			closesocket(clientSocket);
			WSACleanup();
			cout << "The connection is closed..." << endl;
			break;
		}
		else {
			cout << "Message sent: " << buffer << endl;
		}
		byteCount = recv(clientSocket, recbuffer, sizeof(recbuffer), 0);
		
		if (byteCount == SOCKET_ERROR) {
			cout << "Server receive error: " << WSAGetLastError() << endl;
			closesocket(clientSocket);
			WSACleanup();
			return -1;
		}
		else {
			cout << "Message received: " << recbuffer << endl;
		}
	}
	
	//system("pause");
	return 0;
}