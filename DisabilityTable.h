#ifndef disabilitytable_h
#define disabilitytable_h

#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>

using namespace std;

// Represents a table that stores disability data.
class DisabilityTable
{
public:

	// Create the table properties based on the disability.csv file
	DisabilityTable() = default;

	// Delete all rows and hash tabless
	~DisabilityTable();

	// Parse and insert disability data to the table
	void insert(const string &data, ostream &out);

	// Return the result of a selection operation
	void select(const string &data, ostream &out);

	// Perform and delete all that matches the query
	void remove(const string &data, ostream& out);
	
	// Perform an update of data. All field values should be provided.
	void update(const string &data, ostream& out);

	// Output the table contents formatted
	void display(ostream &out);

	// Write the table same format as it was read
	void write(ostream &out);

	void loadTable(const string &csvFilename, ostream &out);

private:
	
	string primaryKeyColumnName;
	int primaryKeyColumnIndex{};

	string *columnNames{};
	int maxColumnNames{};
	int numColumnNames{};

	// A row consists of multiple values that corresponds to the columns of a table
	struct Row
	{
		string *columnValues;

		// It's a linked list so it points to a next row and a previous row
		Row *previous;
		Row *next;

		// Create a row
		Row();

		// Delete column values
		~Row();
	};

	// Our table will have a list of rows
	Row *rowsHead{};
	int numRows{};
	
	// A primary key stores a unique value that points to a row for quick access
	struct PrimaryKeyValue
	{
		string value;
		Row *associatedRow;

		// Create a primary key value
		PrimaryKeyValue();

		// Delete associated row
		~PrimaryKeyValue();
	};

	// We have here our hash table of keys
	PrimaryKeyValue **primaryKeys{};
	int numPrimaryKeys{};
	int maxPrimaryKeys{};

	// Add a column name for this table
	void addColumnName(const string &columnName);

	// Split the CSV data into an array list of columns
	string *parseColumnValues(const string &data);
	
	// Print the row as a CSV format
	void printRowAsCSV(Row *row, ostream& out);

	// Create a hash index of a given value
	int multiplicativeHash(const string &value);

	// This is another insert the same as the ones in public but this one is for internal
	// use only. It is useful when re-hashing rows which usually happens when we need to 
	// internally re-expand the hashtable or when after a delete operation is done
	void insert(PrimaryKeyValue *primaryKey);

	// Delete a primary key and its row in the table
	string removePrimaryKey(const string &primaryKeyValue);
};

#endif
