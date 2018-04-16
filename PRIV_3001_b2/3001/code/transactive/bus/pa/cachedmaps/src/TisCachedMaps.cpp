/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/PRIV_3001/3001/transactive/bus/pa/cachedmaps/src/TisCachedMaps.cpp $
  * @author Sean Liew
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by: $Author: lim.cy $
  * 
  * Maintains all the cached maps for PA applications
  *
  */

#include "core/utilities/src/DebugUtil.h"
#include "core/data_access_interface/pa_4669/src/PaTisMessageMappingAccessFactory.h"
#include "core/data_access_interface/pa_4669/src/PaTisZoneMappingAccessFactory.h"
#include "bus/pa/cachedmaps/src/TisCachedMaps.h"

namespace TA_IRS_Bus
{
	TisCachedMaps* TisCachedMaps::s_pInstance = NULL;
	TA_Base_Core::NonReEntrantThreadLockable TisCachedMaps::s_lockForSingleton;

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    TisCachedMaps::TisCachedMaps() : 
    m_lockForZoneMaps(),
    m_lockForMessageMaps(),
    m_mapPaDvaToTisMsgs(),
    m_mapPaZoneToTisZones()
    {
    }

    TisCachedMaps::~TisCachedMaps()
    {
    }

    TisCachedMaps& TisCachedMaps::getInstance()
    {
	    if ( NULL == s_pInstance )
	    {
		    // Double checking to prevent multiple threads
		    // creating multiple instances.

            // Any lock is okay at this stage.
		    TA_Base_Core::ThreadGuard guard( s_lockForSingleton );

		    if ( NULL == s_pInstance )
		    {
			    s_pInstance = new TisCachedMaps();
    	    }
	    }

	    return *s_pInstance;
    }

    void TisCachedMaps::removeInstance( )
    {
	    // 
	    // Guard this to prevent multiple threads atempting
	    // to delete/create simultaneously
	    //
	    TA_Base_Core::ThreadGuard guard( s_lockForSingleton );
	    if ( s_pInstance == NULL )
	    {
			return;
	    }

		delete s_pInstance;
		s_pInstance = NULL;
    }

	void TisCachedMaps::refreshPaTisMapping( unsigned long ulLocationKey, bool bAllZones ) 
	{
		// Obtain read only array of tis messages
        TA_Base_Core::IPaTisMessageMaps tisMessages;
        try
        {
            tisMessages = TA_Base_Core::PaTisMessageMappingAccessFactory::getInstance().getPaTisMessageMappingByLocationKey(ulLocationKey, false);
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught unknown exception when get PA TIS message mapping" );
        }

        // Refresh PA TIS message mappings cached map
        {
            TA_Base_Core::ThreadGuard guardFirst( m_lockForMessageMaps );
            m_mapPaDvaToTisMsgs.clear();
		    for( TA_Base_Core::IPaTisMessageMaps::iterator itTisMessages = tisMessages.begin(); itTisMessages != tisMessages.end();++itTisMessages )
		    {
                TisMessage tisMessage = { 0u, 0u, 0u, 0u };
			    tisMessage.ulKey = (*itTisMessages)->getKey();
			    tisMessage.ulTisLibrarySection = (*itTisMessages)->getTisLibrarySection();
			    tisMessage.ulTisLibraryVersion = (*itTisMessages)->getTisLibraryVersion();
                tisMessage.ulTisMessageTag = (*itTisMessages)->getTisMessageTag();

                unsigned long ulPaDvaMsgKey = (*itTisMessages)->getPaDvaMessageKey();

                delete *itTisMessages;
                *itTisMessages = NULL;

                std::map<unsigned long, TisMessages>::iterator itExist = m_mapPaDvaToTisMsgs.find( ulPaDvaMsgKey );

                if( itExist == m_mapPaDvaToTisMsgs.end() )
                {
                    TisMessages newTisMessages;
                    std::pair<std::map<unsigned long, TisMessages>::iterator, bool> pirResult = 
                        m_mapPaDvaToTisMsgs.insert(std::map<unsigned long, TisMessages>::value_type( ulPaDvaMsgKey, newTisMessages ));

                    // Check insertion result
                    if ( pirResult.second )
                    {
                        itExist = pirResult.first;
                    }
                    else
                    {
                        continue;
                    }
                }

                if( itExist != m_mapPaDvaToTisMsgs.end() )
                {
                    itExist->second.push_back( tisMessage );
                }
            }

            tisMessages.clear();
        }

        // Retrieve PA TIS zone mappings from database
        TA_Base_Core::IPaTisZoneMaps tisZones;

        try
        {
            if ( bAllZones )
            {
                tisZones = TA_Base_Core::PaTisZoneMappingAccessFactory::getInstance().getAllPaTisZoneMappings(false);
            }
            else
            {
                tisZones = TA_Base_Core::PaTisZoneMappingAccessFactory::getInstance().getPaTisZoneMappingByLocationKey(ulLocationKey, false);
            }
        }
        catch ( std::exception& expWhat )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "std::exception", expWhat.what() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught unknown exception when get PA TIS zone mapping" );
        }

