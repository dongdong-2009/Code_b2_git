/**
* This class encapulates the data associated with a ats method.
* @author Lucky Cuizon
* @version 1.0
* @generator Enterprise Architect
* @created 18-January-2011 08:32:15
*/

#ifndef ATS_METHOD_PROTOCOL_H
#define ATS_METHOD_PROTOCOL_H

#include <vector>

namespace TA_IRS_App
{
    //Method
  #define METHOD_HEADER_FLAG                 0
  #define METHOD_HEADER_COMMAND              1
  #define METHOD_HEADER_TRAIN_ID_NUMBER      2
  #define METHOD_HEADER_LENGTH               3
  #define METHOD_HEADER_ORIGIN               4
  #define METHOD_FCTLO_LENTH                 5

  #define METHOD_DATA_FLAG                   7
  #define METHOD_ATS_OA1TABLE               61
  #define METHOD_ATS_TRAIN_STATUS_REPORT    62

    //Event
  #define EVENT_HEADER_FLAG                 0
  #define EVENT_HEADER_COMMAND              1
  #define EVENT_HEADER_TRAIN_ID_NUMBER      2
  #define EVENT_HEADER_LENGTH               3
  #define EVENT_HEADER_CRC0                 4
  #define EVENT_HEADER_CRC1                 5
  #define HEADER_LENGTH                     6  
  #define HEADER_LENGTH_LOCATION            4 
  #define EVENT_ATS_OA1TABLE               61
  #define EVENT_ATS_TRAIN_STATUS_REPORT    62
  
	// Information
  #define STATUS_OK                       0
  #define SHORTSTRING_LENGTH              32 

  typedef unsigned short DataTypeCRC;
  typedef unsigned char DataGramType;
  typedef std::vector<DataGramType> DataGramVector;
  typedef struct
  {
     /**
      * Storage for the FCTLO which contains 1 byte each. 
      * F = Flag, C = Command, T = Train ID Number, L = Length, O = Origin
      */
	  DataGramVector FCTLO;

     /**
	  * Storage for the Critical Alarm/Events Dataw which contains 1 byte each.
      */
	  DataGramVector CriticalAlarmEvents;

	 /**
	  * The storage of FCTLO, CriticalAlarmEvents and the CRC.
	  */
	  DataGramVector DatagramTable;

	 /**
	  * Storage for the CRC which contains 2 bytes of memory
	  */
	  unsigned char* CRC;

	 // const DataGramVector crcData;





  } DataGram;

};

#endif //End of ATS_METHOD_PROTOCOL_H