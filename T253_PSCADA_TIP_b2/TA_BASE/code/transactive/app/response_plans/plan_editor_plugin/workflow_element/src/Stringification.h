#pragma once

#include <list>
#include <string>
#include <sstream>

class Stringification
{
public:
    Stringification( const std::string& strSource );
    Stringification();
    ~Stringification();

    std::string str();

    template<typename BasicType>
    void serialize( BasicType& oSrc );

    template<typename BasicType>
    void serialize( std::list<BasicType>& lstSrc );

    void serialize( std::list<std::string>& lstSrc );

    template<typename ObjectType>
    void serializeListObject( std::list<ObjectType*>& lstSrc );

private:
    std::stringstream m_ioStream;
    bool m_bInverse;
};

template<typename BasicType>
void Stringification::serialize( BasicType& oSrc )
{
    if ( m_bInverse )
    {
        m_ioStream >> oSrc;
    }
    else
    {
        static const char Delimiter = ' ';
        m_ioStream << oSrc << Delimiter;
    }
}

template<typename BasicType>
void Stringification::serialize( std::list<BasicType>& lstSrc )
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

            BasicType oNew;
            osLine >> oNew;
            lstSrc.push_back( oNew );
        }
    }
    else
    {
        std::ostringstream osInput;
        std::list<BasicType>::iterator itLoop = lstSrc.begin();

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

template<typename ObjectType>
void Stringification::serializeListObject( std::list<ObjectType*>& lstSrc )
{
}