# СПОВМ Курсовой проект

Проект создавался в Microsoft Visual Studio 2019.

Для того,чтобы собрать проект необходимо:


1)Установить библиотеку Boost C++(Project properties - VC++ Directories - Include Directories - $(BOOST_ROOT))

2)Установить через менеджер пакетов VS19 vcpkg(git clone https://github.com/Microsoft/vcpkg.git;.\vcpkg\bootstrap-vcpkg.bat;vcpkg integrate install)

3)Установить sqlite3.h, используя vcpkg через powershell (.\vcpkg.exe install sqlite3:x64-windows)

Проект написан для использования на платформе Twitch.tv
Для тестирования необходимо:

1)Зарегистрировать один аккаунт, к чату которого будет подключаться бот

2)Зарегистрировать второй аккаунт, предназначенный для бота 

3)В файл supersecret.txt записать oauth токен(получить токен канала - https://twitchapps.com/tmi/)

Для хранения комманд используется небольшая БД на основе SQL.
Чтобы правильно хранить комманды, необходимо:

1)Создать фалй ircbot.db

2)Для просмотра содержимого установить DB Browser(SQLite)

3)В [коде](https://github.com/Kry1ov/IRC_Bot/blob/main/command_repository.cpp) указать файл из первого пункта

После того,как проект будет собран, в чате канала бот уведомит об успешном подключении.
