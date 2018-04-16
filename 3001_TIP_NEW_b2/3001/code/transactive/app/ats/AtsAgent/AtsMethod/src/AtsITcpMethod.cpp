/**
* The implentation for the AtsITcpMethod class which encapulates the data associated with a ats method.
* @author Lucky Cuizon
* @version 1.0
* @generator Enterprise Architect
* @created 18-January-2011 08:45:15
*/

#include "app/ats/AtsAgent/AtsMethod/src/AtsITcpMethod.h"
#include "app/ats/AtsAgent/AtsMethod/src/FrameCheckSequence.h"
#include <string>
using namespace TA_IRS_App;
/**
 * Constructor
 * @param: trainIDNumber, criticalAlarmEvent
 */

AtsITcpMethod::AtsITcpMethod(DataGramVector criticalAlarmEvent)
{
    //Initialize FCTLO to 0 with 5bytes of memory. 
	// F = Flag, C = Command, T = Train ID Number, L = Length, O = Origin
	m_dataGram.FCTLO.assign(5,0);

	//set the data flag of the Datagram
	setDataFCTLO(METHOD_HEADER_FLAG, criticalAlarmEvent[0]);
	
	//set the data commnad of the Datagram
	setDataFCTLO(METHOD_HEADER_COMMAND, criticalAlarmEvent[1]);
	
	//set the trainIDNumber data of the Datagram
	setDataFCTLO(METHOD_HEADER_TRAIN_ID_NUMBER,criticalAlarmEvent[2]);

	//set the Length of the Datagram
	//setDataFCTLO(METHOD_HEADER_LENGTH, getLengthCriticalAlarmEvents() + 7);
	setDataFCTLO(METHOD_HEADER_LENGTH, criticalAlarmEvent[3]);

	setDataFCTLO(METHOD_HEADER_ORIGIN,criticalAlarmEvent[4]);

	DataGramVector alarmData;
	// item = 5,Is the start position of the critical alarm data
	for (int item = 5; item < criticalAlarmEvent.size();item++)
	{
		alarmData.push_back(criticalAlarmEvent[item]);
	}
	//set the criticalAlarmEvents data of the Datagram
	setDataCriticalAlarmEvents(alarmData);

	

    //merge the data of FCTLO and CriticalAlarmEvents into DatagramTable.
	dataGramMerge();

	//calculate CRC data
	//calculateCRC();
}

AtsITcpMethod::~AtsITcpMethod()
{
	/*if(m_dataGram.CRC){
		delete[] m_dataGram.CRC;
		m_dataGram.CRC = NULL;
	}*/
}

/**
 * Used to set the data for the FCTLO at a specified location
 * F = Flag, C = Command, T = Train ID Number, L = Length, O = Origin
 */
void AtsITcpMethod::setDataFCTLO(int loc, DataGramType data)
{
	m_dataGram.FCTLO[loc] = data;
}

/**
 * Used to set the data for the CriticalAlarmEvents
 */
void AtsITcpMethod::setDataCriticalAlarmEvents(DataGramVector criticalAlarmEvent)
{
	m_dataGram.CriticalAlarmEvents = criticalAlarmEvent;
}

/**
 * Used to calculate CRC
 */
void AtsITcpMethod::calculateCRC()
{
	//FrameCheckSequence checkSequence;
	//get the length of the TCTLO and CriticalAlarEvents.
	unsigned int length = getLengthFCTLO() + getLengthCriticalAlarmEvents();
	const unsigned short checksum = FrameCheckSequence::calculate(m_dataGram.DatagramTable, length);
	//get the 1st byte
	unsigned char hi = (checksum & 0xff00) >> 8;
	//get the 2nd byte
	unsigned char low = (checksum & 0x00ff);

	//Then put the CRC data to the DatagramTable
	//Put the 1st byte of CRC to the DatagramTable
	m_dataGram.DatagramTable.push_back(hi);
	//Then the 2nd byte of CRC to the DatagramTable
	m_dataGram.DatagramTable.push_back(low);

	
}

