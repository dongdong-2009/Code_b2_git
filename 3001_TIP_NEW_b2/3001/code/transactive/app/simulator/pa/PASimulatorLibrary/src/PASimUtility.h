#ifndef PA_SIM_UTILITY_H
#define PA_SIM_UTILITY_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Declaration file for PASimUtility class
  */

// for ntohs,ntohl,htonl,htons
#if !defined( WIN32 )
#include <arpa/inet.h>
#else
#include <winsock2.h>
#pragma warning(disable : 4786)
#pragma warning(disable : 4284)
#endif // !WIN32

#include "app/simulator/pa/PASimulatorLibrary/src/PASimCommon.h"
#include <string>
#include <set>
#include <list>
#include <limits.h>

namespace TA_IRS_App
{
namespace PA_Sim
{
    /** Type definition for a list of strings. Used by the tokenise and
      * detokenise methods.
      */
    typedef std::list< std::string > TokenList;

    /**
      * @class PASimUtility
      *
      * PASimUtility is a utility class for PASim Tables.  It contains only static
      * methods.
      *
      */
    class PASimUtility
    {
    public:

        /**
          * data2string
          *
          * Converts the binary data in ByteVector to its hexadecimal representation
          *
          * @return the number of bytes converted to hex
          */
        static int data2string(const ByteVector & data, std::string & str);

        /**
          * string2data
          *
          * Converts the binary data in ByteVector to its hexadecimal representation
          *
          * @return the number of binary bytes converted from hex
          */
        static int string2data(const std::string & str, ByteVector & data);

        static int hex2int(const std::string& hexStr);

		static int string2int(const std::string& str);

        typedef enum BooleanConversionOutput
        {
            booleanOutputTrueFalse  = 0,
            booleanOutputYesNo      = 1,
            booleanOutputOneZero    = 2
        };

        /**
          * bool2string
          *
          * Converts the boolean to a string
          *
          * @param b the boolean value
          * @param str the string representation. One of "true " or "false"
          *             (note the trailing space, for formatting purposes)
          * @param format see the BooleanConversionOutput enumueration
          *             Defaults to booleanOutputTrueFalse.
          */
        static void bool2string(bool b, std::string & str, BooleanConversionOutput format=booleanOutputTrueFalse);

        /**
          * string2bool
          *
          * Converts a string to a boolean
          *
          * @param str the string representation. If it starts with one of
          *        "true" or "yes" or is not equal zero, truth is returned.
          *        If it starts with one of "false", "no" or is zero, then
          *        false is returned.
          * @param unknown Specifies the return value if the input is
          *        unrecognised.
          */
        static bool string2bool(const std::string & str, bool unknown=false);

        /**
          * sequenceNumberToString
          * 
          * This method converts an integer to a 5-digit leading-zero-padded
          * string eg. 00023
          *
          * @param sequenceNumber The number to convert to string
          * @param str The converted string
          */
        static void sequenceNumberToString( int sequenceNumber, std::string & str );


        /**
          * location2string
          *
          * Converts the given LocationType to a human-readable string
          * 
          * @param location the location to stringify
          * @return the string representation of the location
          */
        static std::string location2string(LocationType location);


        /**
          * string2Location
          *
          * Converts the given string to one of the permissible LocationType
          * enumerated values.
          * @param s the string to convert
          * @return the LocationType value
          */
        static LocationType string2LocationType(const std::string & s);


        /**
          * string2TableAddress
          *
          * Converts the given string to one of the permissible table addresses
          * 
          * @param s the string to convert
          * @param assume_prefix if the string of parameter s does not begin
          *        with either 'PA' or 'Iscs' then this parameter gives the
          *        assumed prefix.
          * @return the tableAddress value, zero (0) if the table name is not
          *         valid.
          */
        static TableIdentifier string2TableIdentifier(const std::string & s, std::string assume_prefix="PA");

