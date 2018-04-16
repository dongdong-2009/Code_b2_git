#if !defined (TA_3002_App_Bas_BasManager_BasManagerSubscriber_H)
#define TA_3002_App_Bas_BasManager_BasManagerSubscriber_H
#pragma once

#include "boost/shared_ptr.hpp"
#include "app/bas/BASModeManager/src/FireRecoverEquipment.h"

Namespace_TA_IRS_APP_Begin
class FireRecoverEquipment;
class Equipment;
class BasManagerSubscriber 
{
public:
	friend class Equipment;
	BasManagerSubscriber(void);
	~BasManagerSubscriber(void);
public:
	// subscribe the message
	void startSubscriber(unsigned long locationKey);

	// unsubscribe the message
	void stopSubscriber();

	int getSubsystemAutoCfgValue(unsigned long locationKey, unsigned long subsystemKey);
	void setSubsystemAutoCfgValue(unsigned long locationKey, unsigned long subsystemKey, const int nValue);

	void setRecoverFromFireMode(unsigned long locationKey);
protected:
	void updateModeStatus(unsigned long locationKey, unsigned long modeKey, unsigned long subsystemKey, EquipmentValueType valueType, double value, int status);
private:
	typedef boost::shared_ptr<Equipment> Equipment_SmartPtr;
	typedef boost::shared_ptr<FireRecoverEquipment> FireRecoverEquipment_SmartPtr;
	typedef std::vector<Equipment_SmartPtr> LocationEquipmentVct;
	typedef std::map<unsigned long, LocationEquipmentVct> LocationEquipmentVctMap;	
	LocationEquipmentVctMap m_environments;

	std::map<unsigned long, FireRecoverEquipment_SmartPtr> m_fireRecoverEquipment;
};
typedef boost::shared_ptr<BasManagerSubscriber> BasManagerSubscriber_SmartPtr;
Namespace_TA_IRS_APP_End
#endif //TA_3002_App_Bas_BasManager_BasManagerSubscriber_H
