/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/IDataPointUpdateObserver.h $
  * @author:	Huang Jian
  * @version:	$Revision: #1 $
  *
  * Last modification:	$DateTime: 2015/02/27 17:12:34 $
  * Last modified by:	$Author: CM $
  *
  * Description:
  *
  *
  */

#if !defined(APP_BAS_IDATAPOINTUPDATENOFIFICATION_H)
#define APP_BAS_IDATAPOINTUPDATENOFIFICATION_H

namespace TA_Base_Bus
{
    struct DataPointControl;
    struct DataPointValue;
}

namespace TA_IRS_App
{
    enum DataPointControlStateType
    {
        E_STATE_SUCCESS,
        E_STATE_FAIL_GENERAL,
        E_STATE_FAIL_TIME_OUT
    };

    class IDataPointUpdateObserver
    {
    public:

        virtual ~IDataPointUpdateObserver(){}

        virtual void processDataPointControlStateUpdate(const unsigned long dpEntityKey, const int nControlState, const TA_Base_Bus::DataPointControl& controlState) = 0;
        virtual void processDataPointValueUpdate(const unsigned long dpEntityKey, const TA_Base_Bus::DataPointValue& updateValue ) = 0;
    };
}

#endif //APP_BAS_IDATAPOINTUPDATENOFIFICATION_H