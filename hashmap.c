/*
	implementation of the simple hashmap
*/

#include "hashmap.h"
#include <string.h>
#include <malloc.h>
#include <memory.h>

void initiateHashMap(hash_map* map, int size)
{
	map->table = (bucket**)malloc(size*sizeof(bucket*));
	memset(map->table, 0, sizeof(bucket*)*size);

	map->size = 0;
	map->maxSize = size;
}


void expansionHashMap(hash_map** map)
{
	// for temporatory use
	int i,key;
	int mSize = (*map)->maxSize<<1;

	// increase the size to twice of the original
	bucket** newBucks = (bucket**)malloc(sizeof(bucket*)*mSize);

	memset(newBucks, 0, sizeof(bucket*)*mSize);

	// dump the values
	for( i = 0; i < (*map)->maxSize; i++ )
	{
		if( (*map)->table[i] )
		{
			// key
			key = getKey((*map)->table[i]->str, mSize); 

			while( newBucks[key] )
				key = key >= mSize ? 0 : key + 1;

			// move the slot to the new one
			newBucks[key] = (*map)->table[i];
		}
	}

	// free the buckets
	free((*map)->table);

	// assign the new maxSize
	(*map)->maxSize = mSize;

	// assign the new buckets
	(*map)->table = newBucks;
}


void putInHashMap(const char* str, int value, hash_map* map)
{
	bucket* b = 0;
	int i = 0;

	// key
	i = getKey(str, map->maxSize);

	// check if collision, map->table[i] == 0 if the slot is free
	while( map->table[i] )
	{
		/*
			if the key strings are the same, increse the value
			SPECIAL OPTIMIZED for spelling checker. in ordianory
			use, update the bucket instead.
		*/
		if( !strcmp(str, map->table[i]->str) )
		{
			/*free(map->table[i]->str);
			free(map->table[i]);*/

			// increse the bucket
			map->table[i]->value += value;

			return;
		}

		i = i + 1 >= map->maxSize ? 0 : i + 1;
	}

	/* 
		researches show that "open address" has a poor performance when
		load factor is larger than 0.6
	*/
	if( (float)(map->size + 1) / map->maxSize > 0.6 )
		expansionHashMap(&map);

	b = (bucket*)malloc(sizeof(bucket));

	b->str = (char*)malloc((strlen(str) + 1)*sizeof(char));
	strcpy(b->str, str);

	b->value = value;

	// assign the bucket
	map->table[i] = b;

	// increase the size
	map->size++;
}


int fetchData(const char* str, hash_map* map)
{
	int i = 0;
	int j = 0;

	// key
	i = getKey(str, map->maxSize);

	j = 0;

	if( map->table[i] )
	{
		while( strcmp(map->table[i]->str, str) )
		{
			i = i + 1 >= map->maxSize ? 0 : i + 1;
		
			j++;
			if( j == map->maxSize || !map->table[i] )
			{
				// no match found if has searched through or key points to an invalid slot
				return NOT_FOUND; 
			}
		}

		return map->table[i]->value;
	}
	else
	{
		// key points to an invalid slot, no match
		return NOT_FOUND;
	}
}


void dismissHashMap(hash_map* map)
{
	int i;

	for( i = 0; i < map->maxSize; i++ )
	{
		//map->table[i] != 0 if there's item in the slot
		if( map->table[i] )
		{
			// free the buckets
			free(map->table[i]->str);
			free(map->table[i]);
		}
	}

	free(map->table);
}

int getKey(const char* str, int mSize)
{
	// use usigned int to make sure the key >= 0
	unsigned int i = 0;
	unsigned int j = 0;

	// calculate hash value
	while( str[i] != 0 )
	{
		j += (str[i++]<<i);
	}

	// the key
	i = j % mSize;

	return i;
}