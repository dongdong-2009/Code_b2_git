/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimUtility.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Definition/Implementation file for PASimUtility class
  */

#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include <iostream>
#include <stdexcept>
// #include <stdlib.h>
#include <limits>
#include <time.h>
#include <ctype.h>

// using namespace TA_IRS_App;
using namespace TA_IRS_App::PA_Sim;

/**
  * data2string
  *
  * Converts the binary data in ByteVector to its hexadecimal representation
  *
  * @return the number of binary bytes converted to hex
  */
int PASimUtility::data2string(const ByteVector & data, std::string & str)
{
    int converted = 0;
    ByteVector::const_iterator it;

    // pre-flight memory allocation
    str.reserve(data.size()*2 + 2);

    for ( it = data.begin() ; it != data.end() ; it++ )
    {
        // going from *(vector<Byte>::iterator) to int in two steps overcomes
        // some cross-platform compiler type-coercion idiosyncrasies 
        Byte b = *it;
        int i = b;
        char c[3];

        sprintf(c,"%.2X",i);

        str.append(c);

        ++converted;
    }

    return converted;
}

/**
  *
  */
void PASimUtility::sequenceNumberToString( int sequenceNumber, std::string & str )
{
    char buffStr[16];
    sprintf(buffStr, "%.5d", sequenceNumber);
    str=buffStr;
}


/**
  * string2data
  *
  * Converts a string of hexadecimal characters into a vector of binary bytes.
  * An assumption is made that if there are an odd number of characters in the
  * string to be converted, a leading zero is missing (and hence added).
  *
  * @return the number of binary bytes converted from hex.
  *
  * @todo implement this
  */
int PASimUtility::string2data(const std::string & str, ByteVector & data)
{
    throw std::logic_error("PASimUtility::string2data not implemented");

    return 0;
}

/**
  */
void PASimUtility::bool2string(bool b, std::string & str, BooleanConversionOutput format)
{
    if (b)
    {
        switch (format)
        {
            case booleanOutputYesNo:
                str = "yes";
                break;
            case booleanOutputOneZero:
                str = "1";
                break;
            case booleanOutputTrueFalse:
            default:
                str = "true ";
                break;
        }
    }
    else
    {
        switch (format)
        {
            case booleanOutputYesNo:
                str = "no ";
                break;
            case booleanOutputOneZero:
                str = "0";
                break;
            case booleanOutputTrueFalse:
            default:
                str = "false";
                break;
        }
    }
}

/**
  * string2bool
  *
  * Converts a string to a boolean
  *
  * @param str the string representation. If it is one of
  *        "true" or "yes" or is not equal zero, truth is returned.
  *        If it is one of "false", "no" or '0' (not just any
  *        old zero, as atoi returns zero for non-numbers), then
  *        false is returned.
  * @param unknown Specifies the return value if the input is
  *        unrecognised.
  */
bool PASimUtility::string2bool(const std::string & str, bool unknown)
{
    std::string s;

    // begin by lowercasing everything
	// freakin' std::string.append being type-fussy...
	char cc[2]; cc[1]='\0';
    for ( std::string::const_iterator c = str.begin() ; c!=str.end() ; c++ )
    {
		cc[0] = tolower(*c);
        s.append( cc );
    }

    if ( ( s=="true" ) || ( s=="yes" ) || (atoi(s.c_str())!=0) )
    {
        return true;
    }
    else if ( ( s=="false" ) || ( s=="no" ) || ( s=="0" ) )
    {
        return false;
    }

    return unknown;
}

std::string PASimUtility::location2string(LocationType location)
{
    std::string s = "";

    switch (location)
    {
        case OCC:
            s = "OCC";
            break;

        case DEPOT:
            s = "DEPOT";
            break;

        case MSS:
            s = "MSS";
            break;

        case SS1:
            s = "SS1";
            break;

        case SS2:
            s = "SS2";
            break;

        case STATION:
            s = "STATION";
            break;

        default:
            break;
    }

    return s;
}


LocationType PASimUtility::string2LocationType(const std::string & s)
{
    LocationType l = NO_LOCATION;

    if ( ( s=="OCC") )
    {
        l=OCC;
    }
    else if ( ( s=="DEPOT") || ( s=="DPT") )
    {
        l=DEPOT;
    }
    else if ( s=="MSS")
    {
        l=MSS;
    }
    else if ( s=="SS1")
    {
        l=SS1;
    }
    else if ( s=="SS2")
    {
        l=SS2;
    }
    else if ( s=="STATION")
    {
        l=STATION;
    }

    return (l);
}

