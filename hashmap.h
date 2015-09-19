/*
	a very simple hashmap designed for C. This hash map
	can store integers only.

	author: Sefler Zhou
	date: 2010-6-11
*/

#ifndef HASH_MAP
#define HASH_MAP

// return this if not found
#define NOT_FOUND -1

typedef struct
{
	char* str;
	int value;
}bucket;

/*
	structure of the hashmap
*/
typedef struct
{
	bucket** table;
	int size;
	int maxSize;
}hash_map;

/*
	initiate the hashmap

	parameters
	map: the hashmap to be initiated
	size: the initiate size of the hashmap.
*/
void initiateHashMap(hash_map* map, int size);

/*
	expansion the hashmap
*/
void expansionHashMap(hash_map** map);

/*
	put in data

	parameters
	str: the key string
	value: the value to be stored
	map: the map the to operate
*/
void putInHashMap(const char* str, int value, hash_map* map);

/*
	fetch data

	parameters
	str: the key string
	map: the map the to operate
*/
int fetchData(const char* str, hash_map* map);

/*
	dismiss hashmap
*/
void dismissHashMap(hash_map* map);

/*
	Extract the hash key from the string
*/
int getKey(const char* str, int mSize);

#endif