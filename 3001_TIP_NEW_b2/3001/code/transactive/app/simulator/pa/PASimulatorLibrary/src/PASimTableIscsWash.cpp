/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/3001/transactive/app/simulator/pa/PASimulatorLibrary/src/PASimTableIscsWash.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  *
  * Definition file for class PASimTableIscsWash.
  */
#include "app/PA/PASimulatorLibrary/src/PASimTableIscsWash.h"
#include "app/PA/PASimulatorLibrary/src/PASimUtility.h"
#include "app/PA/PASimulatorLibrary/src/PASimUserProtocol.h"

using namespace TA_IRS_App::PA_Sim;

PASimTableIscsWash::PASimTableIscsWash(PASimTableQueryProcessor * parent, LocationType loc)
: PASimTable(IscsWash,parent, loc)
{
    m_numberOfRecords=1;
    
    m_tableData.validityField = Irrelevant;
    m_tableData.wpStatus = 0;
    m_tableData.noWash = false;
    m_tableData.dailyRinse = false;
    m_tableData.dailyDetergent = false;
    m_tableData.weeklyWash = false;
    m_tableData.cabEndWashCompleted = false;
    m_tableData.washCycleFullyCompleted = false;
    m_tableData.spare1 = false;
    m_tableData.trainSpeedOutOfRange = false;
    m_tableData.trainStalledInTWP = false;
    m_tableData.trainIncorrectlyPositioned = false;
    m_tableData.spare2 = false;
    m_tableData.washCycleStartFailure = false;
    m_tableData.pHCheck = false;
    m_tableData.mainsWaterLow = false;
    m_tableData.secondBrushRinseAndFinalRinseSpray = false;
    m_tableData.cabEndWashGantryMovementFailureHor = false;
    m_tableData.cabEndWashGantryMovementFailureVer = false;
    m_tableData.cabEndWashBrushMovementFailure = false;
    m_tableData.emergencyStopActivated = false;
    m_tableData.pnuematicFailure = false;
    m_tableData.brushActuationFailure = false;
    m_tableData.sideRoofBrushingFailure = false;
    m_tableData.endBrushingFailure = false;
    m_tableData.airBlowFailure = false;
    m_tableData.preWetFailure = false;
    m_tableData.recycledRinseFailure = false;
    m_tableData.waterRecyclingFailure = false;
    m_tableData.detergentFailure = false;
    m_tableData.detergentLowAlarms = false;
	m_tableData.airPressureLow = false;
    m_tableData.nonCriticalSummaryAlarm = false;
}


PASimTableIscsWash::~PASimTableIscsWash()
{
}


unsigned short PASimTableIscsWash::getTableSize() const
{
    return (m_numberOfRecords * 34);
}