int PASimUtility::hex2int(const std::string& hexStr)
{
    char *offset;
    if(hexStr.length() > 2)
    {
        if(hexStr[0] == '0' && hexStr[1] == 'x')
        {
            return strtol(hexStr.c_str(), &offset, 0);
        }
    }
    return strtol(hexStr.c_str(), &offset, 16);
}

TableIdentifier PASimUtility::string2TableIdentifier(const std::string & s, std::string assume_prefix)
{
    std::string real_s = s;

    // lowercase everything
    int len = real_s.length();
    for ( int i=0 ; i<len ; i++ )
    {
		real_s[i] = tolower(real_s[i]);
    }

    TableIdentifier rv = INVALID_IDENTIFIER;

    //// add the prefix, if necessary
    //char * apos = strstr(real_s.c_str(), "PA");
    //char * ipos = strstr(real_s.c_str(), "iscs");
    //if ( (apos==0) & (ipos==0) )
    //{
    //    real_s = assume_prefix + real_s;
    //}

    ////
    //// The only real way to check all the values.
    //if ( real_s == "PAdss" )
    //{
    //    rv = PADSS;
    //}
    //else if ( real_s == "PAplatform" )
    //{
    //    rv = PAPlatform;
    //}
    //else if ( real_s == "PAtrain" )
    //{
    //    rv = PATrain;
    //}
    //else if ( real_s == "PAwash" )
    //{
    //    rv = PAWash;
    //}
    //else if ( real_s == "PAwatchdog" )
    //{
    //    rv = PAWatchdog;
    //}
    //else if ( real_s == "PAwild" )
    //{
    //    rv = PAWILD;
    //}
    //else if ( real_s == "iscsatcdata" )
    //{
    //    rv = IscsAtcData;
    //}
    //else if ( real_s == "iscselectrical" )
    //{
    //    rv = IscsElectricalSubsection;
    //}
    //else if ( real_s == "iscsplatform" )
    //{
    //    rv = IscsPlatform;
    //}
    //else if ( real_s == "iscstrain" )
    //{
    //    rv = IscsTrain;
    //}
    //else if ( real_s == "iscswash" )
    //{
    //    rv = IscsWash;
    //}
    //else if ( real_s == "iscswatchdog" )
    //{
    //    rv = IscsWatchdog;
    //}

    return rv;
}


int PASimUtility::tokenise(const char * s, TokenList & tokens, const char * delimiters)
{
    int numchars = strlen(s)+1;

    if (numchars > 1024)
    {
        std::cerr << "WARNING: truncating input to 1024 characters" << std::endl;
        numchars=1024;
    }

    char s_[1024];
    strncpy(s_,s,1024); // pads with zeros if numchars<1024
    s_[numchars] = 0;

    char * token = strtok(s_, delimiters);
    while ( token != 0 )
    {
        tokens.push_back(token);
        token = strtok(0, delimiters);
    }

    return tokens.size();
}


void PASimUtility::detokenise(TokenList & tokens, std::string & s, const char * padding)
{
    // false for the first iteration.
    bool do_pad = false;

    for ( TokenList::const_iterator it = tokens.begin() ;
          it != tokens.end() ;
          it++
        )
    {
        if (do_pad)
        {
            s.append(padding);
        }

        s.append(*it);
        do_pad = true;
    }
}

// declaration.  Required for strict ansi compliance
void expandPartialRange(const std::string & r, RecordRange & range, unsigned int max);
// definition
void expandPartialRange(const std::string & r, RecordRange & range, unsigned int max)
{
    unsigned int range_min = 0;
    unsigned int range_max = max;

    std::string::size_type minus = r.find_first_of('-');
    if (minus != std::string::npos)
    {
        // atoi should just stop when it sees the '-' or alternatively, and
        // conveniently, return 0 if there is nothing in front of it
        range_min = atoi(r.c_str());

        // atoi should return 0 if there is no life beyond the '-'
        range_max = atoi(&((r.c_str())[minus+1]));

        if ((range_max<range_min) || (range_max>max))
        {
            range_max = max;
        }

        for (unsigned int i=range_min; i<=range_max ; i++)
        {
            range.insert(i);
        }
    }
    else // there's hopefully just a number on its own
    {
        range_min = atoi(r.c_str());
        range.insert(range_min);
    }
}

void PASimUtility::expandRange(const std::string & r, RecordRange & range, unsigned int max)
{
    TokenList ranges;
    PASimUtility::tokenise(r.c_str(), ranges, ",");

    for ( TokenList::iterator iter = ranges.begin() ;
          iter != ranges.end() ;
          iter++
        )
    {
        std::string rng = (*iter);
        expandPartialRange(rng, range, max);
    }
}

