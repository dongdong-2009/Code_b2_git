#ifndef _I_TIMETABLE_HELPER_H_
#define _I_TIMETABLE_HELPER_H_

#include <string>
#include <vector>
#include <list>

#include "boost/shared_ptr.hpp"
#include "core/data_access_interface/ats/src/AtsPlannedDataFactory.h"
#include "core/data_access_interface/ats/src/AtsPracticalDataFactory.h"
#include "core/data_access_interface/ats/src/AtsPlatformAccessFactory.h"

using namespace TA_IRS_Core;
typedef boost::shared_ptr<PlannedTrainDataInfo> PlannedTrainTimetableInfoPtr;
typedef boost::shared_ptr<PracticalTrainDataInfo> PracticalTrainTimetableInfoPtr;
namespace TA_IRS_App{
	


	class TimetableHelper{

	public:
		
		static TimetableHelper& getInstance();
		static void removeInstance();
		
		/**
          * checkDirExists
          *
		  * Return true if directory is found, else return false
		  *
		  * @param - directory path to check
		  *
		  * @return bool
          */
		bool checkDirExists(std::string DirPath);	

		/**
          * checkFileExists
          *
		  * Return true if file is found, else return false
		  *
		  * @param - filename (with fullpath) to check
		  *
		  * @return bool
          */
		bool checkFileExists(std::string filename);

		/**
          * setDirPath
          *
          * Set directory where train timetable data are stored
		  *
		  * @param - directory path
		  *
		  * @return bool
          */
		void setDirPath(std::string DirPath);

		/**
          * getDirPath
          *
          * Get directory where train timetable data are stored
		  *
		  * @return - directory path
		  */
		std::string getDirPath();

		/**
          * getCurrentDateDirPath
          *
          * Get the name of the directory which stores current days train timetable
		  *
		  * @return - directory path with current day's timetable
          */
		std::string getCurrentDateDirPath();

		/**
          * getFileNames
          *
          * Returns a list of files within the a designated directory
		  *
		  * @param - true if planned, false if practical
		  * @param - the directory from which the list of files to retrieve
		  *
		  * @return - list of filenames
          */
		std::vector<std::string> getFileNames(bool isPlanned, std::string& currentDateDirectory);

		/**
          * deleteFile
          *
          * Delete the file
		  *
		  * @param - name of file to be deleted
          */
		void deleteFile(std::string fileName);

		/**
          * deleteDirectory
          *
          * Deletes the directory if empty
		  *
		  * @param - name of the directory to be deleted
          */
		void deleteDirectory(std::string folderName);
		
		/**
          * sortFileNameByAscedingTime
          *
          * Sort filenames in increasing time based on the timestamp in the file name
		  * 
		  * @param - vector of filesnames
          */
		void sortFileNameByAscedingTime(std::vector<std::string> &filename);

		/**
          * readPlannedFile
          *
          * Read planned train timetable data from file and return it in a list from
		  * 
		  * @param - list of planned train data
		  * @param - name of the planned file
          */
		bool readPlannedFile(std::list<PlannedTrainTimetableInfoPtr>& PlannedTrainDataList, std::string filename);
        
		/**
          * readPracticalFile
          *
          * Read practical train timetable data from file and return it in a list from
		  * 
		  * @param - list of practical train data
		  * @param - name of the practical file
          */
		bool readPracticalFile(std::list<PracticalTrainTimetableInfoPtr>& PracticalTrainDataList, std::string filename);
			

	private:
		//
		// These are private as this method is a singleton
		//
		TimetableHelper() {};
		TimetableHelper( const TimetableHelper& theTimetableHelper);
		TimetableHelper& operator=(const TimetableHelper &);

		static TimetableHelper* m_instance;
		std::string m_DirPath;

		
		/**
          * getStationID
          *
          * Get the stationID to which the platform belongs
		  *
		  * @param - platformID
		  *
		  * @return - corresponding station id
          */
		unsigned long getStationID(unsigned long PlatformID);

		/**
          * split
          *
          * separates string based on delimiter character
		  *
		  * @param - string to be split
		  * @param - delimiter
		  *
		  * @return - split string in vector 
          */
		std::vector<std::string> split(const std::string &s, char delim);

		/**
          * getPlannedTrainDataInfo
          *
          * Get list of planned train data from input string
		  *
		  * @param - input string 
		  * @param - list of planned train data
		  * 
          */
		void getPlannedTrainDataInfo(std::string Data,std::list<PlannedTrainTimetableInfoPtr> &PlannedDataList );
		
		/**
          * getPracticalTrainDataInfo
          *
          * Get list of practical train data from input string
		  *
		  * @param - input string 
		  * @param - list of practical train data
		  * 
          */
		void getPracticalTrainDataInfo(std::string Data, std::list<PracticalTrainTimetableInfoPtr> &PracticalDataList);
		
		/**
          * convertUTCtoLocatimeString
          *
          * Convert UTC time to local time
		  *
		  * @param - UTC time 
		  *
		  * @return - local time in string format
          */
		std::string convertUTCtoLocatimeString(unsigned long timeUTC);
		
		/**
          * getCurrentDateTime
          *
          * Returns current local time in string format
		  *
		  * @return current local time
          */
		std::string getCurrentDateTime();	
		
		/**
          * convertLongToString
          *
          * Convert long to string
		  *
		  * @param -  number to be converted
		  *
		  * @return - convert number in string format
          */
		std::string convertLongToString(unsigned long number);
		
		/**
          * starts_with
          *
          * Checks if the first string starts with second string
		  *
		  * @param - string to be checked
		  * @param - starting text
		  * 
		  * @return - return true if first string starts with second string, else false
          */
		bool starts_with(std::string s1, std::string s2 );

	};
}

#endif