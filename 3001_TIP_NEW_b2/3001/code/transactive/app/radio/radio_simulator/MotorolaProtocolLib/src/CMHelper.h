/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_simulator/src/CMHelper.h $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  * Helper methods to encode/decode SDTS messages
  */


#ifndef CMHelper_H
#define CMHelper_H

#include <ostream>
#include <vector>
#include <map>
#include <map>
#include "CMTypes.h"
#include "CMData.h"

//#include "app/radio/radio_simulator_library/src/RadioIdentity.h"

#include "app/radio/radio_simulator/TrainSimulator/SimulatorHelper.h"
#include "app/radio/radio_simulator/TrainSimulator/SimulatorConfig.h"

class CMHelper
{
public:
    static std::vector<unsigned char> encodeSSI(unsigned int x);
    static unsigned int decodeSSI(const std::vector<unsigned char> &x);
    static std::string ssiToString(unsigned int x);
    static std::string ssiToString(const std::vector<unsigned char> &x);
    
    static std::vector<unsigned char> encodeCM_PDUType(CMPDUType x);
    static CMPDUType decodeCM_PDUType(const std::vector<unsigned char> &x);
    static std::string CM_PDUTypeToString(CMPDUType x);
    static std::string CM_PDUTypeToString(const std::vector<unsigned char> &x);

    static std::vector<unsigned char> encodeCM_ProtocolIdentifier(CMProtocolIdentifier x);
    static CMProtocolIdentifier decodeCM_ProtocolIdentifier(const std::vector<unsigned char> &x);
    static std::string CM_ProtocolIdentifierToString(CMProtocolIdentifier x);
    static std::string CM_ProtocolIdentifierToString(const std::vector<unsigned char> &x);

    static std::vector<unsigned char> encodeCM_Flag(CMSDSType sdsType, CMServiceSelection select);


    static CMSDSType decodeCM_SDSType(const std::vector<unsigned char> &x);
    static std::string CM_SDSTypeToString(CMSDSType x);
    static std::string CM_SDSTypeToString(const std::vector<unsigned char> &x);

    static CMServiceSelection decodeCM_ServiceSelection(const std::vector<unsigned char> &x);
    static std::string CM_ServiceSelectionToString(CMServiceSelection x);
    static std::string CM_ServiceSelectionToString(const std::vector<unsigned char> &x);

    static std::vector<unsigned char> encodeCM_ChannelSelectionMask(CMChannelSelectionMask x);
    static CMChannelSelectionMask decodeCM_ChannelSelectionMask(const std::vector<unsigned char> &x);
    static std::string CM_ChannelSelectionMaskToString(CMChannelSelectionMask x);
    static std::string CM_ChannelSelectionMaskToString(const std::vector<unsigned char> &x);

    static std::vector<unsigned char> encodeCM_RepeatRequest(CMRepeatRequest x);
    static CMRepeatRequest decodeCM_RepeatRequest(const std::vector<unsigned char> &x);
    static std::string CM_RepeatRequestToString(CMRepeatRequest x);
    static std::string CM_RepeatRequestToString(const std::vector<unsigned char> &x);

    //static std::vector<unsigned char> encodeCM_RegistrationType(CMRegistrationType x);
	static std::vector<unsigned char> encodeCM_RegistrationType();
    static CMRegistrationType decodeCM_RegistrationType(const std::vector<unsigned char> &x);
    static std::string CM_RegistrationTypeToString(CMRegistrationType x);
    static std::string CM_RegistrationTypeToString(const std::vector<unsigned char> &x);

    static std::vector<unsigned char> encodeCM_DeliveryStatus(CMDeliveryStatus x);
    static CMDeliveryStatus decodeCM_DeliveryStatus(const std::vector<unsigned char> &x);
    static std::string CM_DeliveryStatusToString(CMDeliveryStatus x);
    static std::string CM_DeliveryStatusToString(const std::vector<unsigned char> &x);
    
