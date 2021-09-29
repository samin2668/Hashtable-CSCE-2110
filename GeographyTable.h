//
// Created by Andrew on 11/21/2019.
//
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <cmath>
#include <regex>

using namespace std;

class GeographyTable
{
private:
    struct Node* head = NULL;
    vector<struct geogTuple> primaryIndex;
public:
    //Constructor and Destructor
    GeographyTable() = default;
    ~GeographyTable();

    //Secondary Functions
    void generateLinkedList();
    vector<geogTuple> traverseLinkedList(string*);
    string* parseQuery(string);
    geogTuple search(int);
    int midSquareHash(int);
    bool insertToPrimaryIndex(int,geogTuple);

    //Primary Functions
    void loadTable(vector<string>);
    void select(string);
    void display();
    void insert(string);
    void update(string);
    void deleteItem(string);
    void write(string);

};

//Struct used to store the individual values given by the tuple
struct geogTuple {
    int replan_id;
    string geo_name;
    string geo_stusab;
    int geo_sumlev;
    int geo_state;
    int geo_county;
    string geo_geoid;
    int population;
    string* allValues;
};

//Struct used as node within the linked list
struct Node {
    geogTuple *data;
    struct Node *next;
};




