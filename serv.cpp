#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>
#include <sstream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#pragma comment (lib, "Ws2_32.lib")

using namespace std;

atomic<int> connectionCount(0);
mutex mtx;

void handleClient(SOCKET clientSocket) {
	connectionCount++;
	while (true) {
		char receiveBuffer[200];
		memset(receiveBuffer, 0, sizeof(receiveBuffer));
		int byteCount = recv(clientSocket, receiveBuffer, sizeof(receiveBuffer), 0);
		
		if (byteCount > 0) {
			lock_guard<mutex> lock(mtx);
			cout << "Message received: " << receiveBuffer << endl;
		}
		else {
			break;
		}

		if (strcmp(receiveBuffer, "exit") == 0) {
			cout << "Closing this connection!" << endl;
			break;
		}
		char sendBuffer[200];
		memset(sendBuffer, 0, sizeof(sendBuffer));
		int space_cnt = 0;
		char* p;
		int to_send;
		int arr[2];
		const char* delim = " ";
		p = strtok(receiveBuffer, delim);
		bool checker = false;
		while (p /*&& space_cnt <= 1*/) {
			stringstream ss2;
			ss2.str("");
			ss2.str(p);
			ss2 >> to_send;
			if (!to_send) {
				strcpy_s(sendBuffer, "Enter your numbers again!");
				break;
			}
			else if (space_cnt < 2){
				arr[space_cnt] = to_send;
				space_cnt++;
				p = strtok(NULL, " ");
			}
			else {
				checker = true;
				break;
			}
		}
		int result1;
		if (checker || space_cnt!=2) {
			strcpy_s(sendBuffer, "Enter your numbers again!..");
		} else {
			if (connectionCount.load() % 2) {
				result1 = arr[0] * arr[1];
				strcpy_s(sendBuffer, to_string(result1).c_str());
			}
			else {
				result1 = arr[0] + arr[1];
				strcpy_s(sendBuffer, to_string(result1).c_str());
			}
		}
		
		send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
		cout << "Message sent: " << sendBuffer << endl;
	}
	connectionCount--;
	closesocket(clientSocket);
}

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

	SOCKET serverSocket, acceptSocket;
	int port = 55555;
	
	serverSocket = INVALID_SOCKET;
	serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (serverSocket == INVALID_SOCKET) {
		cout << "Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return 0;
	}
	else {
		cout << "socket() is OK!" << endl;
	}

	sockaddr_in service;
	service.sin_family = AF_INET;
	InetPton(AF_INET, _T("127.0.0.1"), &service.sin_addr.s_addr);
	service.sin_port = htons(port);
	if (bind(serverSocket, (SOCKADDR*)&service, sizeof(service)) == SOCKET_ERROR) {
		cout << "bind() failed: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else {
		cout << "bind() is OK!" << endl;
	}

	if (listen(serverSocket, 1) == SOCKET_ERROR) {
		cout << "listen(): Error listening on socket " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return 0;
	}
	else {
		cout << "listen() is OK, waiting for the connections..." << endl;
	}

	while (true) {
		acceptSocket = accept(serverSocket, NULL, NULL);
		if (acceptSocket == INVALID_SOCKET) {
			cout << "accept failed: " << WSAGetLastError() << endl;
			WSACleanup();
			continue;
		}
		int currentCount = connectionCount.load();
		string mes = "You are a client No. " +to_string(currentCount);
		if (currentCount % 2 == 1) {
			mes.append(". Enter two numbers and the server will add them.\n");
		}
		else {
			mes.append(". Enter two numbers and the server will multiply them.\n");
		}
		mes.append("Type 'exit' to terminate connection...");

		send(acceptSocket, mes.c_str(), mes.size(), 0);
	
		thread clientThread(handleClient, acceptSocket);
		clientThread.detach();
	
	}

	system("pause");

	WSACleanup(); 
	return 0;
}