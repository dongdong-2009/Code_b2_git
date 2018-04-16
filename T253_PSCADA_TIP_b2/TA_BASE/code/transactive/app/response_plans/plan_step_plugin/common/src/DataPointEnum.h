#ifndef DATAPOINTENUM_INCLUDE
#define DATAPOINTENUM_INCLUDE

namespace TA_Base_App
{

enum EDataPointTest
{
    EQUAL_TO_TEST,
    NOT_EQUAL_TO_TEST,
    LESS_THAN_TEST,
    GREATER_THAN_TEST,
    LESS_THAN_OR_EQUAL_TO_TEST,
    GREATER_THAN_OR_EQUAL_TO_TEST,
    UNDEFINED_TEST
};

enum EDataPoint
{
	BOOLEAN_DATA_POINT,
    NUMERIC_DATA_POINT,
    COMBINED_DATA_POINT,
	TEXT_DATA_POINT,
	UNDEFINED_DATA_POINT
};

}

#endif