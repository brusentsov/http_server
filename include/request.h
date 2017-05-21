#ifndef REQUEST_H
#define REQUEST_H

#include <iostream>

using namespace std;

class Request {
    string _command;
    string _URL;
    string _key;
    string _value;
public:
    Request();
    ~Request();
    bool parseMessage(string message);

    void setCommand(string command);
    void setURL(string URL);
    void setKey(string key);
    void setValue(string value);

    string getCommand(void);
    string getURL(void);
    string getKey(void);
    string getValue(void);
};

#endif // REQUEST_H