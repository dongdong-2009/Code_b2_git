#include "Stringification.h"

Stringification::Stringification() :
m_ioStream( std::ios_base::out ),
m_bInverse( false )
{
}

Stringification::Stringification( const std::string& strSource ) :
m_ioStream( strSource, std::ios_base::in ),
m_bInverse( true )
{
}

Stringification::~Stringification()
{
}

std::string Stringification::str()
{
    return m_ioStream.str();
}

void Stringification::serialize( std::list<std::string>& lstSrc )
{
    static const char Delimiter = ',';

    if ( m_bInverse )
    {
        std::string strList = "";
        serialize(strList);

        std::istringstream isLines( strList );
        lstSrc.clear();

        std::string strLine = "";

        while ( !isLines.eof() )
        {
            if ( isLines.bad() )
            {
                break;
            }

            std::getline( isLines, strLine, Delimiter );

            std::istringstream osLine( strLine );

            std::string oNew;
            osLine >> oNew;
            lstSrc.push_back( oNew );
        }
    }
    else
    {
        std::ostringstream osInput;
        std::list<std::string>::iterator itLoop = lstSrc.begin( );

        if ( lstSrc.end() != itLoop )
        {
            osInput << *itLoop;
            ++itLoop;
        }

        for ( ; lstSrc.end() != itLoop; ++itLoop )
        {
            osInput << Delimiter << *itLoop;
        }

        serialize( osInput.str() );
    }
}