        /**
          * Breaks the given string into tokens according to the delimiters.
          * Uses strtok().
          *
          * @note applies an arbitrary limit of 1024 characters to the input
          *       string
          * 
          * @param s the string to tokenise
          * @param tokens the resulting list of strings
          * @param delimiters the delimiters given to strtok
          * @return the number of tokens
          */
        static int tokenise(const char * s, TokenList & tokens, const char * delimiters=" ");


        /**
          * Performs the opposite function of tokenise.  Concatenates the
          * strings in the given list, padding with the optional given string>
          * 
          * @param tokens the list of strings to concatenate
          * @param s the resulting string
          * @param padding the string to pad betwixt each string in the list
          */
        static void detokenise(TokenList & tokens, std::string & s, const char * padding=" ");


        /** expands a range of form "x-y" or "x" or "x-" into a set containing
          * the values x..y inclusive.
          *
          * @param r the string range expression
          * @param range the set to expand the range into
          * @param max the maximum value to expand the range to if no upper
          *            bound is specified in r
          */
        static void expandRange(const std::string & r, RecordRange & range, unsigned int max);


        /** forms a range from the set into the form "x-y" or "x"
          *
          * performs the complement of the expandRange function, turning the
          * values in the set into a string range.  Currently, any discontinuities
          * in the set are ignored, effectively the string becomes
          * "range.min()-range.max()"
          *
          * @param range the set to expand the range into
          * @param r the string range expression
          */
        static void formRange(const RecordRange & range, std::string & r);

        /** getByte
          * retrieves a byte from the network byte stream/vector
          * @param v the ByteVector stream
          * @param i the index within the vector
          * @return the retrieved byte
          */
        static Byte getByte(const ByteVector & v, int & i);


        /** getWord
          * retrieves a word from the network word stream/vector
          * @param v the ByteVector stream
          * @param i the index within the vector
          * @return the retrieved word
          */
        static Word getWord(const ByteVector & v, int & i);


        /** putByte
          * places a byte onto the network byte stream/vector
          * @param b the Byte to place
          * @param v the ByteVector stream
          */
        static void putByte(Byte b, ByteVector & v);


        /** putWord
          * places a word onto the network word stream/vector
          * @param w the Word to place
          * @param v the ByteVector stream
          */
        static void putWord(Word w, ByteVector & v);

		 /** putDWord
          * places a Dword onto the network word stream/vector
          * @param w the DWord to place
          * @param v the ByteVector stream
          */
        static void putDWord(DWord w, ByteVector & v);

        /**
          * string full of readable ascii for filling up tables with.  It
          * happens to be the hex bytes from 0x30 to 0x7E
          */
        static const std::string DataFiller;


        /**
          * random unsigned integer generation, within a given range.
          * @param lo the lower end of the range (default=0)
          * @param hi the upper end of the range (default=UINT_MAX=0xFFFFFFFF)
          * @return a random unsigned integer between 'lo' and 'hi'
          */
        static unsigned int random_uint32(unsigned int lo=0, unsigned int hi=UINT_MAX);


        /**
          * random Word generation, within a given range.
          * @param lo the lower end of the range (default=0)
          * @param hi the upper end of the range (default=USHRT_MAX=0xFFFF)
          * @return a random Word (unsigned short integer) between 'lo' and 'hi'
          */
        static Word randomWord(Word lo=0, Word hi=USHRT_MAX);


        /**
          * random unsigned character generation, within a given range.
          * @param lo the lower end of the range (default=0)
          * @param hi the upper end of the range (default=UCHAR_MAX=0xFF)
          * @return a random Byte (unsigned char) between 'lo' and 'hi'
          */
        static Byte randomByte(Byte lo=0, Byte hi=UCHAR_MAX);


        /** fills the string s with a representation of the given date values
          * as YYMMDDhhmmss.  Useful for the strings in PA Platform, Train and
          * WILD tables.
          *
          * @note Purposely NOT null-terminated
          *
          * @param s the 12 character string to fill
          * @param year the year
          * @param month the month
          * @param day the day
          * @param hour the hour
          * @param minutes the minute
          * @param seconds the seconds
          */
        static void get12ByteDateString(char * s, Word year, Word month, Word day, Word hour, Word minutes, Word seconds);


