#if !defined(TA_3002_transactive_core_data_access_interface_bas_SubsystemAccessFactory_H)
#define TA_3002_transactive_core_data_access_interface_bas_SubsystemAccessFactory_H
#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

#include <string>
#include <vector>
#include <map>
#include "boost/smart_ptr.hpp"
#include "core/data_access_interface/bas/src/Subsystem.h"

namespace TA_IRS_Core
{
	typedef boost::shared_ptr<TA_IRS_Core::Subsystem> SubSystem_SmartPtr;
	typedef std::vector<SubSystem_SmartPtr> LocationSubsystem;
	typedef std::map<unsigned long,LocationSubsystem> AllLocationSubsystems;
	class SubsystemAccessFactory
	{
	public:
		~SubsystemAccessFactory();

	public:
		static SubsystemAccessFactory& getInstance();

        unsigned long getAllSubSystemKey();

		AllLocationSubsystems getAllSubsystems();
		
		bool getAllSubsystems(AllLocationSubsystems& subSystems);
	protected:
		SubsystemAccessFactory();

	private:
		SubsystemAccessFactory(const SubsystemAccessFactory&);
		SubsystemAccessFactory& operator=(const SubsystemAccessFactory&);
	private:
		static SubsystemAccessFactory* m_instance;
	};
}
#endif //TA_3002_transactive_core_data_access_interface_bas_SubsystemAccessFactory_H
