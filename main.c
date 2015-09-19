#define CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>

//#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "hashmap.h"
#include "editDistance.h"

#define BUF_LEN 1057480 // 1M cache
#define ONE_STEP_PRI 1000 // the priority of string 1 step distance away
#define TWO_STEP_PRI 15 // the priority of string 2 step distance away
#define DB_INIT_SIZE 1024 // size of the initial hash map database
#define BUF_INIT_SIZE 8 // size of the buffer hash map

extern int superCheck;

void parse(const char* str, hash_map* map)
{
	char* buf = (char*)malloc(sizeof(char)*BUF_LEN);
	char* tpStr = 0;
	int i,start,end;

	// start parsing
	i = 0;

	strcpy(buf, str);

	while( buf[i] )
	{
		start = i;

		while( buf[start] < 65 || (buf[start] > 90 && buf[start] < 97) || buf[start] > 122 )
		{
			if( !buf[start] )
			{
				// free the buffer
				free(buf);

				return; // no word found
			}

			start++; // find the start of the word
		}

		end = start;

		while( (buf[end] >= 65 && buf[end] <= 90) || (buf[end] >= 97 && buf[end] <= 122) )
		{
			if(buf[end] >= 65 && buf[end] <= 90)
			{
				// make all upper case to lower case
				buf[end] = buf[end] + 32;
			}

			end++;
		}

		// end the word
		buf[end] = 0;

		tpStr = &buf[start];

		putInHashMap(tpStr, 1, map);

		i = ++end;
	}

	// free the buffer
	free(buf);
}

void parseDict()
{
	char* str = (char*)malloc(sizeof(char)*BUF_LEN);
	int i;
	// C standard file I/O
	FILE* file;
	FILE* outFile;
	// used to store the words
	hash_map map;

	initiateHashMap(&map, 32);

	if( (file = fopen ("dict.txt", "r")) == NULL )
	{
		printf("Error reading dictionary file. Exiting program\n");

		exit(-1);
	}

	while( i = fread(str, 1, BUF_LEN - 1, file) )
	{
		// end the str
		str[i] = 0;

		// parse the dictionary
		parse(str, &map);
	}

	// free the str
	free(str);

	fclose(file);

	// Now output the hash map
	if( (outFile = fopen ("parsed.txt", "w")) == NULL )
	{
		printf("Can't create parsed file. Exiting program.\n");

		exit(-1);
	}

	for( i = 0; i < map.maxSize; i++ )
	{
		if( map.table[i] )
			fprintf(outFile, "%s %d \n", map.table[i]->str, map.table[i]->value);
	}

	dismissHashMap(&map);

	fclose(outFile);
}

void readDB(hash_map* map, FILE* dbFile)
{
	char* buf = (char*)malloc(sizeof(char)*64);
	int i = 0;

	while( fscanf(dbFile, "%s %d\n", buf, &i) != EOF )
	{
		// put the entry into the map
		putInHashMap(buf, i, map);
	}

	free(buf);
}

// calculate candidate suggestions for the string
void calcuCands(const char* str, hash_map* bufMap, hash_map* db)
{
	hash_map bufMap_2;
	int i = 0;

	initiateHashMap(&bufMap_2, BUF_INIT_SIZE);

	editDistance(str, bufMap, db, ONE_STEP_PRI);

	for( i = 0; i < bufMap->maxSize; i++ )
	{
		if( bufMap->table[i] != 0 )
		{
			// calculate the distances 2 step away
			editDistance(bufMap->table[i]->str, &bufMap_2, db, TWO_STEP_PRI);
		}
	}

	// merge distance 1 & 2
	for( i = 0; i < bufMap_2.maxSize; i++ )
	{
		if( bufMap_2.size == 0 )
		{
			break; // all the candidates have been added
		}

		if( bufMap_2.table[i] != 0 )
		{
			putInHashMap(bufMap_2.table[i]->str, bufMap_2.table[i]->value, bufMap);
			bufMap_2.size--;
		}
	}

	dismissHashMap(&bufMap_2);
}

int main()
{
	FILE* dbFile = NULL;
	hash_map map;
	hash_map bufMap; // store 1 step distance
	int i = 0;
	int maxValue = -1; // point to the best match
	char* sugStr = (char*)malloc(sizeof(char)*64);
	char* inputStr = (char*)malloc(sizeof(char)*64);

	// initiate the map
	initiateHashMap(&map, DB_INIT_SIZE);
	initiateHashMap(&bufMap, BUF_INIT_SIZE);

	// initiate the vars
	strcpy(sugStr, "No spell suggest. Sorry.");

	// check if parsed database exits
	if( (dbFile = fopen ("parsed.txt", "r")) == NULL )
	{
		// dictionary not parsed, parse dictionary
		printf("Parsing dictionary. Please wait...\n");

		parseDict();
	}

	// read database
	if( (dbFile = fopen ("parsed.txt", "r")) == NULL )
	{
		// dictionary not parsed, parse dictionary
		printf("Can't read database. Exiting program.\n");

		exit(-1);
	}
	else
	{
		printf("Reading database. Please wait...\n");

		readDB(&map, dbFile);

		printf("Reading complete.\n");
	}

	// read the user's word which needs correction
	while( 1 )
	{
		printf("Input your WRONG word for correction. Close the window for exiting.\n");
		scanf("%s", inputStr);

		// check if the word is correct
		if( fetchData(inputStr, &map) != -1 )
		{
			printf("This word is correct, no need to check. \n");
			continue;
		}

		// calculate candidates
		calcuCands(inputStr, &bufMap, &map);

		// find the largest one and display it. that's the suggestion
		for( i = 0; i < bufMap.maxSize; i++ )
		{
			if( bufMap.table[i] != 0 )
			{
				if( bufMap.table[i]->value > maxValue )
				{
					maxValue = bufMap.table[i]->value;
					strcpy(sugStr, bufMap.table[i]->str);
				}
			}
		}

		if( maxValue != -1 )
		{
			// print the suggestion
			printf("Did you mean '%s'? \n", sugStr);
		} 
		else
		{
			printf("No spell suggest, sorry.\n");
		}

		// reset the words buffer
		dismissHashMap(&bufMap);
		initiateHashMap(&bufMap, BUF_INIT_SIZE);

		maxValue = -1;
	}

	dismissHashMap(&bufMap);
	dismissHashMap(&map);

	_CrtDumpMemoryLeaks();

	return 0;
}