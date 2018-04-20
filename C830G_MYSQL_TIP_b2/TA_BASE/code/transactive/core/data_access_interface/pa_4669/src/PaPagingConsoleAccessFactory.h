/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/data_access_interface/pa_4669/src/PaPagingConsoleAccessFactory.h $
  * @author Jade Lee
  * @version $Revision: #3 $
  * Last modification: $DateTime: 2012/03/26 10:45:17 $
  * Last modified by:  $Author: qi.huang $
  * 
  * PaPagingConsoleAccessFactory is a singleton that is used to retrieve PaPagingConsole objects either from the
  * database or newly created.
  */

#if !defined(PaPagingConsoleAccessFactory_0069E71D_600A_4a39_9DDD_AC6ED941238F__INCLUDED_)
#define PaPagingConsoleAccessFactory_0069E71D_600A_4a39_9DDD_AC6ED941238F__INCLUDED_

#include <string>
#include <map>


namespace TA_Base_Core
{
	class PaPagingConsoleAccessFactory
	{
    public:

		static PaPagingConsoleAccessFactory& getInstance();

        static void removeInstance();

        std::map<unsigned short, std::string> getPaPagingConsolesByLocationKey( unsigned long ulLocationKey );

	private:
        // Disable the following methods
		PaPagingConsoleAccessFactory();
		PaPagingConsoleAccessFactory( const PaPagingConsoleAccessFactory& thePaPagingConsoleAccessFactory);
		PaPagingConsoleAccessFactory& operator=(const PaPagingConsoleAccessFactory& );

        /** 
         * ~PaPagingConsoleAccessFactory
         *
         * Destructor for basic class cleanup.
         *
         */
        virtual ~PaPagingConsoleAccessFactory();

    private:
        static PaPagingConsoleAccessFactory* m_instance;
	};

} // closes TA_Base_Core

#endif // !defined(PaPagingConsoleAccessFactory_0069E71D_600A_4a39_9DDD_AC6ED941238F__INCLUDED_)
