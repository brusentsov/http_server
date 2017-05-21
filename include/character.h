#ifndef CHARACTER_H
#define CHARACTER_H

#include <iostream>
#include <vector>

using namespace std;

class Character
{
    string _name;
    string _surname;
    int _age;
    int _id;
public:
    Character();
    Character(string name, string surname, int age, int id);
    ~Character();

    void setName(string name);
    void setSurname(string surname);
    void setAge(int age);
    void setId(int id);

    string getName(void);
    string getSurname(void);
    float getAge(void);
    int getId(void);
    string getField(string field);
};

void fillCharacters(vector<Character*> &characters);

#endif // CHARACTER_H