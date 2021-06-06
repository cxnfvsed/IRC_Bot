#pragma once
//message parser header
#include <map>
#include <string>

using user_attribute = std::map<std::string, std::string>;

struct twitch_message
{
	user_attribute attribute;
	std::string username;
	std::string msg_type;
	std::string channel;
	std::string body;

};

user_attribute AttributesFor(const std::string& List);
bool parse_message(const std::string& info, twitch_message& msg);