/**
*
* Source:   $File: //depot/GZL9_TIP/Base/transactive/app/scada/TisAgent/src/CsrzzTrainData.h $
* @author:  Wang Wang
* @version: $Revision: #1 $
*
* Last modification: $DateTime: 2017/04/06 15:30:00 $
* Last modified by:  $Author: wang.wang $
*
*
* TisStandardRtuManager.
*
*/

#ifndef TISTRAINALARMAGENT_TISSTANDARDRTUMANAGER
#define TISTRAINALARMAGENT_TISSTANDARDRTUMANAGER

#include "bus/scada/rtu_library/src/StandardRTUManager.h"

namespace TA_IRS_App
{
    class TisStandardRtuManager: public TA_Base_Bus::StandardRTUManager
    {
    public:
        TisStandardRtuManager(int groupId); // constructor
        virtual ~TisStandardRtuManager(); // destructor

        // override StandardRTUManager function, because normally data point quality is following whole data block quality,
        // but for Tis train data point will follow detailed train quality.
        virtual void dispatchDataBlock(TA_Base_Bus::RTU& rtu, const TA_Base_Bus::DataBlock< WORD > & addressBlock);

    private:
        bool isValidForTrainBlock(const TA_Base_Bus::DataBlock< WORD > & addressBlock, unsigned long startAddr, unsigned long endAddr);

    }; // class TisStandardRtuManager

} // namespace TA_IRS_App

#endif // !TISTRAINALARMAGENT_TISSTANDARDRTUMANAGER
