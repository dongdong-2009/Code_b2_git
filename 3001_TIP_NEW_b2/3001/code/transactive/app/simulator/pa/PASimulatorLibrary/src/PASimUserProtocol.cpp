/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  * 
  * Definition/Implementation file for PASimUserProtocol classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/simulator/pa/PASimulatorLibrary/src/PASimUserProtocol.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimUtility.h"
#include "app/simulator/pa/PASimulatorLibrary/src/PASimTableAddressRegister.h"
#include <limits.h>
#include <iostream>
#include <errno.h>

using namespace TA_IRS_App::PA_Sim;

//
// For strict ansi compliance a declaration is needed prior to definition

UserProtocol::UserProtocol(MessageType type)
: m_type(type)
, m_tableAddress(TableAddressRegister::INVALID_ADDRESS)
{
}

UserProtocol::~UserProtocol()
{
}


MessageType UserProtocol::getType() const
{
    return m_type;
}

unsigned short UserProtocol::getTableAddress() const
{
    return m_tableAddress;
}

void UserProtocol::setTableAddress(unsigned short tAddr)
{
    m_tableAddress = tAddr;
}

void UserProtocol::setType(MessageType type)
{
    m_type = type;
}

UserQuery::UserQuery(MessageType type, TableOutputFormat format)
: UserProtocol(type)
, m_outputFormat(format)
, m_startAddress(-1)
, m_endAddress(-1)
, m_timeStamp(0)
{
}


UserQuery::~UserQuery()
{
}


TableOutputFormat UserQuery::getOutputFormat() const
{
    return m_outputFormat;
}

void UserQuery::setOutputFormat(TableOutputFormat format)
{
    m_outputFormat = format;
}

int UserQuery::setRecord( unsigned long recordNumber )
{
    m_records.insert( recordNumber );

	return m_records.size();
}


int UserQuery::setRecords( const RecordRange & records )
{
    for ( RecordRange::const_iterator iter = records.begin() ;
          iter != records.end() ;
          iter++
        )
    {
        m_records.insert( *iter );
    }

	return m_records.size();
}


int UserQuery::setRecords( const std::string & recordstr, unsigned long max )
{
    PASimUtility::expandRange(recordstr, m_records, max);

	return m_records.size();
}


int UserQuery::removeRecords( const RecordRange & records )
{
    for ( RecordRange::const_iterator iter = records.begin() ;
          iter != records.end() ;
          iter++
        )
    {
        m_records.erase( *iter );
    }

	return m_records.size();
}

void UserQuery::pushArg( const FieldValuePair & fieldvalue )
{
    m_arguments.push( fieldvalue );
}


void UserQuery::pushArg( unsigned long field, const FieldValueType & value )
{
    FieldValuePair fieldvalue(field, value);
    pushArg(fieldvalue);
}


void UserQuery::copyArgs(const ArgumentList & arglist)
{
    for (   ArgumentList::const_iterator iter = arglist.begin() ;
            iter != arglist.end() ;
            iter++
        )
        // while ( !q.empty() )
    {
        pushArg( *iter );
    }
}


unsigned long UserQuery::str2ul(const char * s, std::ostream & err, bool & ok)
{
    errno = 0;
    char * endptr = 0;
    ok = false;
    unsigned long ul = ::strtoul(s, &endptr, 10);
    // int errno_copy = errno;
    if (    (errno == ERANGE)
         || ((endptr!=0) && (*endptr != '\0'))
         || (endptr == s)
       )
    {
        //TODO log a warning
        err << "WARNING: could not convert field to an integer: " << s << std::endl;
    }
    else
    {
        ok = true;
    }

    return ul;
}

UserQuery::FieldValueType UserQuery::str2FieldValueType(const char * s, std::ostream & err, bool & ok)
{
    errno = 0;
    char * endptr = 0;
    ok = false;
    unsigned long ul = ::strtoul(s, &endptr, 0); // 
    // int errno_copy = errno;
    if (    (errno == ERANGE)                   // not in range [0..ULONG_MAX]
         || ((endptr!=0) && (*endptr != '\0'))  // unconverted characters remain
         || (endptr == s)                       // nothing converted
         || (ul > USHRT_MAX)                    // Word is an unsigned short
       )
    {
        //TODO log a warning
        err << "WARNING: could not convert field to a Word value: " << s << std::endl;
    }
    else
    {
        ok = true;
    }

    return ul;
}

int UserQuery::parseArgs
(   const TokenList & args,
    std::ostream & err )
{
    std::string s;
    TokenList tokens;
    TokenList::const_iterator argiter = args.begin();
    bool ok = false;

    while ( argiter != args.end() )
    {
        s = *argiter;
        tokens.clear();
        PASimUtility::tokenise(s.c_str(), tokens, "=");

        if (tokens.size() == 2)
        {

            std::string fieldstr = tokens.front();
            tokens.pop_front();

            unsigned long field = str2ul( fieldstr.c_str(), err, ok );

            if ( ok )
            {
                std::string valuestr = tokens.front();
                tokens.pop_front();

                FieldValueType value = str2FieldValueType( valuestr.c_str(), err, ok );

                if ( ok )
                {
                    pushArg(field, value);
                }
            }
        }
        else
        {
            //TODO log a warning
            err << "WARNING: Could not parse. Expected \"field=value\" pair, got: " << s << std::endl;
        }

        argiter++;
    }

    return m_arguments.size();
}

UserQuery::FieldValuePair UserQuery::popArg()
{
    FieldValuePair fvp(InvalidFieldIndicator,0);
    
    if (!m_arguments.empty())
    {
        fvp = m_arguments.front();
        m_arguments.pop();
    }

    return fvp;
}


bool UserQuery::moreArgs()
{
    return (!(m_arguments.empty()));
}


const RecordRange & UserQuery::getRecords() const
{
    return m_records;
}



UserResponse::UserResponse(UserQuery * query)
: UserProtocol(query->getType())
{
	setTableAddress(query->getTableAddress());
    m_responseData = "";
}