        /** fills the string s with a representation of the given date values.
          *
          * @note Purposely NOT null-terminated
          * @note Uses strftime() but does not copy the null string to s
          * 
          * @param s the 12 character string to fill
          * @param a_time a pointer to a struct tm with the appropriate values.
          *               If this is zero, the current time will be used.
          * @param random if true, a random time will be provided (default:false)
          */
        static void get12ByteDateString(char * s, struct tm * a_time, bool random=false);

        /** processes a byte, setting the bits according to the 8 characters
          * in the string. Characters have 4 values (hence 4 "levels") :
          * 0 - set the bit to 0
          * 1 - set the bit to 1
          * T - toggle the bit
          * L - leave the bit
          *
          * If a character is not one of those mentioned above, the bit is left
          * (ie. as if the character was 'L')
          *
          * @param original the byte to process
          * @param mask the string mask
          * @return the processed byte
          */
        static Byte process4LevelMask(Byte original, const char mask[8]);


        /** processes a word, setting the bits according to the 16 characters
          * in the string. Characters have 4 values (hence 4 "levels") :
          * 0 - set the bit to 0
          * 1 - set the bit to 1
          * T - toggle the bit
          * L - leave the bit
          *
          * If a character is not one of those mentioned above, the bit is left
          * (ie. as if the character was 'L')
          * 
          * @param original the word to process
          * @param mask the string mask
          * @return the processed word
          */
        static Word process4LevelMask(Word original, const char mask[16]);


        /** converts an w.x.y.z form of branch id to the integer as stored
         * in the PATrain table. Accepts just the integer, also.
         *
         * @param s the original string to convert
         * @return the integer converted to. 0 indicates invalid input
         */
        static Word convertBranchId2IntegerString(const std::string & s, std::string & is);

        /** converts a TableOutputFormat to its human readable string
         *
         * @param f the TableOutputFormat to convert from
         * @return the std::string representation of the format
         */
        static std::string outputFormat2String(TableOutputFormat f);

        /**
          *
          */
        static std::string itoa(int num);

        /**
          *
          */
        static std::string uitoa(unsigned long int num);

    private:
        /**
          * Hidden constructor, as this class is not to be instantiated
          */
        PASimUtility();

        /**
          * Hidden destructor
          */
        ~PASimUtility();

    }; // class PASimUtility



    inline Byte PASimUtility::getByte(const ByteVector & v, int & i)
    {
        // normally i'd have gone with "return v[i++];", using the side effect of
        // the post-increment operator.  However i read that that's dangerously
        // unreliable (goodness knows why) and that the following is the best idea.
        i+=1;
        return v[i-1];
    }


    inline Word PASimUtility::getWord(const ByteVector & v, int & i)
    {
        TwoByteUnion tbu;
        tbu.the_bytes[0] = v[i];
        tbu.the_bytes[1] = v[i+1];
        i+=2;
        return (ntohs(tbu.the_word[0]));
    }


    inline void PASimUtility::putByte(Byte b, ByteVector & v)
    {
        v.push_back(b);
    }


    inline void PASimUtility::putWord(Word w, ByteVector & v)
    {
        TwoByteUnion tbu;
        tbu.the_word[0] = htons(w);
        v.push_back(tbu.the_bytes[0]);
        v.push_back(tbu.the_bytes[1]);
    }
	inline void PASimUtility::putDWord(DWord w, ByteVector & v)
    {
        FourByteUnion tbu;
        tbu.the_long[0] = htonl(w);
        v.push_back(tbu.the_bytes[0]);
        v.push_back(tbu.the_bytes[1]);
		v.push_back(tbu.the_bytes[2]);
        v.push_back(tbu.the_bytes[3]);
    }
} // namespace PA_Sim
} // namespace TA_IRS_App
#endif // #ifndef PA_SIM_UTILITY_H
