#include "SchematicStepEditorHelper.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/IScadaDisplay.h"
#include <boost/foreach.hpp>

namespace TA_Base_App
{


	const unsigned int SchematicStepEditorHelper::INVALID_ID = -1;


	SchematicStepEditorHelper::SchematicStepEditorHelper()
	{
		std::vector<TA_Base_Core::IScadaDisplay*> schematics;
		try
		{
			schematics = TA_Base_Core::ScadaDisplayAccessFactory::getInstance().getAllScadaDisplays();
		}
		catch(...)
		{
			// TODO
		}
		unsigned int id = 1;
		BOOST_FOREACH(TA_Base_Core::IScadaDisplay* i, schematics)
		{
			m_schematics.insertItem(id++, i->getPath());
			delete i;
		}
	}

	SchematicStepEditorHelper::~SchematicStepEditorHelper()
	{
	}

	std::vector<ListItem> SchematicStepEditorHelper::getAllItems()
	{
		return m_schematics.getAllItems();
	}

	std::string SchematicStepEditorHelper::getSchematicById( unsigned int id )
	{
		return m_schematics.getItemById(id);
	}

	unsigned int SchematicStepEditorHelper::getIdBySchematic( const std::string& name )
	{
		return m_schematics.getIdByItem(name);
	}

}
