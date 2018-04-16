
#include "MyEventHandler.h"
#include "Command.h"
#include "Iec104Api.h"
#include<stdio.h>


void MyEvent_Handler::handle_sv(int rtuAddr,int infoAddr,b_ui8 * val,b_ui8 len,b_ui8 svType, b_ui8 bresult,b_bool tmflag)
{
	short sval=0;float fval=0.0;
	b_ui16 nva =0;
	switch(svType)
	{
		case 0:
			memcpy(&nva,&val[0],2);
			displayData("归一化值，地址_%x,值_%x\n",infoAddr,nva);
			break;
		case 1:
			memcpy(&sval,&val[0],2);
			displayData("标度化值，地址_%x,值_%x \n",infoAddr,sval);
			break;
		case 2:
			memcpy(&fval,&val[0],4);
			displayData("短浮点数，地址_%x,值_%f \n",infoAddr,fval);
			break;
		default:
			displayData("未知\n");
	}
	CCommand cmd;
	switch(bresult)
	{
		case 0:
			displayData("选择失败\n");
			break;
		case 1:
			displayData("选择成功\n");
			switch(svType)
			{
				case 0:
					//sval = *(unsigned short *)&val[0];					memcpy(&sval,&val[0],2);
					IEC104_API::getInstance()->undo_SetPointNVA_DO(rtuAddr,infoAddr,nva,tmflag,127);
					break;
				case 1:
					//sval = *(unsigned short *)&val[0];
					memcpy(&sval,&val[0],2);
					IEC104_API::getInstance()->undo_SetPointSVA_DO(rtuAddr,infoAddr,sval,tmflag,0);
					break;
				case 2:
					//fval = *(float*)&val[0];
					memcpy(&fval,&val[0],4);
					IEC104_API::getInstance()->undo_SetPointSFA_DO(rtuAddr,infoAddr,fval,tmflag,127);
					break;
				default:
					displayData("未知");
			}
			break;
		case 2:
			displayData("执行失败\n");
			break;
		case 3:
			displayData("执行成功\n");
			break;
		case 4:
			displayData("撤销失败\n");
			break;
		case 5:
			displayData("撤销成功\n");
			break;
	}
}

void MyEvent_Handler::handle_yk(int rtuAddr,int infoAddr,b_ui8 bVal,b_ui8  ykType,b_ui8  bresult,b_bool tmflag,SFepTime *ftime)	
{
	switch(ykType)
	{ 
	//case IEC104_Event_Handler::YK_SP:
	//	displayData("单点遥控，信息体地址_%x, 值_%d\n",infoAddr,bVal);
	//	break;
	case IEC104_Event_Handler::YK_DP:
		displayData("双点遥控，信息体地址_%d, 值_%d\n",infoAddr,bVal);
		break;
	default:
		displayData("步调节，信息体地址_%d, 值_%d\n",infoAddr,bVal);
		break;
	}
	switch(bresult)
	{
		case 0:
			displayData("选择失败\n");
			break;
		case 1:
			displayData("选择成功\n");
			switch(ykType)
			{
				//case IEC104_Event_Handler::YK_SP:
					//IEC104_API::getInstance()->undo_Single_DO(rtuAddr,infoAddr,bVal,tmflag,31);
					//break;
				case IEC104_Event_Handler::YK_DP:
					IEC104_API::getInstance()->undo_Double_DO(rtuAddr,infoAddr,bVal,tmflag,31);
					break;
				default:
					IEC104_API::getInstance()->undo_RegulatingStep_DO(rtuAddr,infoAddr,bVal,tmflag,31);
					break;
			}
			break;
		case 2:
			displayData("执行失败\n");
			break;
		case 3:
			displayData("执行成功\n");
			break;
		case 4:
			displayData("撤销失败\n");
			break;
		case 5:
			displayData("撤销成功\n");
			break;
	}
}
 //=====================----文件传输--------=========================================
 
 void MyEvent_Handler::handle_File_Dir(int rtuAddr,int infoAddr,SFileInfo *FileInfo,int iCount)
 {
 	//printf("rtuAddr_%d infoAddr_%d\n files int directory are\n",rtuAddr,infoAddr);
	for(int i=0;i<iCount;i++)
	{
		//printf("fileName is %d,length %d state %d\n",FileInfo[i].fileName,FileInfo[i].fileLen,FileInfo[i].fileState);
		/*printf("%d年%d月%d日%d时%d分%d秒%d毫秒\n",
			FileInfo[i].timeStamp.year,
			FileInfo[i].timeStamp.month,
			FileInfo[i].timeStamp.day,
			FileInfo[i].timeStamp.hour,
			FileInfo[i].timeStamp.minute,
			FileInfo[i].timeStamp.second,
			FileInfo[i].timeStamp.millisecond);*/
	}
	IEC104_API::getInstance()->select_file(rtuAddr,infoAddr, FileInfo[0].fileName, 0);
 }

 void MyEvent_Handler::handle_File_Ready(int rtuAddr,int infoAddr,b_ui16 fileName)
 {
 	//printf("rtuAddr_%d infoAddr_%d\n",rtuAddr,infoAddr);
	//printf("file_%d is ready\n",fileName);

	IEC104_API::getInstance()->call_file(rtuAddr, infoAddr,fileName,0);
 }

 void MyEvent_Handler::handle_Section_Ready(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName)
 {
 	//printf("rtuAddr_%d infoAddr_%d\n",rtuAddr,infoAddr);
	//printf("fileName_%d  section %dis ready\n",fileName,bSecName);
 }

 void MyEvent_Handler::handle_Last_Segment(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName)
 {
 	//printf("rtuAddr_%d infoAddr_%d\n",rtuAddr,infoAddr);
	//printf("receive last segment\n");

	IEC104_API::getInstance()->affirm_file(rtuAddr, infoAddr, fileName, bSecName, CCommand::ACK_SECTION);

 }
