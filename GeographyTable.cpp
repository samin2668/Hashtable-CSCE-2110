//
// Created by Andrew on 11/21/2019.
//
#include "GeographyTable.h"

//Deconstructor for clearing the linked list and primary index
GeographyTable::~GeographyTable() {
    delete head;
    primaryIndex.clear();
}

/*Using a vector of strings from the input.cpp file, this function parses each string and stores the necessary values into a tuple
*"Fake" tuples are first inserted into the primary index to allocate space before performing a hash function
* Once that's done, each entry goes through the insertToPrimaryIndexFunction()
*/
void GeographyTable::loadTable(vector<string> strings)
{
    vector<geogTuple> preStore;
    for(int i = 0; i < strings.size(); i++)
    {
        string* matcher = parseQuery(strings.at(i));
        geogTuple newTuple;
        geogTuple fakeTuple = geogTuple();
        newTuple.replan_id = stoi(matcher[1]);
        newTuple.geo_name = matcher[2];
        newTuple.geo_stusab = matcher[3];
        newTuple.geo_sumlev = stoi(matcher[4]);
        newTuple.geo_state = stoi(matcher[5]);
        newTuple.geo_county = stoi(matcher[6]);
        newTuple.geo_geoid = matcher[7];
        newTuple.population = stoi(matcher[8]);
        newTuple.allValues = matcher;
        primaryIndex.push_back(fakeTuple);
        preStore.push_back(newTuple);
        cout << "Inserted (" << matcher[0] << ") into geography" << endl;
    }
    for(auto it: preStore) {
        insertToPrimaryIndex(it.replan_id, it);
    }
    generateLinkedList();

}

//PRIMARY FUNCTIONS CALLED BY INPUT USING QUERIES

//Find entries based on a query. Will use Search() if key value in query is not *, otherwise, it will search the linked list for matches
void GeographyTable::select(string query)
{
    string* parsed = parseQuery(query);
    vector<geogTuple> found;

    if(parsed[1] == "*")
    {
        found = traverseLinkedList(parsed);
        if(found.size() != 0)
        {
            cout << "Found entries:" << endl;
            for(auto it : found)
            {
                cout << "(" << it.replan_id << ", \"" << it.geo_name << "\", " << it.geo_stusab << ", " << it.geo_sumlev << ", " << it.geo_state << ", " << it.geo_county << ", " << it.geo_geoid << ", " <<it.population << ") in geography" << endl;
            }
        }
        else
        cout << "No entries match query: " << query << endl;

    }
    else
    {
        geogTuple foundTuple = search(stoi(parsed[1]));
        cout << "Found (" << foundTuple.replan_id << ", \"" << foundTuple.geo_name << "\", " << foundTuple.geo_stusab << ", " << foundTuple.geo_sumlev << ", " << foundTuple.geo_state << ", " << foundTuple.geo_county << ", " << foundTuple.geo_geoid << ", " <<foundTuple.population << ") in geography" << endl;
    }
}

//Display all current values stored within the primary index
void GeographyTable::display()
{
    cout << "replan_id " <<  "geo_name " << "geo_stusab " <<  "geo_sumlev " <<  "geo_state " << "geo_county " <<"geo_geoid " <<"population" << endl;
    for(auto it: primaryIndex)
    {
        cout << it.replan_id << " " << it.geo_name << " " << it.geo_stusab << " " << it.geo_sumlev << " " << it.geo_state << " " << it.geo_county << " " << it.geo_geoid << " " <<it.population << endl;

    }

}

//Insert a new item into the primary index, then regenerate the linked list
void GeographyTable::insert(string query)
{
    geogTuple newTuple;
    geogTuple fakeTuple;
    fakeTuple.replan_id = 0;
    string* parsed = parseQuery(query);
    geogTuple checkExistence = search(stoi(parsed[1]));
    if(checkExistence.replan_id == 0)
    {
        primaryIndex.push_back(fakeTuple);
        newTuple.replan_id = stoi(parsed[1]);
        newTuple.geo_name = parsed[2];
        newTuple.geo_stusab = parsed[3];
        newTuple.geo_sumlev = stoi(parsed[4]);
        newTuple.geo_state = stoi(parsed[5]);
        newTuple.geo_county = stoi(parsed[6]);
        newTuple.geo_geoid = parsed[7];
        newTuple.population = stoi(parsed[8]);
        newTuple.allValues = parsed;

        if(insertToPrimaryIndex(newTuple.replan_id,newTuple))
        {
            delete head;
            cout << "Inserted " << query << " into geography" << endl;
            generateLinkedList();
            traverseLinkedList(parsed);
        }
    }
    else
    {
        cout << "Failed to insert " << query << " into geography" << endl;
    }
}

