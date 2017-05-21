#include <progbase-cpp/net.h>
#include <iostream>
#include <ctime>
#include <fstream>
#include <climits>
#include <cmath>
#include "http_server.h"
#include "request.h"
#include "character.h"
#include <jansson.h>

using namespace progbase::net;
using namespace std;

const string filePath = "/home/brusentcov/projects/lab8/data/data.txt";

void http_server(void) {
    vector<Character*> characters;
    fillCharacters(characters);
    const int serverPort = 8080;
    TcpListener tcpListener;
    NetMessage message(2048);
    try {
        tcpListener.bind(IpAddress("127.0.0.1", serverPort));
        tcpListener.start();
        while (true) {
            cout << "Listening at " << serverPort << "..." << endl;
            TcpClient * tcpClient = tcpListener.accept();
            tcpClient->receive(message);
            cout << "Received: " << endl << message.dataAsString() << endl;
            string data = message.dataAsString();
            string response = createResponse(data, characters);
            message.setDataString(response);
            tcpClient->send(message);
            delete tcpClient;
        }
    } catch(NetException const & exception) {
        cerr << exception.what() << endl;
    }

    for (int i = 0; i < (int)characters.size(); i++) {
        delete characters[i];
    }
}

string createResponse(string message, vector<Character *> characters) {
    Request request;
    string msg;
    string response =
            "HTTP/1.1 400 Bad Request\r\n"
            "Server: nginx/1.11.5\r\n"
            "Connection: close\r\n\r\n";
    if (request.parseMessage(message)) {
        bool failRequest = false;
        std::cout << "\nurl :" << request.getURL() << endl;
        if (request.getURL() == "/") {
            msg = serverInformation();
        }
        else if (request.getURL() == "/favorites") {
            msg = myFavouriteCharacters(characters);
        } 
        else if (request.getURL().find("/favorites/") != string::npos) {
            msg = keyCharacters(characters, "id", request.getValue());
        }
        else if (request.getURL().find("/favorites?") != string::npos) {
            msg = keyCharacters(characters, request.getKey(), request.getValue());
        }
        else if (request.getURL() == "/file") {
            msg = fileInformation();
        }
        else if (request.getURL() == "/file/data") {
            msg = fileNumbers();
        }
        else {
            failRequest = true;
        }
        if (msg == "No such characters") {
           response =
                "HTTP/1.1 404 Not Found\r\n"
                "Server: nginx/1.11.5\r\n"
                "Connection: close\r\n\r\n";
        } else if (!failRequest) {
            response =
                "HTTP/1.1 200 OK\r\n"
                "Server: nginx/1.11.5\r\n"
                "Content-Type: application/json; charset=utf-8\r\n"
                "Content-Length: " + to_string(msg.size()) + "\r\n"
                "Connection: close\r\n\r\n" + msg;
       }
    }
    return response;
}

string serverInformation(void) {
    json_t * json = json_object();
    json_object_set_new(json, "title", json_string("WebServer"));
    json_object_set_new(json, "developer",json_string("Yury Brusentsov"));
    time_t seconds = time(NULL);
    tm* timeinfo = localtime(&seconds);
    json_object_set_new(json, "time", json_string(asctime(timeinfo)));
    char * jsonString = json_dumps(json, JSON_INDENT(2) | JSON_PRESERVE_ORDER);
    string toReturn(jsonString);
    free(jsonString);
    json_decref(json);
    return toReturn;
}


string myFavouriteCharacters(vector<Character*> characters) {
    json_t * array = json_array();
    for (int i = 0; i < (int)characters.size(); i++) {
        json_t * json = json_object();
        json_object_set_new(json, "name", json_string(characters[i]->getName().c_str()));
        json_object_set_new(json, "surname", json_string(characters[i]->getSurname().c_str()));
        json_object_set_new(json, "age", json_integer(characters[i]->getAge()));
        json_object_set_new(json, "id", json_integer(characters[i]->getId()));
        json_array_append(array, json);
        json_decref(json);
    }
    char * jsonString = json_dumps(array, JSON_INDENT(2) | JSON_PRESERVE_ORDER);
    string toReturn(jsonString);
    free(jsonString);
    json_decref(array);
    return toReturn;
}

string keyCharacters(std::vector<Character *> characters, std::string key, std::string value) {
    json_t * array = json_array();
    bool check = false;
    for (int i = 0; i < (int)characters.size(); i++) {
        if (characters[i]->getField(key) == value) {
            check = true;
            json_t * json = json_object();
            json_object_set_new(json, "Name", json_string(characters[i]->getName().c_str()));
            json_object_set_new(json, "surname", json_string(characters[i]->getSurname().c_str()));
            json_object_set_new(json, "age", json_real(characters[i]->getAge()));
            json_object_set_new(json, "id", json_integer(characters[i]->getId()));
            json_array_append(array, json);
            json_decref(json);
        }
    }
    if (!check) return "No such characters";
    char * jsonString = json_dumps(array, JSON_INDENT(2) | JSON_PRESERVE_ORDER);
    string toReturn(jsonString);
    free(jsonString);
//    json_decref(array);
    return toReturn;
}

string fileInformation() {
    string fileContent = readFile(filePath.c_str());
    if (fileContent == "Can't open file") {
        cerr << "Can't open file" << endl;
        return "Can't open file";
    }
    json_t * json = json_object();
    json_object_set_new(json, "filename", json_string( "data.txt"));
    json_object_set_new(json, "file size(byte)",json_integer(fileContent.size()));
    json_object_set_new(json, "file content",  json_string(fileContent.c_str()));
    char * jsonString = json_dumps(json, JSON_INDENT(2) | JSON_PRESERVE_ORDER);
    string toReturn(jsonString);
    free(jsonString);
    json_decref(json);
    return toReturn;
}

string fileNumbers() {
    vector<double> numbers;
    string str = readFile(filePath.c_str());
    double min = INT_MAX, count = 0;
    double num = min;
    int index = str.find_first_of("1234567890");
    while (index != (int)string::npos)
    {
        bool dot = false;
        bool m = false;
        string buffer;
        while (isdigit(str[index]) || (str[index] == '.' && !dot) || (str[index] == '-' && !m)) {
            if (str[index] == '.' && !dot) dot = true;
            if (str[index] == '-' && !m) m = true;
            buffer += str[index];
            index++;
        }
        if (buffer.find_first_of("1234567890") != string::npos) num = stod(buffer);
        
        numbers.push_back(num);

        count++;
        index = str.find_first_of("-.1234567890",index+1);
    }
    json_t * array = json_array();
    int numberCount = 0;
    for (int i = 0; i < (int)numbers.size(); i++) {
        numberCount++;
        if (numberCount % 2 == 1) {
            json_t * json = json_object();
            json_object_set_new(json, "number", json_integer(numbers[i]));
            // json_object_set_new(json, "number", json_real(numbers[i]));
            json_object_set_new(json, "count",json_integer(numberCount));
            json_array_append(array, json);
            json_decref(json);
        }
    }
    if (numberCount == 0) return "File dont have numbers";
    char * jsonString = json_dumps(array, JSON_INDENT(2) | JSON_PRESERVE_ORDER);
    string toReturn(jsonString);
    free(jsonString);
//    json_decref(array);
    return toReturn;
}

string readFile(const char * filePath) {
    string line;
    string toReturn;
    ifstream myfile (filePath);
    if (!myfile.is_open()){
        return "Can't open file";
    }
    while (! myfile.eof() ){
        getline (myfile,line);
        toReturn += line;
    }
    myfile.close();
    return toReturn;
}