void MyEvent_Handler::handle_Rx_Segment(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName,b_ui8 *btBuf,b_ui8 bLen)
{
	//printf("rtuAddr_%d infoAddr_%d\n",rtuAddr,infoAddr);
	//printf("receive	segment\n");
	writeToFile(btBuf, bLen,fileName);
}
void MyEvent_Handler::handle_Last_Section(int rtuAddr,int infoAddr,b_ui16 fileName,b_ui8 bSecName)
{
	//printf("rtuAddr_%d infoAddr_%d\n",rtuAddr,infoAddr);
	//printf("receive last section\n");

	IEC104_API::getInstance()->affirm_file(rtuAddr, infoAddr, fileName, bSecName, CCommand::ACK_FILE);
 
}

int  MyEvent_Handler::writeToFile(b_ui8 *buf,int size,int name)
{	char fileName[24];
	FILE*fp;
	int count=0,remain=size;
	
	 //itoa(name,fileName,10);
	 sprintf(fileName,"%d",name);
	 if((fp = fopen(fileName,"at+"))==NULL)
	 {
	 	//printf("open file %s fail!\n",fileName);
		return -1;
	 }

         if(fwrite(buf,remain,1,fp)!=1)
         {
         	//printf("write file error!\n");
         }
	fclose(fp);
	return 1;
	 
}

//========================------103扩展----====================

void MyEvent_Handler::handle_Modify_Value(struct SExResult SResult)
{
	int ret;
	CCommand cmd;
	//printf("rtuaddr_%d",SResult.iRtu);
	//printf("areaNo_%d",SResult.bAreaNo);
	//printf("修改定值");
	if((SResult.bResult&0X80) > 0)
		//printf("区号");
		;
	ret = SResult.bResult&0X07;
	switch(ret)
	{
		case 0:
			//printf("预置失败\n");
			break;
		case 1:
			//printf("预置成功\n");	
			if((SResult.bResult&0X80) > 0)//最高位是1，代表修改定值区号
			{			
				IEC104_API::getInstance()->modify_value_area_no_cancle(SResult.iRtu, 255, SResult.bAreaNo);
			}
			else
			{
				IEC104_API::getInstance()->modify_value_cancle(SResult.iRtu, 255, SResult.bAreaNo);
			}
			break;
		case 2:
			//printf("执行失败\n");
			break;
		case 3:
			//printf("执行成功\n");
			break;
		case 4:
			//printf("撤销失败\n");
			break;
		case 5:
			//printf("撤销成功\n");
			break;
		default:
			//printf("未知的结果");
			;
	}
	
}
void MyEvent_Handler::handle_Call_Value(int rtuAddr, struct SExCallValueResult sResult)
{
	int i;
	//printf("召唤定值");
	if((sResult.bResult&0x80) > 0)
	{
		//printf("区号");
		//printf("\n区号_%d\n",sResult.sData[0].bAreaNo);
		return;
	}
	//printf("\n总序号%d\n",sResult.bTotalNo);
	for(i=0;i<sResult.bCount;i++)
	{
		//printf("区号_%d",sResult.sData[i].bAreaNo);
		//printf(" 定值序号_%d",sResult.sData[i].bValueNo);
		//printf(" 定值_%4.4f\n",sResult.sData[i].fVal);
	}
	
}


