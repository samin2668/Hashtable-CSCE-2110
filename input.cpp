#include "input.h"
#include <vector>
#include <cstdlib>

//vectors to hold tuples for each table
vector <string> A;
vector <string> D;
vector <string> G;


void getInput()
{
    GeographyTable geoTable;
    DisabilityTable disTable;
    string CSV, tableName, inFile, line, dummy;
    string geoCSV;
    //get input file
    cout << "Please enter the main file's name: ";
    cin >> inFile;
    ifstream input, ifstr;
    input.open(inFile);
    if(input.fail())
    {
        cout << "Failed to open input file.\n";
        EXIT_FAILURE;
    }
    //get data tuples from .csv files
    for(int i = 0; i < 3; i++)
    {
        input >> CSV >> tableName;
        cout << ""+ CSV +" "+ tableName +"\n";
        ifstr.open(CSV);
        if(ifstr.fail())
        {
            cout << "Unable to open table " << tableName << endl;
        }
        else
        {
            cout << "Processing " << tableName + " table" << endl;
            //loading geo table
            if(tableName == "geography")
            {
                geoCSV = CSV;
                //inset geography table
                getline(ifstr, dummy);
                getline(ifstr, dummy);
                while(getline(ifstr, line))
                {
                    G.emplace_back(line);
                }
               geoTable.loadTable(G);
            }
            //loading age table
            else if(tableName == "age")
            {
                //insert age table
                getline(ifstr, dummy);
                getline(ifstr, dummy);
                while(getline(ifstr, line))
                {
                    A.emplace_back(line);
                }
                LoadAgeTable(A);
            }
            //loading dis table
            else if(tableName == "disability")
            {
                //insert disability table
               disTable.loadTable(CSV,cout);
        }
            ifstr.close();
        }
    }
    //parse function calls to get function name, tuple, and table name
    smatch matcher;
    regex pattern("(.+)\\(\\((.+)\\),(.+)\\)");
    while(!input.eof())
    {
        getline(input, line);
        //diplay function
        if(line == "DISPLAY()")
        {
            cout << ""+ line +"\n";
            geoTable.display();
            cout << endl;
            displayAGE();
            cout << endl;
            disTable.display(cout);
        }
        //write function
        else if(line == "WRITE()")
        {
            cout << ""+ line +"\n";
            writeAGE();
            geoTable.write(geoCSV);
            ofstream outFile("disability1_new.csv");
            disTable.write(outFile);
            cout <<"Processing complete!\n";
        }
        regex_search(line, matcher, pattern);
        //geting function tuple and table
        string function = matcher[1].str();
        string Tuple = matcher[2].str();
        string table = matcher[3].str();
        //age function
        if(table == "age")
        {
            if(function == "INSERT")
            {
                InsertToTable(Tuple);
            }
            else if(function == "UPDATE")
            {
                updateAGE(Tuple);
            }
            else if(function == "SELECT")
            {
                selectAGE(Tuple);
            }
            else if(function == "DELETE")
            {
                deleteAGE(Tuple);
            }
        }
        //geo function
        else if(table == "geography")
        {
            if(function == "INSERT")
            {
                geoTable.insert(Tuple);
            }
            else if(function == "UPDATE")
            {
                geoTable.update(Tuple);
            }
            else if(function == "SELECT")
            {
                geoTable.select(Tuple);
            }
            else if(function == "DELETE")
            {
                geoTable.deleteItem(Tuple);
            }
        }
        //disability function
        else if(table == "disability")
        {
            if(function == "INSERT")
            {
               disTable.insert(Tuple,cout);
            }
            else if(function == "UPDATE")
            {
               disTable.update(Tuple,cout);
            }
            else if(function == "SELECT")
            {
                disTable.select(Tuple,cout);
            }
            else if(function == "DELETE")
            {
              disTable.remove(Tuple,cout);
            }
        }
    }
    //close file
    input.close();
}