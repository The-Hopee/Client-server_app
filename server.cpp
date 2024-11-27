// #include <iostream>
// #include <map>
// #include <vector>
// #include <fstream>
// #include <fcntl.h>
// #include <iomanip>
// #include <sstream>
// #include <string>
// #include <stdio.h>
// #include "hfa.h"
// #include <cryptlib.h>
// #include "rijndael.h"
// #include "modes.h"
// #include "files.h"
// #include "osrng.h"
// #include "hex.h"
// #include "base64.h"
#include "Mystorage.h"

/*
        ПО ФАКТУ, ВСЯ КЛИЕНТ-СЕРВЕРНАЯ АРХИТЕКТУРА - ЭТО ПОСТОЯННОЕ ИСПОЛЬЗЛОВАНИЕ 2Х ФУКНЦИЙ: send и receive
        ОТПРАВИЛ - ПРИНЯЛ, ОТПРАВИЛ - ПРИНЯЛ И ТД
*/

std::vector<int> connect_users; //вектор юзеров которые присоединились
int count; // количество присоединенных юзеров

std::string Message_from_client(int ind) // принимаемыми параметрами будут сообщение клиента и мапа
{
    int msg_size;

    int nreadb;
    nreadb = recv(ind, (char*)&msg_size,sizeof(int), 0); // принимаем пакет с размером строки и записываем его в переменную

    if(nreadb != sizeof(int))
    {
        return "";
    }
    char* msg = new char[msg_size + 1];
    msg[msg_size] =  '\0';
    nreadb = recv(ind,msg,msg_size,0);// приняли пакет с сообщением записали в динамический массив
    if(nreadb != msg_size)
    {
        return "";
    }
    // дешифруем данные
    std::string str = msg; //копируем в стринг
    delete[] msg;
    return str;
    // проверяем данные
}
 
using namespace CryptoPP;
 
