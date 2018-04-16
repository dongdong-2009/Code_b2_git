#if !defined(ILoaderObserver_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_)
#define ILoaderObserver_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_

/**
  * ILoaderObserver.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/trending/trend_viewer/src/ILoaderObserver.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class provides a contract between any class that needs to retrieve items from the
  * database and an actual Loader Observer. Any class retrieving items that want to be notified
  * when the load is complete needs to implement this interface.
  *
  */

#include <string>

namespace TA_Base_App
{
    //
    // ILoaderObserver
    //
    class ILoaderObserver
    {

    public:

        /**
          * ~ILoaderObserver
          * 
          * Destructor.
          */
        virtual ~ILoaderObserver() {}

        /**
          * loadingComplete
          * 
          * This method is called once all items have been loaded from the database.
          */
        virtual void loadingComplete() =0;

        /**
          * processError
          * 
          * This method is called if there has been a database loading error.
          *
          * @param error - error message
          */
        virtual void processError(const std::string& error) =0;
    };
}

#endif // !defined(ILoaderObserver_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_)
