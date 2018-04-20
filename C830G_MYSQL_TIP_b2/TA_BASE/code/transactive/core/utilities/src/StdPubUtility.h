/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/CODE_4677_T00060500/transactive/app/maintenance_management/mms_agent/src/mq/PubFunction.h $
  * @author:  xinsong xia
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2006/12/08 11:20:56 $
  * Last modified by:  $Author: buildteam $
  * 
  * This class privide some static public function for MMS Agent, those function will be refined to form a public class library 
  */

#if !defined(AFX_STD_PUBUTILITY_H__1E5CCA2B_9CF1_4096_A15A_492220F783D6__INCLUDED_)
#define AFX_STD_PUBUTILITY_H__1E5CCA2B_9CF1_4096_A15A_492220F783D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <assert.h>
#include <wchar.h>
#include <time.h>
#include <vector>
#include <list>
#include <map>
#include <algorithm>

namespace TA_Base_Core
{

	class CStdPubUtility  
	{
	public:
		CStdPubUtility(){};
		virtual ~CStdPubUtility(){};
	public:
		inline static bool isEmptyString( const char * szText )
		{
			return ( szText == 0x0 || *szText == 0x0 );
		}

		static bool isEmptyString( const wchar_t * wszText )
		{
			return ( wszText == NULL || *wszText == 0x0 );
		}

		static bool isRealSpace( wchar_t ch )
		{
			if ( L'\t' == ch || L' ' == ch )
				return true;
			return false;
		}

		static bool isRealSpace( char ch )
		{
			if ( '\t' == ch || ' ' == ch )
				return true;
			return false;
		}

		static bool isSpace( char ch )
		{
			if ( '\r' == ch || '\t' == ch || '\n' == ch || ' ' == ch )
				return true;
			return false;

		}

		static bool isSpace(wchar_t ch)
		{
			if ( L'\r' == ch || L'\t' == ch || L'\n' == ch || L' ' == ch )
				return true;
			return false;
		}

		static char * ltrim( char * szSourceText );
		static wchar_t * ltrim( wchar_t * wszSourceText );

		static char * trim( char * szSourceText );
		static wchar_t * trim( wchar_t * wszSourceText );
		static wchar_t * rtrim( wchar_t * wszSourceText );



		static char * removeSpeicalCharacter( const char * szSourceText , char * szDestBuffer, char chSpecialChar );

		static wchar_t * removeSpeicalCharacter( const wchar_t * wszSourceText , wchar_t * wszDestBuffer, wchar_t chSpecialChar );


		inline static std::string getLimitedString( std::string & inputStr, unsigned int maximumLength, bool allowTruncate = true )
		{
			if ( allowTruncate && inputStr.length() > maximumLength )
			{
				std::string tempStr = inputStr.substr(0, maximumLength);
				assert( tempStr.length() == maximumLength );
				return tempStr;
			}
			else
			{
				return inputStr;
			}
		}

		static std::string getCurrentDateTimeStr( )
		{
			time_t long_time;
			time( &long_time ); // Get time as long integer.
			return ctime( &long_time ); 
		}

		static void string_replace( std::string & strSource, const std::string & strOld, const std::string &strNew);

		static void trimString( std::string & strSource );

		static void copyWString(wchar_t ** szDestWord, const wchar_t * szSourceWord);
		
		static wchar_t * mergeWString(wchar_t  *szDestResult , wchar_t * szSourceResult , unsigned int * nRemainSize, unsigned int nInitSize);

		static char * cutToToken( char ** szText);

		static wchar_t * seperateText(wchar_t ** wszText, wchar_t wch);
        static char * seperateText(char ** szText, char ch);
		
		static wchar_t * cutToToken(wchar_t ** wszText);

		static bool readWholeFileContent( const char * fileName, bool shouldTrim, std::string & outputStr );

		static bool pathExists( const char * pathName, std::string & reason );

		static void toUpper( char * szSource );

		static void toLower( char * szSource );

        static int stricmp(const char* string1, const char* string2) 
        {
            const char* ch1 = string1;
            const char* ch2 = string2;
            int res;

            while (true) 
            {
                res = toupper(*ch1) - toupper(*ch2);
                if (res)
                    return res;
                else
                    if (!*ch1)
                        return 0;

                ch1++;
                ch2++;
            }
        }
	};
	
};

#endif // !defined(AFX_PUBFUNCTION_H__1E5CCA2B_9CF1_4096_A15A_492220F783D6__INCLUDED_)
