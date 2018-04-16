#include <time.h>
#include <fstream>
#include <sstream>
#include <cstdio>

#include "boost/filesystem.hpp"

#include "app/ats/AtsAgent/TimetableManager/src/TimetableHelper.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/AtsAgentException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"



namespace TA_IRS_App{

	const std::string PRACTICAL_FILE_PREFIX = "M01_ACT_TT_";
	const std::string PLANNED_FILE_PREFIX = "M01_PLA_TT_";
	const std::string TIMETABLE_FILE_EXTENSION = ".csv";
	
	//column number counted from 0
	const short COL_NO_TRAIN_ID = 1;
	const short COL_NO_SERVICE_ID = 2;
	const short COL_NO_DEST_ID = 4;
	const short COL_NO_TOTAL_PLATFORM = 5;

	const short COL_NO_INNER_LOOP_PLATFORM_ID = 0;
	const short COL_NO_INNER_LOOP_ARRIVAL_TM_NO = 1;
	const short COL_NO_INNER_LOOP_DEST_TM_NO = 2;
	const short COL_NO_REPEATITION_START = 6 ;
	const short REPTITION_SIZE = 3;

	TimetableHelper* TimetableHelper::m_instance = NULL;
	TimetableHelper& TimetableHelper::getInstance()
	{
		if( m_instance == NULL )
		{
			m_instance = new TimetableHelper();
		}
		return *m_instance;
	}

	void TimetableHelper::removeInstance()
	{
		if( m_instance != NULL )
		{
			delete m_instance;
			m_instance = NULL;
		}

	}

	std::vector<std::string> TimetableHelper::getFileNames(bool isPlanned, std::string& currentDateDirectory)
	{	
		FUNCTION_ENTRY("getFileName");

		std::string filePrefix;
		if(isPlanned)
		{
			filePrefix =  PLANNED_FILE_PREFIX;
		}
		else
		{
			filePrefix = PRACTICAL_FILE_PREFIX;
		}

		boost::filesystem::path dirPath(currentDateDirectory);
		std::vector<std::string> FileNameList;
		
		//check if directory exists
		if(boost::filesystem::exists(dirPath) || boost::filesystem::is_directory(dirPath))
		{
			boost::filesystem::recursive_directory_iterator it(dirPath);
			boost::filesystem::recursive_directory_iterator endit;
			
			//iterate through all the files in the directory 
			//find the planned/practical files for current day

			while(it != endit)
			{	
				if(boost::filesystem::is_regular_file(*it) && it->path().extension() == TIMETABLE_FILE_EXTENSION) 
				{	
					std::string filename = it->path().filename().string();

					//check if the directory name format is M01_PLA_TT_yyyymmddhhmmss.csv
					std::string dateTime = filename.substr(filename.rfind(filePrefix) + 11, 14);
					if(starts_with(filename,filePrefix) && filename.length() == 29 && strspn( dateTime.c_str(), "0123456789" ) == dateTime.size())
					{		
						 FileNameList.push_back(it->path().string());
					}
					
				}				
				++it;
			}
		}
		FUNCTION_EXIT;
		return FileNameList;
	}

	std::string TimetableHelper::getCurrentDateDirPath()
	{
		FUNCTION_ENTRY("getCurrentDateDirPath");

		time_t currentTime;
		time(&currentTime);
		char buffer[80];
		
		struct tm * currentTimeInfo  = localtime(&currentTime);
		
		//get dirpath based on current time
		strftime(buffer,80,"%y.%m.%d",currentTimeInfo);
		std::string currentDateString = std::string(buffer);


		std::string DirPath = getDirPath() + "/" + currentDateString;

		FUNCTION_EXIT;
		return DirPath;


	}

	void TimetableHelper::setDirPath(std::string DirPath)
	{
		m_DirPath = DirPath;
	}

	std::string TimetableHelper::getDirPath()
	{
		return m_DirPath;
	}

