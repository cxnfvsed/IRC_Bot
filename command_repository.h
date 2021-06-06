#include <sqlite3.h>
#pragma once
namespace eb {
	class command_repository
	{
	public:
		command_repository(const std::string& filename);
		~command_repository();

		std::map<std::string, std::string> load_commands();
		void add_command(std::string const& name, std::string const& reply);
	private:
		std::unique_ptr <sqlite3, decltype(&sqlite3_close)> db_handle; //sqlite3 - template class;  decltype(&sqlite3_close) - new type(used for default deleter)
	};
}
