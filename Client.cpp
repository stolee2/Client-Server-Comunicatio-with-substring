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

bool isValidMessage(const string& message) {
    // Прверка дали во пораката има '/' симбол
    size_t slashPos = message.find('/');
    if (slashPos == string::npos) {
        return false;
    }   

    // Издвојување на деловите од пораката
    string originalString = message.substr(0, slashPos); // Ги земаат деловите од низата message пред и по првото појавување на '/' соодветно и ги зачувуваат во променливите originalString и remainingString.
    string remainingString = message.substr(slashPos + 1);
    size_t secondSlashPos = remainingString.find('/');
    if (secondSlashPos == string::npos) {
        return false;
    }

    string subString = remainingString.substr(0, secondSlashPos);
    string otherOriginalString = remainingString.substr(secondSlashPos + 1);

    // Проверка дали стринговите се составени само од броеви
    bool hasLetters = false;
    for (char c : originalString) {
        if (isalpha(c)) {
            hasLetters = true;
            break;
        }
    }

    if (!hasLetters) {
        return false;
    }

    hasLetters = false;
    for (char c : subString) {
        if (isalpha(c)) {
            hasLetters = true;
            break;
        }
    }

    if (!hasLetters) {
        return false;
    }

    hasLetters = false;
    for (char c : otherOriginalString) {
        if (isalpha(c)) {
            hasLetters = true;
            break;
        }
    }

    if (!hasLetters) {
        return false;
    }

    return true;
}

int main(int argc, char* argv[]) {
    // Проверка на валиден број на аргументи
    if (argc != 3) {
        cerr << "Greska vo parsiranje na argumenti." << endl;
        cerr << "Usage: " << argv[0] << " <server_ip_address> <server_port>" << endl;
        exit(0);
    }

    char msg[300];

    sockaddr_in servAddr;
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &(servAddr.sin_addr));

    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (clientSocket < 0) {
        cerr << "Soketot ne moze da se kreira" << endl;
        exit(0);
    }

    // Конектирање на клиентот со серверот
    int connectStatus = connect(clientSocket, (struct sockaddr*)&servAddr, sizeof(servAddr));

    if (connectStatus < 0) {
        cerr << "Greska pri konekcija so serverot" << endl;
        exit(0);
    }

    cout << "Uspesno vospostavena konekcija so serverot" << endl;

    while (1) {
        string data;
        cout << "Vnesete poraka: ";
        getline(cin, data);

        if (data == "kraj") {
            send(clientSocket, data.c_str(), strlen(data.c_str()), 0);
            cout << "Kraj na sesija" << endl;
            break;
        } else if (!isValidMessage(data)) {
            cout << "Ve molime vnesete nova poraka" << endl;
            continue;
        } else {
            send(clientSocket, data.c_str(), strlen(data.c_str()), 0);

            memset(&msg, 0, sizeof(msg));
            recv(clientSocket, (char*)&msg, sizeof(msg), 0); //Примена порака од Server-от
            cout << "Poraka od serverot: " << msg << endl;
        }
    }

    close(clientSocket);
    return 0;
}