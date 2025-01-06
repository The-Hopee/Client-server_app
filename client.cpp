#include <iostream>
#include <vector>
#include "hfa.h"

int connection;

std::string ServerMessage(int client_connection) //Получение сообщения от сервера
{
    //ПРОБЛЕМА КРОЕТСЯ ЗДЕСЬ
    int msg_size = 0;
    char *msg;
    int nreadb=0;//переменная для проверки
    nreadb = recv(client_connection,(char*)&msg_size,sizeof(int),0);
    //std::cout << "recieving msg_size = " << msg_size << std::endl;
    if(nreadb != sizeof(int))
    {
        //std::cout<<" fitrst " << std::endl;
        return "";
    }
    msg = new char[msg_size+1];
    msg[msg_size] = '\0';
    //std::cout << "Выделенная память под msg: " << strlen(msg) << std::endl;
    nreadb = recv(client_connection,msg,msg_size,0);
    // int count_zero = 0;
    // for(int i = 0; i < msg_size;i++)
    // {
    //     if(msg[i] == '\0')
    //     {
    //         std::cout << i << " " << msg[i] << std::endl;
    //         count_zero++;
    //     }
    // }
    // std::cout << "Ноль-Терминалов: " << count_zero << std::endl;
    if(nreadb != msg_size)
    {
        //std::cout<<" second " << std::endl;
        return "";
    }
    //std::cout << msg << std::endl;
    //std:: cout << "Recieved msg in function: " << msg << std::endl;
    
    //          ОНО ПОЧЕМУ-ТО СКИПАЕТ БОЛЬШУЮ ЧАСТЬ СООБЩЕНИЯ
    std::string msg_from_srv;
    for(int i = 0; i < msg_size; i++)
    {
        msg_from_srv[i] = msg[i];
    }
    delete msg;
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
    bool flag = false;
    std::string msg;
    std::string msg_from_serv;
    std::vector<std::string> vec_com;
    while(true)
    {
        std::cout << "Введите команду(Help - для вывода доступных команд): " << std::endl;
        msg = "";
        getline(std::cin, msg);
        vec_com = Parse(msg);
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
            }
            else
            {
                std::cout << "Некорректный ввод!" << std::endl;
                flag = true;
            }

            // Начинается код ShowData
            std::cout << ServerMessage(connection) << std::endl;
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
                std::cout << "Зашел и послал " << msg << std::endl;
                msg_size = msg.size();
                send(connection,(char*)&msg_size,sizeof(int),0);
                send(connection,vec_com[0].c_str(),msg_size,0);
            }
            else
            {
                std::cout << "Некорректный ввод!" << std::endl;
                flag = true;
            }

            //Реализация Delete
            msg = ServerMessage(connection);
            std::cout << msg << std::endl;
        }
        else if (vec_com[0] == "ChPass")
        {
            if(vec_com.size() == 2)
            {
                //Послали команду
                msg_size = msg.size();
                send(connection,(char*)&msg_size,sizeof(int),0);
                send(connection,msg.c_str(),msg_size,0);
            }
            else
            {
                std::cout << "Некорректный ввод!" << std::endl;
                flag = true;
            }

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
        else if(vec_com[0] == "Find")
        {
            if(vec_com.size() == 2)
            {
                //Послали команду
                msg_size = msg.size();
                send(connection,(char*)&msg_size,sizeof(int),0);
                send(connection,msg.c_str(),msg_size,0);
            }
            else
            {
                std::cout << "Некорректный ввод!" << std::endl;
                flag = true;
            }

            //Реализация Find
            std::cout << ServerMessage(connection) << std::endl;
        }
        else
        {
            std::cout << "Некорректный ввод!" << std::endl;
        }
    }
    close(connection); //Отсоединяемся от сервака
    return 0;
}