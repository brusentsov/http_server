#include "request.h"

using namespace std;

Request::Request() {
    _command = "";
    _URL = "";
    _key = "";
    _value = "";
}

Request::~Request(){}

bool Request::parseMessage(string message) {
    int command = message.find(" ");
    if (command != (int)string::npos && command > 2) {
        _command = message.substr(0, command);
    } else {
        return false;
    }
    int get = message.find("GET ");
    int http = message.find(" HTTP/1.");
    if (get != (int)string::npos && http != (int)string::npos) {
        _URL = message.substr(get + 4, http - get - 4);
        if (_URL == "/") {
            _value = _URL;
            return true;
        }
        if (_URL == "/favorites") {
            _value = _URL;
            return true;
        }

        int key1 = _URL.find("?");
        int key2 = _URL.find("=");
        // cout << "key1 : " << key1 << "//" << endl;
        // cout << "key2 : " << key2 << "//" << endl;
        //      favorites?name=Goveo

        if (key1 == (int)string::npos && key2 == (int)string::npos) {
            int value1 = _URL.find("/favorites/");
            int value2 = _URL.size();
            // cout << "key1 : " << key1 << "//" << endl;
            // cout << "key2 : " << key2 << "//" << endl;
            if (value1 != (int)string::npos && value2 != (int)string::npos) {
                _value = _URL.substr(value1 + 11, value2 - value1 - 1);
                cout << "_value : " << _value << "///" << endl;
            }
        }
        else {
            _value = _URL.substr(key2 + 1, _URL.size());
            _key = _URL.substr(key1 + 1, key2 - key1 - 1);
            if (key2 == -1) {
                return false;
            }
        } 
    }
    else return false;
    return true;
}

void Request::setCommand(string command) {
    _command = command;
}

void Request::setURL(string URL) {
    _URL = URL;
}

void Request::setKey(string key) {
    _key = key;
}

void Request::setValue(string value) {
    _value = value;
}

string Request::getCommand() {
    return _command;
}

string Request::getURL() {
    return _URL;
}

string Request::getValue() {
    return _value;
}

string Request::getKey() {
    return _key;
}