/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4669_T01271350/TA_BASE/transactive/core/data_access_interface/sound_manager/src/SoundCategoryAccessFactory.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2008/11/28 16:26:01 $
  * Last modified by:  $Author: builder $
  *
  * SoundCategoryAccessFactory is a singleton that is used
  * to create ISoundCategory and IConfigSoundCategory objects.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable:4786)
#endif

#include "SoundCategoryAccessFactory.h"
#include "SoundCategory.h"

#include "core/data_access_interface/src/IDatabase.h"
#include "core/data_access_interface/src/DatabaseFactory.h"

#include <string>
#include <vector>
#include <sstream>

namespace TA_Base_Core
{

    SoundCategoryAccessFactory* SoundCategoryAccessFactory::m_instance = 0;


	SoundCategoryAccessFactory& SoundCategoryAccessFactory::getInstance()
	{
        if (m_instance == NULL)
        {
            m_instance = new SoundCategoryAccessFactory();
        }
        return *m_instance;
	}


	std::vector<ISoundCategory*> SoundCategoryAccessFactory::getAllSoundCategories()
	{
        // Get a connection to the database
        IDatabase* databaseConnection = 
                TA_Base_Core::DatabaseFactory::getInstance().getDatabase(Sound_Cd, Read);

        // Get all of the sound category primary keys.
		std::ostringstream sql;
        sql << "SELECT soscat_id FROM so_sound_category";

        // Set up the columnNames vector to be passed to executeQuery()
		std::vector<std::string> columnNames;
		std::string pkeyColumn = "soscat_id";
        columnNames.push_back(pkeyColumn);

        // Execute the query. 
        IData* data = databaseConnection->executeQuery(sql.str(), columnNames);

        // Create a vector of ISoundCategory pointers, and populate it.
        std::vector<ISoundCategory*> result;
        int numRows = data->getNumRows();
        for (int i = 0; i < numRows; i++)
        {
            result.push_back(new SoundCategory( data->getUnsignedLongData(i, pkeyColumn) ));
        }
        return result;
	}


    ISoundCategory* SoundCategoryAccessFactory::getSoundCategory( const unsigned long key )
    {
        return new SoundCategory( key );
    }


    IConfigSoundCategory* createSoundCategory()
	{
		// TODO.
		return NULL;
	}

} // TA_Base_Core
