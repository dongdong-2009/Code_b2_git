/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/PRIV_3001/3001/transactive/app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsWash.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2018/03/14 13:51:20 $
  * Last modified by:  $Author: lim.cy $
  *
  *
  * Definition file for class ATSSimTableIscsWash.
  */
#include "app/ats/ATSSimulatorLibrary/src/ATSSimTableIscsWash.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUtility.h"
#include "app/ats/ATSSimulatorLibrary/src/ATSSimUserProtocol.h"

using namespace TA_IRS_App::ATS_Sim;

ATSSimTableIscsWash::ATSSimTableIscsWash(ATSSimTableQueryProcessor * parent, LocationType loc)
: ATSSimTable(IscsWash,parent, loc)
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


ATSSimTableIscsWash::~ATSSimTableIscsWash()
{
}


unsigned short ATSSimTableIscsWash::getTableSize() const
{
    return (m_numberOfRecords * 34);
}



UserResponse * ATSSimTableIscsWash::processUserRead(UserQuery * ur)
{
    if (ur->getType() == Show)
    {
        UserResponse * response = new UserResponse(ur);

        RecordRange record_range = ur->getRecords();
        if ( record_range.empty() )
        {
            ATSSimUtility::expandRange("1-", record_range, getNumberOfRecords());
        }


        if (record_range.size() > 0)
        {
            if (ur->getOutputFormat() == f_hex)
            {
                // pre-flight std::string memory-handling performance-enhancement
                // the '2' is for the characters that each byte takes in hex
                response->m_responseData.reserve(2 * sizeof(ATSSimTableIscsWashRecord) * record_range.size() );
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

                ATSSimUtility::bool2string( (m_tableData.validityField==Relevant), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|             Validity field | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                sprintf(buffStr, "|                  WP Status | %.3u |\n", m_tableData.wpStatus );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.noWash!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|                    No Wash | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.dailyRinse!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|                Daily Rinse | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.dailyDetergent!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|            Daily Detergent | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.weeklyWash!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|                Weekly Wash | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.cabEndWashCompleted!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|     Cab End Wash Completed | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.washCycleFullyCompleted!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| Wash Cycle Fully Completed | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.trainSpeedOutOfRange!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|   Train Speed Out of Range | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.trainStalledInTWP!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|       Train Stalled in TWP | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.trainIncorrectlyPositioned!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| Train Incorrectly Positn'd | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.washCycleStartFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|   Wash Cycle Start Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.pHCheck!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|                   pH Check | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.mainsWaterLow!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|            Mains Water Low | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.secondBrushRinseAndFinalRinseSpray!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| 2nd Brush Rinse & Final... | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.cabEndWashGantryMovementFailureHor!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| End Wash Gantry Mvmt F Hor | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.cabEndWashGantryMovementFailureVer!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| End Wash Gantry Mvmt F Ver | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.cabEndWashBrushMovementFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| End Wash Brush Mvmt Failur | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.emergencyStopActivated!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|   Emergency Stop Activated | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.pnuematicFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|          Pnuematic Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.brushActuationFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|    Brush Actuation Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.sideRoofBrushingFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "| Side/Roof Brushing Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.endBrushingFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|       End Brushing Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.airBlowFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|           Air Blow Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.preWetFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|            Pre-Wet Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.recycledRinseFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|     Recycled Rinse Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.waterRecyclingFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|    Water Recycling Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.detergentFailure!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|          Detergent Failure | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

                ATSSimUtility::bool2string( (m_tableData.detergentLowAlarms!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|       Detergent Low Alarms | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

				ATSSimUtility::bool2string( (m_tableData.airPressureLow!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
                sprintf(buffStr, "|           Air Pressure Low | %s |\n", boolstr.c_str() );
                response->m_responseData.append(buffStr);
                response->m_responseData.append(interline);

				ATSSimUtility::bool2string( (m_tableData.nonCriticalSummaryAlarm!=0), boolstr, ATSSimUtility::booleanOutputYesNo );
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


UserResponse * ATSSimTableIscsWash::processUserWrite(UserQuery * ur)
{
    return 0;
}


void ATSSimTableIscsWash::toStream(std::ostream& os) const
{
}


void ATSSimTableIscsWash::fromStream(const std::ostream& os)
{
}


void ATSSimTableIscsWash::toNetwork(ByteVector & n)
{
    ATSSimUtility::putByte((Byte)(m_tableData.validityField), n);
    ATSSimUtility::putByte(m_tableData.wpStatus, n);
    ATSSimUtility::putByte(m_tableData.noWash, n);
    ATSSimUtility::putByte(m_tableData.dailyRinse, n);
    ATSSimUtility::putByte(m_tableData.dailyDetergent, n);
    ATSSimUtility::putByte(m_tableData.weeklyWash, n);
    ATSSimUtility::putByte(m_tableData.cabEndWashCompleted, n);
    ATSSimUtility::putByte(m_tableData.washCycleFullyCompleted, n);
    ATSSimUtility::putByte(m_tableData.spare1, n);
    ATSSimUtility::putByte(m_tableData.trainSpeedOutOfRange, n);
    ATSSimUtility::putByte(m_tableData.trainStalledInTWP, n);
    ATSSimUtility::putByte(m_tableData.trainIncorrectlyPositioned, n);
    ATSSimUtility::putByte(m_tableData.spare2, n);
    ATSSimUtility::putByte(m_tableData.washCycleStartFailure, n);
    ATSSimUtility::putByte(m_tableData.pHCheck, n);
    ATSSimUtility::putByte(m_tableData.mainsWaterLow, n);
    ATSSimUtility::putByte(m_tableData.secondBrushRinseAndFinalRinseSpray, n);
    ATSSimUtility::putByte(m_tableData.cabEndWashGantryMovementFailureHor, n);
    ATSSimUtility::putByte(m_tableData.cabEndWashGantryMovementFailureVer, n);
    ATSSimUtility::putByte(m_tableData.cabEndWashBrushMovementFailure, n);
    ATSSimUtility::putByte(m_tableData.emergencyStopActivated, n);
    ATSSimUtility::putByte(m_tableData.pnuematicFailure, n);
    ATSSimUtility::putByte(m_tableData.brushActuationFailure, n);
    ATSSimUtility::putByte(m_tableData.sideRoofBrushingFailure, n);
    ATSSimUtility::putByte(m_tableData.endBrushingFailure, n);
    ATSSimUtility::putByte(m_tableData.airBlowFailure, n);
    ATSSimUtility::putByte(m_tableData.preWetFailure, n);
    ATSSimUtility::putByte(m_tableData.recycledRinseFailure, n);
    ATSSimUtility::putByte(m_tableData.waterRecyclingFailure, n);
    ATSSimUtility::putByte(m_tableData.detergentFailure, n);
    ATSSimUtility::putByte(m_tableData.detergentLowAlarms, n);
	ATSSimUtility::putByte(m_tableData.airPressureLow, n);
    ATSSimUtility::putByte(m_tableData.nonCriticalSummaryAlarm, n);
    ATSSimUtility::putByte(0, n);
}


void ATSSimTableIscsWash::fromNetwork(const ByteVector & s)
{
    int x = 0;

    m_tableData.validityField = (DataValidity)ATSSimUtility::getByte(s,x);
    m_tableData.wpStatus = ATSSimUtility::getByte(s,x);
    m_tableData.noWash = ATSSimUtility::getByte(s,x);
    m_tableData.dailyRinse = ATSSimUtility::getByte(s,x);
    m_tableData.dailyDetergent = ATSSimUtility::getByte(s,x);
    m_tableData.weeklyWash = ATSSimUtility::getByte(s,x);
    m_tableData.cabEndWashCompleted = ATSSimUtility::getByte(s,x);
    m_tableData.washCycleFullyCompleted = ATSSimUtility::getByte(s,x);
    m_tableData.spare1 = ATSSimUtility::getByte(s,x);
    m_tableData.trainSpeedOutOfRange = ATSSimUtility::getByte(s,x);
    m_tableData.trainStalledInTWP = ATSSimUtility::getByte(s,x);
    m_tableData.trainIncorrectlyPositioned = ATSSimUtility::getByte(s,x);
    m_tableData.spare2 = ATSSimUtility::getByte(s,x);
    m_tableData.washCycleStartFailure = ATSSimUtility::getByte(s,x);
    m_tableData.pHCheck = ATSSimUtility::getByte(s,x);
    m_tableData.mainsWaterLow = ATSSimUtility::getByte(s,x);
    m_tableData.secondBrushRinseAndFinalRinseSpray = ATSSimUtility::getByte(s,x);
    m_tableData.cabEndWashGantryMovementFailureHor = ATSSimUtility::getByte(s,x);
    m_tableData.cabEndWashGantryMovementFailureVer = ATSSimUtility::getByte(s,x);
    m_tableData.cabEndWashBrushMovementFailure = ATSSimUtility::getByte(s,x);
    m_tableData.emergencyStopActivated = ATSSimUtility::getByte(s,x);
    m_tableData.pnuematicFailure = ATSSimUtility::getByte(s,x);
    m_tableData.brushActuationFailure = ATSSimUtility::getByte(s,x);
    m_tableData.sideRoofBrushingFailure = ATSSimUtility::getByte(s,x);
    m_tableData.endBrushingFailure = ATSSimUtility::getByte(s,x);
    m_tableData.airBlowFailure = ATSSimUtility::getByte(s,x);
    m_tableData.preWetFailure = ATSSimUtility::getByte(s,x);
    m_tableData.recycledRinseFailure = ATSSimUtility::getByte(s,x);
    m_tableData.waterRecyclingFailure = ATSSimUtility::getByte(s,x);
    m_tableData.detergentFailure = ATSSimUtility::getByte(s,x);
    m_tableData.detergentLowAlarms = ATSSimUtility::getByte(s,x);
	m_tableData.airPressureLow = ATSSimUtility::getByte(s,x);
    m_tableData.nonCriticalSummaryAlarm = ATSSimUtility::getByte(s,x);
}


void ATSSimTableIscsWash::getByteVector(ByteVector & bv) const
{
}