void  MyEvent_Handler::handle_Protect_Action(SProtectActionRaw *pRaw,int iNum)
{
	int i,j;
	for(i=0;i<iNum;i++)
	{
		//printf("总序号 %d",pRaw[i].iTotalNo);
		//printf("保护动作号%d" ,pRaw[i].bProtectActionNo);
		//printf("双点值%d",pRaw[i].bDpointVal);
		//printf("相对时间%d",pRaw[i].iRelTime);
		//printf("故障号%d\n",pRaw[i].iFailNo);
		//printf("%d小时",pRaw[i].time.hour);
		//printf("%d分钟",pRaw[i].time.minute);
		//printf("%d毫秒\n",pRaw[i].time.milisecond);
		//printf("故障数%d\n",pRaw[i].bFailCount);
		for(j=0;j<pRaw[i].bFailCount;j++)
		{
			//printf("%d ",pRaw[i].sFailEq[j].iOffset);
			//printf("%f\n",pRaw[i].sFailEq[j].fVal);
		}
	}
}
void MyEvent_Handler::displayData(const char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char buffer[1024];
#ifdef _WIN32
	_vsnprintf(buffer, sizeof(buffer)-1, fmt, ap);
#else
	vsprintf(buffer, fmt, ap);
#endif
	va_end(ap);
	buffer[sizeof(buffer)-1] = '\0';
	std::string logStr=buffer;
	IEC104_API::getInstance()->logRecord(LOG_LEVEL_WARNING, logStr);
}

