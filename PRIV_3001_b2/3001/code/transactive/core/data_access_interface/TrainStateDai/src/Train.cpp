/**
  *
  *
  *
  * Implementation of the Train class.
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"

#include "core/data_access_interface/TrainStateDai/src/Train.h"


namespace TA_IRS_Core
{    
    /**
	  * The following constants represent user-friendly version
      * of the columns names in TR_TRAIN .
      */
  
    static const std::string TRAINID("TRAIN ID");
    static const std::string PRIMARYTSI("PRIMARY TSI");
    static const std::string SECONDARYTSI("SECONDARY TSI");
    


    unsigned long Train::s_nextAvailableIdentifier = 0;
    
    //
    // Constructors and destructor
    //

    Train::Train()
    :   m_trainHelper(new TrainHelper()),
        m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
        FUNCTION_ENTRY("Train ctor()");
        FUNCTION_EXIT;
    }
    
    Train::Train(const Train& theTrain)
    :   m_trainHelper(new TrainHelper(*theTrain.m_trainHelper)),
        m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
        FUNCTION_ENTRY("Train copy ctor");
        FUNCTION_EXIT;
    }
    
    Train::Train(const unsigned long key)
    :   m_trainHelper(new TrainHelper(key)),
        m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
        FUNCTION_ENTRY("Train ctor(const unsigned long)");
        FUNCTION_EXIT;
    }
    
    Train::Train(const unsigned long row, TA_Base_Core::IData& data)
    :   m_trainHelper(new TrainHelper(row, data)),
        m_uniqueIdentifier(++s_nextAvailableIdentifier)
    {
        FUNCTION_ENTRY("Train ctor(const unsigned long, TA_Base_Core::IData&)");
        FUNCTION_EXIT;
    }

    Train::~Train()
    {  
        FUNCTION_ENTRY("Train dtor");
        // m_trainHelper will auto cleanup
        FUNCTION_EXIT;
    }

    //
    // ITrain implementation (public)
    //

	void Train::setTrainId(unsigned long id)
    {
        FUNCTION_ENTRY("setTrainId");
        
        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

        // convert ids to a string
        std::ostringstream originalTrainId, newTrainId;
        originalTrainId << m_trainHelper->getTrainId();
        newTrainId << id;
	

        updateTrainChanges(TRAINID, originalTrainId.str(), newTrainId.str());
        m_trainHelper->setTrainId(id);

        FUNCTION_EXIT;
    }

	void Train::setPrimaryTsi(const std::string& primaryTsi)
    {
        FUNCTION_ENTRY("setPrimaryEmuId");
        
        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");
		
		std::ostringstream originalPrimaryTsi, newPrimaryTsi;
        originalPrimaryTsi << m_trainHelper->getPrimaryTsi();
		newPrimaryTsi << primaryTsi;

        updateTrainChanges(PRIMARYTSI, originalPrimaryTsi.str(), newPrimaryTsi.str());
        m_trainHelper->setPrimaryTsi(primaryTsi);

        FUNCTION_EXIT;
    }

	void Train::setSecondaryTsi(const std::string& secondaryTsi)
    {
        FUNCTION_ENTRY("setSecondaryEmuId");

        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

		std::ostringstream originalSecondaryTsi, newSecondaryTsi;
        originalSecondaryTsi << m_trainHelper->getSecondaryTsi();
		newSecondaryTsi << secondaryTsi;

        updateTrainChanges(SECONDARYTSI, originalSecondaryTsi.str(), newSecondaryTsi.str());
        m_trainHelper->setSecondaryTsi(secondaryTsi);

        FUNCTION_EXIT;
    }

	unsigned long Train::getTrainId()
    {
        FUNCTION_ENTRY("getTrainId");

        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

        FUNCTION_EXIT;
        return m_trainHelper->getTrainId();
    }
	std::string Train::getPrimaryTsi()
	{
        FUNCTION_ENTRY("getPrimaryTsi");
		
        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");
		
        FUNCTION_EXIT;
        return m_trainHelper->getPrimaryTsi(); 
    }
	
	std::string Train::getSecondaryTsi()
	{
        FUNCTION_ENTRY("getSecondaryTsi");
		
        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");
		
        FUNCTION_EXIT;
        return m_trainHelper->getSecondaryTsi();
    }
    void Train::deleteThisObject()
    {
        FUNCTION_ENTRY("deleteThisObject");

        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

        // Delete the Train from the database
        m_trainHelper->deleteTrain();
        
        // this is equivalent to m_trainHelper.reset(), VC++ STL missing this feature
        m_trainHelper = std::auto_ptr<TrainHelper>(NULL);

        FUNCTION_EXIT;
    }

    //
    // IItem implementation
    //

    unsigned long Train::getKey()
    {
        FUNCTION_ENTRY("getKey");

        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

        FUNCTION_EXIT;
		//It is not use already
        //return m_trainHelper->getKey();
		return 0;
    }

    std::string Train::getName()
    {
        FUNCTION_ENTRY("getName");

        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

        std::ostringstream name;
		name << "Train id " << static_cast<unsigned short>(m_trainHelper->getTrainId());

        FUNCTION_EXIT;
        return std::string(name.str());
    }

    void Train::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

        m_trainHelper->invalidate();
        m_itemChanges.clear();

        FUNCTION_EXIT;
    }

    //
    // IConfigItem implementation
    //

    unsigned long Train::getUniqueIdentifier()
    {
        FUNCTION_ENTRY("getUniqueIdentifier");

        FUNCTION_EXIT;
        return m_uniqueIdentifier;
    }

    bool Train::hasChanged()
    {
        FUNCTION_ENTRY("hasChanged");

        FUNCTION_EXIT;
        return ! m_itemChanges.empty();
    }

    bool Train::isNew()
    {
        FUNCTION_ENTRY("isNew");

        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

        FUNCTION_EXIT;
        return m_trainHelper->isNew();
    }
    
    void Train::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");

        TA_ASSERT(false, "setName() not implemented!");

        FUNCTION_EXIT;
    }

    void Train::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

        m_trainHelper->writeTrainData();
        m_itemChanges.clear();

        FUNCTION_EXIT;
    }

    TA_Base_Core::ItemChanges Train::getAllItemChanges()
    {
        FUNCTION_ENTRY("getAllItemChanges");
        
        FUNCTION_EXIT;
        return m_itemChanges;
    }

    //
    // ITrain implementation (private)
    //

    void Train::updateTrainChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateTrainChanges");

        // Search for this name in the map
        TA_Base_Core::ItemChanges::iterator matching = m_itemChanges.find(name);

        if (matching != m_itemChanges.end())
        {
            // This item is already listed, i.e. it has been changed before

            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old (original) value so we might as well 
                // remove this occurrence from the list as it is not a valid change
                m_itemChanges.erase(matching);
            }
            else
            {
                //Update the stored new value.
                matching->second.newValue = newValue;
            }
        }        
        else if (oldValue != newValue)
        {
            // The item has not been changed before and its new value is not equal to 
            // the original value. Therefore we must save the occurrence of this change.

            TA_Base_Core::Values values;

            values.newValue = newValue;
            values.oldValue = oldValue;

            m_itemChanges.insert(TA_Base_Core::ItemChanges::value_type(name, values));
        }

        FUNCTION_EXIT;
    }

	time_t Train::getDateCreated()
	{
		FUNCTION_ENTRY("getDateCreated");
		TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

		FUNCTION_EXIT;
		return m_trainHelper->getDateCreated();
	}

	time_t Train::getDateModified()
	{
		FUNCTION_ENTRY("getDateModified");
		TA_ASSERT(m_trainHelper.get() != NULL,"The m_trainHelper pointer is null.");

		FUNCTION_EXIT;
		return m_trainHelper->getDateModified();
	}

}