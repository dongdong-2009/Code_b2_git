/**
  * The source code in this file is the property of ST Electronics and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/GZL9_TIP/GZL9/transactive/app/bas/BASAgent/src/BasModeEquipmentFactory.h $
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
#if !defined(APP_BAS_BASAgent_BasModeEquipmentFactory_H)
#define APP_BAS_BASAgent_BasModeEquipmentFactory_H
#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

#include <string>
#include <vector>
#include <map>
#include "boost/smart_ptr.hpp"
#include "app/bas/BASAgent/src/BasModeEquipment.h"
#include "core/data_access_interface/bas/src/Subsystem.h"

namespace TA_IRS_App
{
    typedef std::map< unsigned long, BasModeEquipment* > AllBasModeEquipments;

    class BasModeEquipmentFactory
    {
    public:
        ~BasModeEquipmentFactory();

    public:
        static BasModeEquipmentFactory& getInstance();

        AllBasModeEquipments getAllBasModeEquipments();

    private:
        BasModeEquipmentFactory();
        BasModeEquipmentFactory(const BasModeEquipmentFactory&);
        BasModeEquipmentFactory& operator=(const BasModeEquipmentFactory&);
    private:
        static BasModeEquipmentFactory* m_instance;
    };
}
#endif //APP_BAS_BASAgent_BasModeEquipmentFactory_H
