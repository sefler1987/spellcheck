/*
	provide edit distance function
*/

#include "hashmap.h"

#ifndef EDIT_DIS
#define EDIT_DIS

/*
	this function calculate one step distance
	to the give string. this function has been
	optimized for the spelling check algorithm

	parameters
	str: the string need to be calculated
	map: all of the output will be stored in the map
	     since the function is optimized for the spelling
		 algorithm, only valid words (which is in the database)
		 will be returned.
	db: the database of the words
	baseValue: a base value given to the value of each distance. this is 
	           for spelling checker's use.
*/
void editDistance(const char* str, hash_map* map, hash_map* db, int baseValue);

#endif