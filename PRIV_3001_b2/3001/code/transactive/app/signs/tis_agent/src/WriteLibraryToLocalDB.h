#ifndef WRITELIBRARYTOLOCALDB_H_INCLUDE
#define WRITELIBRARYTOLOCALDB_H_INCLUDE

#include "core/threads/src/Thread.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "MessageLibraryManager.h"
#include "Exceptions.h"
#include "STISManager.h"
#include "StatusMonitor.h"
#include "StringConverter.h"
#include <string>

/*
namespace
{
	template<class T,int N = sizeof(T)/sizeof(unsigned char)> class ConvertStringToVecChar
	{
	public:
		static std::vector<unsigned char> convert(const std::basic_string<T>& src)
		{
			const T t = 0xff;	
			std::vector<unsigned char> tmpVec;
			for(unsigned long i = 0; i != src.size(); ++i)
			{
				T tmp1 = src[i];
				for(unsigned long j = 0; j != N; ++j)
				{
					T tmp2 = (t << (j*8));
					tmpVec.push_back((tmp1 & tmp2) >> (j*8));
				}
			}
			return tmpVec;
		}
	};
}
*/

namespace TA_IRS_App
{
	template<EMessageLibraryType N> class WriteLibraryToLocalDB : public TA_Base_Core::Thread
	{
	public:
		WriteLibraryToLocalDB()
			:m_libraryType(N),
			 m_indicateVersion(0),
			 m_isRunning(false)
		{

		}

		virtual void run()
		{
			FUNCTION_ENTRY("run");
			m_isRunning = true;
			TA_Base_Core::EDataTypes conntype;
			if(STISManager::getInstance()->getLocationType() == TA_Base_Core::ILocation::OCC)
			{
				conntype = TA_Base_Core::Tis_OCC_Cd;
			}
			else
			{
				conntype = TA_Base_Core::Tis_Cd;
			}			
			//create corresponding parser
			std::auto_ptr<MessageLibraryParser> parser
				(ParserCreateFactory::CreateParser( m_file, N, conntype));
			if( NULL == parser.get() )
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Failed create %s library parser", STATION_LIBRARY == m_libraryType?"STIS" : "TTIS");
				FUNCTION_EXIT;
				return;
			}
			try
			{
				parser->parseLibrary();
				// Change datapoint value
				unsigned long newLibraryVersion = parser->getLibraryVersion();
				if( STATION_LIBRARY == m_libraryType )
				{
		            if( StatusMonitor::getInstance()->getNextISCSStationLibraryVersion() != newLibraryVersion )
		            {
		                StatusMonitor::getInstance()->setNextISCSStationLibraryVersion( newLibraryVersion );
		            }
				}
				else
				{
		            if( StatusMonitor::getInstance()->getNextISCSTrainLibraryVersion() != newLibraryVersion )
		            {
		                StatusMonitor::getInstance()->setNextISCSTrainLibraryVersion( newLibraryVersion );
		            }
				}
			}
			catch( const TA_IRS_Core::TISAgentException& e )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_IRS_Core::TISAgentException", e.what());
			}
			catch( ... )
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "Unknown");
			}		
			FUNCTION_EXIT;
		}

		virtual void terminate()
		{
			m_isRunning = false;
		}

		bool isRunning()
		{
			return m_isRunning;
		}

		void setLibraryData(unsigned long libaryversion, const std::vector<unsigned char>& content)
		{			
			m_indicateVersion = libaryversion;
			m_file = content;
		}

		unsigned long getInicateVersion()
		{
			return m_indicateVersion;
		}
	private:
		unsigned long m_indicateVersion;
		std::vector<unsigned char> m_file;		
		EMessageLibraryType m_libraryType;
		bool m_isRunning;
	};
}
#endif