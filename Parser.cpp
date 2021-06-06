#include "pch.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include "Parser.h"

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;
namespace fusion = boost::fusion;

std::string to_string(const std::vector<char> &vec)
{
	return std::string(vec.begin(), vec.end());
}

bool parse_message(const std::string& info,twitch_message &msg)
{
	using ascii::char_;
	//twitch_message msg;

	const auto attrs = [&msg](auto attribute) -> void { //capture msg by its link [&msg] ,  attribute as a parameter, and body of expression, and void as a return type
		msg.attribute = AttributesFor(to_string(attribute));
	};

	const auto user = [&msg](auto username) -> void {
		msg.username = to_string(username);
	};

	const auto msgtype = [&msg](auto type) -> void {
		msg.msg_type = to_string(type);
	};

	const auto channel = [&msg](auto channel) -> void {
		msg.channel = to_string(channel);
	};

	const auto body = [&msg](auto body) -> void {
		msg.body = to_string(body);
	};
	// @key = value;key=value ::exr666bot!exr666bot@exr666bot.tmi.twitch.tv PRIVMSG #exr666of :hello
	//std::regex re(R"RE(^@(.+) :([^!]+)![^@]+@[^\.]+\.tmi\.twitch\.tv PRIVMSG #([^\s]+) :(.*))RE");
	auto messageParser =
		'@'
		>> (+(char_ - ' '))[attrs] >> " :"
		>> (+(char_ - '!'))[user]			//anything that is not ! - search of username
		>> (+(char_ - '@')) >> '@'			//anything thaht is not @ - then take @
		>> (+(char_ - '.')) >> '.'			//anything thaht is not @ - then take .
		>> "tmi.twitch.tv "					//find the tmi.twitch.tv in attrs
		>> (+(char_ - ' '))[msgtype] >> " #"//anything that is not space - message type - then find " #"
		>> (+(char_ - ' '))[channel] >> " :"//again anything that is not space - finding the channel name and then " :"
		>> (+(char_ - '\r'))[body] >> "\r\n";//and finnaly our message body
		//get the @sign
		//parse the attrs AttributesFor
		//get the username
		//get the IRC command(PRIVMSG)
		//message body

	return qi::parse(info.begin(), info.end(), messageParser);

}


user_attribute AttributesFor(const std::string& List) //transforms info string to a map using boost spirit additional library
{
	using fusion::at_c;
	using ascii::char_;
	using qi::parse;

	user_attribute res;

	auto kvp = [&res](const fusion::vector<std::vector<char>, std::vector<char>>& pair)
	{
		//at_c returns the n-th element from the beginning of the sequence
		auto key = at_c<0>(pair);
		auto val = at_c<1>(pair);

		res[std::string(key.begin(), key.end())] = to_string(val);
	};

	auto kvpParser = ((+(char_ - '=')) >> '=' >> (*(char_ - ';')))[kvp]; //not equals then equals then not ';' (>> указывает,что после чего следует) - это одна пара key = value
	auto attributeParser = kvpParser >> *(';' >> kvpParser); //any amount of ; (парсит все пары key = value)

	parse(List.begin(), List.end(), attributeParser);

	return res;

}