UserResponse * PASimTableIscsWash::processUserRead(UserQuery * ur)
{
    if (ur->getType() == Show)
    {
        UserResponse * response = new UserResponse(ur);

        RecordRange record_range = ur->getRecords();
        if ( record_range.empty() )
        {
            PASimUtility::expandRange("1-", record_range, getNumberOfRecords());
        }


        if (record_range.size() > 0)
        {
            if (ur->getOutputFormat() == f_hex)
            {
                // pre-flight std::string memory-handling performance-enhancement
                // the '2' is for the characters that each byte takes in hex
                response->m_responseData.reserve(2 * sizeof(PASimTableIscsWashRecord) * record_range.size() );
            }
			else if (ur->getOutputFormat() == f_tabular)
			{
                response->m_responseData.reserve((2 * 38 * record_range.size()) + 38 );
			}

            char buffStr[64];

            if (ur->getOutputFormat() == f_tabular)
            {

                std::string interline = "+----------------------------+-----+\n";
                std::string boolstr = "";
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.validityField==Relevant), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|             Validity field | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                sprintf(buffStr, "|                  WP Status | %.3u |\n", m_tableData.wpStatus );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.noWash!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|                    No Wash | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.dailyRinse!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|                Daily Rinse | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.dailyDetergent!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|            Daily Detergent | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.weeklyWash!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|                Weekly Wash | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.cabEndWashCompleted!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|     Cab End Wash Completed | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.washCycleFullyCompleted!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| Wash Cycle Fully Completed | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.trainSpeedOutOfRange!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|   Train Speed Out of Range | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.trainStalledInTWP!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|       Train Stalled in TWP | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.trainIncorrectlyPositioned!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| Train Incorrectly Positn'd | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.washCycleStartFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|   Wash Cycle Start Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.pHCheck!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|                   pH Check | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.mainsWaterLow!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|            Mains Water Low | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.secondBrushRinseAndFinalRinseSpray!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| 2nd Brush Rinse & Final... | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.cabEndWashGantryMovementFailureHor!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| End Wash Gantry Mvmt F Hor | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.cabEndWashGantryMovementFailureVer!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| End Wash Gantry Mvmt F Ver | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.cabEndWashBrushMovementFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| End Wash Brush Mvmt Failur | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.emergencyStopActivated!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|   Emergency Stop Activated | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.pnuematicFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|          Pnuematic Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.brushActuationFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|    Brush Actuation Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.sideRoofBrushingFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| Side/Roof Brushing Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.endBrushingFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|       End Brushing Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.airBlowFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|           Air Blow Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.preWetFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|            Pre-Wet Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.recycledRinseFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|     Recycled Rinse Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.waterRecyclingFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|    Water Recycling Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.detergentFailure!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|          Detergent Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                PASimUtility::bool2string( (m_tableData.detergentLowAlarms!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|       Detergent Low Alarms | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

				PASimUtility::bool2string( (m_tableData.airPressureLow!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|           Air Pressure Low | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

				PASimUtility::bool2string( (m_tableData.nonCriticalSummaryAlarm!=0), boolstr, PASimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|         Non Critical Alarm | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);
            }
            else if (ur->getOutputFormat() == f_hex)
            {
#define APPENDBYTESTRING(field) sprintf(buffStr, "%.2X", (Byte)(m_tableData.field)); response->m_responseData.append(buffStr)
                APPENDBYTESTRING(validityField);
                APPENDBYTESTRING(wpStatus);
                APPENDBYTESTRING(noWash);
                APPENDBYTESTRING(dailyRinse);
                APPENDBYTESTRING(dailyDetergent);
                APPENDBYTESTRING(weeklyWash);
                APPENDBYTESTRING(cabEndWashCompleted);
                APPENDBYTESTRING(washCycleFullyCompleted);
                APPENDBYTESTRING(spare1);
                APPENDBYTESTRING(trainSpeedOutOfRange);
                APPENDBYTESTRING(trainStalledInTWP);
                APPENDBYTESTRING(trainIncorrectlyPositioned);
                APPENDBYTESTRING(spare2);
                APPENDBYTESTRING(washCycleStartFailure);
                APPENDBYTESTRING(pHCheck);
                APPENDBYTESTRING(mainsWaterLow);
                APPENDBYTESTRING(secondBrushRinseAndFinalRinseSpray);
                APPENDBYTESTRING(cabEndWashGantryMovementFailureHor);
                APPENDBYTESTRING(cabEndWashGantryMovementFailureVer);
                APPENDBYTESTRING(cabEndWashBrushMovementFailure);
                APPENDBYTESTRING(emergencyStopActivated);
                APPENDBYTESTRING(pnuematicFailure);
                APPENDBYTESTRING(brushActuationFailure);
                APPENDBYTESTRING(sideRoofBrushingFailure);
                APPENDBYTESTRING(endBrushingFailure);
                APPENDBYTESTRING(airBlowFailure);
                APPENDBYTESTRING(preWetFailure);
                APPENDBYTESTRING(recycledRinseFailure);
                APPENDBYTESTRING(waterRecyclingFailure);
                APPENDBYTESTRING(detergentFailure);
                APPENDBYTESTRING(detergentLowAlarms);
				APPENDBYTESTRING(airPressureLow);
                APPENDBYTESTRING(nonCriticalSummaryAlarm);
            }
        }

        return response;
    }
    return 0;
}


UserResponse * PASimTableIscsWash::processUserWrite(UserQuery * ur)
{
    return 0;
}


void PASimTableIscsWash::toStream(std::ostream& os) const
{
}


void PASimTableIscsWash::fromStream(const std::ostream& os)
{
}


void PASimTableIscsWash::toNetwork(ByteVector & n)
{
    PASimUtility::putByte((Byte)(m_tableData.validityField), n);
    PASimUtility::putByte(m_tableData.wpStatus, n);
    PASimUtility::putByte(m_tableData.noWash, n);
    PASimUtility::putByte(m_tableData.dailyRinse, n);
    PASimUtility::putByte(m_tableData.dailyDetergent, n);
    PASimUtility::putByte(m_tableData.weeklyWash, n);
    PASimUtility::putByte(m_tableData.cabEndWashCompleted, n);
    PASimUtility::putByte(m_tableData.washCycleFullyCompleted, n);
    PASimUtility::putByte(m_tableData.spare1, n);
    PASimUtility::putByte(m_tableData.trainSpeedOutOfRange, n);
    PASimUtility::putByte(m_tableData.trainStalledInTWP, n);
    PASimUtility::putByte(m_tableData.trainIncorrectlyPositioned, n);
    PASimUtility::putByte(m_tableData.spare2, n);
    PASimUtility::putByte(m_tableData.washCycleStartFailure, n);
    PASimUtility::putByte(m_tableData.pHCheck, n);
    PASimUtility::putByte(m_tableData.mainsWaterLow, n);
    PASimUtility::putByte(m_tableData.secondBrushRinseAndFinalRinseSpray, n);
    PASimUtility::putByte(m_tableData.cabEndWashGantryMovementFailureHor, n);
    PASimUtility::putByte(m_tableData.cabEndWashGantryMovementFailureVer, n);
    PASimUtility::putByte(m_tableData.cabEndWashBrushMovementFailure, n);
    PASimUtility::putByte(m_tableData.emergencyStopActivated, n);
    PASimUtility::putByte(m_tableData.pnuematicFailure, n);
    PASimUtility::putByte(m_tableData.brushActuationFailure, n);
    PASimUtility::putByte(m_tableData.sideRoofBrushingFailure, n);
    PASimUtility::putByte(m_tableData.endBrushingFailure, n);
    PASimUtility::putByte(m_tableData.airBlowFailure, n);
    PASimUtility::putByte(m_tableData.preWetFailure, n);
    PASimUtility::putByte(m_tableData.recycledRinseFailure, n);
    PASimUtility::putByte(m_tableData.waterRecyclingFailure, n);
    PASimUtility::putByte(m_tableData.detergentFailure, n);
    PASimUtility::putByte(m_tableData.detergentLowAlarms, n);
	PASimUtility::putByte(m_tableData.airPressureLow, n);
    PASimUtility::putByte(m_tableData.nonCriticalSummaryAlarm, n);
    PASimUtility::putByte(0, n);
}


void PASimTableIscsWash::fromNetwork(const ByteVector & s)
{
    int x = 0;

    m_tableData.validityField = (DataValidity)PASimUtility::getByte(s,x);
    m_tableData.wpStatus = PASimUtility::getByte(s,x);
    m_tableData.noWash = PASimUtility::getByte(s,x);
    m_tableData.dailyRinse = PASimUtility::getByte(s,x);
    m_tableData.dailyDetergent = PASimUtility::getByte(s,x);
    m_tableData.weeklyWash = PASimUtility::getByte(s,x);
    m_tableData.cabEndWashCompleted = PASimUtility::getByte(s,x);
    m_tableData.washCycleFullyCompleted = PASimUtility::getByte(s,x);
    m_tableData.spare1 = PASimUtility::getByte(s,x);
    m_tableData.trainSpeedOutOfRange = PASimUtility::getByte(s,x);
    m_tableData.trainStalledInTWP = PASimUtility::getByte(s,x);
    m_tableData.trainIncorrectlyPositioned = PASimUtility::getByte(s,x);
    m_tableData.spare2 = PASimUtility::getByte(s,x);
    m_tableData.washCycleStartFailure = PASimUtility::getByte(s,x);
    m_tableData.pHCheck = PASimUtility::getByte(s,x);
    m_tableData.mainsWaterLow = PASimUtility::getByte(s,x);
    m_tableData.secondBrushRinseAndFinalRinseSpray = PASimUtility::getByte(s,x);
    m_tableData.cabEndWashGantryMovementFailureHor = PASimUtility::getByte(s,x);
    m_tableData.cabEndWashGantryMovementFailureVer = PASimUtility::getByte(s,x);
    m_tableData.cabEndWashBrushMovementFailure = PASimUtility::getByte(s,x);
    m_tableData.emergencyStopActivated = PASimUtility::getByte(s,x);
    m_tableData.pnuematicFailure = PASimUtility::getByte(s,x);
    m_tableData.brushActuationFailure = PASimUtility::getByte(s,x);
    m_tableData.sideRoofBrushingFailure = PASimUtility::getByte(s,x);
    m_tableData.endBrushingFailure = PASimUtility::getByte(s,x);
    m_tableData.airBlowFailure = PASimUtility::getByte(s,x);
    m_tableData.preWetFailure = PASimUtility::getByte(s,x);
    m_tableData.recycledRinseFailure = PASimUtility::getByte(s,x);
    m_tableData.waterRecyclingFailure = PASimUtility::getByte(s,x);
    m_tableData.detergentFailure = PASimUtility::getByte(s,x);
    m_tableData.detergentLowAlarms = PASimUtility::getByte(s,x);
	m_tableData.airPressureLow = PASimUtility::getByte(s,x);
    m_tableData.nonCriticalSummaryAlarm = PASimUtility::getByte(s,x);
}


void PASimTableIscsWash::getByteVector(ByteVector & bv) const
{
}

