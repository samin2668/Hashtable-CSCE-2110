#include "DisabilityTable.h"

// Create the table properties based on the disability.csv file

// Delete all rows and hash tabless
DisabilityTable::~DisabilityTable()
{
	for (int i = 0; i < maxPrimaryKeys; i++)
		if (primaryKeys[i] != NULL)
			delete primaryKeys[i];

	delete[] primaryKeys;
	delete[] columnNames;
}

void DisabilityTable::loadTable(const string &csvFilename, ostream &out)
{
    // Initialize defaults
    rowsHead = NULL;
    numRows = 0;

    maxColumnNames = 10;
    numColumnNames = 0;
    columnNames = new string[maxColumnNames];

    maxPrimaryKeys = 100;
    numPrimaryKeys = 0;
    primaryKeys = new PrimaryKeyValue*[maxPrimaryKeys];

    for (int i = 0; i < maxPrimaryKeys; i++)
        primaryKeys[i] = NULL;

    // Open the file to build the table, stop if the file cannot be opened
    ifstream inFile(csvFilename.c_str());

    if (!inFile.is_open())
    {
        cerr << "Failed to open " << csvFilename << "." << endl;
        exit(0);
    }

    // First line is the primary key column
    primaryKeyColumnName = "geo_name";
    getline(inFile, primaryKeyColumnName);

    // The second line are the column names
    string line;
    getline(inFile, line);
    stringstream ss(line);
    string columnName;

    primaryKeyColumnIndex = -1;
    int currentIndex = 0;

    while (getline(ss, columnName, ','))
    {
        // Keep track the column with the primary key so we can know its position
        if (columnName == primaryKeyColumnName)
            primaryKeyColumnIndex = currentIndex;

        addColumnName(columnName);
        currentIndex++;
    }

    // Next is to load the initial data
    while (getline(inFile, line))
        insert(line, out);

    inFile.close();
}

// Parse and insert disability data to the table
void DisabilityTable::insert(const string &data, ostream &out)
{
	// Prepare an error message just in case
	string errorMessage = "Failed to insert (" + data + ") into disability";
	string successMessage = "Inserted (" + data + ") into disability";

	// Extract the column values from the data
	string *columnValues = parseColumnValues(data);

	if (columnValues == NULL)
	{
		out << errorMessage << endl;
		return;
	}

	// Create a row object that stores the volumn values
	Row *row = new Row();
	row->columnValues = columnValues;

	// Get the key value and store it
	PrimaryKeyValue *primaryKey = new PrimaryKeyValue();
	primaryKey->value = columnValues[primaryKeyColumnIndex];
	primaryKey->associatedRow = row;

	int hash = multiplicativeHash(primaryKey->value);

	// We insert the key to the table and we linear probing for collision resolution
	for (int i = 0; i < maxPrimaryKeys; i++)
	{
		if (primaryKeys[hash] == NULL)
		{
			// Found a slot, add the primary key
			primaryKeys[hash] = primaryKey;

			// Add the row to the table too
			row->next = rowsHead;

			if (rowsHead != NULL)
				rowsHead->previous = row;

			rowsHead = row;

			numRows++;
			out << successMessage << endl;
			return;
		}

		if (primaryKeys[hash]->value == primaryKey->value)
		{
			// Duplicate found!
			delete primaryKey;
			out << errorMessage << endl;
			return;
		}

		// Probe to next index
		hash = (hash + 1) % maxPrimaryKeys;
	}

	// If code reaches, here then the hash table is full
	out << errorMessage << endl;
}

// Return the result of a selection operation
void DisabilityTable::select(const string &data, ostream &out)
{
	// Prepare an error message just in case
	string errorMessage = "No entries match query (" + data + ") in disability";

	// Extract the column values from the data
	string *columnValues = parseColumnValues(data);

	if (columnValues == NULL)
	{
		out << errorMessage << endl;
		return;
	}

	if (columnValues[primaryKeyColumnIndex] != "*")
	{
		// If the primary key is provided then we search by primary key and all attributes are ignored
		// We will use hash searching
		int hash = multiplicativeHash(columnValues[primaryKeyColumnIndex]);

		for (int i = 0; i < maxPrimaryKeys; i++)
		{
			if (primaryKeys[hash] == NULL)
			{
				// Dead end! search fails
				out << errorMessage << endl;
				break;
			}

			if (primaryKeys[hash]->value == columnValues[primaryKeyColumnIndex])
			{
				// Found it! search ends
				out << "Found (";
				printRowAsCSV(primaryKeys[hash]->associatedRow, out);
				out << ") in disability" << endl;

				break;
			}

			// Probe to next index
			hash = (hash + 1) % maxPrimaryKeys;
		}
	}
	else
	{
		// If the primary key is not provided then we search one row to another that matches the required column values
		stringstream result;
		int numResultsFound = 0;
		Row *currentRow = rowsHead;

		while (currentRow != NULL)
		{
			bool matched = true;

			for (int i = 0; i < numColumnNames; i++)
			{
				if (columnValues[i] == "*" || columnValues[i] == currentRow->columnValues[i])
					continue;

				matched = false;
				break;
			}

			// Accumulate results
			if (matched)
			{
				result << "(";
				printRowAsCSV(currentRow, result);
				result << ")" << endl;
				numResultsFound++;
			}

			currentRow = currentRow->next;
		}

		// Display final results
		if (numResultsFound == 0)
		{
			out << errorMessage << endl;
		}
		else
		{
			out << "Found entries:" << endl;
			out << result.str();
		}
	}

	delete[] columnValues;
}