void AtsITcpMethod::dataGramMerge()
{
	//get the size of the FCTLO data
	int sizeFCTLO = m_dataGram.FCTLO.size();
	//get the size of the CriticalAlarmEvent
	int sizeCriticalAlarmEvent = getLengthCriticalAlarmEvents();
	//reserve memory space for the DataGramTable
	m_dataGram.DatagramTable.reserve(sizeFCTLO + sizeCriticalAlarmEvent);
	//Start inserting data from FCTLO to DatagramTable
	m_dataGram.DatagramTable.insert(m_dataGram.DatagramTable.end(),m_dataGram.FCTLO.begin(),m_dataGram.FCTLO.end());
	//Then append the data of the CriticalAlarmEvent.
	m_dataGram.DatagramTable.insert(m_dataGram.DatagramTable.end(),m_dataGram.CriticalAlarmEvents.begin(),m_dataGram.CriticalAlarmEvents.end());
	/*int length = strlen((char*)m_dataGram.CRC);
	for(int x = 0; x < length ;x++)
		m_dataGram.DatagramTable.push_back(m_dataGram.CRC[x]);*/

}

/**
 * Returned FCTLO data from the Datagram.
 */
const DataGramVector AtsITcpMethod::getMessageDataFromFCTLO()const
{
	return m_dataGram.FCTLO;
}

/**
 * Returned Critical Alarm/Events data from the Datagram.
 */
const DataGramVector AtsITcpMethod::getMessageDataCriticalAlarmEvent()const
{
	return m_dataGram.CriticalAlarmEvents;
}

/**
 * Returned DatagramTable Data.
 */
const DataGramVector AtsITcpMethod::getMessageDatagramTable() const
{
	return m_dataGram.DatagramTable;
}

/**
 * Returned the CRC Data from the Datagram
 */
const DataGramType* AtsITcpMethod::getMessageDataCRC() const
{
	return m_dataGram.CRC;
}

/**
 * Returned the length of the CriticalAlarmEvents
 */
unsigned short AtsITcpMethod::getLengthCriticalAlarmEvents()
{
	return m_dataGram.CriticalAlarmEvents.size();
}
/**
 * Returned the length of the CriticalAlarmEvents
 */
unsigned short AtsITcpMethod::getLengthFCTLO()
{
	return m_dataGram.FCTLO.size();
}

/**
 * Get the message data in printable format
 */
void AtsITcpMethod::getLogText(std::string& hexText, std::string& consoleText) const
{
	int length = m_dataGram.DatagramTable.size();
	char* temp = new char[length+1];
	memset(temp, 0, length+1);

	for(unsigned int loop=0; loop< length; loop++)
	{
		consoleText += "..";
		sprintf(temp,"%3.2X",m_dataGram.DatagramTable[loop]);
		hexText += temp;
	}
	delete[] temp;
}

/**
 * Returned the Critical Alarm
 */

void AtsITcpMethod::getCriticalAlarm(std::string& criticalAlarm)const
{
	int length = m_dataGram.CriticalAlarmEvents.size();
	char* temp = new char[length];
	memset(temp, 0, length);
	for(unsigned int loop=0; loop< length; loop++)
	{

		sprintf(temp,"%3.2X", m_dataGram.CriticalAlarmEvents[loop]);
		criticalAlarm += temp;
	}
	delete[] temp;
}

/**
 * Returned the Flag
 */
const unsigned char AtsITcpMethod::getFlag()const
{
	return m_dataGram.FCTLO[METHOD_HEADER_FLAG];
}

/**
 * Returned the Command
 */
const unsigned char AtsITcpMethod::getCommand()const
{
	return m_dataGram.FCTLO[METHOD_HEADER_COMMAND];
}

/**
 * Get trainId Number
 */
const unsigned char AtsITcpMethod::getTrainId()const
{
	return m_dataGram.FCTLO[METHOD_HEADER_TRAIN_ID_NUMBER];
}

/**
 * Get the Length
 */
const unsigned char AtsITcpMethod::getLength()const
{
	return m_dataGram.FCTLO[METHOD_HEADER_LENGTH];
}

/**
 * Get the Origin
 */
const unsigned char AtsITcpMethod::getOrigin()const
{
	return m_dataGram.FCTLO[METHOD_HEADER_ORIGIN];
}