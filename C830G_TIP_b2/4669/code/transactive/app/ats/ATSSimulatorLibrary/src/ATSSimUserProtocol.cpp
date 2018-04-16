/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/4669/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  * 
  * Definition/Implementation file for ATSSimUserProtocol classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableAddressRegister.h"
#include <limits.h>
#include <iostream>
#include <errno.h>

using namespace TA_IRS_App::ATS_Sim;

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
    ATSSimUtility::expandRange(recordstr, m_records, max);

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
        ATSSimUtility::tokenise(s.c_str(), tokens, "=");

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