        // Refresh PA TIS zone mappings cached map
        {
		    TA_Base_Core::ThreadGuard guardSecond( m_lockForZoneMaps );

            m_mapPaZoneToTisZones.clear();
		    for( TA_Base_Core::IPaTisZoneMaps::iterator itTisZones = tisZones.begin(); itTisZones != tisZones.end();++itTisZones)
		    {
                TisZone tisZone = { 0u, "" };
			    tisZone.ulKey = (*itTisZones)->getKey();
			    tisZone.strEquipmentEntity = (*itTisZones)->getTisZoneEntity();
                unsigned long ulPaZoneKey = (*itTisZones)->getPaZoneKey();

                delete *itTisZones;
                *itTisZones = NULL;

                std::map<unsigned long, TisZones>::iterator itExist = m_mapPaZoneToTisZones.find( ulPaZoneKey );

                if( itExist == m_mapPaZoneToTisZones.end() )
                {
                    TisZones newTisZones;
                    std::pair<std::map<unsigned long, TisZones>::iterator, bool> pirResult = 
                        m_mapPaZoneToTisZones.insert(std::map<unsigned long, TisZones>::value_type( ulPaZoneKey, newTisZones ));

                    // Check insertion result
                    if ( pirResult.second )
                    {
                        itExist = pirResult.first;
                    }
                    else
                    {
                        continue;
                    }
                }

			    if( itExist != m_mapPaZoneToTisZones.end() )
			    {
				    itExist->second.push_back( tisZone );
			    }
		    }

            tisZones.clear();
        }
	}

	TisMessages TisCachedMaps::getTisMsgsByPADvaMsgKey( unsigned long ulMsgKey )
	{
        TA_Base_Core::ThreadGuard guardSecond( m_lockForMessageMaps );

        std::map<unsigned long,TisMessages>::iterator itExist = m_mapPaDvaToTisMsgs.find( ulMsgKey );
        if ( itExist == m_mapPaDvaToTisMsgs.end() )
        {
            TisMessages listTisMsgs;
            return listTisMsgs;
        }

        return itExist->second;
	}

	TisZones TisCachedMaps::getTisZonesByPaZoneKey( unsigned long ulZoneKey )
    {
        TA_Base_Core::ThreadGuard guardSecond( m_lockForZoneMaps );

        std::map<unsigned long,TisZones>::iterator itExist = m_mapPaZoneToTisZones.find( ulZoneKey );
        if ( itExist == m_mapPaZoneToTisZones.end() )
        {
            TisZones listTisZones;
            return listTisZones;
        }

        return itExist->second;
	}

	bool TisCachedMaps::isPADvaMsgMapped( unsigned long ulMsgKey )
    {
        TA_Base_Core::ThreadGuard guardSecond( m_lockForMessageMaps );

		return ( m_mapPaDvaToTisMsgs.find( ulMsgKey ) != m_mapPaDvaToTisMsgs.end() );
	}

	bool TisCachedMaps::isPAZoneMapped( unsigned long ulZoneKey )
    { 
        TA_Base_Core::ThreadGuard guardSecond( m_lockForZoneMaps );

		return ( m_mapPaZoneToTisZones.find( ulZoneKey ) != m_mapPaZoneToTisZones.end() );
    }

    bool TisCachedMaps::areAllPaDvaMsgMapped( const std::vector<unsigned long>& vecMsgKeys )
    {
        TA_Base_Core::ThreadGuard guardSecond( m_lockForMessageMaps );

        bool bAllMapped = true;

        for( std::vector<unsigned long>::const_iterator itLoop = vecMsgKeys.begin(); itLoop != vecMsgKeys.end(); ++itLoop)
        {
            if( m_mapPaDvaToTisMsgs.find( *itLoop ) == m_mapPaDvaToTisMsgs.end() )
            {
                bAllMapped = false;
                break;
            }
        }

        return bAllMapped;
    }

	bool TisCachedMaps::areAllPaZoneMapped( const std::vector<unsigned long>& vecZoneKeys )
    {
        TA_Base_Core::ThreadGuard guardSecond( m_lockForZoneMaps );

        bool bAllMapped = true;

		for( std::vector<unsigned long>::const_iterator itLoop = vecZoneKeys.begin(); itLoop != vecZoneKeys.end(); ++itLoop)
		{
			if( m_mapPaZoneToTisZones.find( *itLoop ) == m_mapPaZoneToTisZones.end() )
			{
				bAllMapped = false;
                break;
			}
		}

		return bAllMapped;
	}

} // end namespace TA_IRS_Bus