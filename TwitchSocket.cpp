#include "pch.hpp"

#include <WinSock2.h>
#include <Windows.h>
#include <ws2tcpip.h>
#include <string>
#include <vector>
#include <iostream>
#include "TwitchSocket.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Winmm.lib")

namespace bot
{
	//send to server
	int TwitchSocket :: send(const std::string& information) 
	{
		//ѕервый аргумент - сокет-дескриптор, в который записываютс€ данные. ¬торой и третий аргументы - соответственно, адрес и длина буфера с записываемыми данными.
		//„етвертый параметр - это комбинаци€ битовых флагов, управл€ющих режимами записи. ≈сли аргумент flags равен нулю, то запись в сокет (и, соответственно, считывание )
		//происходит в пор€дке поступлени€ байтов.
		return ::send(this->Connection, information.c_str(), information.length(), NULL);
	}
	//send message to channel (lambda expr are used)
	std::function<int(const std::string&)> TwitchSocket::sendToChannel(const std::string& channel) 
	{
		return[=](const std::string& message) -> int {
			return this->send("PRIVMSG #" + channel + " :" + message + "\r\n");
		};
	}
	//get answer from server
	std::string TwitchSocket:: receive() 
	{
		//recv аналогична send, но дл€ чтени€(такие же параметры)
		int bytesRecieved = recv(this->Connection, buffer.data(), BufferLength, NULL);
		if (bytesRecieved < 0) 
		{
			return "";
		}
		return std::string(buffer.begin(), buffer.begin() + bytesRecieved);
	}
	//construct which takes parameters described in twitch documentation
	TwitchSocket::TwitchSocket(std::string username,std::string password)
	{
		this->Connection = INVALID_SOCKET;
		this->username = username;
		this->password = password;
	}
	//connect to server with all needed error checks
	int TwitchSocket::connect() 
	{
		WSADATA wsaData;
		addrinfo hints{ };
		addrinfo* result;

		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //WinSock initialization
		{
			std::cout << "Startup failed!" << std::endl;
			return 1;
		}

		hints.ai_family = AF_UNSPEC;		//addres(unspecified)
		hints.ai_socktype = SOCK_STREAM;	//socket type
		hints.ai_protocol = IPPROTO_TCP;	//protocol type(TCP - Transmission Control Protocol )

		if (getaddrinfo("irc.chat.twitch.tv", "6667", &hints, &result) != 0) //host name to adress (host name, port number, hints(info about socket),response info about host)
		{ 
			std::cout << "getaddrinfo failed: <<RETURN CODE CAN BE PASSED>>" << std::endl;
		}

		this->Connection = socket(result->ai_family, result->ai_socktype, result->ai_protocol); //making a socket
		if (this->Connection == INVALID_SOCKET) 
		{
			std::cout << "Erro at socket(): " << WSAGetLastError() << std::endl;
			freeaddrinfo(result); //free the adress
			WSACleanup();	      //terminates use of WinSock
			return 1;
		}

		if (::connect(this->Connection, result->ai_addr, result->ai_addrlen) == SOCKET_ERROR) 
		{
			std::cout << "[ERR] Connected with error" << std::endl;
			closesocket(this->Connection); //if error - close the existing socket
			this->Connection = INVALID_SOCKET;
			return 1;
		}

		freeaddrinfo(result);

		if (this->Connection == INVALID_SOCKET)
		{
			std::cout << "Cannot connect to server" << std::endl;
			WSACleanup();
			return 1;
		}

		//sending the password and username to socket as required
		send("PASS " + this->password + "\r\n");
		send("NICK " + this->username + "\r\n");
	}
}