// declaration.  Required for strict ansi compliance
void formPartialRange_PA(unsigned int min, unsigned int max, std::string & r);
// definition
void formPartialRange_PA(unsigned int min, unsigned int max, std::string & r)
{
    char r_[64];
    if ( min == max )
    {
        sprintf(r_, "%d",max);
    }
    else
    {
        sprintf(r_, "%d-%d",min,max);
    }

    if (!r.empty())
    {
        r += ",";
    }

    r += r_;
}

void PASimUtility::formRange(const RecordRange & range, std::string & r)
{
    if (range.empty())
    {
        r="";
        return;
    }

    unsigned int min = UINT_MAX;
    unsigned int max = 0;
    unsigned int last_val = 0;
    bool needs_forming = false; // decides whether we need a final call to formPartialRange_PA

    for ( RecordRange::const_iterator iter = range.begin() ;
          iter != range.end() ;
          iter++
        )
    {
        if (*iter > last_val+1)
        {
            formPartialRange_PA(min,max,r);
            min = max = *iter;
            needs_forming = false;
        }
        else
        {
            if ((*iter) > max)
            {
                max = *iter;
            }

            if ((*iter) < min)
            {
                min = *iter;
            }

            needs_forming = true;
        }

        last_val = *iter;
    }

    if (needs_forming)
    {
        formPartialRange_PA(min,max,r);
    }
}


unsigned int PASimUtility::random_uint32(unsigned int lo, unsigned int hi)
{
    double normalised_rand = (double)rand()/(double)RAND_MAX;

    return ((normalised_rand*(hi-lo))+lo);
}


Word PASimUtility::randomWord(Word lo, Word hi)
{
    return (PASimUtility::random_uint32(lo,hi));
}

Byte PASimUtility::randomByte(Byte lo, Byte hi)
{
    return (PASimUtility::random_uint32(lo,hi));
}


void PASimUtility::get12ByteDateString(char * s, Word year, Word month, Word day, Word hour, Word minutes, Word seconds)
{
    struct tm the_time={seconds,minutes,hour,day,month,year};
    PASimUtility::get12ByteDateString(s, &the_time, false);
}


void PASimUtility::get12ByteDateString(char * s, struct tm * a_time, bool random)
{
    char ss[16];

    //
    // If a randomtime was asked for then the tm struct probably hasn't been
    // allocated.
    // The struct definitely hasn't been allocated if the given a_time is zero
    struct tm theTime;

    if (random)
    {
        // struct tm {
        //         int tm_sec;     /* seconds after the minute - [0,59] */
        //         int tm_min;     /* minutes after the hour - [0,59] */
        //         int tm_hour;    /* hours since midnight - [0,23] */
        //         int tm_mday;    /* day of the month - [1,31] */
        //         int tm_mon;     /* months since January - [0,11] */
        //         int tm_year;    /* years since 1900 */
        //         int tm_wday;    /* days since Sunday - [0,6] */
        //         int tm_yday;    /* days since January 1 - [0,365] */
        //         int tm_isdst;   /* daylight savings time flag */
        //         };
        theTime.tm_year = 104;
        theTime.tm_mon  = PASimUtility::randomWord(0,11);
        theTime.tm_mday = PASimUtility::randomWord(1,28);
        theTime.tm_hour = PASimUtility::randomWord(0,23);
        theTime.tm_min  = PASimUtility::randomWord(0,59);
        theTime.tm_sec  = PASimUtility::randomWord(0,59);
    }
    else if ( a_time == 0 )
    {
        time_t ltime;
        tzset();
        time( &ltime );

        struct tm * rightnow = localtime( &ltime );
        theTime = *rightnow;
    }
    else
    {
        // copy the given time to our local declaration
        theTime = *a_time;
    }

    strftime(ss,16,"%y%m%d%H%M%S",&theTime);
    memcpy(s,ss,12);
}


Byte PASimUtility::process4LevelMask(Byte original, const char mask[8])
{
    Byte processed = original;
    Byte mask_ = 0x80;


    for (int foo=0; foo<8 ; foo++,mask_>>=1)
    {
        if ( mask[foo] == '1' )
        {
            // bitwise-OR to set the bit
            processed |= mask_;
        }
        else if ( mask[foo] == '0' )
        {
            // bitwise-AND with the bitwise-NOT of the mask to clear the bit
            processed &= (~mask_);
        }
        else if ( mask[foo] == 'T' )
        {
            if ( original & mask_ )
            {
                // The bit is set. Clear it.
                // bitwise-AND with the bitwise-NOT of the mask to clear the bit
                processed &= (~mask_);
            }
            else
            {
                // The bit is clear. Set it.
                // bitwise-OR to set the bit
                processed |= mask_;
            }
        }
        else
        {
            // 
            // leave the blessed bit alone.
        }

    }

    return processed;
}


