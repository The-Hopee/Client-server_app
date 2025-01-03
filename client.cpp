#include <iostream>
#include "hfa.h"

int connection;
int msg_size;

std::string ServerMessage() //Получение сообщения от сервера
{
    char *msg;
    int nreadb=0;//переменная для проверки
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
    delete msg;
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
    std::string msg;
    std::string msg_from_serv;
    std::string str;

    while(true)
    {
        //std::cout << "received message1" << std::endl;
        msg_from_serv = ServerMessage();
        // while(msg_from_serv.size() != msg_size)
        // {
        //     msg = "Неполные данные";
        //     msg_size= msg.size();
        //     send(connection,(char*)&msg_size,sizeof(int),0);
        //     send(connection,msg.c_str(),msg_size,0);
        //     msg_from_serv = ServerMessage();
        // }
        //std::cout << "Ответ от клиента: ";
        getline(std::cin, msg);
        msg_size= msg.size();
        send(connection,(char*)&msg_size,sizeof(int),0);// передаем размер строки
        send(connection,msg.c_str(),msg_size,0);// затем передаем саму строку
        
    }
    return 0;
}