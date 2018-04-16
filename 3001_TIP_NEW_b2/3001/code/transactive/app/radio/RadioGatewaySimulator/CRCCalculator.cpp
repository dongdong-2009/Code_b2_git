#include "StdAfx.h"
#include "CRCCalculator.h"

unsigned short CRCCalculator::getCRC(string data)
{
	unsigned short crc = 0xffff;
	unsigned short carry;
    unsigned short temp1;
    unsigned short temp2;
    unsigned short temp3;
    unsigned short temp4;
	for ( int i=0; i<data.length(); i++ )
    {
        temp1 = crc & 0x00ff;
        temp4 = (unsigned short) data[i];
        temp4 &= 0x00ff;
        temp2 = temp1 ^ (temp4 & 0x00ff);
        temp3 = crc & 0xff00;
        crc = temp3 | temp2;
        for (int j=0; j<8; j++)
        {
            carry = crc & 0x0001;
            crc >>= 1;
            if (carry)
				crc ^= 0xa001;
        }
    }
	return crc;
}