//Delete entries based on a query. Will use Search() if key value in query is not *, otherwise, it will search the linked list for matches
void GeographyTable::deleteItem(string query) {

    string* parsed = parseQuery(query);
    bool deleted = false;
    vector<geogTuple> found;

    if(parsed[1] == "*")
    {
        found = traverseLinkedList(parsed);
        if(found.size() != 0) {
            cout << "Deleted entries:" << endl;
            for (int i = 0; i < primaryIndex.size(); i++) {
                for (auto it : found) {
                    if (primaryIndex.at(i).replan_id == it.replan_id) {
                        primaryIndex.erase(primaryIndex.begin() + i);
                        cout << "(" << it.replan_id << ", \"" << it.geo_name << "\", " << it.geo_stusab << ", "
                             << it.geo_sumlev << ", " << it.geo_state << ", " << it.geo_county << ", " << it.geo_geoid
                             << ", " << it.population << ") in geography" << endl;
                        deleted = true;
                        break;
                    }

                }

            }
            generateLinkedList();
        }
        else
        {
            cout << "No entries match query: " << query << endl;
        }

    }
    else
    {
        //delete a single item
        for(int i = 0; i < primaryIndex.size();i++)
        {
            if(stoi(parsed[1]) == primaryIndex.at(i).replan_id)
            {
                primaryIndex.erase(primaryIndex.begin()+i);
                generateLinkedList();
                cout << "Deleted " << query << " from geography" << endl;
                deleted = true;
                break;
            }
        }
    }
    if(!deleted)
    {
        cout << "Failed to delete " << query << " from geography" << endl;
    }


}

//Update a value within the primary index
void GeographyTable::update(string query) {
    geogTuple updateTuple;
    string* parsed = parseQuery(query);
    geogTuple checkExistence = search(stoi(parsed[1]));
    if(checkExistence.replan_id != 0)
    {
        updateTuple.replan_id = stoi(parsed[1]);
        updateTuple.geo_name = parsed[2];
        updateTuple.geo_stusab = parsed[3];
        updateTuple.geo_sumlev = stoi(parsed[4]);
        updateTuple.geo_state = stoi(parsed[5]);
        updateTuple.geo_county = stoi(parsed[6]);
        updateTuple.geo_geoid = parsed[7];
        updateTuple.population = stoi(parsed[8]);
        updateTuple.allValues = parsed;


        for(int i = 0; i < primaryIndex.size(); i++)
        {
            if(primaryIndex.at(i).replan_id == checkExistence.replan_id)
            {
                primaryIndex.at(i) = updateTuple;
                cout << "Updated " << query << " in geography" << endl;
            }
        }

    }
    else
    {
        cout << "Failed to update " << query << " in geography" << endl;
    }
}

//Write to a new file based on the file name used to read in
void GeographyTable::write(string oldFileName)
{
    string newFileName = "out_" + oldFileName;
    ofstream output;
    output.open(newFileName);
    output << "replan_id\n";
    output << "replan_id,geo_name,geo_stusab,geo_sumlev,geo_state,geo_county,geo_geoid,population\n";
    for(auto it: primaryIndex)
    {
        output << it.replan_id << ",\"" << it.geo_name << "\"," << it.geo_stusab << ","
             << it.geo_sumlev << "," << it.geo_state << "," << it.geo_county << "," << it.geo_geoid
             << "," << it.population << "\n";
    }
    output.close();
}

//SECONDARY FUNCTIONS FOR USE INSIDE OF THE PRIMARY FUNCTIONS

//Perform Mid-Square Hashing on an integer key
int GeographyTable::midSquareHash(int key)
{
    int R = ceil(log(primaryIndex.size()));
    int pos = pow(key,2);
    string spos = to_string(pos);
    string mpos = spos.substr(floor(spos.size()/6),R);
    return stoi(mpos) % primaryIndex.size();
}

