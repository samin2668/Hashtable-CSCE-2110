#include "AgeTable.h"
//table to hold data
HashTable *AgeArr[SIZE];
List *head;

//default constructor
AgeData::AgeData()
{
    key = "EMPTY";
    under_5 = 0;
    under_18 = 0;
    over_65 = 0;
}
//specified contructor
AgeData::AgeData(string k, int under1, int under2, int over)
{
    key = k;
    under_5 = under1;
    under_18 = under2;
    over_65 = over;
}
//get tuple as string
string AgeData::getTuple()
{
    return(""+ key +","+ to_string(under_5) +","+ to_string(under_18) +","+ to_string(over_65) +"");
}

//Modulo hashing function, return position as int
int ModuloHash(string id)
{
    int sum = 0;
    for(int i = 0; i < id.length(); i++)
    {
        sum += id[i];
    }

    return(sum % SIZE);

}
//function to insert to table
void InsertToTable(string line)
{
    //regex to parse data
    smatch matcher;
    regex pattern("(.+),(.+),(.+),(.+)");
    regex_search(line, matcher, pattern);
    AgeData temp(matcher[1].str(), stoi(matcher[2].str()), stoi(matcher[3].str()), stoi(matcher[4].str()));
    //if key already exists
    if(searchTable(temp.getKey()) != NULL)
    {
        cout << "Failed to insert ("+ line +") into age\n";
        return;
    }
    int pos = ModuloHash(temp.getKey());
    if(AgeArr[pos] == NULL)
    {
        AgeArr[pos] = new HashTable;
        AgeArr[pos]->data = temp;
        AgeArr[pos]->next = NULL;
        AgeArr[pos]->previous = NULL;
        AgeData *DATA = &AgeArr[pos]->data;
        appendList(DATA);
        cout << "Inserted ("+ line +") into age\n";
    }
    //chaining
    else
    {
        HashTable *ptr = AgeArr[pos];
        HashTable *tmp = new HashTable;
        tmp->data = temp;
        tmp->next = NULL;
        //traversing to the last node
        while(ptr->next != NULL)
        {
            ptr = ptr->next;
        }
        tmp->previous = ptr;
        ptr->next = tmp;
        AgeData *DATA = &tmp->data;
        appendList(DATA);
        cout << "Inserted ("+ line +") into age\n";
    }
}
//search function
HashTable* searchTable(string key)
{
    int pos = ModuloHash(key);
    if(AgeArr[pos] != NULL)
    {
        //checking array
        if(AgeArr[pos]->data.getKey() == key)
        {
            return AgeArr[pos];
        }
        //traversing chain 
        else
        {
            HashTable *ptr = AgeArr[pos]->next;
            if(ptr != NULL)
            {
                while(ptr != NULL)
                {
                    if(ptr->data.getKey() == key)
                    {
                        return ptr;
                    }
                    else
                    {
                        ptr = ptr->next;
                    }
                }
            }
        }
    }
    return NULL;
}
//delete function
void deleteAGE(string line)
{
    //regex to parse data
    smatch matcher;
    regex pattern("(.+),(.+),(.+),(.+)");
    regex_search(line, matcher, pattern);
    string key = matcher[1].str();
    //if all values are "*"
    if(matcher[1].str() == "*"
    && matcher[2].str() == "*"
    && matcher[3].str() == "*"
    && matcher[4].str() == "*")
    {
        freeList();
        freeTable();
    }
    //if key is provided
    if(key != "*")
    {
        if(matcher[2].str() != "*"
        && matcher[3].str() != "*"
        && matcher[4].str() != "*")
        {
            HashTable *ptr = searchTable(key);
            int x = stoi(matcher[2].str());
            int y = stoi(matcher[3].str());
            int z = stoi(matcher[4].str());
            if(ptr != NULL)
            {
                deleteList(ptr->data.getKey());
                if(ptr->data.getUnder_5() == x
                && ptr->data.getUnder_18() == y
                && ptr->data.getOver_65() == z)
                {
                    deleteList(ptr->data.getKey());
                    if(ptr->next == NULL && ptr->previous != NULL)
                    {
                        ptr->previous->next = NULL;
                    }
                    else if(ptr->previous == NULL)
                    {
                        int pos = ModuloHash(key);
                        AgeArr[pos] = ptr->next;
                    }
                    else
                    {
                        ptr->previous->next = ptr->next;
                    }
                    cout << "Deleted ("+ line +") in age\n";
                    return;
                }
                else
                {
                    cout << "Failed to delete ("+ line +") in age\n";
                    return;
                }
            }
            else
            {
                cout << "Failed to delete ("+ line +") in age\n";
                return;
            }
            
        }
        else
        {
            HashTable *ptr = searchTable(key);
            if(ptr != NULL)
            {
                deleteList(ptr->data.getKey());
                if(ptr->next == NULL && ptr->previous != NULL)
                {
                    ptr->previous->next = NULL;
                }
                else if(ptr->previous == NULL)
                {
                    int pos = ModuloHash(key);
                    AgeArr[pos] = ptr->next;
                }
                else
                {
                    ptr->previous->next = ptr->next;
                }
                cout << "Deleted ("+ line +") in age\n";
                return;
            }
            else
            {
                cout << "Failed to delete ("+ line +") in age\n";
                return;
            }
        }
    }
    //if key us not provided use linked list
    else
    {
        List *ptr = head;
        if(matcher[2].str() != "*")
        {
            vector <List*> matches;
            int x = stoi(matcher[2].str());
            while(ptr != NULL)
            {
                if(ptr->data->getUnder_5() == x)
                    matches.emplace_back(ptr);
                ptr = ptr->next;
            }
            if(!matches.empty())
            {
                cout << "Deleted entries:\n";
                for(auto it : matches)
                {
                    deleteList(it->data->getKey());
                    cout << "("+ it->data->getTuple() +") in age\n";
                    HashTable *del = searchTable(it->data->getKey());
                    if(del->next == NULL && del->previous != NULL)
                    {
                        del->previous->next = NULL;
                    }
                    else if(del->previous == NULL)
                    {
                        int pos = ModuloHash(del->data.getKey());
                        AgeArr[pos] = del->next;
                    }
                    else
                    {
                        del->previous->next = del->next;
                    }
                }
            }
            else
            {
                cout << "No entries match query ("+ line +") in age\n";
            }
        }
        else if(matcher[3].str() != "*")
        {
            vector <List*> matches;
            int y = stoi(matcher[3].str());
            while(ptr != NULL)
            {
                if(ptr->data->getUnder_18() == y)
                    matches.emplace_back(ptr);
                ptr = ptr->next;
            }
            if(!matches.empty())
            {
                cout << "Deleted entries:\n";
                for(auto it : matches)
                {
                    deleteList(it->data->getKey());
                    cout << "("+ it->data->getTuple() +") in age\n";
                    HashTable *del = searchTable(it->data->getKey());
                    if(del->next == NULL && del->previous != NULL)
                    {
                        del->previous->next = NULL;
                    }
                    else if(del->previous == NULL)
                    {
                        int pos = ModuloHash(del->data.getKey());
                        AgeArr[pos] = del->next;
                    }
                    else
                    {
                        del->previous->next = del->next;
                    }
                }
            }
            else
            {
                cout << "No entries match query ("+ line +") in age\n";
            }
        }
        else if(matcher[4].str() != "*")
        {
            vector <List*> matches;
            int z = stoi(matcher[4].str());
            while(ptr != NULL)
            {
                if(ptr->data->getOver_65() == z)
                    matches.emplace_back(ptr);
                ptr = ptr->next;
            }
            if(!matches.empty())
            {
                cout << "Deleted entries:\n";
                for(auto it : matches)
                {
                    deleteList(it->data->getKey());
                    cout << "("+ it->data->getTuple() +") in age\n";
                    HashTable *del = searchTable(it->data->getKey());
                    if(del->next == NULL && del->previous != NULL)
                    {
                        del->previous->next = NULL;
                    }
                    else if(del->previous == NULL)
                    {
                        int pos = ModuloHash(del->data.getKey());
                        AgeArr[pos] = del->next;
                    }
                    else
                    {
                        del->previous->next = del->next;
                    }
                }
            }
            else
            {
                cout << "No entries match query ("+ line +") in age\n";
            }
        }
    }
}
//initialize table with data from file
void LoadAgeTable(vector <string> data)
{
    smatch matcher;
    regex pattern("(.+),(.+),(.+),(.+)");
    for(auto it : data)
    {
        regex_search(it, matcher, pattern);
        int x = stoi(matcher[2].str());
        int y = stoi(matcher[3].str()); 
        int z = stoi(matcher[4].str());
        AgeData temp(matcher[1].str(), x, y, z);
        InsertToTable(temp.getTuple());
    }
}
//display table
void displayAGE()
{
    cout << "geo_geoid\tunder_5\tunder_18 over_65\n";
    for(auto it : AgeArr)
    {
        if(it != NULL)
        {
            cout << it->data.getKey() + "\t" << it->data.getUnder_5() << "\t" 
            << it->data.getUnder_18() << "\t " << it->data.getOver_65() << "\n";
            HashTable *ptr = it->next;
            //traverse chain
            while(ptr != NULL)
            {
                cout << ptr->data.getKey() + "\t" << ptr->data.getUnder_5() << "\t" 
                << ptr->data.getUnder_18() << "\t " << ptr->data.getOver_65() << "\n";
                ptr = ptr->next;
            }
        }
    }
}
//write table to file
void writeAGE()
{
    ofstream file;
    file.open("AgeTable.csv");
    if(file.fail())
    {
        cout << "Failed to create file.\n";
        return;
    }

    file << "geo_geoid\ngeo_geoid,under_5,under_18,over_65\n";
    for(auto it : AgeArr)
    {
        if(it != NULL)
        {
            file << it->data.getTuple() << endl;
            HashTable *ptr = it->next;
            while(ptr != NULL)
            {
                file << ptr->data.getTuple() << endl;
                ptr = ptr->next;
            }
        }
    }
}
//select function
void selectAGE(string line)
{
    //regex to parse data
    smatch matcher;
    regex pattern("(.+),(.+),(.+),(.+)");
    regex_search(line, matcher, pattern);
    int query;
    List *ptr = head;
    //if no key is provided
    if(matcher[1].str() == "*")
    {
        vector <string> matches;
        if(matcher[2].str() != "*")
        {
            query = stoi(matcher[2].str());
            while(ptr != NULL)
            {
                if(ptr->data->getUnder_5() == query)
                {
                    matches.emplace_back(ptr->data->getTuple());
                }
                ptr = ptr->next;
            }
        }
        else if(matcher[3].str() != "*")
        {
            query = stoi(matcher[3].str());
            while(ptr != NULL)
            {
                if(ptr->data->getUnder_18() == query)
                {
                    matches.emplace_back(ptr->data->getTuple());
                }
                ptr = ptr->next;
            }
        }
        else if(matcher[4].str() != "*")
        {
            query = stoi(matcher[4].str());
            while(ptr != NULL)
            {
                if(ptr->data->getOver_65() == query)
                {
                    matches.emplace_back(ptr->data->getTuple());
                }
                ptr = ptr->next;
            }
        }

        if(!matches.empty())
        {
            cout << "Found entries:\n";
            for(auto it : matches)
            {
                cout << "("+ it +") in age\n";
            }
        }
        else
        {
            cout << "No entries match query ("+ line +") in age\n";
        }
        
    }
    //if key is provided
    else
    {
        HashTable *ptr = searchTable(matcher[1].str());
        int x = stoi(matcher[2].str());
        int y = stoi(matcher[3].str());
        int z = stoi(matcher[4].str());

        if(ptr != NULL)
        {
            if(ptr->data.getUnder_5() == x 
            && ptr->data.getUnder_18() == y 
            && ptr->data.getOver_65() == z)
            {
                cout << "Found ("+ ptr->data.getTuple() +") in age\n";
            }
        }
        else
        {
            cout << "No entries match query ("+ line +") in age\n";
        }
        
    }
    
}
//update age
void updateAGE(string line)
{
    //regex to parse data
    smatch matcher;
    regex pattern("(.+),(.+),(.+),(.+)");
    regex_search(line, matcher, pattern);
    int x = stoi(matcher[2].str());
    int y = stoi(matcher[3].str());
    int z = stoi(matcher[4].str());

    HashTable *ptr = searchTable(matcher[1].str());
    //search function returns null if key does not exist
    //if found
    if(ptr != NULL)
    {
        ptr->data.setUnder_5(x);
        ptr->data.setUnder_18(y);
        ptr->data.setOver_65(z);
        cout << "Updated ("+ line +") in age\n";
    }
    //not found
    else
    {
        cout << "Failed to update ("+ line +") in age\n";
    }
    

}
//linked list functions
void appendList(AgeData *data)
{
    List *node = new List;
    node->data = data;    
    node->next = NULL;
    node->prev = NULL;

    if(head == NULL)
    {
        head = node;
    }
    else
    {
        List *last = head;
        while(last->next != NULL)
        {
            last = last->next;
        }
        last->next = node;
        node->prev = last;
    }
}
void deleteList(string key)
{
    List *temp = head;
    while(temp != NULL)
    {
        if(temp->data->getKey() == key)
        {
            temp->prev->next = temp->next;
            break;
        }
        temp = temp->next;
    }
}
//free memory
void freeList()
{
    List *current = head;
    List *next;
    while(current != NULL)
    {
        next = current->next;
        delete current;
        current = next;
    }
    head = NULL;
}

void freeTable()
{
    for(int i = 0; i < SIZE; i++)
    {
        delete AgeArr[i];
        AgeArr[i] = NULL;
    }
}

void printList()
{
    List* ptr = head;
    while(ptr != NULL)
    {
        cout <<ptr->data->getTuple() << endl;
        ptr = ptr->next;
    }
}