// AES EBC encryption (output Base64)
std::string aes_encrypt_ecb_base64(std::string data , unsigned char* key, int keylen)
{
    std::string encrypt_str;
    try 
    {
        CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption ecb_encription(key, keylen);
        CryptoPP::StreamTransformationFilter stf_encription(
            ecb_encription,
            new CryptoPP::Base64Encoder(new CryptoPP::StringSink(encrypt_str)),
            CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING
        );
        stf_encription.Put(reinterpret_cast<const unsigned char*>(data.c_str()), data.length() + 1);
        stf_encription.MessageEnd();
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
 
    return encrypt_str;
}
 
 // AES EBC encryption (output HEX) 
std::string aes_encrypt_ecb_hex(std::string data , unsigned char* key, int keylen)
{
    std::string encrypt_str;
 
    try 
    {
        CryptoPP::ECB_Mode<CryptoPP::AES>::Encryption ecb_encription(key, keylen);
        CryptoPP::StreamTransformationFilter stf_encription(
            ecb_encription,
            new CryptoPP::HexEncoder(new CryptoPP::StringSink(encrypt_str)),
            CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING
        );
        stf_encription.Put(reinterpret_cast<const unsigned char*>(data.c_str()), data.length() + 1);
        stf_encription.MessageEnd();
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
    }
 
    return encrypt_str;
}
 
 // AES EBC decryption (output Base64)
std::string aes_decrypt_ecb_base64(std::string base64_data, unsigned char* key, int keylen)
{
    try 
    {
        std::string aes_encrypt_data;
        CryptoPP::Base64Decoder decoder;
        decoder.Attach(new CryptoPP::StringSink(aes_encrypt_data));
        decoder.Put(reinterpret_cast<const unsigned char*>(base64_data.c_str()), base64_data.length());
        decoder.MessageEnd();
 
        std::string decrypt_data;
        CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption ebc_description(key, keylen);
        CryptoPP::StreamTransformationFilter stf_description(
            ebc_description,
            new CryptoPP::StringSink(decrypt_data), 
            CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING
        );
 
        stf_description.Put(
            reinterpret_cast<const unsigned char*>(aes_encrypt_data.c_str()), 
            aes_encrypt_data.length()
        );
        stf_description.MessageEnd();
 
        return decrypt_data;
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
        return "";
    }
}
 
 // AES EBC Decryption (Output HEX)
std::string aes_decrypt_ecb_hex(std::string hex_data, unsigned char* key, int keylen)
{
    try
    {
        std::string aes_encrypt_data;
        CryptoPP::HexDecoder decoder;
        decoder.Attach(new CryptoPP::StringSink(aes_encrypt_data));
        decoder.Put(reinterpret_cast<const unsigned char*>(hex_data.c_str()), hex_data.length());
        decoder.MessageEnd();
 
        std::string decrypt_data;
        CryptoPP::ECB_Mode<CryptoPP::AES>::Decryption ebc_description(key, keylen);
        CryptoPP::StreamTransformationFilter stf_description(
            ebc_description,
            new CryptoPP::StringSink(decrypt_data),
            CryptoPP::BlockPaddingSchemeDef::ZEROS_PADDING
        );
 
        stf_description.Put(
            reinterpret_cast<const unsigned char*>(aes_encrypt_data.c_str()),
            aes_encrypt_data.length()
        );
        stf_description.MessageEnd();
 
        return decrypt_data;
    }
    catch (std::exception e) {
        std::cout << e.what() << std::endl;
        return "";
    }
}

const int SIZE = 16;

std::string generator_pass()
{
	std::srand(std::time(nullptr));
	std::string str = "";// создаем пустую строчку
	for (int i = 0; i < SIZE; i++)
	{
		if (i % 2 == 0)
		{
			if (i % 4 == 0)
			{
				//буквы в нижнем регистре
				str.push_back(char('a' + rand() % 26));
			}
			else
			{
				str.push_back(char('0' + rand() % 10));
			}
		}
		else
		{
			if (i % 3 == 0)
			{
				//#,&,* и т.д
				str.push_back(char('!' + rand() % 14));
			}
			else
			{
				str.push_back(char('A' + rand() % 26)); // буквы в верхнем регистре
			}
		}
	}
	return str;
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
	vec.shrink_to_fit();
	return vec;
}

int check_pass(std::string &password)
{
	int i = 0;
	int count_symb = 0;
	int count_low_reg = 0;
	int count_up_reg = 0;
	while(i != password.size()) // проверка на криптоустойчивость
	{
		for(int j = 33;j < 48; j++)
		{
			if(password[i] == char(j)) //если находим хотя-бы 1 из этих символов
			{
				count_symb++;
			}
		}
		for (int k = 97; k < 123; k++) // и из этих
		{
			if(password[i] == char(k))
			{
				count_low_reg++;
			}
		}
		for (int p = 65; p < 91; p++) // и из этих
		{
			if (password[i] == char(p))
			{
				count_up_reg++;
			}
		}
		i++;
	}
	if(count_low_reg > 0 && count_symb > 0 && count_up_reg > 0)
	{
		return 1;
	}
	return 0;
}

class base
{
private:
	std::string login;
	std::string password;
	std::string servis;
    int newConnection;
public:
	void getdata(std::map<std::string , std::pair<std::string,std::string>>& mapa)
	{
		std::string ch;
		std::string pass;
        std::string msg;
        int msg_size;
		msg = "Введите предпочитаемый сервис: ";
		msg_size = msg.size();
        send(newConnection,(char*)&msg_size,sizeof(int),0);
        send(newConnection,msg.c_str(),msg_size,0);
        servis = Message_from_client(newConnection);
		msg = "Введите логин: ";
		msg_size = msg.size();
        send(newConnection,(char*)&msg_size,sizeof(int),0);
        send(newConnection,msg.c_str(),msg_size,0);
        login = Message_from_client(newConnection);
		msg = "Сгенерировать пароль или произвести ввод самостоятельно?(1/0): ";
		msg_size = msg.size();
        send(newConnection,(char*)&msg_size,sizeof(int),0);
        send(newConnection,msg.c_str(),msg_size,0);
        ch = Message_from_client(newConnection);
		if(ch == "1")
		{
			pass = generator_pass();
			mapa[servis] = std::make_pair(login,pass);
			msg = "Пользователь добавлен!";
            msg_size = msg.size();
            send(newConnection,(char*)&msg_size,sizeof(int),0);
            send(newConnection,msg.c_str(),msg_size,0);
            pass = Message_from_client(newConnection);// относится к возвращению автоответа от клиента(Автоответ - это какой-то фитбек, по умолчанию от клиента)
		}
		else if(ch == "0")
		{
			msg = "Введите пароль: ";
			msg_size = msg.size();
            send(newConnection,(char*)&msg_size,sizeof(int),0);
            send(newConnection,msg.c_str(),msg_size,0);
            password = Message_from_client(newConnection);
			if(check_pass(password) == 1)
			{
				mapa[servis] = std::make_pair(login,password);
				msg = "Пользователь добавлен!";
                msg_size = msg.size();
                send(newConnection,(char*)&msg_size,sizeof(int),0);
                send(newConnection,msg.c_str(),msg_size,0);
                pass = Message_from_client(newConnection); // Так же автоответ
			}
			else
			{
				msg  =  "Пароль слабый, используйте больше специальных символов или прописных букв!";
				msg += "\nРекомендуется сгенерировать пароль или ввести его еще раз";
				msg += "\nИспользовать этот пароль или сгенерировать более защищенный?(1/0): ";
                msg_size = msg.size();
                send(newConnection,(char*)&msg_size,sizeof(int),0);
                send(newConnection,msg.c_str(),msg_size,0);
                ch = Message_from_client(newConnection);
				if(ch == "1") // при этой команде срабатывает else в мейне
				{
					mapa[servis] = std::make_pair(login,password);
					msg = "Пользователь добавлен!";
                    msg_size = msg.size();
                    send(newConnection,(char*)&msg_size,sizeof(int),0);
                    send(newConnection,msg.c_str(),msg_size,0);
                    pass = Message_from_client(newConnection); // Так же приняли "автоответ"
					return;
				}
				else if(ch == "0")
				{
					password = generator_pass();
					mapa[servis] = std::make_pair(login,password);
					msg = "Пользователь добавлен!";
                    msg_size = msg.size();
                    send(newConnection,(char*)&msg_size,sizeof(int),0);
                    send(newConnection,msg.c_str(),msg_size,0);
                    pass = Message_from_client(newConnection); // Так же приняли "автоответ"
					return;
				}
			}
		}
		else
		{
			msg = "Неверный выбор!";
            msg_size = msg.size();
            send(newConnection,(char*)&msg_size,sizeof(int),0);
            send(newConnection,msg.c_str(),msg_size,0);
            pass = Message_from_client(newConnection); // Так же приняли "автоответ"
			return;
		}
	}
	void showdata(std::map < std::string, std::pair< std::string,std::string > > &mapa, std::string serv)
	{
        std::string msg = "";
        int msg_size;
        bool flag = false;
		for (auto iter : mapa)
		{
            flag = true;
			if(serv == "All" || serv == "all") //собираем здесь на вывод наши пароли
			{
				msg +="Пароль: " + iter.second.second +"\n";
			}
			else if(serv != "All" || serv != "all")
			{
				if(serv == iter.first)
				{
					msg = "Пароль: " + iter.second.second +"\n";
				}
			}
		}
        // посылаем пароли клиенту и ждем "автоответ"
        if(flag == false)
        {
            msg = "Нет данных!";
        }
        msg_size = msg.size();
        send(newConnection,(char*)&msg_size,sizeof(int),0);
        send(newConnection,msg.c_str(),msg_size,0);
        msg = Message_from_client(newConnection); // Используем автоответ
	}
	void showcom()
	{
		int msg_size = 0;
        std::string msg;
		msg = "NewData - создаются новые данные";
		msg += "\nShowdata all/servis - показывает все или какие-то конкретные данные";
		msg += "\nHelp - показывает все команды";
		msg += "\nExit - выходит из программы";
		msg += "\nDelete servis - Удаляет данные из хранилища с использованием ввода логина";
		msg += "\nChPass servis - изменяет пароль используя верный логин";
		msg += "\nFind servis - находит сервис по его имени";
        msg_size = msg.size();
        send(newConnection,(char*)&msg_size,sizeof(int),0);
        send(newConnection,msg.c_str(),msg_size,0);
        msg = Message_from_client(newConnection); // Используем автоответ
	}
    void get_connect(int con)
    {
        newConnection = con;
    }
	void show_servises(std::map<std::string,std::pair<std::string,std::string>> &mapa, std::string name_servis)
	{
        std::string msg = ""; 
        int msg_size;
        bool is_target = false; // для отслеживания сервиса: есть он или нет
		if(name_servis == "") // все сервисы
		{
			std::cout << "Все сервисы: " << std::endl;
			for(auto iter: mapa)
			{
				msg += iter.first + "\n";
			}
		}
		else // конкретный сервис
		{
			for(auto iter: mapa)
			{
				if(name_servis == iter.first)
				{
					msg = "Данные для этого сервиса уже есть!";
                    is_target = true;
				}
			}
		}
        if(is_target == false)
        {
            msg = "Данные для этого сервиса не найдены! ";
        }
        msg_size = msg.size();
        send(newConnection,(char*)&msg_size,sizeof(int),0);
        send(newConnection,msg.c_str(),msg_size,0);
        msg = Message_from_client(newConnection);
	}
    void delete_user(std::map<std::string,std::pair<std::string,std::string>> &mapa, std::string name_user)
    {
        mapa.erase(name_user);
        std::string msg = "Пользователь удален!";
        int msg_size = msg.size();
        send(newConnection,(char*)&msg_size,sizeof(int),0);
        send(newConnection,msg.c_str(),msg_size,0);
        msg = Message_from_client(newConnection); //Так же  автоответ
    }
    void chpass_user(std::map<std::string,std::pair<std::string,std::string>> &mapa, std::string newPass) // передаем мапу и новый пароль
    {
        std::string login, msg,srvs;
        int msg_size;
        msg = "Введите логин: ";
        msg_size = msg.size();
        send(newConnection,(char*)&msg_size,sizeof(int),0);
        send(newConnection,msg.c_str(),msg_size,0);
        login = Message_from_client(newConnection);
        msg = "Введите сервис: ";
        msg_size = msg.size();
        send(newConnection,(char*)&msg_size,sizeof(int),0);
        send(newConnection,msg.c_str(),msg_size,0);
        srvs = Message_from_client(newConnection);
        if(check_pass(newPass) == 1)
        {
            mapa[srvs] = std::make_pair(login,newPass);
            msg = "Пароль изменен!";
            msg_size = msg.size();
            send(newConnection,(char*)&msg_size,sizeof(int),0);
            send(newConnection,msg.c_str(),msg_size,0);
        }
        else
        {
            msg = "Пароль не валидный. Используется автогенерация для лучшей защиты данных: ";
            mapa[srvs] == std::make_pair(login,generator_pass());
            msg_size = msg.size();
            send(newConnection,(char*)&msg_size,sizeof(int),0);
            send(newConnection,msg.c_str(),msg_size,0);
        }
        msg = Message_from_client(newConnection); // Используем автоответ
    }
};

int main(int argc,char* argv[]) 
{
	//Добавить большую мапу, которая хранит в себе std::pair(логин юзера пароль) и мапу с его сервисом, логином и паролем
	setlocale(LC_ALL,"ru");
	std::map<std::string, std::pair<std::string, std::string>> myMap; // должен быть контейнер который хранит логин и пароль от ячейки, а затем сохраняет все данные от ячейки

	std::vector<std::string> vec_com;

	std::string key, value1,value2;

	base b1;

    sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(3425);

    int sListen = socket(AF_INET,SOCK_STREAM,0);
    bind(sListen,(sockaddr*)&addr,sizeof(addr));// связываем адрес и сокет
    listen(sListen,SOMAXCONN);

    int attempts = 3;
    int newConnection;
    unsigned int size = sizeof(addr);

    std::string log = "timofej.derevyankin@inbox.ru", pass = "Winter@2025"; // логин и пароль для юзера

	std::ifstream fin("MyStorage.txt");
	while (fin >> key >> value1>> value2) // key - сервис, value1 - логин, value2 - пароль
	{
		std::string data_pass = aes_decrypt_ecb_hex(value2, (unsigned char*)"123456789ABCDEF", 16);// Дешифруем пароль
		std::string data_log = aes_decrypt_ecb_hex(value1, (unsigned char*)"123456789ABCDEF", 16);// Дешифруем логин
		std::string data_servis = aes_decrypt_ecb_hex(key, (unsigned char*)"123456789ABCDEF", 16);// Дешифруем сервис
		myMap[data_servis] = std::make_pair(data_log,data_pass);
	}
	std::ofstream fout("MyStorage.txt");

    newConnection = accept(sListen,(sockaddr*)&addr,&size);
    if (newConnection == 0) // проверка на соединение
	{
		std::cout << "Ошибка соединения!" << std::endl;
		exit(1);
	}
    else
    {
        std::string newMessage;
        std::vector<std::string> data_for_open;
        std::string msg = "Приветствуем! Введите свои логин и пароль(логин первым)\n";
        int count_toopen = 0;
        b1.get_connect(newConnection);// посылаем номер соединения, для методов сенд
        int msg_size = msg.size();
        send(newConnection,(char*)&msg_size,sizeof(int),0); // посылаем сначала размер сообщения
        send(newConnection,msg.c_str(),msg_size,0); // затем само сообщение
        while(attempts != 0)
        {
            newMessage = Message_from_client(newConnection);
            if(newMessage.size() != 0 )
            {
                //значит сообщение принялось
                if(attempts == 0)
                {
                    msg = "Вы заблокированы!\n";
                    msg_size = msg.size();
                    send(newConnection,(char*)&msg_size,sizeof(int),0);
                    send(newConnection,msg.c_str(),msg_size,0);
                    //Придумать санкции
                }
                else if(newMessage != log && count_toopen ==0)
                {
                    msg = "Неверный ввод логина! Осталось попыток: " + std::to_string(--attempts);
                    msg_size = msg.size();
                    send(newConnection,(char*)&msg_size,sizeof(int),0);
                    send(newConnection,msg.c_str(),msg_size,0);
                }
                else if(newMessage != pass && count_toopen ==1)
                {
                    msg = "Неверный ввод пароля! Осталось попыток: " + std::to_string(--attempts);
                    msg_size = msg.size();
                    send(newConnection,(char*)&msg_size,sizeof(int),0);
                    send(newConnection,msg.c_str(),msg_size,0);
                }
                if(newMessage == log) //если введен логин
                {
                    msg = "Введите пароль";
                    msg_size = msg.size();
                    send(newConnection,(char*)&msg_size,sizeof(int),0);
                    send(newConnection,msg.c_str(),msg_size,0);
                    count_toopen++;
                }
                if(count_toopen == 1 && newMessage == pass)
                {
                    msg = "Добро пожаловать на сервер! Теперь вводите команды(Help для вывода доступных команд).";
                    msg_size = msg.size();
                    send(newConnection,(char*)&msg_size,sizeof(int),0); // посылаем сначала размер сообщения
                    send(newConnection,msg.c_str(),msg_size,0); // затем само сообщение
                    while(true)
                    {
                        try
                        {
                            newMessage = Message_from_client(newConnection);
                            vec_com = Parse(newMessage); //распарсили строку команды
                            if (vec_com[0] == "NewData")
                            {
                                b1.getdata(myMap);
                            }
                            else if (vec_com[0] == "ShowData")
                            {
                                b1.showdata(myMap,vec_com[1]);
                            }
                            else if (vec_com[0] == "Help")
                            {
                                b1.showcom();
                            }
                            else if (vec_com[0] == "Exit")
                            {
                                std::cout << "Выход из приложения..." << std::endl;
                                for (auto iter : myMap)
                                {
                                    //записываем все как обычно (сервис + логин + шифр значение пароля)
                                    std::string shiphr_pass = aes_encrypt_ecb_hex(iter.second.second, (unsigned char*)"123456789ABCDEF", 16);
                                    std::string shiphr_log = aes_encrypt_ecb_hex(iter.second.first, (unsigned char*)"123456789ABCDEF", 16);
                                    std::string shiphr_serv = aes_encrypt_ecb_hex(iter.first, (unsigned char*)"123456789ABCDEF", 16);
                                    fout << shiphr_serv << "\t" << shiphr_log << "\t" << shiphr_pass <<"\n";
                                }
                                attempts = 0;
                                exit(0);
                            }
                            else if (vec_com[0] == "Delete")
                            {
                                b1.delete_user(myMap,vec_com[1]);
                            }
                            else if (vec_com[0] == "ChPass")
                            {
                                b1.chpass_user(myMap,vec_com[1]);
                            }
                            else if(vec_com[0] == "Find")
                            {
                                if(vec_com.size() == 1)
                                {
                                    b1.show_servises(myMap, "");
                                }
                                else
                                {
                                    b1.show_servises(myMap,vec_com[1]);
                                }
                            }
                        }
                        catch (const std::exception& ex)
                        {
                            std::cout << ex.what() << std::endl;
                            std::cout << "Сохранение данных: " << std::endl;
                        }
                    }
                }
            }
            else
            {
                std::cout << "Нет сообщения!" << std::endl;
            }
        } 
    }
	for (auto iter : myMap)
	{
		//записываем все как обычно (сервис + логин + шифр значение пароля)
		std::string shiphr_pass = aes_encrypt_ecb_hex(iter.second.second, (unsigned char*)"123456789ABCDEF", 16);
		std::string shiphr_log = aes_encrypt_ecb_hex(iter.second.first, (unsigned char*)"123456789ABCDEF", 16);
		std::string shiphr_serv = aes_encrypt_ecb_hex(iter.first, (unsigned char*)"123456789ABCDEF", 16);
		fout << shiphr_serv << "\t" << shiphr_log << "\t" << shiphr_pass <<"\n";
	}
	fout.close();
	fin.close();
	return 0;
}