//Using a specified key and value, insert the value into the primary index. Uses Quadratic Probing for Collisions (c1 = 5, c2 = 2)
//Returns True if the value was inserted successfully
bool GeographyTable::insertToPrimaryIndex(int key,geogTuple value)
{
    int pos = midSquareHash(key);
    int probed = 0;
    while(probed < primaryIndex.size())
    {
        if(pos < primaryIndex.size()) {
            if (primaryIndex.at(pos).replan_id == 0) {
                primaryIndex.at(pos) = value;
                return true;
            }
        }
        probed++;
        pos = (midSquareHash(key)+(5*probed)+(2*probed^2)) % primaryIndex.size();
    }
    return false;
}

//Returns a value from the primary index based on a hashed key value. Uses Quadratic Probing for Collisions(c1 = 5, c2 = 2)
//Returns an empty tuple if the requested key is not found
geogTuple GeographyTable::search(int key)
{
    geogTuple fakeReturn = geogTuple();
    int pos = midSquareHash(key);
    int probed = 0;
    while(probed < primaryIndex.size())
    {
        if(pos < primaryIndex.size()) {
            if (primaryIndex.at(pos).replan_id == key) {
                return primaryIndex.at(pos);
            }
        }
        probed++;
        pos = (midSquareHash(key)+(5*probed)+(2*probed^2)) % primaryIndex.size();
    }
    return fakeReturn;
}

//Generates a linked list of pointers to Tuples stored in the primary index
void GeographyTable::generateLinkedList()
{
    head = NULL;
    for(int i = 0; i < primaryIndex.size(); i++)
    {
        struct Node* new_node = (struct Node*) malloc(sizeof(struct Node));
        new_node->data = &primaryIndex.at(i);
        new_node->next = head;
        head = new_node;
    }
}

/*
 * Traversal of the linked list is used when a query has a * in the key value.
 * Checks[8] takes the parsed values and makes and array of bools that matches parsed such that * = false and [value] = true
 * Matched[8] is set to all false, then is checked against the current tuple being looked at in the linked list.
 * If a value within the linked list matches the specified values in the query, matched is then updated such that correct values are marked as true
 * Then, if Matched is exactly the same as Checks, the value from the linked list is stored in a vector
 * All found values are then returned
 */
vector<geogTuple> GeographyTable::traverseLinkedList(string* parsed)
{
    geogTuple searchTuple;
    vector<geogTuple> ret;
    bool checks[8];
    bool matched[8];
    bool fail = false;
    for(int i = 1; i < 9; i++)
    {
        checks[i - 1] = parsed[i] != "*";
    }
    struct Node* ptr;
    ptr = head;
    while (ptr != NULL) {
        fail = false;
        for(int i = 0; i < 8; i++)
        {
            matched[i] = false;
        }
        for(int i = 1; i < 9; i++)
        {
            if(checks[i-1])
            {
                if(parsed[i] == ptr->data->allValues[i])
                {
                    matched[i-1] = true;
                }
            }
        }
        for(int i = 0; i < 8; i++)
        {
            if(checks[i] == matched[i])
                continue;

            fail = true;
            break;
        }
        if(!fail)
        {
            searchTuple.replan_id = ptr->data->replan_id;
            searchTuple.geo_name = ptr->data->geo_name;
            searchTuple.geo_stusab = ptr->data->geo_stusab;
            searchTuple.geo_sumlev = ptr->data->geo_sumlev;
            searchTuple.geo_state = ptr->data->geo_state;
            searchTuple.geo_geoid = ptr->data->geo_geoid;
            searchTuple.geo_county = ptr->data->geo_county;
            searchTuple.population = ptr->data->population;
            searchTuple.allValues = ptr->data->allValues;

            ret.push_back(searchTuple);

        }

        ptr = ptr->next;
    }
    cout << endl;
    return ret;
}

//Parse any query given using the regex pattern shown below
string* GeographyTable::parseQuery(string query)
{
    smatch matcher;
    regex dataPattern("[\\(]?([0-9]+|[*]),\"?(.+[^\"]|[*])\"?,(.+),(.+),(.+),(.+),(.+),(.+[^\\)]|[*])[\\)]?");
    regex_search(query,matcher,dataPattern);
    string *ret = new string[matcher.size()];
    for(int i = 0; i < matcher.size(); i++)
    {
        ret[i] = matcher[i].str();
    }

    return ret;
}







