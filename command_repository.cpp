#include "pch.hpp"
#include "command_repository.h"
namespace {
	std::unique_ptr<sqlite3, decltype(&sqlite3_close)>
		connect(std::string const& filename)
	{
		sqlite3* handle; //db handle
		int result = sqlite3_open("ircbot.db", &handle); //opens db and writing handle to variable handle
		return std::unique_ptr <sqlite3, decltype(&sqlite3_close)>(handle, &sqlite3_close);
	}

	const std::string sqlite3_column_string(sqlite3_stmt* statement, const int column)
	{
		return std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, column)));
	}

}

namespace eb {
	command_repository::command_repository(const std::string& filename)
		: db_handle(connect(filename))
	{

	}

	command_repository:: ~command_repository()
	{
		sqlite3_close(db_handle.get()); //close the db connection (get funct returns the adress of unique ptr)
	}

	std::map<std::string, std::string > command_repository::load_commands()
	{
		std::map<std::string, std::string > commands;
		sqlite3_stmt* statement; //pointer to a single SQl statement
		int result;
		result = sqlite3_prepare_v2(this->db_handle.get(), "SELECT Name, Reply FROM Commands", -1, &statement, NULL);

	   //looking through the rows of db
		//sqlite3_step - advance an sqlite3 stmt to the next result row or to completion(move to next result row or to end)
		while ((result = sqlite3_step(statement)) == SQLITE_ROW)
		{
			std::string name(sqlite3_column_string(statement, 0)); //name text result
			std::string reply(sqlite3_column_string(statement, 1));//command reply text result
			commands[name] = reply; //save the reply by name
		}
	
		sqlite3_finalize(statement); //deleting a prepared sql statement which was declarated before
		return commands;
	}

	void command_repository::add_command(std::string const& name, std::string const& reply)
	{
		sqlite3_stmt* add_command_statement; //new statement

		sqlite3_prepare_v2(this->db_handle.get(), "INSERT INTO Commands (Name, Reply) VALUES(:NAME, :REPLY)", -1, &add_command_statement, NULL); //insert to db
		//binding values to prepared statements(statements were prepared by sqlite3_prepare_v2 com)
		sqlite3_bind_text(add_command_statement, 1, name.c_str(), name.length(), SQLITE_TRANSIENT);//привязывает новые данные к запросу
		sqlite3_bind_text(add_command_statement, 2, reply.c_str(), reply.length(), SQLITE_TRANSIENT);

		sqlite3_step(add_command_statement);
		sqlite3_finalize(add_command_statement); //delete a prepared sql statement
	}
}