//---------------------------------------new --------------------------
void MyEvent_Handler::handleSinglePoint(int rtuAddr,b_ui8 bCot, vector<SSinglePoint> &vSinglePoint)
{
	int num = vSinglePoint.empty() ? -1 : static_cast<int>(vSinglePoint.size());
	SSinglePoint sp;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	for(int i=0;i< num;i++)
	{
		sp = vSinglePoint[i];
		displayData("point_0X%06x,SIQ{IV_%x,NT_%x,SB_%x,BL_%x,SPI_%x}\n",sp.iOffset,sp.SIQ.IV,sp.SIQ.NT,sp.SIQ.SB,sp.SIQ.BL,sp.SIQ.SPI);
	}
}
void MyEvent_Handler::handleSinglePointWithTimeTag(int rtuAddr,b_ui8 bCot, vector<SSinglePointTimeTag> &vSinglePointTm)
{
	int num = vSinglePointTm.empty() ? -1 : static_cast<int>(vSinglePointTm.size());
	SSinglePointTimeTag sptm;
	SFepTime time;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	for(int i=0;i< num;i++)
	{
		sptm = vSinglePointTm[i];
		time = sptm.cp56TimeTag;
		displayData("point_0X%06x,SIQ{IV_%x,NT_%x,SB_%x,BL_%x,SPI_%x}\n",sptm.iOffset,sptm.SIQ.IV,sptm.SIQ.NT,sptm.SIQ.SB,sptm.SIQ.BL,sptm.SIQ.SPI);
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}

}
void MyEvent_Handler::handleDoublePoint(int rtuAddr,b_ui8 bCot, vector<SDoublePoint> &vDoublePoint)
{
	int num = vDoublePoint.empty() ? -1 : static_cast<int>(vDoublePoint.size());
	SDoublePoint dp;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	for(int i=0;i< num;i++)
	{
		dp = vDoublePoint[i];
		displayData("point_0X%06x,DIQ{IV_%x,NT_%x,SB_%x,BL_%x,DPI_%x}\n",dp.iOffset,dp.DIQ.IV,dp.DIQ.NT,dp.DIQ.SB,dp.DIQ.BL,dp.DIQ.DPI);
	}
}
void MyEvent_Handler::handleDoublePointWithTimeTag(int rtuAddr,b_ui8 bCot, vector<SDoublePointTimeTag> &vDoublePointTm)
{
	int num = vDoublePointTm.empty() ? -1 : static_cast<int>(vDoublePointTm.size());
	SDoublePointTimeTag dptm;
	SFepTime time;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	for(int i=0;i< num;i++)
	{
		dptm =vDoublePointTm[i];
		time = dptm.cp56TimeTag;
		displayData("point_0X%06x,DIQ{IV_%x,NT_%x,SB_%x,BL_%x,DPI_%x}\n",dptm.iOffset,dptm.DIQ.IV,dptm.DIQ.NT,dptm.DIQ.SB,dptm.DIQ.BL,dptm.DIQ.DPI);
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}
}
void MyEvent_Handler::handleStepPosition(int rtuAddr,b_ui8 bCot, vector<SStepPosition> &vStepPosition)
{
	int num = vStepPosition.empty() ? -1 : static_cast<int>(vStepPosition.size());
	SStepPosition sp;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	b_ui8 VTI,QDS;
	for(int i=0;i< num;i++)
	{
		sp = vStepPosition[i];
		memcpy(&VTI,&sp.VTI,1);
		memcpy(&QDS,&sp.QDS,1);
		displayData("point_0X%06x,VTI{Tra_%x,Val_%x},QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",sp.iOffset, sp.VTI.Transient,sp.VTI.Value,\
			sp.QDS.IV,sp.QDS.NT,sp.QDS.SB,sp.QDS.BL,sp.QDS.OV);
		
	}
}
void MyEvent_Handler::handleStepPositionWithTimeTag(int rtuAddr, b_ui8 bCot, vector<SStepPositionTimeTag> &vStepPositionTm)
{
	int num = vStepPositionTm.empty() ? -1 : static_cast<int>(vStepPositionTm.size());
	SStepPositionTimeTag sptm;
	SFepTime time;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	for(int i=0;i< num;i++)
	{
		sptm =vStepPositionTm[i];
		time = sptm.cp56TimeTag;
		displayData("point_0X%06x,VTI{Tra_%x,Val_%x},QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",sptm.iOffset, sptm.VTI.Transient,sptm.VTI.Value,\
			sptm.QDS.IV,sptm.QDS.NT,sptm.QDS.SB,sptm.QDS.BL,sptm.QDS.OV);
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}
}
void MyEvent_Handler::handleBitstring32(int rtuAddr, b_ui8 bCot, vector<SBitstring32> &vBitstring32)
{
	int num = vBitstring32.empty() ? -1 : static_cast<int>(vBitstring32.size());
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	SBitstring32 bts32;
	for(int i=0;i< num;i++)
	{
		bts32 = vBitstring32[i];
		displayData("point_0X%06x,BSI_%x\n",bts32.iOffset,bts32.BSI);
		displayData("	QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",bts32.QDS.IV,bts32.QDS.NT,bts32.QDS.SB,bts32.QDS.BL,bts32.QDS.OV);
	}
}
void MyEvent_Handler::handleBitstring32WithTimeTag(int rtuAddr, b_ui8 bCot, vector<SBitstring32TimeTag> &vBitstring32Tm)
{
	int num = vBitstring32Tm.empty() ? -1 : static_cast<int>(vBitstring32Tm.size());
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	SBitstring32TimeTag bts32Tm;
	SFepTime time;
	for(int i=0;i< num;i++)
	{
		bts32Tm = vBitstring32Tm[i];
		time = bts32Tm.cp56TimeTag;
		displayData("point_0X%06x,BSI_%x\n",bts32Tm.iOffset,bts32Tm.BSI);
		displayData("	QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",bts32Tm.QDS.IV,bts32Tm.QDS.NT,bts32Tm.QDS.SB,bts32Tm.QDS.BL,bts32Tm.QDS.OV);
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}
}
void MyEvent_Handler::handleMeasureValueNVA(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueNVA> &vMeasuredValueNVA)
{
	int num = vMeasuredValueNVA.empty() ? -1 : static_cast<int>(vMeasuredValueNVA.size());
	SMeasuredValueNVA nva;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	b_ui16 NVA;
	for(int i=0;i< num;i++)
	{
		nva =vMeasuredValueNVA[i];
		memcpy(&NVA,&nva.NVA,2);
		displayData("point_0X%06x, NVA_%04x{%d}, QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",nva.iOffset,NVA,NVA,\
			nva.QDS.IV,nva.QDS.NT,nva.QDS.SB,nva.QDS.BL,nva.QDS.OV);
	}
}
void MyEvent_Handler::handleMeasureValueNVAWithoutQualityDescriptor(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueNVA2> &vMeasuredValueNVA2)
{
	int num = vMeasuredValueNVA2.empty() ? -1 : static_cast<int>(vMeasuredValueNVA2.size());
	SMeasuredValueNVA2 nva2;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	b_ui16 NVA;
	for(int i=0;i< num;i++)
	{
		nva2 = vMeasuredValueNVA2[i];
		memcpy(&NVA,&nva2.NVA,2);
		displayData("point_0X%06x, NVA_%04x{%d}\n",nva2.iOffset,NVA,NVA);
	}
}
void MyEvent_Handler::handleMeasureValueNVAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueNVATimeTag> &vMeasuredValueNVATm)
{
	int num = vMeasuredValueNVATm.empty() ? -1 : static_cast<int>(vMeasuredValueNVATm.size());
	SMeasuredValueNVATimeTag nvaTm;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	SFepTime time;
	b_ui16 NVA;
	for(int i=0;i< num;i++)
	{
		nvaTm =vMeasuredValueNVATm[i];
		memcpy(&NVA,&nvaTm.NVA,2);
		time = nvaTm.cp56TimeTag;
		displayData("point_0X%06x, NVA_%04x{%d}, QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",nvaTm.iOffset,NVA,NVA,\
			nvaTm.QDS.IV,nvaTm.QDS.NT,nvaTm.QDS.SB,nvaTm.QDS.BL,nvaTm.QDS.OV);
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}
}
void MyEvent_Handler::handleMeasureValueSVA(int rtuAddr, b_ui8 bCot,vector<SMeasuredValueSVA> &vMeasuredValueSVA)
{
	int num = vMeasuredValueSVA.empty() ? -1 : static_cast<int>(vMeasuredValueSVA.size());
	SMeasuredValueSVA sva;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	b_ui16 SVA;
	for(int i=0;i< num;i++)
	{
		sva =vMeasuredValueSVA[i];
		memcpy(&SVA, &sva.SVA,2);
		displayData("point_%06x,SVA_%04x{%d},QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",sva.iOffset,SVA,sva.SVA,\
			sva.QDS.IV,sva.QDS.NT,sva.QDS.SB,sva.QDS.BL,sva.QDS.OV);
	}
}
void MyEvent_Handler::handleMeasureValueSVAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSVATimeTag> &vMeasuredValueSVATm)
{
	int num = vMeasuredValueSVATm.empty() ? -1 : static_cast<int>(vMeasuredValueSVATm.size());
	SMeasuredValueSVATimeTag svaTm;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	SFepTime time;
	b_ui16 SVA;b_ui8 QDS;
	for(int i=0;i< num;i++)
	{
		svaTm =vMeasuredValueSVATm[i];
		time = svaTm.cp56TimeTag;
		memcpy(&SVA,&svaTm.SVA,2);
		memcpy(&QDS,&svaTm.QDS,1);
		displayData("point_%06x,SVA_%04x{%d},QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",svaTm.iOffset,SVA,svaTm.SVA,\
			svaTm.QDS.IV,svaTm.QDS.NT,svaTm.QDS.SB,svaTm.QDS.BL,svaTm.QDS.OV);
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}
}
void MyEvent_Handler::handleMeasureValueSFA(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSFA> &vMeasuredValueSFA)
{
	int num = vMeasuredValueSFA.empty() ? -1 : static_cast<int>(vMeasuredValueSFA.size());
	SMeasuredValueSFA sfa;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	b_ui32 SFA;float fval;
	for(int i=0;i< num;i++)
	{
		sfa =vMeasuredValueSFA[i];
		memcpy(&SFA,&sfa.SFA,4);
		memcpy(&fval,&sfa.SFA,4);
		displayData("point_0X%06x,SFA_%x{%f},QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",sfa .iOffset, SFA, fval,\
			sfa.QDS.IV,sfa.QDS.NT,sfa.QDS.SB,sfa.QDS.BL,sfa.QDS.OV);
	}
}
void MyEvent_Handler::handleMeasureValueSFAWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SMeasuredValueSFATimeTag> &vMeasuredValueSFATm)
{
	int num = vMeasuredValueSFATm.empty() ? -1 : static_cast<int>(vMeasuredValueSFATm.size());
	SMeasuredValueSFATimeTag sfaTm;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	SFepTime time;
	b_ui32 SFA;b_ui8 QDS;float fval;
	for(int i=0;i< num;i++)
	{
		sfaTm =vMeasuredValueSFATm[i];
		time = sfaTm.cp56TimeTag;
		memcpy(&SFA,&sfaTm .SFA,4);
		memcpy(&fval,&sfaTm .SFA,4);
		memcpy(&QDS,&sfaTm .QDS,1);
		displayData("point_0X%06x,SFA_%x{%f},QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",sfaTm.iOffset, SFA, fval,\
			sfaTm.QDS.IV,sfaTm.QDS.NT,sfaTm.QDS.SB,sfaTm.QDS.BL,sfaTm.QDS.OV);
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}
}
void MyEvent_Handler::handleIntegratedTotal(int rtuAddr,b_ui8 bCot,vector<SCounterValue>&vCounterValue)
{
	int num = vCounterValue.empty() ? -1 : static_cast<int>(vCounterValue.size());
	SCounterValue cv;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	b_ui8 sequence;
	for(int i=0;i< num;i++)
	{
		cv =vCounterValue[i];
		memcpy(&sequence, &cv.BCR.SequenceNotation,1);
		displayData("point_0X%06x, BCR{Count_%x{%d},Seq{IV_%x,CA_%x,CY_%x,SQ_%x}}\n",cv.iOffset, cv.BCR.Counter,cv.BCR.Counter,\
			cv.BCR.SequenceNotation.IV,cv.BCR.SequenceNotation.CA,cv.BCR.SequenceNotation.CY,cv.BCR.SequenceNotation.SQ);
	}
}
void MyEvent_Handler::handleIntegratedTotalWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SCounterValueTimeTag>&vCounterValueTm)
{
	int num = vCounterValueTm.empty() ? -1 : static_cast<int>(vCounterValueTm.size());
	SCounterValueTimeTag cvTm;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	SFepTime time;
	b_ui8 sequence;
	for(int i=0;i< num;i++)
	{
		cvTm=vCounterValueTm[i];
		time = cvTm.cp56TimeTag;
		memcpy(&sequence, &cvTm.BCR.SequenceNotation,1);
		displayData("point_0X%06x, BCR{Count_%x{%d},Seq{IV_%x,CA_%x,CY_%x,SQ_%x}}\n",cvTm.iOffset, cvTm.BCR.Counter,cvTm.BCR.Counter,\
			cvTm.BCR.SequenceNotation.IV,cvTm.BCR.SequenceNotation.CA,cvTm.BCR.SequenceNotation.CY,cvTm.BCR.SequenceNotation.SQ);
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}
}
void MyEvent_Handler:: handlePackedSinglePoint(int rtuAddr,b_ui8 bCot,vector<SPackedSinglePoint>&vPackedSinglePoint)
{
	int num = vPackedSinglePoint.empty() ? -1 : static_cast<int>(vPackedSinglePoint.size());
	SPackedSinglePoint psp;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	b_ui16 ST,CD;
	for(int i=0;i< num;i++)
	{
		psp=vPackedSinglePoint[i];
		memcpy(&ST, &psp.SCD.ST,2);
		memcpy(&CD,&psp.SCD.CD,2);
		displayData("point_0X%06x, SCD{ ST_%04x, CD_%04x}\n",psp.iOffset,ST,CD);
		displayData("	QDS{IV_%x,NT_%x,SB_%x,BL_%x,OV_%x}\n",psp.QDS.IV,psp.QDS.NT,psp.QDS.SB,psp.QDS.BL,psp.QDS.OV);
	}		
}
void MyEvent_Handler::handleEventProtectionWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SEventProtectionTimeTag>&vEventProtectionTimeTag) 
{
	int num = vEventProtectionTimeTag.empty() ? -1 : static_cast<int>(vEventProtectionTimeTag.size());
	SEventProtectionTimeTag epTm;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	SFepTime time;
	b_ui8 SEP;b_ui16 CP16;
	for(int i=0;i< num;i++)
	{
		epTm=vEventProtectionTimeTag[i];
		memcpy(&SEP, &epTm.SEP,1);
		memcpy(&CP16,&epTm.cp16TimeTag,2);
		displayData("point_0X%06x,SEP{IV_%x,NT_%x,SB_%x,BL_%x,EI_%x,ES_%x}, CP16_%04x }\n",epTm.iOffset, \
			epTm.SEP.IV,epTm.SEP.NT,epTm.SEP.SB,epTm.SEP.BL,epTm.SEP.EI,epTm.SEP.EventState,CP16);
		time = epTm.cp56TimeTag;
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}
}
void MyEvent_Handler::handleStartEventsWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SStartEventsTimeTag>&vStartEventsTimeTag)
{
	int num = vStartEventsTimeTag.empty() ? -1 : static_cast<int>(vStartEventsTimeTag.size());
	SStartEventsTimeTag seTm;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	SFepTime time;
	b_ui8 SPE,QDP;b_ui16 CP16;
	for(int i=0;i< num;i++)
	{	
		seTm=vStartEventsTimeTag[i];
		memcpy(&SPE, &seTm.SPE,1);
		memcpy(&QDP,&seTm.QDP,1);
		memcpy(&CP16,&seTm.cp16TimeTag,2);
		displayData("point_0X%06x,SPE{SRD_%x,SIE_%x,SL3_%x,SL2_%x,SL1_%x,GS_%x}\n",seTm.iOffset,seTm.SPE.SRD,\
			seTm.SPE.SIE,seTm.SPE.SL3,seTm.SPE.SL2,seTm.SPE.SL1,seTm.SPE.GeneralStart);
		displayData("	QDP{IV_%x,NT_%x,SB_%x,BL_%x,EI_%x},CP16_%04X\n",seTm.QDP.IV,seTm.QDP.NT,seTm.QDP.SB,seTm.QDP.BL,seTm.QDP.EI,CP16);
		time = seTm.cp56TimeTag;
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}
}

