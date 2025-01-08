#include <iostream>
#include <vector>
#include "hfa.h"

int connection;

std::string ServerMessage(int client_connection) //Получение сообщения от сервера
{
    int msg_size = 0;
    char msg[1500];
    int nreadb=0;//переменная для проверки
    nreadb = recv(client_connection,(char*)&msg_size,sizeof(int),0);
    if(nreadb != sizeof(int))
    {
        return "";
    }
    nreadb = recv(client_connection,msg,sizeof(msg),0);
    if(nreadb != msg_size)
    {
        return "";
    }
    std::string msg_from_srv;
    msg_from_srv.resize(msg_size);
    for(int i = 0; i < msg_size; i++)
    {
        msg_from_srv[i] = msg[i];
    }
    return msg_from_srv;
}

std::vector <std::string> Parse(std::string str_in) 
{
	std::string str_out = "";
	std::vector<std::string> vec;
	for (int i = 0; i < str_in.size(); i++)
	{
		if (str_in[i] != ' ') // собираем строчку пока не встретили " "
		{
			str_out.push_back(str_in[i]);
		}
		else // как только встретили " "
		{
			vec.push_back(str_out);
			str_out = "";
		}
	}
	vec.push_back(str_out);
	return vec;
}

int main()
{
    sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);

    connection = socket(AF_INET,SOCK_STREAM,0);

    if(connect(connection,(sockaddr*)&addr,sizeof(addr)) < 0)
    {
        std::cout << "Ошибка подсоединения к серверу!"<<std::endl;
        exit(1);
    }

    //вклинить сюда процесс авторизации
    std::cout << "Соединение установлено. Добро пожаловать на сервер! " << std::endl;

    int msg_size = 0;
    std::string msg;
    std::string msg_from_serv;
    std::vector<std::string> vec_com;
    while(true)
    {
        msg = "";
        std::cout << "\nВведите команду(Help - для вывода доступных команд): " << std::endl;
        getline(std::cin, msg);
        //std::cout << "Ваша команда: " << msg << std::endl;
        vec_com = Parse(msg);
        //std::cout << "Parse: " << std::endl;
        // for(auto iter: vec_com)
        // {
        //     std::cout << iter << "\t";
        // }
        std::cout << std::endl;
        if(vec_com[0] == "NewData")
        {
            msg_size = msg.size();
            send(connection,(char*)&msg_size,sizeof(int),0);
            send(connection,msg.c_str(),msg_size,0);
            // Начинается код Newdata
            while(msg_from_serv != "Пользователь добавлен!"  || msg_from_serv != "Неверный выбор!")
            {
                msg_from_serv = ServerMessage(connection);
                std::cout << msg_from_serv << std::endl;
                if(msg_from_serv == "Пользователь добавлен!"  || msg_from_serv == "Неверный выбор!")
                {
                    break;
                }
                getline(std::cin,msg);
                msg_size = msg.size();
                send(connection,(char*)&msg_size,sizeof(int),0);
                send(connection,msg.c_str(),msg_size,0);
            }
        }
        else if (vec_com[0] == "ShowData")
        {
            //Послали команду
            if(vec_com.size() == 2)
            {   
                msg_size = msg.size();
                send(connection,(char*)&msg_size,sizeof(int),0);
                send(connection,msg.c_str(),msg_size,0);

                // Начинается код ShowData
                std::cout << ServerMessage(connection) << std::endl;
            }
            else
            {
                std::cout << "Некорректный ввод!" << std::endl;
            }
        }
        else if (vec_com[0] == "Help")
        {
            msg_size = msg.size();
            send(connection,(char*)&msg_size,sizeof(int),0);
            send(connection,msg.c_str(),msg_size,0);
            std::cout << ServerMessage(connection) << std::endl;
        }
        else if (vec_com[0] == "Exit")
        {
            std::cout << "Выход из приложения..." << std::endl;
            msg_size = msg.size();
            send(connection,(char*)&msg_size,sizeof(int),0);
            send(connection,msg.c_str(),msg_size,0);
            break;
        }
        else if (vec_com[0] == "Delete")
        {
            if(vec_com.size() == 2)
            {
                //Послали команду
                msg_size = msg.size();
                send(connection,(char*)&msg_size,sizeof(int),0);
                send(connection,msg.c_str(),msg_size,0);

                //Реализация Delete
                std::cout << ServerMessage(connection) << std::endl;
            }
            else
            {
                std::cout << "Некорректный ввод!" << std::endl;
            }
        }
        else if (vec_com[0] == "ChPass")
        {
            if(vec_com.size() == 2)
            {
                //Послали команду
                msg_size = msg.size();
                send(connection,(char*)&msg_size,sizeof(int),0);
                send(connection,msg.c_str(),msg_size,0);

                //Реализация ChPass
                while(msg_from_serv != "Пароль изменен!" || msg_from_serv != "Пароль не валидный. Используется автогенерация для лучшей защиты данных: ")
                {
                    msg_from_serv = ServerMessage(connection);
                    std::cout << msg_from_serv << std::endl;
                    if( msg_from_serv == "Пароль изменен!" || msg_from_serv == "Пароль не валидный. Используется автогенерация для лучшей защиты данных: " )
                    {
                        break;
                    }
                    else
                    {
                        getline(std::cin,msg);
                        msg_size = msg.size();
                        send(connection,(char*)&msg_size,sizeof(int),0);
                        send(connection,msg.c_str(),msg_size,0);
                    }
                }
            }
            else
            {
                std::cout << "Некорректный ввод!" << std::endl;
            }
        }
        else if(vec_com[0] == "Find")
        {
            if(vec_com.size() == 2)
            {
                //Послали команду
                msg_size = msg.size();
                send(connection,(char*)&msg_size,sizeof(int),0);
                send(connection,msg.c_str(),msg_size,0);

                //Реализация Find
                std::cout << ServerMessage(connection) << std::endl;
            }
            else
            {
                std::cout << "Некорректный ввод!" << std::endl;
            }
        }
        else
        {
            std::cout << "Некорректный ввод!" << std::endl;
        }
    }
    close(connection); //Отсоединяемся от сервака
    return 0;
}