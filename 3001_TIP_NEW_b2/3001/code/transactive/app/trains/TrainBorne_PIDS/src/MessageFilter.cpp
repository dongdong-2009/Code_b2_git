#include "StdAfx.h"
#include "MessageFilter.h"
#include <string>
#include <algorithm>

namespace TA_IRS_App
{
    MessageFilter::MessageFilter( const std::string& filter )
        : m_filterString( filter )
    {
    }

    bool MessageFilter::operator()( const TA_Base_Core::PredefinedMessage& message )
    {
        //Transform to upper case
        /*
        std::string tmpFilter(m_filterString.size());
        std::string
        if(message.description.find(m_filterString) != std::string::npos)
        	return true;
        return false;
        */
        return upperCaseCmp( message.description );
    }
    bool MessageFilter::upperCaseCmp( const std::string& beFilter )
    {
        std::string tmpbeFilter( beFilter );
        std::transform( tmpbeFilter.begin(), tmpbeFilter.end(), tmpbeFilter.begin(), ::toupper );
        std::string tmpFilter( m_filterString );
        std::transform( tmpFilter.begin(), tmpFilter.end(), tmpFilter.begin(), ::toupper );

        if( tmpbeFilter.find( tmpFilter ) != std::string::npos )
            return true;
        return false;
    }
}