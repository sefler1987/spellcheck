#include "editDistance.h"
#include "hashmap.h"
#include <malloc.h>
#include <string.h>

int superCheck = 0;

void editDistance(const char* str, hash_map* map, hash_map* db, int baseValue)
{
	int len = strlen(str);

	// for temporary use
	int i = 0;
	int m = 0;
	int j = 0;

	// used for swapping
	char tp = 0;

	// buf of the new words
	char* buf = (char*)malloc(sizeof(char)*64);

	// try insert option
	for( i = 0; i < len + 1; i++ )
	{
		// a total of 24 letters, loop 24 times
		for( j = 0; j < 24; j++ )
		{
			for( m = 0; m < i; m++ )
			{
				// copy the previous part
				buf[m] = str[m];
			}

			// insert the new letter
			buf[m] = 97 + j;

			for( m = i + 1; m < len + 1; m++ )
			{
				// copy the latter part
				buf[m] = str[m - 1];
			}

			// end the string
			buf[m] = 0;

			/*
				super check will calculate all possibilities while cost more time.
				super check is disabled by default.
			*/
			if( !superCheck )
			{
				m = fetchData(buf, db);

				if( m != -1 )
				{
					/*
						this word is valid, put in the return map.
						since people are likely to lose letters while
						typing. we give insert option a higher priority
					*/
					putInHashMap(buf, m*baseValue*4, map);
				}
			}
			else
				putInHashMap(buf, m*baseValue*4, map);
		}
	}

	// try replace option
	for( i = 0; i < len; i++ )
	{
		// a total of 24 letters, loop 24 times
		for( j = 0; j < 24; j++ )
		{
			strcpy(buf, str);

			// replace the letter
			buf[i] = 97 + j;

			m = fetchData(buf, db);

			if( m != -1 )
			{
				// this word is valid, put in the return map
				putInHashMap(buf, m*baseValue, map);
			}
		}
	}
	
	// try delete option
	for( i = 0; i < len; i++ )
	{
		for( m = 0; m < i; m++ )
		{
			// copy the previous part
			buf[m] = str[m];
		}

		// this letter is ignored

		for( m = i; m < len - 1; m++ )
		{
			// copy the latter part
			buf[m] = str[m + 1];
		}

		// end the string
		buf[m] = 0;

		m = fetchData(buf, db);

		if( m != -1 )
		{
			// this word is valid, put in the return map
			putInHashMap(buf, m*baseValue, map);
		}
	}

	// at last, try transportation
	for( i = 0; i < len; i++ )
	{
		for( m = i + 1; m < len; m++ )
		{
			// reset the string
			strcpy(buf, str);

			// swap the letters
			tp = buf[i];
			buf[i] = buf[m];
			buf[m] = tp;

			j = fetchData(buf, db);

			if( j != -1 )
			{
				// this word is valid, put in the return map
				putInHashMap(buf, j*baseValue, map);
			}
		}
	}


	free(buf);

	// done!
}