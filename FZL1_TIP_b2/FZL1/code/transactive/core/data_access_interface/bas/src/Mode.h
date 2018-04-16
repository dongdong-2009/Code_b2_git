#if !defined(TA_3002_transactive_core_data_access_interface_bas_Mode_H)
#define TA_3002_transactive_core_data_access_interface_bas_Mode_H
#if defined(_MSC_VER)
#pragma once
#endif // _MSC_VER

#include <memory>
#include "core/data_access_interface/src/IData.h"

namespace TA_IRS_Core
{
	class ModeHelper;
	class Mode
	{
	public:

		Mode(const unsigned long row, TA_Base_Core::IData& data);

		virtual ~Mode(void);

		virtual unsigned long getKey();

		virtual std::string getName();

		virtual unsigned int getValue();

		virtual std::string getDescription();

		virtual unsigned int getLocationKey();

		virtual unsigned int getSubsystemKey();

		virtual unsigned int getTypeKey();
		
		virtual std::string getLocationName();

		virtual std::string getSubsystemName();

		virtual std::string getTypeName();

		virtual void invalidate();

	private:
		Mode(const Mode&);
		Mode& operator=(const Mode&);

	private:
		std::auto_ptr<ModeHelper> m_helper;

	};
}

#endif //TA_3002_transactive_core_data_access_interface_bas_Mode_H