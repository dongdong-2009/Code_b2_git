/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/4677/transactive/app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSMsgFactory.cpp $
  * @author:  Anggiono
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 11:47:25 $
  * Last modified by:  $Author: builder $
  *
  */


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSMsgFactory.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"


#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSProtocolEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Data.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/SDTSHelper.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_DataEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_ReportEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TL_ShortReportEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_RegisterResponse.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_ReportEvent.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/CM_Broadcast.h"
#include "app/radio/radio_protocol/MotorolaElite/SDTSProtocol/src/TLCommon.h"

using namespace TA_Base_Core;

namespace TA_IRS_App {


	SDTSProtocolEvent* SDTSMsgFactory::createSDTSProtocolMsg(SDTSProtocolEvent* x)
	{
		FUNCTION_ENTRY("createSDTSProtocolMsg");
		SDTSProtocolEvent *ret = NULL;

		TA_ASSERT( x != NULL, "SDTSProtocolEvent is NULL");

		std::vector<unsigned char> rawData = x->getRawMessage();
		TA_ASSERT( rawData.size() > 0, "Raw data is empty");

		switch(rawData[CMCommon1::CM_PDUTYPE_INDEX])
		{
			case CMPDUTYPE_DATA_REQ:
                {
                    CM_Data* pCMdata = new CM_Data();

                    try
                    {
                        if (!pCMdata->parseData(rawData))
                        {
                            delete pCMdata;
                            pCMdata = NULL;
                            return NULL;
                        }
                    }
                    catch (TransactiveException& e)
                    {
                        delete pCMdata;
                        throw e;
                    }

					switch(rawData[TLCommon::TL_DATA_PDU_TYPE_INDEX])
					{
						case TLPDUTYPE_DATA:
						{
							TL_DataEvent* pTL = new TL_DataEvent();
							pTL->setCMHeader(pCMdata);

                            try
                            {
							    if ( !pTL->parseData(rawData) )
							    {
								    delete pTL;
								    pTL = NULL;
							    }
                            }
                            catch (TransactiveException& e)
                            {
                                delete pTL;
                                throw e;
                            }
							ret = pTL;
						}
						break;
						case TLPDUTYPE_REPORT:
						{
						
							TL_ReportEvent* pTL = new TL_ReportEvent();
							pTL->setCMHeader(pCMdata);

                            try
                            {
							    if ( !pTL->parseData(rawData) )
							    {
								    delete pTL;
								    pTL = NULL;
							    }
                            }
                            catch (TransactiveException& e)
                            {
                                delete pTL;
                                throw e;
                            }
							ret = pTL;
						}
						break;
						case TLPDUTYPE_SHORTREPORT:
							TL_ShortReportEvent* pTL = new TL_ShortReportEvent();
							pTL->setCMHeader(pCMdata);

                            try
                            {
							    if ( !pTL->parseData(rawData) )
							    {
								    delete pTL;
								    pTL = NULL;
							    }
                            }
                            catch (TransactiveException& e)
                            {
                                delete pTL;
                                throw e;
                            }
							ret = pTL;
					}
                }
				break;
			case CMPDUTYPE_REPORT_RESP: 
                {
                    CM_ReportEvent* report= new CM_ReportEvent();

                    try
                    {
                        if (!report->parseData(rawData))
                        {
                            delete report;
                            report = NULL;
                        }
                    }
                    catch (TransactiveException& e)
                    {
                        delete report;
                        throw e;
                    }
                    ret = report;
                }				 
				 break;
			case CMPDUTYPE_REGISTER_RESP: 
                {
                    CM_RegisterResponse* response = new CM_RegisterResponse();

                    try
                    {
                        if (!response->parseData(rawData))
                        {
                            delete response;
                            response = NULL;
                        }
                    }
                    catch (TransactiveException& e)
                    {
                        delete response;
                        throw e;
                    }
                    ret = response;
                }
                break;

			case CMPDUTYPE_BROADCAST:
				{
                    CM_Broadcast* pbroadcast = new CM_Broadcast();

                    try
                    {
                        if (!pbroadcast->parseData(rawData))
                        {
                            delete pbroadcast;
                            pbroadcast = NULL;
                            return NULL;
                        }
                    }
                    catch (TransactiveException& e)
                    {
                        delete pbroadcast;
                        throw e;
                    }
					
					TL_DataEvent* pTL = new TL_DataEvent();
					pTL->setCMHeader(pbroadcast);

                    try
                    {
					    if ( !pTL->parseData(rawData) )
					    {
                            delete pTL;
                            pTL = NULL;
					    }
                    }
                    catch (TransactiveException& e)
                    {
                        delete pTL;
                        throw e;
                    }
					ret = pTL;
				}
				 break;
		}

		FUNCTION_EXIT;
		return ret;
	}



}
