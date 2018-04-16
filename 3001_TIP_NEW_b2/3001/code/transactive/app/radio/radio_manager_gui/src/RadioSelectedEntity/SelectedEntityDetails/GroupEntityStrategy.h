#pragma once

#include "bus\radio\radio_bus\src\radiotypes.h"

typedef std::vector <RadioResource> RadioResourceCollection;
typedef std::vector <std::string> GroupNameCollection;

class GroupEntityStrategy
{
	public:
		GroupEntityStrategy(void);
		~GroupEntityStrategy(void);
		
	protected:
		virtual bool isDescriptionAlreadyExist (const std::string & strDesc,
												RadioResourceType radioGrpType);

		virtual void setFlagIfNewEntry (RadioResource radioResource);
		virtual bool isNewEntry ();


	private:
		GroupNameCollection getAllGroupName(RadioResourceCollection radioGrpCollection);
		bool m_bNewEntry;
		RadioResource m_resourceBeingEdit;
};