// Perform and delete all that matches the query
void DisabilityTable::remove(const string &data, ostream& out)
{
	// Prepare an error message just in case
	string errorMessage = "Failed to delete (" + data + ") in disability";

	// Extract the column values from the data
	string *columnValues = parseColumnValues(data);

	if (columnValues == NULL)
	{
		out << errorMessage << endl;
		return;
	}

	if (columnValues[primaryKeyColumnIndex] != "*")
	{
		string result = removePrimaryKey(columnValues[primaryKeyColumnIndex]);

		if (result == "")
			out << errorMessage << endl;
		else
			out << "Deleted (" << result << ") in disability" << endl;
	}
	else
	{
		// If the primary key is not provided then we search one row to another that matches the required column values
		// Find all the primary keys of a row that is to be deleted
		string *primaryKeyValues = new string[numRows];
		int numFound = 0;

		Row *currentRow = rowsHead;

		while (currentRow != NULL)
		{
			bool matched = true;

			for (int i = 0; i < numColumnNames; i++)
			{
				if (columnValues[i] == "*" || columnValues[i] == currentRow->columnValues[i])
					continue;

				matched = false;
				break;
			}

			// Accumulate results
			if (matched)
				primaryKeyValues[numFound++] = currentRow->columnValues[primaryKeyColumnIndex];

			currentRow = currentRow->next;
		}

		// Delete those that are found
		if (numFound == 0)
		{
			out << errorMessage << endl;
		}
		else
		{
			out << "Deleted entries:" << endl;

			for (int i = 0; i < numFound; i++)
			{
				string result = removePrimaryKey(columnValues[primaryKeyColumnIndex]);
				out << "(" << result << ") in disability" << endl;
			}
		}

		delete[] primaryKeyValues;
	}

	delete[] columnValues;
}

// Perform an update of data. All field values should be provided.
void DisabilityTable::update(const string &data, ostream& out)
{
	// Prepare an error message just in case
	string errorMessage = "Failed to update (" + data + ") in disability";
	string successMessage = "Updated (" + data + ") in disability";

	// Extract the column values from the data
	string *columnValues = parseColumnValues(data);

	if (columnValues == NULL)
	{
		out << errorMessage << endl;
		return;
	}

	// Find the row being updated
	int hash = multiplicativeHash(columnValues[primaryKeyColumnIndex]);

	for (int i = 0; i < maxPrimaryKeys; i++)
	{
		if (primaryKeys[hash] == NULL)
		{
			// Dead end! search fails
			delete[] columnValues;
			out << errorMessage << endl;
			break;
		}

		if (primaryKeys[hash]->value == columnValues[primaryKeyColumnIndex])
		{
			// Found it! do updates
			delete[] primaryKeys[hash]->associatedRow->columnValues;
			primaryKeys[hash]->associatedRow->columnValues = columnValues;
			out << successMessage << endl;
			break;
		}

		// Probe to next index
		hash = (hash + 1) % maxPrimaryKeys;
	}
}

// Output the table contents formatted
void DisabilityTable::display(ostream &out)
{
	// Find the maximum column sizes for each column
	int *columnSizes = new int[numColumnNames];

	for (int i = 0; i < numColumnNames; i++)
		columnSizes[i] = (int)columnNames[i].size();;

	Row *currentRow = rowsHead;

	while (currentRow != NULL)
	{
		for (int i = 0; i < numColumnNames; i++)
			if ((int)currentRow->columnValues[i].size() > columnSizes[i])
				columnSizes[i] = (int)currentRow->columnValues[i].size();

		currentRow = currentRow->next;
	}

	// Now do display
	for (int i = 0; i < numColumnNames; i++)
		out << setw(columnSizes[i] + 2) << left << columnNames[i];

	out << endl;

	currentRow = rowsHead;

	while (currentRow != NULL)
	{
		for (int i = 0; i < numColumnNames; i++)
			out << setw(columnSizes[i] + 2) << left << currentRow->columnValues[i];

		out << endl;
		currentRow = currentRow->next;
	}

	delete[] columnSizes;
}

// Write the table same format as it was read
void DisabilityTable::write(ostream &out)
{
	// Write keys
	out << primaryKeyColumnName << endl;

	// Write column names
	for (int i = 0; i < numColumnNames; i++)
	{
		out << columnNames[i];

		if (i + 1 < numColumnNames)
			out << ",";
	}

	out << endl;

	// Write contents
	Row *current = rowsHead;

	while (current != NULL)
	{
		for (int i = 0; i < numColumnNames; i++)
		{
			out << current->columnValues[i];

			if (i + 1 < numColumnNames)
				out << ",";
		}

		out << endl;
		current = current->next;
	}
}

