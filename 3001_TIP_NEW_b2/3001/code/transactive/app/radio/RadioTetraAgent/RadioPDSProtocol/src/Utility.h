/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $Source$
  * @author:  Adam Radics
  * @version: $Revision$
  *
  * Last modification: $Date$
  * Last modified by:  $Author$
  *
  */
#if !defined(UTILITY_H)
#define UTILITY_H

#include "core/defaults/src/DefaultEnvironment.h"

#include <string>
#include <sstream>

namespace
{
    /**
     * Template function for types that can be iterated through, and have a getLogString method
     */
    template< typename ListType > std::string structListToString( const ListType& listType )
    {
        std::ostringstream result;

        for ( typename ListType::const_iterator iter = listType.begin();
              listType.end() != iter; ++iter )
        {
            if ( listType.begin() != iter )
            {
                result << std::endl;
            }

            result << iter->getLogString();
        }

        return result.str();
    }


    /**
     * Template function for types that can be iterated through, and must be cast to an integer
     */
    template< typename IntegerListType > std::string integerListToString( const IntegerListType& integerList )
    {
        std::ostringstream result;

        for ( typename IntegerListType::const_iterator iter = integerList.begin();
              integerList.end() != iter; ++iter )
        {
            if ( integerList.begin() != iter )
            {
                result << ", ";
            }

            result << static_cast< int>( *iter );
        }

        return result.str();
    }


    /**
     * Template function for types that can be iterated through, and can be insert directly to a stream
     */
    template< typename ListType > std::string listToString( const ListType& listType )
    {
        std::ostringstream result;

        for ( typename ListType::const_iterator iter = listType.begin();
              listType.end() != iter; ++iter )
        {
            if ( listType.begin() != iter )
            {
                result << ", ";
            }

            result << *iter;
        }

        return result.str();
    }

}

#endif // !defined(Utility_H)