void MyEvent_Handler::handleCircuitInformationWithTimeTag(int rtuAddr,b_ui8 bCot,vector<SCircuitInformationTimeTag>&vCircuitInformationTimeTag)
{
	int num = vCircuitInformationTimeTag.empty() ? -1 : static_cast<int>(vCircuitInformationTimeTag.size());
	SCircuitInformationTimeTag ciTm;
	displayData("rtu_%02x, cot_%02x, num_ %d\n",rtuAddr,bCot,num);
	SFepTime time;
	b_ui16 CP16;
	for(int i=0;i< num;i++)
	{	
		ciTm=vCircuitInformationTimeTag[i];
		memcpy(&CP16,&ciTm.cp16TimeTag,2);
		displayData("point_0X%06x,OCI{CL3_%x,CL2_%x,CL1_%x,GC_%x}\n",ciTm.iOffset, ciTm.OCI.CL3,ciTm.OCI.CL2,\
			ciTm.OCI.CL1,ciTm.OCI.GC);
		displayData("	QDP{IV_%x,NT_%x,SB_%x,BL_%x,EI_%x},CP16_%04X\n",ciTm.QDP.IV,ciTm.QDP.NT,ciTm.QDP.SB,ciTm.QDP.BL,ciTm.QDP.EI,CP16);
		time = ciTm.cp56TimeTag;
		displayData("y_%d,m_%d,d_%d,h_%d,m_%d,s_%d,ms_%d\n",time.year,time.month,time.day,time.hour,time.minute,time.second,time.millisecond);
	}
}
void MyEvent_Handler::handleParameterNVA(int rtuAddr,b_ui8 bCot,SParameterNVA paraNva)
{
	displayData("rtu_%02x, cot_%02x \n",rtuAddr,bCot);
	b_ui16 NVA;b_ui8 QPM;
	memcpy(&NVA,&paraNva.NVA,2);
	memcpy(&QPM,&paraNva.QPM,1);
	displayData("point_0X%06x,NVA_%04x{%d},QPM{POP_%x,LPC_%x,KPA_%x}\n",paraNva.iOffset, NVA,NVA,\
		paraNva.QPM.POP,paraNva.QPM.LPC,paraNva.QPM.KPA);
}
void MyEvent_Handler:: handleParameterSVA(int rtuAddr,b_ui8 bCot,SParameterSVA paraSva)
{
	displayData("rtu_%02x, cot_%02x \n",rtuAddr,bCot);
	b_ui16 SVA;b_ui8 QPM;
	memcpy(&SVA,&paraSva.SVA,2);
	memcpy(&QPM,&paraSva.QPM,1);
	displayData("point_0X%06x, SVA_%04x{%d},QPM{POP_%x,LPC_%x,KPA_%x}\n",paraSva.iOffset, SVA,paraSva.SVA,\
		paraSva.QPM.POP,paraSva.QPM.LPC,paraSva.QPM.KPA);
}
void MyEvent_Handler::handleParameterSFA(int rtuAddr,b_ui8 bCot,SParameterSFA paraSfa)
{
	displayData("rtu_%02x, cot_%02x \n",rtuAddr,bCot);
	float  SFA;b_ui8 QPM;
	memcpy(&SFA,&paraSfa.SFA,4);
	memcpy(&QPM,&paraSfa.QPM,1);
	displayData("point_0X%06x,SFA_%08x{%f},QPM{POP_%x,LPC_%x,KPA_%x}\n",paraSfa.iOffset,paraSfa.SFA,SFA,\
		paraSfa.QPM.POP,paraSfa.QPM.LPC,paraSfa.QPM.KPA);
}
void MyEvent_Handler::handleParameterActivation(int rtuAddr,b_ui8 bCot,SParameterAct  paraAct)
{
	displayData("rtu_%02x, cot_%02x \n",rtuAddr,bCot);
	displayData("point_0X%06x, { QPA_%02x}\n",paraAct.iOffset, paraAct.QPA);
}