//
// This may have been easier to do with cut and pasting the code from above.
Word PASimUtility::process4LevelMask(Word original, const char mask[16])
{
    Word processed = original;
    Word mask_ = 0x8000;

    for (int foo=0; foo<16 ; foo++,mask_>>=1)
    {
        if ( mask[foo] == '1' )
        {
            // bitwise-OR to set the bit
            processed |= mask_;
        }
        else if ( mask[foo] == '0' )
        {
            // bitwise-AND with the bitwise-NOT of the mask to clear the bit
            processed &= (~mask_);
        }
        else if ( mask[foo] == 'T' )
        {
            if ( original & mask_ )
            {
                // The bit is set. Clear it.
                // bitwise-AND with the bitwise-NOT of the mask to clear the bit
                processed &= (~mask_);
            }
            else
            {
                // The bit is clear. Set it.
                // bitwise-OR to set the bit
                processed |= mask_;
            }
        }
        else
        {
            // 
            // leave the blessed bit alone.
        }

    }

    return processed;
}


/** converts an w.x.y.z form of branch id to the integer as stored
 * in the PATrain table. Accepts just the integer, also.
 *
 * @param s the original string to convert
 * @return the integer converted to. 0 indicates invalid input
 */
Word PASimUtility::convertBranchId2IntegerString(const std::string & s, std::string & is)
{
    Word return_integer = 0;

    int s_len=s.length();
    int dotCount=0;
    for ( int i=0 ; i<s_len ; i++ )
    {
        if ( s[i] == '.' )
        {
            dotCount++;
        }
    }

    if (dotCount==3)
    {
        // From an email from Quek:
        //
        // <Fixed Block's Branch ID> is presented in a 16-bit field as follows:
        // Sector   | Section   | Subsection | Branch
        // 5 bits   | 5 bits    | 2 bits     | 4 bits   ( = 16 bits )
        //
        TokenList fields;
        int numfields = PASimUtility::tokenise(s.c_str(), fields, ".");
        std::string branch_id_str_1 = fields.front();
        fields.pop_front();
        int branch_id_1 = atoi(branch_id_str_1.c_str());
        std::string branch_id_str_2 = fields.front();
        fields.pop_front();
        int branch_id_2 = atoi(branch_id_str_2.c_str());
        std::string branch_id_str_3 = fields.front();
        fields.pop_front();
        int branch_id_3 = atoi(branch_id_str_3.c_str());
        std::string branch_id_str_4 = fields.front();
        fields.pop_front();
        int branch_id_4 = atoi(branch_id_str_4.c_str());
        
        // return_integer = ((branch_id_1*(1<<11)) + (branch_id_2*(1<<6))  + (branch_id_3*(1<<4)) + branch_id_4);
        return_integer = ((branch_id_1*2048) + (branch_id_2*64)  + (branch_id_3*16) + branch_id_4);
    }
    else if (dotCount==0)
    {
        return_integer = atoi(s.c_str());
    }
    else
    {
        // do nuffin'
    }

    char s_[32];
    sprintf(s_,"%d",return_integer);
    is = s_;

    return return_integer;
}

std::string PASimUtility::outputFormat2String(TableOutputFormat f)
{
    std::string s = "Unknown";
    if ( f == f_hex )
    {
        s = "hex";
    }
    else if ( f == f_tabular )
    {
        s = "tabular";
    }

    return s;
}

std::string PASimUtility::itoa(int num)
{
    char s_[32];
    sprintf(s_,"%d",num);
    std::string s = s_;

    return s;
}

std::string PASimUtility::uitoa(unsigned long int num)
{
    char s_[32];
    sprintf(s_,"%lu",num);
    std::string s = s_;

    return s;
}


const std::string PASimUtility::DataFiller = "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`abcdefghijklmnopqrstuvwxyz{|}~";

int TA_IRS_App::PA_Sim::PASimUtility::string2int( const std::string& str )
{
	int rt = -1;
	if ( '0' == str[0] && ('X' == str[1] || 'x' == str[1]))
	{
		sscanf(str.c_str(), "%x", &rt);
	}
	else
	{
		sscanf(str.c_str(), "%d", &rt);
	}
	return rt;
}
