#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <iostream>
#include <vector>
#include "character.h"

using namespace std;

void http_server(void);
string createResponse(string message, vector<Character *> characters);
string serverInformation(void);
string myFavouriteCharacters(vector<Character*> characters);
string keyCharacters(vector<Character*> characters, string key, string value);
string fileInformation(void);
string fileNumbers(void);
string readFile(const char * filePath);

#endif // HTTP_SERVER_H