#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <regex>
#include <iomanip>
#define SIZE 10
using namespace std;

//class used to hold age data
class AgeData
{
    private:
        string key;
        int under_5;
        int under_18;
        int over_65;
    public:
        AgeData();
        AgeData(string k, int under1, int under2, int over);
        ~AgeData(){}
        void setKey(string k) {key = k;}
        void setUnder_5(int k) {under_5 = k;}
        void setUnder_18(int k) {under_18 = k;}
        void setOver_65(int k) {over_65 = k;}

        string getKey() {return key;}
        int getUnder_5() {return under_5;}
        int getUnder_18() {return under_18;}
        int getOver_65() { return over_65;}
        string getTuple();
};
//struct used to construct table and to implement chaining for collision 
typedef struct HashTable
{
    AgeData data;
    HashTable *next;
    HashTable *previous;
}HashTable;
//struct used to contruct secondary index 
typedef struct List
{
    AgeData *data;
    List *next;
    List *prev;
}List;
//functions for primary index
int ModuloHash(string id);
void InsertToTable(string line);
HashTable* searchTable(string key);
void deleteAGE(string line);
void LoadAgeTable(vector <string> data);
void displayAGE();
void writeAGE();
void selectAGE(string line);
void updateAGE(string line);

//functions for secondary index
void appendList(AgeData *data);
void deleteList(string key);
void printList();
//functions for freeing memory
void freeTable();
void freeList();
