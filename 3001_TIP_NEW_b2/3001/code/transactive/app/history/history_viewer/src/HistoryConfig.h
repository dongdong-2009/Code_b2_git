#ifndef H_HISTORY_CONFIG_INCLUDE
#define H_HISTORY_CONFIG_INCLUDE

/**
  * The source code in this file is the property of 
  * MI Consulting Group (Australia) and is not for 
  * redistribution in any form.
  *
  * Source: $File: //depot/3001_TIP_NEW/3001/transactive/app/history/history_viewer/src/HistoryConfig.h $
  * @author Craig Grant
  * @version $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by: $Author: haijun.li $
  * 
  * This class loads the info from the database for the History Configuration.
  * This class is a singleton.
  */

#include <string>
#include <map>
#include <vector>

namespace TA_App
{
    struct HistConfig
    {
        long entityKey;
        std::string entityName;
        std::string shortDesc;
        std::string longDesc;
        long samplePeriod;
        std::string tableName;
        std::string baseData;
    };

    typedef std::map <unsigned long, HistConfig> HistConfigMap;

    class HistoryConfig
    {

    public:

        /**
		  * getInstance
		  * 
		  * Returns the one and only HistoryConfig instance.
		  *
		  */
        static HistoryConfig& getInstance()
        {
	        static HistoryConfig e;
	        return e;
        }

        /**
		  * LoadConfig
		  * 
		  * This gets the connection string from RunParams and opens the database.
          * If this is successful it then loads all the required information for
          * the history configuration.
          *
          * @exception CouldNotLoadException - thrown if it cannot get connection
          * parameters or connect and open the database.
		  *
		  */
        void LoadConfig();

        /**
		  * GetTableName
		  * 
		  * Retrieves the table name given the entity key.
          *
          * @param entityKey - Pkey of the HistoryConfig entity
          *
          * @return - Table name
          *
          */
        const std::string GetTableName(const unsigned long entityKey);

        /**
		  * GetSamplePeriod
		  * 
		  * Retrieves the sample period given the entity key.
          *
          * @param entityKey - Pkey of the HistoryConfig entity
          *
          * @return - Sample period
          *
          */
        const long GetSamplePeriod(const unsigned long entityKey);

        /**
		  * GetHistConfigMap
		  * 
		  * Retrive the list of history configurations.
          *
          * @return - Map of all the history config configurations
          *
          */
        const HistConfigMap& GetHistConfigMap();

        /**
		  * AddShortDesc
		  * 
		  * Add the short description attribute to the combo box.
          *
          * @param historyTypeCtrl - The ComboBox control
          *
          */
        void AddShortDesc(CComboBox& historyTypeCtrl);

        class CouldNotLoadException
        {
        public:
	        const std::string GetErrorMsg() 
            { 
                return m_ErrorMsg; 
            }

	        void SetErrorMsg(std::string errorMsg) 
            { 
                m_ErrorMsg = errorMsg; 
            }

        private:
	        std::string m_ErrorMsg;
        };

    private:

        HistoryConfig(HistoryConfig&) {}
        HistoryConfig& operator=(const HistoryConfig &) {}

    private:
        
        HistoryConfig() 
        {
            m_Loaded = false;
        }

    private:
        
        bool m_Loaded;
        HistConfigMap m_HistConfigMap;
    };

} // TA_History

#endif //H_HISTORY_CONFIG_INCLUDE