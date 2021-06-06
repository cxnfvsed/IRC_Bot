#include <vector>
#include <string>
#include <stdint.h>
#include <functional>

#pragma once
namespace bot {
	constexpr int BufferLength = 512;
	class TwitchSocket {

	private:
		//SOCKET Connection;
		std::vector<char> buffer = std::vector<char>(BufferLength);
		std::string username;
		std::string password;
		uint64_t Connection;

	public:
		std::function<int(const std::string&)> sendToChannel(const std::string& channel);
		int send(const std::string& information);
		std::string receive();
		TwitchSocket(const TwitchSocket&) = delete;
		TwitchSocket& operator=(const TwitchSocket&) = delete;
		TwitchSocket(std::string username, std::string password);
		int connect();
		
	};
}