#include "pch.hpp"

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <chrono>
#include <map>
#include <sstream>
#include <time.h>
//#include <sqlite3.h>
#include <codecvt>
#include <boost/algorithm/string.hpp>
#include "TwitchSocket.h"
#include "Parser.h"
#include "command_repository.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Winmm.lib")
//for some reason dosent work in release mod(perhaps need to turn off opitimzation for made it work)
using namespace std;


string rollDie()
{
	int roll;
	int min = 1; // the min number a die can roll is 1
	int max = 10;// this->dieSize; // the max value is the die size

	roll = rand() % (max - min + 1) + min;
	string res = to_string(roll);
	return res;
}

bool botCommand(const string& commandName, const string& command, string* args)
{
	int split = commandName.length() + 1;
	if (command.substr(0, split) == commandName + " ") 
	{
		*args = command.substr(split);
		return true;
	}

	
	return false;
}


const bool isMod(const map<string, string> &attributes) 
{		
	if (auto mod_search = attributes.find("display-name"); mod_search != attributes.end())
	{
		if (mod_search->second == "exr666of")
			return true;
	}
	
	//checks if user is a moderator of a channel
	if (auto mod_search = attributes.find("user-type"); mod_search != attributes.end())
	{
		return mod_search->second == "mod"; //find pair key value with value = mod
	}
	return false;
}

int main() {

	
	ifstream file("supersecret.txt");
	string OAuthToken;

	if (file.is_open())//open the file with Twitch channel token
	{ 
		getline(file,OAuthToken,'\n');
		file.close();
	}

	bot::TwitchSocket twitch("exr666bot",OAuthToken); //connection 
	if (twitch.connect() == 1) //if connected
	{ 
		return 1;
	}
	
	cout << twitch.receive() << endl;

	twitch.send("JOIN #exr666of\r\n"); //join the channel
	twitch.send("CAP REQ :twitch.tv/tags\r\n"); //let us watch the status of user who send the message

	chrono::system_clock clock;
	chrono::time_point<chrono::system_clock> last_horn; //timer for !horn usage by using system clock
	last_horn = clock.now() - chrono::seconds(30);

	chrono::system_clock clock1;
	chrono::time_point<chrono::system_clock> last_dice; //timer for !horn usage by using system clock
	last_dice = clock1.now() - chrono::seconds(15);

	const auto sendMessage = twitch.sendToChannel("exr666of"); //sends message to particular channel
	
	eb::command_repository repo("ircbot.db");

	map<string, string> commands = repo.load_commands();

	sendMessage("IRCBot is online!");

	while (true) 
	{
		string reply = twitch.receive(); //reply contains the twitch server recieve 
		twitch_message message;
		if (parse_message(reply,message))
		{

			map<string, string> attributes = message.attribute;

			cout << reply << endl;
			cout << "User: " << message.username << endl;
			cout << "Channel: " << message.channel << " message: " << message.body << endl;

			string arguments;

			//bot commands
			if (message.body == "!sound" && isMod(attributes)) //only moderator can use this command
			{
				auto lastUsed = clock.now() - last_horn;
				if (lastUsed >= chrono::seconds(30))//cooldown check
				{
					PlaySound(TEXT("horn.wav"), NULL, SND_FILENAME); //play the sound
					last_horn = clock.now();
				}
				else
				{
					auto wait = chrono::duration_cast<chrono::seconds>(chrono::seconds(30) - lastUsed).count(); //will send the value in sendMessage if command is in cooldown
					stringstream strWait;
					strWait << "You have to wait " << wait << " seconds until new call";
					sendMessage(strWait.str());
				}
			}

			//to the random 
			if (message.body == "!dice")
			{
				srand(time(0));
				auto last_used = clock1.now() - last_dice;
				if (last_used >= chrono::seconds(15))
				{
					sendMessage("@" + message.username + " rolled number " + rollDie());
					//sendMessage("You rolled " + rollDie());
					last_dice = clock.now();
				}
				else
				{
					auto waitDice = chrono::duration_cast<chrono::seconds>(chrono::seconds(15) - last_used).count(); //will send the value in sendMessage if command is in cooldown
					stringstream strWaitDice;
					strWaitDice << "Dice will be avialable in " << waitDice << " seconds";
					sendMessage(strWaitDice.str());
				}

			}

			if (isMod(attributes) && botCommand("!addcom", message.body, &arguments))
			{

				//parse
				int index = arguments.find_first_of(" ");
				if (index != string::npos) //nothing found
				{
					string command_to_add;
					string reply;
					
					command_to_add = arguments.substr(0,index);
					reply = arguments.substr(index + 1);

					repo.add_command(command_to_add, reply);
					commands[command_to_add] = reply;
				}
				
			}

			if (botCommand("!welcome", message.body, &arguments))
			{
				//welcome particular user
				sendMessage("Glad to see ya " + arguments + "!");
			}

			if (!message.body.empty() && message.body.front() == '!') //if the string not empty and the first element of list is '!'
			{
				auto command = commands.find(message.body.substr(1));
				if (command != commands.end())
				{
					sendMessage(command->second);
				}
				else
				{
					cout << "Command: " << message.body.substr(1) << " not found" << endl;
				}
			}
		}
	}

	return 0;
}