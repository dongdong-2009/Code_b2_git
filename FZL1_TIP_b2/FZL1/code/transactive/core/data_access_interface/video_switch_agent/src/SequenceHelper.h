#if !defined(SequenceHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
#define SequenceHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/GZL6_TIP/3002/transactive/core/data_access_interface/video_switch_agent/src/SequenceHelper.h $
  * @author Katherine Thomson
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  * 
  * SequenceHelper is an object that is held by Sequence and ConfigSequence objects. 
  * It contains all data used by Sequences, and manipulation
  * methods for the data. It helps avoid re-writing code for both Sequence and ConfigSequence.
  *
  * Note: The dwell time for Sequences can be retrieved/set using the 
  * Sequence object accessed through the EntityAccessFactory.
  */

#ifdef _MSC_VER
#pragma warning( disable : 4786 )
#endif // _MSC_VER

#include <map>
#include <climits>
#include <memory>

#include "core/data_access_interface/entity_access/src/Sequence.h"

namespace TA_IRS_Core
{
    class SequenceHelper
    {
    public:
        /**
         * SequenceHelper
         * 
         * This constructor creates a new SequenceHelper for configuration.
         */

        SequenceHelper();
   
       /**
         * SequenceHelper
         * 
         * This constructor creates a new SequenceHelper for the specified key.
         *
         * @param       unsigned long sequenceKey
         *              The database (entity) key for this Sequence.
         */

        SequenceHelper( unsigned long sequenceKey );

        /**
          * SequenceHelper
          *
          * Standard copy constructor.
          */

        SequenceHelper( const SequenceHelper& theSequenceHelper );

        /**
         * ~SequenceHelper
         *
         * Standard destructor.
         */

        virtual ~SequenceHelper() {};

        /**
         * getKey
         *
         * Returns the Sequence key for this Sequence entry.
         *
         * Preconditions:   Either - this Sequence entry was initially 
         *                           loaded from the database
         *                  OR     - writeSequenceData() has already been called
         *
         * @return      unsigned long
         *              The key for this Sequence.
         */

	    unsigned long getKey();

        /**
         * getVideoInputs
         *
         * Returns the VideoInput keys and their positions in the Sequence.
         *
         * Preconditions:   Either - this Sequence entry was initially 
         *                           loaded from the database
         *                  OR     - writeSequenceData() has already been called
         *
         * @return      std::map< unsigned short, unsigned long >
         *              The VideoInput keys referenced by their position in the Sequence.
         */

	    std::map< unsigned short, unsigned long > getVideoInputs();

        /**
         * setVideoInputs
         *
         * Sets the VideoInputs and their positions for this Sequence.
         * This should only be used by the ConfigSequence class.
         *
         * @param       const std::map< unsigned short, unsigned long >& videoInputs
         *              The VideoInput keys referenced by their position in the Sequence.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException 
         *              Thrown if there are multiple entries for the same position.
         */

        void setVideoInputs( const std::map< unsigned short, unsigned long >& videoInputs );

        /**
         * invalidate
         *
         * Marks the data contained by this Sequence as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * Precondition:    Either - this Sequence was initially loaded from the database
         *                  OR     - writeSequenceData() has already been called, as it does 
         *                           not make any sense to invalidate a Sequence that has not 
         *                           yet been written to the database.
         */

	    void invalidate();

        /**
         * writeSequenceData
         * 
         * Write this Sequence to the database.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException 
         *              Thrown if:
         *              1) There are multiple entries for the same position.
         * @exception   DataConfigurationException 
         *              Thrown if the data contained in the Sequence object
         *              is not sufficent to create an entry in the database.
         */

        void writeSequenceData();

        /**
         * deleteSequence
         *
         * Remove this Sequence from the database. Once this method has been executed, the
         * SequenceHelper object will have been destroyed.
         *
         * Precondition:    Either - this Sequence was initially loaded from the database
         *                  OR     - writeSequenceData() has already been called.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         */

        void deleteSequence();

        struct sequence_input
        {
            unsigned long m_sequenceKey;
            std::vector<unsigned long> m_inputKeys;
            unsigned short m_dwellTime;
            std::string m_descriptions;
            sequence_input() : m_sequenceKey(0), m_dwellTime(0) {}
        };
        typedef std::map<unsigned long, sequence_input> sequence_to_input_map;
        void getSequenceRelationShipInSequences(const std::vector<unsigned long>& sequenceKeys, sequence_to_input_map& sequences);
		void getSequenceRelationShip(unsigned long sequenceKey,unsigned short& dwellTime,std::string& description,std::vector<unsigned long>& videoInputKeys);
		void setSequenceRelationShip(unsigned long sequenceKey,unsigned short dwellTime,const std::string& description, std::vector<unsigned long> inputVideoKeys);
        void getRelationShipFromParentKey(unsigned long parentKey, unsigned long& pkeys, std::string& description, int* ptype=NULL);
        void setRelationShipOfParentKey(unsigned long parentKey, unsigned long pkeys,const std::string& description, int ptype=13);
		void delRelationShipOfParentKey(unsigned long parentKey);
    private:
        //
        // Disable the assignment operator.
        //

		SequenceHelper& operator=(const SequenceHelper &);
        
        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * Precondition:    Either - this Sequence was initially loaded from the database
         *                  OR     - writeSequenceData() has already been called.
         */

	    void reload();

        /**
         * insertNewSequence
         *
         * Inserts a new Sequence into the database.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException
         *              Thrown if the Sequence already exists in the database.
         */

        void insertNewSequence();

        /**
         * updateExistingSequence
         *
         * Updates an existing Sequence in the database.
         *
         * @exception   DatabaseException 
         *              Thrown if there are any problems in communicating with the database due to
         *              1) An invalid connection string (as stored in RunParams)
         *              2) The database is not/cannot be opened
         *              3) An error while attempting to execute a database query.
         * @exception   TA_Base_Core::DataException
         *              Thrown if the Sequence does not already exist in the database.
         */

        void updateExistingSequence();

        //
        // Flag to indicate if the data needs reloading.
        //

	    bool m_isValidData;

        //
        // Flag to indicate if this is a new Sequence or an existing one
        // loaded from the database.
        //

        bool m_isNew;
        
        //
        // The Sequence database (entity) key and the keys of the 
        // VideoInputs in this Sequence referenced by position.
        //
        unsigned long m_key;
        std::map< unsigned short, unsigned long > m_videoInputs;
    };

} // closes TA_IRS_Core

#endif // !defined(SequenceHelper_C75ECB74_E656_4a04_840E_48CB84AFF27C__INCLUDED_)