    static std::vector<unsigned char> encodeTL_PDUType(TLPDUType x);
    static TLPDUType decodeTL_PDUType(const std::vector<unsigned char> &x);
    static std::string TL_PDUTypeToString(TLPDUType x);
    static std::string TL_PDUTypeToString(const std::vector<unsigned char> &x);

    static std::vector<unsigned char> encodeTL_DeliveryStatus(TLDeliveryStatus x);
    static TLDeliveryStatus decodeTL_DeliveryStatus(const std::vector<unsigned char> &x);
    static std::string TL_DeliveryStatusToString(TLDeliveryStatus x);
    static std::string TL_DeliveryStatusToString(const std::vector<unsigned char> &x);

	static std::vector<unsigned char> encodeCM_RegistrationStatus(CMRegistrationStatus x);

	static std::vector<unsigned char> encodeTL_Flag(bool recFlag, bool conFlag, bool stoFlag);    
	static bool decodeTL_REC(unsigned char x);
	static bool decodeTL_CON(unsigned char x);
	static bool decodeTL_STO(unsigned char x);

	static std::vector<unsigned char> encode_u16( unsigned short x);
	static std::vector<unsigned char> encode_u24( unsigned long x);
	static std::vector<unsigned char> encode_u32( unsigned long x);

	static unsigned short decode_u16( const std::vector<unsigned char> &x);
	static unsigned long decode_u32( const std::vector<unsigned char> &x);

	static std::vector<unsigned char> buildMotoralaProtocolHeaders(const std::string& pecEventMsge );

	static std::vector<unsigned char> buildMotoralaProtocolHeaders(const std::vector<unsigned char>& data);

	static std::vector<unsigned char>* buildMotoralaProtocolHeadersHeartBeat(const std::vector<unsigned char>& data);

	static std::vector<unsigned char> buildMotorolaHeadersTIMSCommand(const std::vector<unsigned char>& data,bool bIsUsePrimary);

    //friend std::ostream &operator<<(std::ostream &os, CMPDUType dat);
    //friend std::ostream &operator<<(std::ostream &os, CMProtocolIdentifier dat);
    //friend std::ostream &operator<<(std::ostream &os, CMSDSType dat);
    //friend std::ostream &operator<<(std::ostream &os, CMServiceSelection dat);
    //friend std::ostream &operator<<(std::ostream &os, CMChannelSelectionMask dat);
    //friend std::ostream &operator<<(std::ostream &os, CMRepeatRequest dat);
    //friend std::ostream &operator<<(std::ostream &os, CMRegistrationType dat);
    //friend std::ostream &operator<<(std::ostream &os, CMDeliveryStatus dat);
    //friend std::ostream &operator<<(std::ostream &os, TLPDUType dat);
    //friend std::ostream &operator<<(std::ostream &os, TLDeliveryStatus dat);

	static unsigned int Hex2Int(char character, int pos);
	static int getDigitNumber(unsigned long number);

	static std::string decode_cmData(const std::string& cmdValue);
	static std::string decode_tlLength(const std::string& cmdValue);
	
	static void buildCMheader(CMData& cmData, std::vector<unsigned char>& rawData);
	static const char* MOTORAL_PROTOCOL_HEADERS;

	typedef std::map<std::string, int> MapReference;

	static void stringTokenizer(const std::string& strMge, const std::string& delimeter, std::vector<unsigned char>& output);
	static std::map<std::string, int> getMapReferenceTable();
	static unsigned char computePECNumber(unsigned char carNumber,unsigned char PECNumber );
	static void constructTrainEvent(const std::vector<unsigned char>& pecEventMsg, std::vector<unsigned char>& output);
	
	static void constructTIMSCommand(const std::vector<unsigned char>& trainInfo,  std::vector<unsigned char>& output);


	static void calculateHeaderLength(int protcolLength, std::vector<unsigned char>& output);
	static int getRadioIDFrmTrainIDConfig(int trainID);
	static std::string getPECHookMessage(const std::string& data,const std::string& pecSource);

private:
    // disabled methods
    CMHelper();

	static SimulatorConfig* m_simulatorConfig;

	
};

#endif