	 bool TimetableHelper::readPlannedFile(std::list<PlannedTrainTimetableInfoPtr>& PlannedTrainDataList,std::string filename)
	{	
		
		FUNCTION_ENTRY("readPlannedFile");
		try
		{
			std::ifstream file(filename.c_str());
			std::string line;

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Being Reading %s...",
				filename.c_str());	

			while(getline (file,line))
			{
	
				getPlannedTrainDataInfo(line, PlannedTrainDataList);

			}
			
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"File successfully read %s, PlannedTimetableList Size %d",
				filename.c_str(),PlannedTrainDataList.size());	
			FUNCTION_EXIT;

			return true;
		}

		catch (std::ifstream::failure e) {

			std::string text = "readPlannedFile()  Filename %s - " + std::string(e.what()) ;
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text);
		}

		catch (...)
		{	
			std::string text = "readPlannedFile() Filename %s" + filename ;
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text);
			
		}

		FUNCTION_EXIT;
		return false;
		
		
		
	}
	bool TimetableHelper::readPracticalFile(std::list<PracticalTrainTimetableInfoPtr>& PracticalTrainDataList,std::string filename)
	{	
		FUNCTION_ENTRY("readPracticalFile");
		try
		{
			std::ifstream file(filename.c_str());
			std::string line;

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Being Reading %s...",
				filename.c_str());	

			while(getline (file,line))
			{				
			
				getPracticalTrainDataInfo(line, PracticalTrainDataList);

			}

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"File successfully read %s, PracticalTrainDataList Size %d",
				filename.c_str(),PracticalTrainDataList.size());	

			FUNCTION_EXIT;

			return true;
		}

		catch (std::ifstream::failure e) {

			std::string text = "readPracticalFile()  Filename %s - " + std::string(e.what()) ;
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text);
		}

		catch (...)
		{
			std::string text = "readPracticalFile()  Filename %s" + filename  ;
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text);
			
		}

		FUNCTION_EXIT;
		return false;

	}

	void TimetableHelper::deleteFile(std::string fileName)
	{
		FUNCTION_ENTRY("deleteFile");
		if( remove(fileName.c_str()) != 0 )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"File %s could not be deleted",
				fileName.c_str());	
		}
			
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Deleted File %s",
				fileName.c_str());	
		}

		FUNCTION_EXIT;
		return ;
	}

	void TimetableHelper::deleteDirectory(std::string folderName)
	{	
		FUNCTION_ENTRY("deleteDirectory");

		boost::filesystem::path FolderPath(folderName);

		//delete folder if its empty
		if(boost::filesystem::is_empty(FolderPath))
		{	
			//check if the folder can has been deleted
			if(boost::filesystem::remove_all(FolderPath) > 1)
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"Deleted Directory %s",
					folderName.c_str());
			}
			else
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"Directory %s could not be deleted",
					folderName.c_str());
			}
		}

		FUNCTION_EXIT;
	}

	unsigned long TimetableHelper::getStationID(unsigned long PlatformID)
	{	
		FUNCTION_ENTRY("getStationID");

		return AtsPlatformAccessFactory::getInstance().getAtsStationIdWhere(PlatformID);

		FUNCTION_EXIT;
	}

	void TimetableHelper::getPlannedTrainDataInfo(std::string Data,std::list<PlannedTrainTimetableInfoPtr>& PlannedDataList )
	{
		FUNCTION_ENTRY("getPlannedTrainDataInfo");
		try{
			//split the line into vector of string using delimiter
			std::vector<std::string> Elem = split(Data,',');

			std::string serviceNo = Elem.at(COL_NO_SERVICE_ID);
			std::string destID = Elem.at(COL_NO_DEST_ID).c_str();
			int totalNumPlatforms = atoi(Elem.at(COL_NO_TOTAL_PLATFORM).c_str());

			for(int i =0; i < totalNumPlatforms; i++)
			{
				PlannedTrainTimetableInfoPtr PlannedData( new PlannedTrainDataInfo());
				PlannedData->strServiceID = serviceNo;

				int col_platform_start = COL_NO_REPEATITION_START + i * REPTITION_SIZE;
				PlannedData->strPlatformID =  Elem.at(col_platform_start + COL_NO_INNER_LOOP_PLATFORM_ID);

				std::string arr_time = Elem.at(col_platform_start + COL_NO_INNER_LOOP_ARRIVAL_TM_NO);
				std::string dept_time = Elem.at(col_platform_start + COL_NO_INNER_LOOP_DEST_TM_NO);

				PlannedData->strATime = convertUTCtoLocatimeString(atol(arr_time.c_str()));
				PlannedData->strDTime = convertUTCtoLocatimeString(atol(dept_time.c_str()));

				unsigned long iID = atol(PlannedData->strPlatformID.c_str());
				unsigned long StationID = getStationID(iID);
				PlannedData->strStationID = convertLongToString(StationID);
				PlannedData->strDate =  getCurrentDateTime();
				PlannedData->strDestinationID = destID;
				PlannedData->strTrainID ="0";
				
				//flag, reserved attributes are not found in the planned file
				//set all to 0 since not used
				PlannedData->strFlag = "0";
				PlannedData->strReserved = "0";

				PlannedDataList.push_back(PlannedData);

			}
			
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Add timetable data: %s, add %d new planned train data, total Planned List size %d",
				Data.c_str(), totalNumPlatforms, PlannedDataList.size());

		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			// Log and re-throw the exception
			std::string text = "getPlannedTrainDataInfo() - " + std::string(e.what())  + " Data from file: " + Data;
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);

		}
		catch (TA_Base_Core::DataException& e)
		{
			// Log the exception
			std::string text = "getPlannedTrainDataInfo() - " + std::string(e.what())  + " Data from file: " + Data;
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);

		}	
		catch(...)
		{
			
			std::string text = "getPlannedTrainDataInfo(). Data from file: " + Data;
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text);
		}
		FUNCTION_EXIT;
	}

	void TimetableHelper::getPracticalTrainDataInfo(std::string Data, std::list<PracticalTrainTimetableInfoPtr> &PracticalDataList)
	{
		FUNCTION_ENTRY("getPracticalTrainDataInfo");

		try{
			//split the line into vector of string using delimitor
			std::vector<std::string> Elem = split(Data,',');

			std::string trainID = Elem.at(COL_NO_TRAIN_ID).c_str();
			std::string serviceNo = Elem.at(COL_NO_SERVICE_ID);
			std::string destID = Elem.at(COL_NO_DEST_ID).c_str();
			int totalNumPlatforms = atoi(Elem.at(COL_NO_TOTAL_PLATFORM).c_str());

			for(int i =0; i < totalNumPlatforms; i++)
			{
				PracticalTrainTimetableInfoPtr PracticalData( new PracticalTrainDataInfo());
				PracticalData->strServiceID = serviceNo;

				int col_platform_start = COL_NO_REPEATITION_START + i * REPTITION_SIZE;
				PracticalData->strPlatformID =  Elem.at(col_platform_start + COL_NO_INNER_LOOP_PLATFORM_ID);
				
				std::string arr_time = Elem.at(col_platform_start + COL_NO_INNER_LOOP_ARRIVAL_TM_NO);
				std::string dept_time = Elem.at(col_platform_start + COL_NO_INNER_LOOP_DEST_TM_NO);

				PracticalData->strATime = convertUTCtoLocatimeString(atol(arr_time.c_str()));
				PracticalData->strDTime = convertUTCtoLocatimeString(atol(dept_time.c_str()));
				unsigned long StationID = getStationID(atol(PracticalData->strPlatformID.c_str()));
				PracticalData->strStationID = convertLongToString(StationID);
				PracticalData->strDate =  getCurrentDateTime();
				PracticalData->strDestinationID = destID;
				PracticalData->strTrainID = trainID;

				//flag, reserved, type attributes are not found in the actual file
				//set all to 0 since not used
				PracticalData->strFlag = "0";
				PracticalData->strReserved = "0";
				PracticalData->strType = "0";

				PracticalDataList.push_back(PracticalData);

			}

			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Add timetable data: %s, add %d new practical train data, total practical List size %d",
				Data.c_str(), totalNumPlatforms, PracticalDataList.size());
		}
		catch (TA_Base_Core::DatabaseException& e)
		{
			// Log and re-throw the exception
			std::string text = "getPracticalTrainDataInfo() - " + std::string(e.what())  + " Data from file: " + Data;
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", text);

		}
		catch (TA_Base_Core::DataException& e)
		{
			// Log the exception
			std::string text = "getPracticalTrainDataInfo() - " + std::string(e.what())  + " Data from file: " + Data;
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", text);

		}	
		catch(...)
		{

			std::string text = "getPracticalTrainDataInfo(). Data from file: " + Data;
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", text);
		}
		FUNCTION_EXIT;

	}

	bool TimetableHelper::checkDirExists(std::string dirPath)
	{	
		FUNCTION_ENTRY("checkDirExists");

		//check if directory exists
		if(boost::filesystem::exists(dirPath) || boost::filesystem::is_directory(dirPath))
		{	
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Directory %s found",dirPath.c_str());
			FUNCTION_EXIT;
			return true;
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"Directory %s not found",dirPath.c_str());
			FUNCTION_EXIT;
			return false;
		}
	}

	bool TimetableHelper::checkFileExists(std::string filename)
	{
		
		FUNCTION_ENTRY("checkFileExists");

		boost::filesystem::path dirPath(filename);

		//check if directory exists
		if(boost::filesystem::exists(dirPath))
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"File %s found",filename.c_str());
			FUNCTION_EXIT;
			return true;
		}
		else
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"File %s not found",filename.c_str());
			FUNCTION_EXIT;
			return false;
		}
	}


	std::vector<std::string>  TimetableHelper::split(const std::string &s, char delim) 
	{
		FUNCTION_ENTRY("split");
		std::stringstream ss(s);
		std::string item;
		std::vector<std::string> elems;
		while (std::getline(ss, item, delim)) {
			elems.push_back(item);
		}

		FUNCTION_EXIT;
		return elems;
	}

	std::string TimetableHelper::convertUTCtoLocatimeString(unsigned long timeUTC)
	{	
		FUNCTION_ENTRY("convertUTCtoLocatimeString");
		//Convert UTC time to local Time
		time_t _UTCTime = timeUTC;
		struct tm* timeStruct = localtime(&_UTCTime);
		if (timeStruct == NULL) return "";
		char buffer[80];

		strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeStruct);

		std::string _ret(buffer);
		
		FUNCTION_EXIT;
		return _ret;
	}

	std::string TimetableHelper::getCurrentDateTime()
	{
		FUNCTION_ENTRY("getCurrentDateTime");
		time_t now = time(0);
		struct tm *timeStruct = localtime(&now);
		if (timeStruct == NULL) return "";
		char buffer[80];

		strftime(buffer,80,"%d-%m-%Y %H:%M:%S",timeStruct);
		std::string _ret(buffer);
		
		FUNCTION_EXIT;
		return _ret;
	}

	std::string TimetableHelper::convertLongToString(unsigned long number)
	{
		FUNCTION_ENTRY("convertLongToString");

		std::stringstream ss;
		ss << number;

		FUNCTION_EXIT;
		return ss.str();
	}

	bool TimetableHelper::starts_with(std::string s1, std::string s2 )
	{
		//return true if s1 starts with s2
		return (s2.size() <= s1.size()) && (s1.compare(0, s2.size(), s2) == 0);
	}

	void TimetableHelper::sortFileNameByAscedingTime(std::vector<std::string> &filename)
	{

		std::sort(filename.begin(), filename.end(), std::less<std::string>());
	}


}