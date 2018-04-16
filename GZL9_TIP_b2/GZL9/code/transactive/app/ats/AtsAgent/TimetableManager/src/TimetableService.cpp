
#include "app/ats/AtsAgent/TimetableManager/src/TimetableService.h"
#include "core/utilities/src/DebugUtil.h"
#include "boost/filesystem.hpp"

namespace TA_IRS_App
{
	namespace fs = boost::filesystem;

	TimetableService::TimetableService(ITimetableProcessor& _timetableProcessor,
		unsigned long ChkInterval, std::string DirPath):
	m_timetableProcessor(_timetableProcessor),
	m_checkDirIntervalSec(ChkInterval*60)
	{
		m_keepRunning = true;
		m_inControlMode = false;


		TimetableHelper::getInstance().setDirPath(DirPath);

	}

	TimetableService::~TimetableService()
	{
		TimetableHelper::removeInstance();
	}

	void TimetableService::OnControlMode()
	{	
		FUNCTION_ENTRY("OnControlMode");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set to control mode");	
		m_inControlMode = true;

		FUNCTION_EXIT;
	}

	void TimetableService::OnMonitorMode()
	{
		FUNCTION_ENTRY("OnMonitorMode");

		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Set to monitor mode");	

		m_inControlMode = false;

		FUNCTION_EXIT;
	}

	void TimetableService::run()
	{
		while(m_keepRunning)
		{	
			if(m_inControlMode)
			{
				try
				{
					//Process the Time table information.
					processTimeTable();
				}
				catch (const fs::filesystem_error & ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, ex.what(), "File system exception happened in Time Table processing");
				}
				catch(...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Exception happened in Time Table processing");
				}

				TA_Base_Core::Thread::sleep(m_checkDirIntervalSec * 1000);
			}
			//in monitor mode
			else
			{	
				//sleep 15 secs
				sleep(15 * 1000);
			}
		}
	}

	void TimetableService::processTimeTable()
	{
		FUNCTION_ENTRY("processTimeTable");
		//get the directory name for the current date
		std::string path = TimetableHelper::getInstance().getDirPath();
		//Iterate through the source path directory
// 		for(fs::directory_iterator directory(path);
// 			directory != fs::directory_iterator();
// 			++directory)
		{
// 			fs::path current(directory->path());
// 
// 			std::string currentFolderPath = current.string();
			std::string currentFolderPath = path;
			//FZLONE-36, boost::filesystem is compatible for both Windows and Linux file system directory path
// 			std::string currentFolderName = current.filename().string();
// 			//check if the directory name format is YYYYMMDDHHMMSS
// 			if(!boost::filesystem::is_directory(current) || currentFolderName.length() != 14 || strspn( currentFolderName.c_str(), "0123456789" ) != currentFolderName.size() )
// 			{
// 				continue;
// 			}
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Start to process folder=%s",
				currentFolderPath.c_str());

			processPlanTimeTable(currentFolderPath);
			processPracticalTimeTable(currentFolderPath);

			//delete  if directory is empty
			/*LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Deleting Directory %s",
				currentFolderPath.c_str());	
			TimetableHelper::getInstance().deleteDirectory(currentFolderPath);*/
		}
		FUNCTION_EXIT;
	}

	void TimetableService::processPlanTimeTable(std::string & currentFolderPath)
	{
		FUNCTION_ENTRY("processPlanTimeTable");
		//get a list of all planned train timetable files for current date
		std::vector<std::string> plannedFileNameList = TimetableHelper::getInstance().getFileNames(true, currentFolderPath);
		
		//sort the filenames in ascending time
		TimetableHelper::getInstance().sortFileNameByAscedingTime(plannedFileNameList);
		
		//read files only if in control mode
		for(int i = 0; i < (int)plannedFileNameList.size() && m_inControlMode; i++)
		{	
			/*for each file read the file;
			store the planned train timetable data in database
			delete the file */

			std::string plannedFileName = plannedFileNameList[i];
			if(TimetableHelper::getInstance().checkFileExists(plannedFileName))
			{	
				std::list<PlannedTrainTimetableInfoPtr> plannedTimeTableList;
				TimetableHelper::getInstance().readPlannedFile(plannedTimeTableList,plannedFileName);							

				m_timetableProcessor.processPlannedTimetable(plannedTimeTableList);
				TimetableHelper::getInstance().deleteFile(plannedFileName);

			}
		}
		FUNCTION_EXIT;
	}

	void TimetableService::processPracticalTimeTable(std::string & currentFolderPath)
	{
		FUNCTION_ENTRY("processPracticalTimeTable");
		//get a list of all practical train timetable files for current date
		std::vector<std::string> practicalFileNameList = TimetableHelper::getInstance().getFileNames(false, currentFolderPath);
		
		//sort the filenames in ascending time
		TimetableHelper::getInstance().sortFileNameByAscedingTime(practicalFileNameList);
		
		//read files only if in control mode
		for(int i = 0; i < (int)practicalFileNameList.size() && m_inControlMode; i++)
		{
			
			/*for each file read the file;
			store the practical train timetable data in database
			delete the file */

			std::string practicalFileName = practicalFileNameList[i];
			if(TimetableHelper::getInstance().checkFileExists(practicalFileName))
			{	
				std::list<PracticalTrainTimetableInfoPtr> practicalTimeTableList;
				TimetableHelper::getInstance().readPracticalFile(practicalTimeTableList, practicalFileName);

				m_timetableProcessor.processPracticalTimetable(practicalTimeTableList);
				TimetableHelper::getInstance().deleteFile(practicalFileName);
			}
		}
		FUNCTION_EXIT;
	}

	void TimetableService::terminate()
	{	
		FUNCTION_ENTRY("terminate");

		m_keepRunning = false;

		FUNCTION_EXIT;
	}
}