void MyEvent_Handler::handleSingleCommand(int rtuAddr,int iInfoAddr,b_ui8 btCot,b_ui8 bSco)
{
	displayData("rtu_%02x,info_%06x cot_%02x Sco_%02x\n",rtuAddr,iInfoAddr,btCot,bSco);
	displayData("单点遥控\n");
	switch(btCot)
	{
		case IEC104_COT_ACT_CON:
			if((bSco&0x80)>0)
			{
				displayData("选择确认\n");
				IEC104_API::getInstance()->singleCommandExec(rtuAddr,iInfoAddr,bSco&0x01,(bSco&0x7c)>>2);
			}
			else
				displayData("执行确认\n");
			break;
		case IEC104_COT_DEACT_CON:
			displayData("撤销确认\n");
			break;
		case IEC104_COT_ACT_TERM:
			displayData("单点遥控结束\n");
			break;
		default:
			displayData("未知的传送原因\n");
	}
	
}	
/*void MyEvent_Handler::handleAllLinkDown()
{
	std::string logStr="all connect down\n";
	IEC104_API::getInstance()->logRecord(LOG_LEVEL_ERROR,  logStr);
}*/
void MyEvent_Handler::handleLinkConnectStatusChanged(b_ui8 groupID,string &linkIP,int connectStatus)
{
	std::string logStr;
	char tmp[100];
	sprintf(tmp,"%d",groupID);
	logStr+=" link IP="+linkIP+",Rtu="+tmp;
	if(connectStatus==LINK_STATUS_DISCONNECT)//
		logStr+=", Disconnect!\n";
	else
		logStr+=", Connect!\n";
	IEC104_API::getInstance()->logRecord(LOG_LEVEL_WARNING,  logStr);
}
void MyEvent_Handler::handleDataTransferLinkChanged(b_ui8 newGroupID,string &newLinkIP,b_ui8 oldGroupID,string&oldLinkIP,int changeReason)
{
	std::string logStr; 
	char tmp[100];
	sprintf(tmp,"%d",newGroupID);
	logStr="DataTransferLinkChange,";
	logStr+=" new linkIP="+newLinkIP+", newGroupID="+tmp;
	sprintf(tmp,"%d",oldGroupID);
	logStr+="<----old linkIP="+oldLinkIP+", oldGroupID="+tmp;
	logStr+=", cause of change: ";
	switch(changeReason)
	{	
	case CAUSE_LINK_DISCONNECT:
		logStr+="link disconnect";
		break;
	case CAUSE_STATUS_SLAVE2MASTER:
		logStr+="status change slave2master";
		break;
	case CAUSE_STATUS_MASTER2SLAVE:
		logStr+="status change master2slave";
		break;
	case CAUSE_LINK_CONNECT:
		logStr+="link connect";
		break;
	case CAUSE_WORKMODE_MONITOR2CONTROLER:
		logStr+="workmode chage monitor2controler";
	default:
		logStr+="unknown";
		break;
	}
	logStr+="\n";	
	IEC104_API::getInstance()->logRecord(LOG_LEVEL_WARNING,  logStr);
}
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~new end~~~~~~~~~~~~~~~~~~