// A row consists of multiple values that corresponds to the columns of a table
DisabilityTable::Row::Row()
{
	previous = NULL;
	next = NULL;
};

// Delete column values
DisabilityTable::Row::~Row()
{
	if (columnValues != NULL)
		delete[] columnValues;
}

// A primary key stores a unique value that points to a row for quick access
DisabilityTable::PrimaryKeyValue::PrimaryKeyValue()
{
	associatedRow = NULL;
};

// Delete associated row
DisabilityTable::PrimaryKeyValue::~PrimaryKeyValue()
{
	if (associatedRow != NULL)
		delete associatedRow;
}

// Add a column name for this table
void DisabilityTable::addColumnName(const string &columnName)
{
	if (numColumnNames >= maxColumnNames)
	{
		// Expand the storage of column names if out of space
		string * tempColumnNames = new string[maxColumnNames * 2];

		for (int i = 0; i < numColumnNames; i++)
			tempColumnNames[i] = columnNames[i];

		delete[] columnNames;
		columnNames = tempColumnNames;
		maxColumnNames *= 2;
	}

	columnNames[numColumnNames++] = columnName;
}

// Split the CSV data into an array list of columns
string *DisabilityTable::parseColumnValues(const string &data)
{
    smatch matcher;
    regex dataPattern("[\\(]?\"?(.+[^\"]|[*])\"?,(.+),(.+),(.+),(.+),(.+),(.+[^\\)]|[*])[\\)]?");
    regex_search(data,matcher,dataPattern);
    string *ret = new string[matcher.size()];
    for(int i = 0; i < matcher.size(); i++)
    {
        ret[i] = matcher[i+1].str();
    }

    return ret;
}

// Print the row as a CSV format
void DisabilityTable::printRowAsCSV(Row *row, ostream& out)
{
	for (int i = 0; i < numColumnNames; i++)
	{
		if (row->columnValues[i].find(',') != string::npos)
			out << "\"" << row->columnValues[i] << "\""; // Wrap with quotes with those values that has a ',' on it
		else
			out << row->columnValues[i];

		if (i + 1 < numColumnNames)
			out << ",";
	}
}

// Create a hash index of a given value
int DisabilityTable::multiplicativeHash(const string &value)
{
	// Well, multiplicative hashing works for numbers. So first we need to get
	// the integer value of the string.. just add their ascii up
	long hash = 0;

	for (unsigned i = 0; i < value.size(); i++)
		hash += value[i];

	// Now we do multiplicative hash
	hash *= 2654435761;
	return (int)abs((hash >> 32)) % maxPrimaryKeys;
}

// This is another insert the same as the ones in public but this one is for internal
// use only. It is useful when re-hashing rows which usually happens when we need to 
// internally re-expand the hashtable or when after a delete operation is done
void DisabilityTable::insert(PrimaryKeyValue *primaryKey)
{
	int hash = multiplicativeHash(primaryKey->value);

	// We insert the key to the table and we linear probing for collision resolution
	for (int i = 0; i < maxPrimaryKeys; i++)
	{
		if (primaryKeys[hash] == NULL)
		{
			// Found a slot, add the primary key
			primaryKeys[hash] = primaryKey;
			return;
		}

		// Probe to next index
		hash = (hash + 1) % maxPrimaryKeys;
	}
}

// Delete a primary key and its row in the table
string DisabilityTable::removePrimaryKey(const string &primaryKeyValue)
{
	// If the primary key is provided then we search by primary key and delete a single row
	int hash = multiplicativeHash(primaryKeyValue);

	for (int i = 0; i < maxPrimaryKeys; i++)
	{
		if (primaryKeys[hash] == NULL)
			return "";

		if (primaryKeys[hash]->value == primaryKeyValue)
			break; // Found it! search ends

		// Probe to next index
		hash = (hash + 1) % maxPrimaryKeys;
	}

	// Do delete
	if (primaryKeys[hash] != NULL)
	{

		// Re-arrange the links of rows, before deleting the row
		Row *row = primaryKeys[hash]->associatedRow;

		stringstream ss;
		printRowAsCSV(row, ss);

		if (row == rowsHead)
		{
			// Case deleting the head
			rowsHead = rowsHead->next;

			if (rowsHead != NULL)
				rowsHead->previous = NULL;
		}
		else
		{
			// Case deleting in between
			row->previous->next = row->next;

			if (row->next != NULL)
				row->next->previous = row->previous;
		}

		numRows--;
		delete primaryKeys[hash];
		primaryKeys[hash] = NULL;

		// Re-hash those that are affected with linear probing collisions
		for (hash = (hash + 1) % maxPrimaryKeys; primaryKeys[hash] != NULL; hash = (hash + 1) % maxPrimaryKeys)
		{
			PrimaryKeyValue *primaryKey = primaryKeys[hash];
			primaryKeys[hash] = NULL;
			insert(primaryKey);
		}

		return ss.str();
	}

	return "";
}


