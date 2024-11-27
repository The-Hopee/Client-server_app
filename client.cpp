#include <iostream>
#include "hfa.h"
#include <vector>

std::vector<std::string> vec_ans_from_serv = {"Добро пожаловать на сервер! Теперь вводите команды(Help для вывода доступных команд). ",
"Пароль не валидный. Используется автогенерация для лучшей защиты данных: ", "Пароль изменен!", "Пользователь удален!",
"Данные для этого сервиса не найдены! ","Данные для этого сервиса уже есть!", "Все сервисы: ", "NewData - создаются новые данные",
"Пароль: ","Неверный выбор!","Пользователь добавлен!"};
int connection;

std::string ServerMessage() //Получение сообщения от сервера
{
    int msg_size;
    char *msg;
    int nreadb=0;
    nreadb = recv(connection,(char*)&msg_size,sizeof(int),0);
    if(nreadb != sizeof(int))
    {
        return "";
    }
    msg = new char[msg_size+1];
    msg[msg_size] = '\0';
    nreadb = recv(connection,msg,msg_size,0);
    if(nreadb != msg_size)
    {
        return "";
    }
    std::cout << msg << std::endl;
    std::string msg_from_srv = msg;
    return msg_from_srv;
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
    std::cout << "Соединение установлено. " << std::endl;
    int msg_size;
    std::string msg;
    std::string msg_from_serv;
    std::string str;

    while(true)
    {
        //std::cout << "received message1" << std::endl;
        msg_from_serv = ServerMessage();
        for(auto iter: vec_ans_from_serv)
        {
            if(msg_from_serv == iter) //Если в принятом сообщении содержится фраза из списка для автоответов, то отправляем автоответ
            {
                str = "Спасибо!";
                msg_size= str.size();
                send(connection,(char*)&msg_size,sizeof(int),0);// передаем размер строки
                send(connection,str.c_str(),msg_size,0);
            }
        }
        std::cout << "Ответ от клиента: ";
        getline(std::cin, msg);
        msg_size= msg.size();
        send(connection,(char*)&msg_size,sizeof(int),0);// передаем размер строки
        send(connection,msg.c_str(),msg_size,0);// затем передаем саму строку
        //std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
   /// system("pause");  //read -p \"Press any key to continue...\" -p 1
    return 0;
}