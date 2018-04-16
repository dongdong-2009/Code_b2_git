#ifndef GLOBALDEF_H
#define GLOBALDEF_H

#include <climits>
#include <boost/static_assert.hpp>
//#include "core/data_access_interface/Pids_agent/src/DisplayRequestAccessFactory.h"
//#include "core/data_access_interface/Pids_agent/src/DisplayRequestData.h"
BOOST_STATIC_ASSERT(sizeof(unsigned int) == 4);
BOOST_STATIC_ASSERT(sizeof(unsigned short) == 2);
BOOST_STATIC_ASSERT(sizeof(unsigned char) == 1);

#define MSW(UINT_VAL) (unsigned short)((UINT_VAL) >> 16)
#define LSW(UINT_VAL) (unsigned short)((UINT_VAL) & USHRT_MAX)
#define TOBCD(VAL) ((((unsigned char)((VAL)/10)) << 4) | (unsigned char)((VAL)%10))

const int MODBUS_START_ADDRESS = 1;
const int MODBUS_END_ADDRESS = 516;
const int SIGINFO_FEP_REGISTER_ADDRESS = 517;
const int MODBUS_TRAINNO_MSW_ADDRESS = 10;
const int MODBUS_TRAINNO_LSW_ADDRESS = 11;
const int MESSAGE_HEADER_LENGTH = 10;	//word index

const unsigned long MESSAGE_PRIORITY_BIT_IN_WORD = 0u;
const unsigned long DISPLAY_LOCATION_BIT_IN_WORD = 1u;
const unsigned long CLEAR_MESSAGE_BIT_IN_WORD = 2u;
const unsigned long TRAIN_MESSAGE_BIT_IN_WORD = 15u;

const unsigned int ALL_TRAIN_SELECT_VALUE = UINT_MAX;

#endif