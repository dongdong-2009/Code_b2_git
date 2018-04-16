

#ifndef _COMMON_TYPE_H
#define _COMMON_TYPE_H

#include <string>
#include <vector>

namespace TA_Base_Core
{
    // Data will be retrieved in Rows into a Buffer.
    typedef std::vector<std::string> Row;
    typedef std::vector<Row> Buffer;
	

	typedef std::vector<unsigned char> DataClob;
	typedef std::vector<DataClob> DataClobArray;

	//const unsigned long MAX_FIELDSIZE  = 4000;
	const unsigned long FETCH_MAX_ROWS = 110;//maybe 200 is too large for first startup, change it to 100
	const unsigned long FETCH_MAX_COLS = 100; 
}



#endif