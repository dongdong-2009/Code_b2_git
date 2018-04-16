/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/3002/transactive/app/ecs/ECSAgent/src/ItaEcsCachedConfigInitialiser.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ItaEcsCachedConfigInitialiser.h: interface for the ItaEcsCachedConfigInitialiser class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ItaEcsCachedConfigInitialiser_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
#define AFX_ItaEcsCachedConfigInitialiser_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_


namespace TA_Base_Core
{
    class IEntityData;
}

namespace TA_Base_Bus
{
    class DataPoints;
}




class ItaEcsCachedConfigInitialiser
{

public:

  
    ///////////////////////////////////////////////////////////////////////////
    // SetMethods
    ///////////////////////////////////////////////////////////////////////////
    // All setX methods can throw(TA_Base_Core::InvalidECSConfigurationException);
    //
    // Removed setAgentName() as this is now taken from RunParams 
    ///////////////////////////////////////////////////////////////////////////

    virtual void initialiseAllConfigAndMaps(TA_Base_Core::IEntityDataPtr entityData, const DataPoints& datapoints)=0;

    virtual void setEntityData(TA_Base_Core::IEntityDataPtr entityData)=0;
    virtual void setDataPoints(const DataPoints& datapoints)=0;

    virtual void processEntityData()=0;
    virtual void processGlobalParameters()=0;

    // Common
    virtual void setIsMaster(bool param)=0;
    virtual void setIsInControl(bool param)=0;
    virtual void setAgentInRuntime(bool param)=0;

    virtual void setAgentKey(unsigned long param)=0;
    virtual void setAgentTypeKey(unsigned long param)=0;
    virtual void setAgentLocationKey(unsigned long param)=0;
    virtual void setAgentSubsystemKey(unsigned long param)=0;   

    // Station
    virtual void setIsOccOnline(bool param)=0;
    virtual void setOccGroupName(const std::string& param)=0;



};



#endif // !defined(AFX_ItaEcsCachedConfigInitialiser_H__AC46F392_4FE1_4341_B976_D4DBE3CB8EAD__INCLUDED_)
