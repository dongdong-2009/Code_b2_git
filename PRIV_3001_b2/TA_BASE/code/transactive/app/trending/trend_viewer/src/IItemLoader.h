#if !defined(IItemLoader_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_)
#define IItemLoader_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_

/**
  * IItemLoader.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/TA_BASE/transactive/app/trending/trend_viewer/src/IItemLoader.h $
  * @author:  Craig Grant
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  * 
  * This class provides a contract between a object that stores items and an actual Item Loader.
  * The storage class will call methods on this interface to retrieve items from the database.
  *
  */

namespace TA_Base_App
{
    //
    // IItemLoader
    //
    class IItemLoader
    {

    public:

        /**
          * ~IItemLoader
          * 
          * Destructor.
          */
        virtual ~IItemLoader() {}

        /**
          * retrieveFromDatabase
          * 
          * This method executes the sql query to retrieve the items from the database.
          */
        virtual void retrieveFromDatabase() =0;
    };
}

#endif // !defined(IItemLoader_1666D572_B1E2_4e76_8AD1_5C747DEDE6AF__INCLUDED_)
