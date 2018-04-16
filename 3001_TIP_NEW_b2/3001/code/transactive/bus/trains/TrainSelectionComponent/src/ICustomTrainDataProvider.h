/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/3001/transactive/bus/trains/TrainSelectionComponent/src/ICustomTrainDataProvider.h $
  * @author:   Adam Radics
  * @version:  $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  * The interface that must be implemented by GUIs using the
  * TrainListCtrl, where the train list has custom data columns.
  *
  * For example, the video display manager provides data for one of the columns in its train list.
  * THis is how the list queries for that data,
  */

#if !defined(ICustomTrainDataProvider_H)
#define ICustomTrainDataProvider_H


#include <string>

namespace TA_IRS_Bus
{
    class ICustomTrainDataProvider
    {

    public:

        /** 
          * getCustomData
          *
          * Return the value that should populate 'columnName' for
          * the given train.
          *
          * @param trainId  The train the data is needed for
          * @param columnName   The column to populate.
          *
          */
        virtual std::string getCustomData( unsigned int trainId,const std::string& columnName) = 0;

    };

} // TA_IRS_Bus

#endif // !defined(ICustomTrainDataProvider_H)
