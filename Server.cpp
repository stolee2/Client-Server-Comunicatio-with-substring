#include <iostream>
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
    // при извршување на серверот, од командна линија се проследува бројот на пората
    // argv[0]->ime na file, argv[1] port number
    if(argc != 2)
    {
        cerr << "Greska vo parsiranje na argumenti." << endl;
        exit(0);
    }
    // Проверка на бројот на аргументи од командната линија

    char msg[300]; //msg серверски бафер за испраќање/прием на пораки

    sockaddr_in serverAddr, clientAddr;
    serverAddr.sin_family = AF_INET; // за TCP и UDP адресната фамила е AF_INET (го сетираме полето sin_family)
    serverAddr.sin_port = htons(atoi(argv[1]));// Сетирање на број на порта преку терминал (htons конвертира unsigned 16 b int од host byte order во network byte order)
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //Сетирање на адреса Wildcard (htonl конвертира unsigned 32 b int од host byte order во network byte order)

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);// Креирање на серверски сокет

    if (serverSocket < 0) // ако функцијата врати <0, сокетот на може да се крира
    {
        cerr << "Soketot ne moze da se kreira" << endl;
        exit(0);
    }

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        cerr << "Serverskiot soket ne moze da se vrze so adresata." << endl;
        exit(0);
    }   // Врзување на серверскиот сокет на специфицираната адреса и порта

    if (listen(serverSocket, 3) < 0)  // Слушање за влезни конекции
    {
        cerr << "Greska pri listen" << endl;
        exit(0);
    }

    cout << "Ocekuvam baranja za ostvaruvanje na konekcija od strana na klientite" << endl;

    int clientAddrSize = sizeof(clientAddr);
    int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, (socklen_t *)&clientAddrSize);

    if (clientSocket < 0)
    {
        cerr << "Nastana greska pri prifakanje na konekcija. Obidete se povtorno!" << endl;
        exit(0);
    }

    while (1)
    {
        memset(&msg, 0, sizeof(msg));
        recv(clientSocket, (char *)&msg, sizeof(msg), 0);   // Примање на порака од клиентот
        cout << "Primena poraka: " << msg << endl;

        string data(msg);

        if (data == "kraj") //Доколку примената порака од клиентот е kraj тука завршува сесијата
        {
            cout << "Kraj na sesija" << endl;
            break;
        }

        size_t slashPos = data.find('/'); //Го наоѓа индексот на првото појавување на знакот '/' во низата data и го зачувува индексот во променливата slashPos
        string originalString = data.substr(0, slashPos); //Го зема делот од низата data од почетокот до индексот slashPos и го зачувува резултатот во новата низа originalString.
        string remainingString = data.substr(slashPos + 1); //Го зема делот од низата data од индексот slashPos + 1 до крајот и го зачувува резултатот во новата низа remainingString.
        size_t secondSlashPos = remainingString.find('/'); //Го наоѓа индексот на првото појавување на знакот '/' во низата remainingString и го зачувува индексот во променливата secondSlashPos.
        string subString = remainingString.substr(0, secondSlashPos); //Го зема делот од низата remainingString од почетокот до индексот secondSlashPos и го зачувува резултатот во новата низа subString.
        string otherOriginalString = remainingString.substr(secondSlashPos + 1); //Го зема делот од низата remainingString од индексот secondSlashPos + 1 до крајот и го зачувува резултатот во новата низа otherOriginalString.

        if (originalString.find(subString) != string::npos && otherOriginalString.find(subString) != string::npos)
        {
            string response = "Podstring2";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            // Праќање на одговор до клиентот
        }
        else if (originalString.find(subString) != string::npos)
        // Проверка дали subString се наоѓа само во originalString
        {
            string response = "PodstringOriginalen";
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            // Праќање на одговор до клиентот
        }
        else if (otherOriginalString.find(subString) != string::npos)
        // Проверка дали subString се наоѓа само во otherOriginalString
        {
            string response = "PodstringDrug";
            
            send(clientSocket, response.c_str(), strlen(response.c_str()), 0);
            // Праќање на одговор до клиентот
        }
    }
   
    close(clientSocket);
    close(serverSocket);
    // Затворање на клиентскиот и серверскиот сокет
    return 0;
}