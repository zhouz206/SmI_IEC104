/*
 * datatoCSWI.c
 *
 *  Created on: 20.04.2017
 *      Author: sagok
 */



#include "main.h"
#include "IEC850.h"

#include "iec850.h"
#include "iec61850_server.h"

#include "modbus.h"

extern	xQueueHandle 	ModbusSentQueue;		// ������� ��� �������� � ������

/*******************************************************
 * MR5_600
 *******************************************************/
#if defined (MR5_600)
#include "static_model_MR5_600.h"

/*******************************************************
 * ��������
 *******************************************************/
int		Set_CSWI	(uint16_t QTnum, uint64_t currentTime ){return false;}
int		Set_XCBR	(uint16_t QTnum, uint64_t currentTime ){return false;}

#endif
/*******************************************************
 * MR851
 *******************************************************/
#if defined (MR851)
#include "static_model_MR851.h"

/*******************************************************
 * ��������
 *******************************************************/
int		Set_CSWI	(uint16_t QTnum, uint64_t currentTime ){return false;}
int		Set_XCBR	(uint16_t QTnum, uint64_t currentTime ){return false;}

#endif
/*******************************************************
 * MR901 MR902
 *******************************************************/
#if defined (MR901) || defined (MR902)
#include "static_model_MR901_902.h"
/*******************************************************
 * ��������
 *******************************************************/
int		Set_CSWI	(uint16_t QTnum, uint64_t currentTime ){return false;}
int		Set_XCBR	(uint16_t QTnum, uint64_t currentTime ){return false;}

#endif
/*******************************************************
 * T12N5D58R51
 *******************************************************/
#if defined	(MR801) && defined (T12N5D58R51)
#include "static_model_MR801_T12N5D58R51.h"

extern uint16_t   ucMDiscInBuf[MB_Size_Discreet];
extern uint16_t   ucConfigAPWBuf[MB_Size_ConfigAPW];
extern uint16_t   ucConfigBufSW[MB_Size_ConfigSW];
extern uint16_t   ucSWCrash[MB_Size_SWCrash];


/*******************************************************
 * Set_CSWI ��������� ������������ �������
 *******************************************************/
int		Set_CSWI	(uint16_t QTnum, uint64_t currentTime ){

	uint32_t	Mod;
	uint32_t	ctlModel;
	uint32_t	Health = STVALINT32_OK;
	int			Qual;
	uint32_t	stVal;
// ----------------------------------------------------------------------
// Mod Beh
			Mod = STVALINT32_OFF;
			ctlModel = CTLMODELEDENUM_statusOnly;

			if ((ucConfigBufSW[MB_offset_ConfigSW+MB_rOffsetControlSW] & MB_bControlSW_ON_OFF)>0) {				// ���� ����� �����������
				Mod = STVALINT32_ON;

				if ((ucConfigBufSW[MB_offset_ConfigSW+MB_rOffsetControlSW] & MB_bControlSW_SDTU)>0) {			// ���� ��������� ���������� �� ����
					ctlModel = CTLMODELEDENUM_DirectWithNormalSecurity;
				} else {
					ctlModel = CTLMODELEDENUM_statusOnly;
				}
			}

			if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Mod_stVal, Mod)){
				IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Mod_t, currentTime);
			if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Beh_stVal, Mod))
				IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Beh_t, currentTime);
			}

// ----------------------------------------------------------------------
// Health
			if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) {Health = STVALINT32_Warning;}

			if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Health_stVal, Health))
			IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Health_t, currentTime);

// ----------------------------------------------------------------------
// Quality (Mod Beh)
			Qual = QUALITY_VALIDITY_GOOD;
			if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

			if (IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Mod_q,Qual))
				IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Beh_q,Qual);

// ----------------------------------------------------------------------
// Pos
			IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Mod_ctlModel, ctlModel);
			IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_ctlModel, ctlModel);

			stVal = STVALCODEDENUM_INTERMEDIATE;
			if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_ON) {stVal |= STVALCODEDENUM_OFF;}
			if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_OFF) {stVal |= STVALCODEDENUM_ON;}

			if (IedServer_updateBitStringAttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_stVal, stVal))
				IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_t, currentTime);

//  Quality (Pos)
			{
				Qual = QUALITY_VALIDITY_GOOD;
				int	Module	=	0;
				// �������� ���� ������������ �����������, � ������� ����������� ���� ������

				if (ucConfigBufSW[MB_rOffsetControlSW_ON]   >= _begin_Discreet_M2*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ON]    <= _end_Discreet_M2*2-1)	Module |= MB_bOffset_errorM2;
				if (ucConfigBufSW[MB_rOffsetControlSW_OFF]  >= _begin_Discreet_M2*2-1 && ucConfigBufSW[MB_rOffsetControlSW_OFF]   <= _end_Discreet_M2*2-1)	Module |= MB_bOffset_errorM2;
				if (ucConfigBufSW[MB_rOffsetControlSW_ERROR]>= _begin_Discreet_M2*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] <= _end_Discreet_M2*2-1)	Module |= MB_bOffset_errorM2;

				if (ucConfigBufSW[MB_rOffsetControlSW_ON]   >= _begin_Discreet_M3*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ON]    <= _end_Discreet_M3*2-1)	Module |= MB_bOffset_errorM3;
				if (ucConfigBufSW[MB_rOffsetControlSW_OFF]  >= _begin_Discreet_M3*2-1 && ucConfigBufSW[MB_rOffsetControlSW_OFF]   <= _end_Discreet_M3*2-1)	Module |= MB_bOffset_errorM3;
				if (ucConfigBufSW[MB_rOffsetControlSW_ERROR]>= _begin_Discreet_M3*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] <= _end_Discreet_M3*2-1)	Module |= MB_bOffset_errorM3;

				if (ucConfigBufSW[MB_rOffsetControlSW_ON]   >= _begin_Discreet_M4*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ON]    <= _end_Discreet_M4*2-1)	Module |= MB_bOffset_errorM4;
				if (ucConfigBufSW[MB_rOffsetControlSW_OFF]  >= _begin_Discreet_M4*2-1 && ucConfigBufSW[MB_rOffsetControlSW_OFF]   <= _end_Discreet_M4*2-1)	Module |= MB_bOffset_errorM4;
				if (ucConfigBufSW[MB_rOffsetControlSW_ERROR]>= _begin_Discreet_M4*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] <= _end_Discreet_M4*2-1)	Module |= MB_bOffset_errorM4;

				if ((Module & MB_bOffset_errorM2) && (ucMDiscInBuf[MB_offset_errorM2] & MB_bOffset_errorM2)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
				if ((Module & MB_bOffset_errorM3) && (ucMDiscInBuf[MB_offset_errorM3] & MB_bOffset_errorM3)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
				if ((Module & MB_bOffset_errorM4) && (ucMDiscInBuf[MB_offset_errorM4] & MB_bOffset_errorM4)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

				IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Pos_q,Qual);

			}
return true;
}
/*******************************************************
 * Set_XCBR ��������� ������������ �������
 *******************************************************/
int		Set_XCBR	(uint16_t QTnum, uint64_t currentTime ){
	int	ret = false;
	uint32_t	Mod;
	uint32_t	stVal,ctlVal;
	uint32_t	Health = STVALINT32_OK;
	Quality		Qual;
	bool 		bolVal;

// ----------------------------------------------------------------------
// Mod Beh
			Mod = STVALINT32_OFF;
			if ((ucConfigBufSW[MB_offset_ConfigSW+MB_rOffsetControlSW] & MB_bControlSW_ON_OFF)>0) {				// ���� ����� �����������
				Mod = STVALINT32_ON;
			}

			if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_XCBR1_Mod_stVal, Mod)){
				IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_Mod_t, currentTime);
			if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_XCBR1_Beh_stVal, Mod))
				IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_Beh_t, currentTime);
			}


	if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) {Health = STVALINT32_Warning;}

	if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_XCBR1_Health_stVal, Health))
	IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_Health_t, currentTime);

	//pos
	stVal = STVALCODEDENUM_INTERMEDIATE;
	if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_ON) {stVal |= STVALCODEDENUM_OFF;}
	if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_OFF) {stVal |= STVALCODEDENUM_ON;}

	if (IedServer_updateBitStringAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_stVal, stVal)){
	IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_t, currentTime);
	// �������� ������ �����������
	AddToQueueMB(ModbusSentQueue, 	MB_Rd_ConfigSWCrash	,MB_Slaveaddr);
	ret = true;

	}

//  Quality (Pos)
	{
		Qual = QUALITY_VALIDITY_GOOD;
		int	Module	=	0;
		// �������� ���� ������������ �����������, � ������� ����������� ���� ������

		if (ucConfigBufSW[MB_rOffsetControlSW_ON]   >= _begin_Discreet_M2*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ON]    <= _end_Discreet_M2*2-1)	Module |= MB_bOffset_errorM2;
		if (ucConfigBufSW[MB_rOffsetControlSW_OFF]  >= _begin_Discreet_M2*2-1 && ucConfigBufSW[MB_rOffsetControlSW_OFF]   <= _end_Discreet_M2*2-1)	Module |= MB_bOffset_errorM2;
		if (ucConfigBufSW[MB_rOffsetControlSW_ERROR]>= _begin_Discreet_M2*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] <= _end_Discreet_M2*2-1)	Module |= MB_bOffset_errorM2;

		if (ucConfigBufSW[MB_rOffsetControlSW_ON]   >= _begin_Discreet_M3*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ON]    <= _end_Discreet_M3*2-1)	Module |= MB_bOffset_errorM3;
		if (ucConfigBufSW[MB_rOffsetControlSW_OFF]  >= _begin_Discreet_M3*2-1 && ucConfigBufSW[MB_rOffsetControlSW_OFF]   <= _end_Discreet_M3*2-1)	Module |= MB_bOffset_errorM3;
		if (ucConfigBufSW[MB_rOffsetControlSW_ERROR]>= _begin_Discreet_M3*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] <= _end_Discreet_M3*2-1)	Module |= MB_bOffset_errorM3;

		if (ucConfigBufSW[MB_rOffsetControlSW_ON]   >= _begin_Discreet_M4*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ON]    <= _end_Discreet_M4*2-1)	Module |= MB_bOffset_errorM4;
		if (ucConfigBufSW[MB_rOffsetControlSW_OFF]  >= _begin_Discreet_M4*2-1 && ucConfigBufSW[MB_rOffsetControlSW_OFF]   <= _end_Discreet_M4*2-1)	Module |= MB_bOffset_errorM4;
		if (ucConfigBufSW[MB_rOffsetControlSW_ERROR]>= _begin_Discreet_M4*2-1 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] <= _end_Discreet_M4*2-1)	Module |= MB_bOffset_errorM4;

		if ((Module & MB_bOffset_errorM2) && (ucMDiscInBuf[MB_offset_errorM2] & MB_bOffset_errorM2)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
		if ((Module & MB_bOffset_errorM3) && (ucMDiscInBuf[MB_offset_errorM3] & MB_bOffset_errorM3)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
		if ((Module & MB_bOffset_errorM4) && (ucMDiscInBuf[MB_offset_errorM4] & MB_bOffset_errorM4)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

		IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_Pos_q,Qual);
	}

// BlkOpn
	bolVal = true;
	if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorCtrl1) {bolVal = false;}

	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkOpn_stVal, bolVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkOpn_t, currentTime);
		ret = true;
	}

// BlkCIs
	bolVal = true;
	if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorCtrl2) {bolVal = false;}

	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_stVal, bolVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_t, currentTime);
		ret = true;
	}

// OpCnt
	if (IedServer_updateUnsignedAttributeValue(iedServer, &iedModel_CTRL_XCBR1_OpCnt_stVal, ucSWCrash[MB_offset_Oper_cnt])){
		IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_OpCnt_t, currentTime);
		ret = true;
	}

	Qual = QUALITY_VALIDITY_GOOD;
	if (ucMDiscInBuf[MB_offset_errorM1] & MB_bOffset_errorM1){
		Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;
	}
	IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_BlkCls_q, Qual);
	IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_BlkOpn_q, Qual);

return ret;
}

#endif

/*******************************************************
 * MR801 OLD
 *******************************************************/
#if defined	(MR801) && defined (OLD)
#include "static_model_MR801.h"

extern uint16_t   ucMDiscInBuf[MB_Size_Discreet];
extern uint16_t   ucConfigAPWBuf[MB_Size_ConfigAPW];
extern uint16_t   ucConfigBufSW[MB_Size_ConfSW];
extern uint16_t   ucSWCrash[MB_Size_SWCrash];


/*******************************************************
 * Set_CSWI ��������� ������������ �������
 *******************************************************/
int		Set_CSWI	(uint16_t QTnum, uint64_t currentTime )
{
	uint32_t	Mod;
	uint32_t	ctlModel,stVal,ctlVal;
	uint32_t	Health = STVALINT32_OK;

	// Health

				if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) {Health = STVALINT32_Warning;}

				if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Health_stVal, Health))
				IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Health_t, currentTime);

	//  Mod Beh
				if (ucConfigBufSW[MB_offset_ConfigSW] & MB_bControlSW_SDTU) {				// ���� ��������� ���������� �� ����
					Mod = STVALINT32_ON;
					ctlModel = CTLMODELEDENUM_DirectWithNormalSecurity;
				} else {
					Mod = STVALINT32_OFF;
					ctlModel = CTLMODELEDENUM_statusOnly;
				}

				if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Mod_stVal, Mod)){
					IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Mod_t, currentTime);
				if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Beh_stVal, Mod))
					IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Beh_t, currentTime);
				}


	//  Quality (Mod Beh)
				{
				int	Qual = QUALITY_VALIDITY_GOOD;
				if (ucMDiscInBuf[MB_offsetHardFaults] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

				IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Mod_q,Qual);
				IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Beh_q,Qual);

				}

	// Pos
				IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Mod_ctlModel, ctlModel);
				IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_ctlModel, ctlModel);



				stVal = STVALCODEDENUM_INTERMEDIATE;
				if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_ON) {stVal |= STVALCODEDENUM_OFF;}
				if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_OFF) {stVal |= STVALCODEDENUM_ON;}
				//if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffsetErrorBKExt1) {stVal |= STVALCODEDENUM_BADSTATE;}

				if (IedServer_updateBitStringAttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_stVal, stVal))
					IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_t, currentTime);

	//  Quality (Pos)
				{
					int	Qual = QUALITY_VALIDITY_GOOD;
					int	Module	=	0;
					// �������� ���� ������������ �����������, � ������� ����������� ���� ������

					if (ucConfigBufSW[MB_rOffsetControlSW_ON] > 0 && ucConfigBufSW[MB_rOffsetControlSW_ON] < 17)			Module |= MB_bOffset_errorM2;
					if (ucConfigBufSW[MB_rOffsetControlSW_OFF] > 0 && ucConfigBufSW[MB_rOffsetControlSW_OFF] < 17)			Module |= MB_bOffset_errorM2;
					if (ucConfigBufSW[MB_rOffsetControlSW_ERROR] > 0 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] < 17)		Module |= MB_bOffset_errorM2;

					if (ucConfigBufSW[MB_rOffsetControlSW_ON] > 16 && ucConfigBufSW[MB_rOffsetControlSW_ON] < 49)			Module |= MB_bOffset_errorM3;
					if (ucConfigBufSW[MB_rOffsetControlSW_OFF] > 16 && ucConfigBufSW[MB_rOffsetControlSW_OFF] < 49)			Module |= MB_bOffset_errorM3;
					if (ucConfigBufSW[MB_rOffsetControlSW_ERROR] > 16 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] < 49)		Module |= MB_bOffset_errorM3;


					if ((Module & MB_bOffset_errorM2) && (ucMDiscInBuf[MB_offsetHardFaults] & MB_bOffset_errorM2)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
					if ((Module & MB_bOffset_errorM3) && (ucMDiscInBuf[MB_offsetHardFaults] & MB_bOffset_errorM3)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}


					IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Pos_q,Qual);

				}


}

/*******************************************************
 * Set_XCBR ��������� ������������ �������
 *******************************************************/
int		Set_XCBR	(uint16_t QTnum, uint64_t currentTime )
{
	int	ret = false;
	uint32_t	Mod;
	uint32_t	ctlModel,stVal,ctlVal;
	uint32_t	Health = STVALINT32_OK;
	Quality		Qual;

	bool bolVal;

	if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) {Health = STVALINT32_Warning;}

	if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_XCBR1_Health_stVal, Health))
	IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_Health_t, currentTime);

	//pos
	stVal = STVALCODEDENUM_INTERMEDIATE;
	if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_ON) {stVal |= STVALCODEDENUM_OFF;}
	if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_OFF) {stVal |= STVALCODEDENUM_ON;}
	//if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffsetErrorBKExt1) {stVal |= STVALCODEDENUM_BADSTATE;}

	if (IedServer_updateBitStringAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_stVal, stVal)){
	IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_t, currentTime);
	// �������� ������ �����������
	AddToQueueMB(ModbusSentQueue, 	MB_Rd_ConfigSWCrash	,MB_Slaveaddr);
	ret = true;

	}

	//  Quality (Pos)
				{
					Qual = QUALITY_VALIDITY_GOOD;
					int	Module	=	0;
					// �������� ���� ������������ �����������, � ������� ����������� ���� ������

					if (ucConfigBufSW[MB_rOffsetControlSW_ON] > 0 && ucConfigBufSW[MB_rOffsetControlSW_ON] < 17)		Module |= MB_bOffset_errorM2;
					if (ucConfigBufSW[MB_rOffsetControlSW_OFF] > 0 && ucConfigBufSW[MB_rOffsetControlSW_OFF] < 17)		Module |= MB_bOffset_errorM2;
					if (ucConfigBufSW[MB_rOffsetControlSW_ERROR] > 0 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] < 17)	Module |= MB_bOffset_errorM2;

					if (ucConfigBufSW[MB_rOffsetControlSW_ON] > 16 && ucConfigBufSW[MB_rOffsetControlSW_ON] < 49)		Module |= MB_bOffset_errorM3;
					if (ucConfigBufSW[MB_rOffsetControlSW_OFF] > 16 && ucConfigBufSW[MB_rOffsetControlSW_OFF] < 49)		Module |= MB_bOffset_errorM3;
					if (ucConfigBufSW[MB_rOffsetControlSW_ERROR] > 16 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] < 49)	Module |= MB_bOffset_errorM3;


					if ((Module & MB_bOffset_errorM2) && (ucMDiscInBuf[MB_offsetHardFaults] & MB_bOffset_errorM2)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
					if ((Module & MB_bOffset_errorM3) && (ucMDiscInBuf[MB_offsetHardFaults] & MB_bOffset_errorM3)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

					IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_Pos_q,Qual);
				}

	//BlkOpn
	bolVal = true;
	if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffsetErrorCtrl1) {bolVal = false;}

	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkOpn_stVal, bolVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkOpn_t, currentTime);
		ret = true;
	}

	//BlkCIs
	bolVal = true;
	if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorCtrl2) {bolVal = false;}

	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_stVal, bolVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_t, currentTime);
		ret = true;
	}

	//OpCnt
	if (IedServer_updateUnsignedAttributeValue(iedServer, &iedModel_CTRL_XCBR1_OpCnt_stVal, ucSWCrash[MB_offset_Oper_cnt])){
		IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_OpCnt_t, currentTime);
		ret = true;
	}

	Qual = QUALITY_VALIDITY_GOOD;
	if (ucMDiscInBuf[MB_offset_errorM1] & MB_bOffset_errorM1){
		Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;
	}
	IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_BlkCls_q, Qual);
	IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_BlkOpn_q, Qual);

return ret;
}
#endif
/*******************************************************
 * MR771 MR761 MR762 MR763
 *******************************************************/
#if defined	(MR771) || defined	(MR761) || defined	(MR762) || defined	(MR763)

#if defined	(MR771)
#include "static_model_MR771.h"
#endif

#if defined	(MR761) || defined	(MR762) || defined	(MR763)
#include "static_model_MR76x.h"
#endif
#if  defined (MR761OBR)
#include "static_model_MR761OBR.h"
#endif

extern uint16_t   	ucMDiscInBuf[MB_Size_Discreet];
extern uint16_t   	ucSWCrash[MB_Size_SWCrash];
extern uint16_t   	ucOtherUstavkiInBuf[MB_Size_OtherUstavki];

//extern uint16_t   ucConfigAPWBuf[MB_Size_ConfigAPW];
//extern uint16_t   ucConfigBufSW[MB_Size_ConfSW];

/*******************************************************
 * Set_CSWI ��������� ������������ �������
 *******************************************************/
int		Set_CSWI	(uint16_t QTnum, uint64_t currentTime )
{
	uint32_t	Mod = STVALINT32_OFF;
	uint32_t	ctlModel,stVal,ctlVal;
	uint32_t	Health = STVALINT32_OK;
	int	Qual;
	// Health

				if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) {Health = STVALINT32_Warning;}
				if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Health = STVALINT32_Warning;}

				if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Health_stVal, Health))
				IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Health_t, currentTime);

	//  Mod Beh
				if ((ucOtherUstavkiInBuf[MB_offset_ConfigSW + MB_rOffsetControlSW] & MB_bControlSW_SDTU)>0) {				// ���� ��������� ���������� �� ����
					Mod = STVALINT32_ON;
					ctlModel = CTLMODELEDENUM_DirectWithNormalSecurity;
				} else {
					Mod = STVALINT32_OFF;
					ctlModel = CTLMODELEDENUM_statusOnly;
				}

				if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Mod_stVal, Mod)){
					IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Mod_t, currentTime);
					if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Beh_stVal, Mod))
						IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Beh_t, currentTime);
				}


	//  Quality (Mod Beh)
				{
				Qual = QUALITY_VALIDITY_GOOD;
				if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

					IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Mod_q,Qual);
					IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Beh_q,Qual);
				}

	// Pos
				IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Mod_ctlModel, ctlModel);
				IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_ctlModel, ctlModel);



				stVal = STVALCODEDENUM_INTERMEDIATE;
				if (ucMDiscInBuf[MB_offset_SW_ON_status] & MB_b_SW_ON) {stVal |= STVALCODEDENUM_OFF;}
				if (ucMDiscInBuf[MB_offset_SW_OFF_status] & MB_b_SW_OFF) {stVal |= STVALCODEDENUM_ON;}
				//if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffsetErrorBKExt1) {stVal |= STVALCODEDENUM_BADSTATE;}

				if (IedServer_updateBitStringAttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_stVal, stVal))
					IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_t, currentTime);

	//  Quality (Pos)
				{
					Qual = QUALITY_VALIDITY_GOOD;
					int	Module	=	0;
					// �������� ���� ������������ �����������, � ������� ����������� ���� ������

					// � ���������� 3 �� 2 ������ �� �������, ������ ���������. ������� 2 ������D1-D8 1..16 3 ������D9-D24 19..50 4 ������D25-D40 19..50
					if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 17)		Module |= MB_bOffset_errorM2;
					if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 17)		Module |= MB_bOffset_errorM2;
					if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 17)	Module |= MB_bOffset_errorM2;

					if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 49)		Module |= MB_bOffset_errorM3;
					if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 49)		Module |= MB_bOffset_errorM3;
					if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 49)	Module |= MB_bOffset_errorM3;

					if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 81)		Module |= MB_bOffset_errorM4;
					if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 81)		Module |= MB_bOffset_errorM4;
					if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 81)	Module |= MB_bOffset_errorM4;


					if ((Module & MB_bOffset_errorM2) && (ucMDiscInBuf[MB_offset_errorM2] & MB_bOffset_errorM2)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
					if ((Module & MB_bOffset_errorM3) && (ucMDiscInBuf[MB_offset_errorM3] & MB_bOffset_errorM3)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
					if ((Module & MB_bOffset_errorM4) && (ucMDiscInBuf[MB_offset_errorM4] & MB_bOffset_errorM4)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
					if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}


					IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Pos_q,Qual);

				}

}

/*******************************************************
 * Set_XCBR ��������� ������������ �������
 *******************************************************/
int		Set_XCBR	(uint16_t QTnum, uint64_t currentTime )
{
	int	ret = false;
	uint32_t	Mod;
	uint32_t	ctlModel,stVal,ctlVal;
	uint32_t	Health = STVALINT32_OK;
	Quality		Qual = QUALITY_VALIDITY_GOOD;

	bool 		bolVal;


	if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) {Health = STVALINT32_Warning;}
	if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Health = STVALINT32_Warning;}

	if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_XCBR1_Health_stVal, Health))
	IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_Health_t, currentTime);

//pos
	stVal = STVALCODEDENUM_INTERMEDIATE;
	if (ucMDiscInBuf[MB_offset_SW_ON_status] & MB_b_SW_ON) {stVal |= STVALCODEDENUM_OFF;}
	if (ucMDiscInBuf[MB_offset_SW_OFF_status] & MB_b_SW_OFF) {stVal |= STVALCODEDENUM_ON;}
	//if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffsetErrorBKExt1) {stVal |= STVALCODEDENUM_BADSTATE;}

	if (IedServer_updateBitStringAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_stVal, stVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_t, currentTime);
		// �������� ������ �����������
		AddToQueueMB(ModbusSentQueue, 	MB_Rd_ConfigSWCrash	,MB_Slaveaddr);
		ret = true;
	}

	//  Quality (Pos)
	{
		Qual = QUALITY_VALIDITY_GOOD;
		int	Module	=	0;
		// �������� ���� ������������ �����������, � ������� ����������� ���� ������
		// � ���������� 3 �� 2 ������ �� �������, ������ ���������. ������� 2 ������D1-D8 1..16 3 ������D9-D24 19..50 4 ������D25-D40 19..50
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 17)		Module |= MB_bOffset_errorM2;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 17)		Module |= MB_bOffset_errorM2;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 17)	Module |= MB_bOffset_errorM2;

		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 49)		Module |= MB_bOffset_errorM3;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 49)		Module |= MB_bOffset_errorM3;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 49)	Module |= MB_bOffset_errorM3;

		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 81)		Module |= MB_bOffset_errorM4;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 81)		Module |= MB_bOffset_errorM4;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 81)	Module |= MB_bOffset_errorM4;


		if ((Module & MB_bOffset_errorM2) && (ucMDiscInBuf[MB_offset_errorM2] & MB_bOffset_errorM2)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
		if ((Module & MB_bOffset_errorM3) && (ucMDiscInBuf[MB_offset_errorM3] & MB_bOffset_errorM3)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
		if ((Module & MB_bOffset_errorM4) && (ucMDiscInBuf[MB_offset_errorM4] & MB_bOffset_errorM4)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
		if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

		IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_Pos_q,Qual);
	}

//Loc --------------------------------------------------------------------------------------------------
	uint32_t 	CtrlD,CtrlD1;
	uint8_t		Nword;
	uint8_t		PosONword;
	uint16_t 	diskreet;

	bolVal = false;
	CtrlD = ucOtherUstavkiInBuf[MB_rOffsetControlSW_BlockSDTU];			// ����� ��������������� ��������

	if (CtrlD) {

		CtrlD1 = (CtrlD - 1)%2;
		CtrlD = (CtrlD - 1)/2;		// ����� ��������, LS....SSL

		Nword 		= CtrlD/16;
		PosONword	= CtrlD%16;

		diskreet = (uint64_t)ucMDiscInBuf[MB_offsetDiscreet_1+Nword];

		if (CtrlD1 > 0) {							// ���� �� ����� ��������� �� ��������� ���������
			if ((diskreet & 1<<PosONword) == 0) bolVal = true;
		} else{
			if ((diskreet & 1<<PosONword)>0) bolVal = true;
		}
	}
	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Loc_stVal, bolVal))
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Loc_t, currentTime);
//!Loc -------------------------------------------------------------------------------------------------
//BlkOpn
	bolVal = false;
	if (
			(ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorCtrl2) ||\
			(ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorCtrl3)
		)
		{bolVal = true;}

	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkOpn_stVal, bolVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkOpn_t, currentTime);
		ret = true;
	}

//BlkCIs
	bolVal = false;
	if (
#if !defined (MR761OBR)
			(ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) ||\
			(ucMDiscInBuf[MB_offset_IO_BlkQ] & MB_b_IO_BlkQ) ||\
			(ucMDiscInBuf[MB_offset_BlkN] & MB_b_BlkN)
#endif
#if defined (MR761OBR)
			(ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll)
#endif
	)
	{bolVal = true;}

	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_stVal, bolVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_t, currentTime);
		ret = true;
	}

//OpCnt
	if (IedServer_updateUnsignedAttributeValue(iedServer, &iedModel_CTRL_XCBR1_OpCnt_stVal, ucSWCrash[MB_offset_Oper_cnt])){
		IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_OpCnt_t, currentTime);
		ret = true;
	}

	Qual = QUALITY_VALIDITY_GOOD;
	if (ucMDiscInBuf[MB_offset_errorM1] & MB_bOffset_errorM1)		Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;
	if ((ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki)) Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;

	IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_BlkCls_q, Qual);
	IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_BlkOpn_q, Qual);


return	ret;
}
#endif
/*******************************************************
 * MR761OBR
 *******************************************************/
#if  defined (MR761OBR)

#include "static_model_MR761OBR.h"

extern uint16_t   	ucMDiscInBuf[MB_Size_Discreet];
extern uint16_t   	ucSWCrash[MB_Size_SWCrash];
extern uint16_t   	ucOtherUstavkiInBuf[MB_Size_OtherUstavki];


/*******************************************************
 * Set_CSWI ��������� ������������ �������
 *******************************************************/
int		Set_CSWI	(uint16_t QTnum, uint64_t currentTime )
{
uint32_t	Mod = STVALINT32_OFF;
uint32_t	ctlModel,stVal,ctlVal;
uint32_t	Health = STVALINT32_OK;
int	Qual;
// Health

			if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) 	{Health = STVALINT32_Warning;}
			if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Health = STVALINT32_Warning;}

			if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Health_stVal, Health))
			IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Health_t, currentTime);

//  Mod Beh
			if ((ucOtherUstavkiInBuf[MB_offset_ConfigSW + MB_rOffsetControlSW] & MB_bControlSW_SDTU)>0) {				// ���� ��������� ���������� �� ����
				Mod = STVALINT32_ON;
				ctlModel = CTLMODELEDENUM_DirectWithNormalSecurity;
			} else {
				Mod = STVALINT32_OFF;
				ctlModel = CTLMODELEDENUM_statusOnly;
			}

			if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Mod_stVal, Mod)){
				IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Mod_t, currentTime);
				if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Beh_stVal, Mod))
					IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Beh_t, currentTime);
			}

//  Quality (Mod Beh)
			{
			Qual = QUALITY_VALIDITY_GOOD;
			if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

				IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Mod_q,Qual);
				IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Beh_q,Qual);
			}

// Pos
			IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Mod_ctlModel, ctlModel);
			IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_ctlModel, ctlModel);


			stVal = STVALCODEDENUM_INTERMEDIATE;
			if (ucMDiscInBuf[MB_offset_SW_ON_status] & MB_b_SW_ON) 		{stVal |= STVALCODEDENUM_OFF;}
			if (ucMDiscInBuf[MB_offset_SW_OFF_status] & MB_b_SW_OFF) 	{stVal |= STVALCODEDENUM_ON;}
			//if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffsetErrorBKExt1) {stVal |= STVALCODEDENUM_BADSTATE;}

			if (IedServer_updateBitStringAttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_stVal, stVal))
				IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_CSWI1_Pos_t, currentTime);

//  Quality (Pos)
			{
				Qual = QUALITY_VALIDITY_GOOD;
				int	Module	=	0;
				// �������� ���� ������������ �����������, � ������� ����������� ���� ������

				// � ���������� 3 �� 2 ������ �� �������, ������ ���������. ������� 2 ������D1-D8 1..16 3 ������D9-D24 19..50 4 ������D25-D40 19..50
				if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 17)		Module |= MB_bOffset_errorM2;
				if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 17)		Module |= MB_bOffset_errorM2;
				if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 17)	Module |= MB_bOffset_errorM2;

				if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 49)		Module |= MB_bOffset_errorM3;
				if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 49)		Module |= MB_bOffset_errorM3;
				if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 49)	Module |= MB_bOffset_errorM3;

				if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 81)		Module |= MB_bOffset_errorM4;
				if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 81)		Module |= MB_bOffset_errorM4;
				if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 81)	Module |= MB_bOffset_errorM4;


				if ((Module & MB_bOffset_errorM2) && (ucMDiscInBuf[MB_offset_errorM2] & MB_bOffset_errorM2)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
				if ((Module & MB_bOffset_errorM3) && (ucMDiscInBuf[MB_offset_errorM3] & MB_bOffset_errorM3)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
				if ((Module & MB_bOffset_errorM4) && (ucMDiscInBuf[MB_offset_errorM4] & MB_bOffset_errorM4)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
				if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

				IedServer_updateQuality(iedServer,&iedModel_CTRL_CSWI1_Pos_q,Qual);

			}
}

/*******************************************************
 * Set_XCBR ��������� ������������ �������
 *******************************************************/
int		Set_XCBR	(uint16_t QTnum, uint64_t currentTime )
{
	int	ret = false;
	uint32_t	Mod;
	uint32_t	ctlModel,stVal,ctlVal;
	uint32_t	Health = STVALINT32_OK;
	Quality		Qual = QUALITY_VALIDITY_GOOD;

	bool 		bolVal;


	if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) 	{Health = STVALINT32_Warning;}
	if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Health = STVALINT32_Warning;}

	if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_XCBR1_Health_stVal, Health))
	IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_Health_t, currentTime);

//pos
	stVal = STVALCODEDENUM_INTERMEDIATE;
	if (ucMDiscInBuf[MB_offset_SW_ON_status] & MB_b_SW_ON) 		{stVal |= STVALCODEDENUM_OFF;}
	if (ucMDiscInBuf[MB_offset_SW_OFF_status] & MB_b_SW_OFF) 	{stVal |= STVALCODEDENUM_ON;}
	//if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffsetErrorBKExt1) {stVal |= STVALCODEDENUM_BADSTATE;}

	if (IedServer_updateBitStringAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_stVal, stVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_t, currentTime);
		// �������� ������ �����������
		AddToQueueMB(ModbusSentQueue, 	MB_Rd_ConfigSWCrash	,MB_Slaveaddr);
		ret = true;
	}

	//  Quality (Pos)
	{
		Qual = QUALITY_VALIDITY_GOOD;
		int	Module	=	0;
		// �������� ���� ������������ �����������, � ������� ����������� ���� ������
		// � ���������� 3 �� 2 ������ �� �������, ������ ���������. ������� 2 ������D1-D8 1..16 3 ������D9-D24 19..50 4 ������D25-D40 19..50
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 17)		Module |= MB_bOffset_errorM2;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 17)		Module |= MB_bOffset_errorM2;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 0 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 17)	Module |= MB_bOffset_errorM2;

		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 49)		Module |= MB_bOffset_errorM3;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 49)		Module |= MB_bOffset_errorM3;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 16 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 49)	Module |= MB_bOffset_errorM3;

		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ON] < 81)		Module |= MB_bOffset_errorM4;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_OFF] < 81)		Module |= MB_bOffset_errorM4;
		if (ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] > 48 && ucOtherUstavkiInBuf[MB_rOffsetControlSW_ERROR] < 81)	Module |= MB_bOffset_errorM4;


		if ((Module & MB_bOffset_errorM2) && (ucMDiscInBuf[MB_offset_errorM2] & MB_bOffset_errorM2)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
		if ((Module & MB_bOffset_errorM3) && (ucMDiscInBuf[MB_offset_errorM3] & MB_bOffset_errorM3)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
		if ((Module & MB_bOffset_errorM4) && (ucMDiscInBuf[MB_offset_errorM4] & MB_bOffset_errorM4)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
		if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

		IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_Pos_q,Qual);
	}

//Loc --------------------------------------------------------------------------------------------------
	uint32_t 	CtrlD,CtrlD1;
	uint8_t		Nword;
	uint8_t		PosONword;
	uint16_t 	diskreet;

	bolVal = false;
	CtrlD = ucOtherUstavkiInBuf[MB_rOffsetControlSW_BlockSDTU];			// ����� ��������������� ��������

	if (CtrlD) {

		CtrlD1 = (CtrlD - 1)%2;
		CtrlD = (CtrlD - 1)/2;		// ����� ��������, LS....SSL

		Nword 		= CtrlD/16;
		PosONword	= CtrlD%16;

		diskreet = (uint64_t)ucMDiscInBuf[MB_offsetDiscreet_1+Nword];

		if (CtrlD1 > 0) {							// ���� �� ����� ��������� �� ��������� ���������
			if ((diskreet & 1<<PosONword) == 0) bolVal = true;
		} else{
			if ((diskreet & 1<<PosONword)>0) bolVal = true;
		}
	}
	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Loc_stVal, bolVal))
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Loc_t, currentTime);
//!Loc -------------------------------------------------------------------------------------------------
//BlkOpn
	bolVal = false;
	if (
			(ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorCtrl2) ||\
			(ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorCtrl3)
		)
		{bolVal = true;}

	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkOpn_stVal, bolVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkOpn_t, currentTime);
		ret = true;
	}

//BlkCIs
	bolVal = false;
	if (
#if !defined (MR761OBR)
			(ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) ||\
			(ucMDiscInBuf[MB_offset_IO_BlkQ] & MB_b_IO_BlkQ) ||\
			(ucMDiscInBuf[MB_offset_BlkN] & MB_b_BlkN)
#endif
#if defined (MR761OBR)
			(ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll)
#endif
	)
	{bolVal = true;}

	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_stVal, bolVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_t, currentTime);
		ret = true;
	}

//OpCnt
	if (IedServer_updateUnsignedAttributeValue(iedServer, &iedModel_CTRL_XCBR1_OpCnt_stVal, ucSWCrash[MB_offset_Oper_cnt])){
		IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_OpCnt_t, currentTime);
		ret = true;
	}

	Qual = QUALITY_VALIDITY_GOOD;
	if (ucMDiscInBuf[MB_offset_errorM1] & MB_bOffset_errorM1)		Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;
	if ((ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki)) Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;

	IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_BlkCls_q, Qual);
	IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_BlkOpn_q, Qual);


return	ret;
}
#endif
/*******************************************************
 * MR5_700
 *******************************************************/
#if defined (MR5_700) || defined (MR5_500) ||  defined (MR741)

#if defined (MR5_700)
#include "static_model_MR5_700.h"
#endif
#if defined (MR741)
#include "static_model_MR741.h"
#endif

#if defined (MR5_500)
#include "static_model_MR5_500.h"
#endif

extern uint16_t   ucMDiscInBuf[MB_Size_Discreet];
extern uint16_t   ucConfigBufSW[MB_Size_ConfigSW];
extern uint16_t   ucSWCrash[MB_Size_SWCrash];
//extern uint16_t   ucUstavkiInBuf[MB_Size_Ustavki];
extern uint16_t   ucMUstavkiInBuf[MB_Size_Ustavki];

/*******************************************************
 * Set_RREC ��������� ������������ �������
 *******************************************************/
int		Set_CSWI	(uint16_t QTnum, uint64_t currentTime )
{
	uint32_t	Mod;
	uint32_t	ctlModel,stVal;
	uint32_t	Health = STVALINT32_OK;
	int	Qual;

// Health

			if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) {Health = STVALINT32_Warning;}
			if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Health = STVALINT32_Warning;}


			if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Health_stVal, Health))
				IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Health_t, currentTime);

//  Mod Beh
			if (ucConfigBufSW[MB_offset_ConfigSW] & MB_bControlSW_SDTU) {				// ���� ��������� ���������� �� ����
				Mod = STVALINT32_ON;
				ctlModel = CTLMODELEDENUM_DirectWithNormalSecurity;
			} else {
				Mod = STVALINT32_OFF;
				ctlModel = CTLMODELEDENUM_statusOnly;
			}

			if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Mod_stVal, Mod)){
				IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Mod_t, currentTime);
				if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_CSWI1_Beh_stVal, Mod))
					IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_CSWI1_Beh_t, currentTime);
			}

//  Quality (Mod Beh)
			Qual = QUALITY_VALIDITY_GOOD;
			if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

			if (IedServer_updateQualityWithTime(iedServer, &iedModel_CTRL_CSWI1_Mod_q,&iedModel_CTRL_CSWI1_Mod_t,currentTime,Qual)){
				IedServer_updateQualityWithTime(iedServer, &iedModel_CTRL_CSWI1_Beh_q,&iedModel_CTRL_CSWI1_Beh_t,currentTime,Qual);
			}

// Pos
			IedServer_updateInt32AttributeValueWithTime(iedServer, &iedModel_CTRL_CSWI1_Mod_ctlModel,&iedModel_CTRL_CSWI1_Mod_t,currentTime,ctlModel);//stVal
			IedServer_updateInt32AttributeValueWithTime(iedServer, &iedModel_CTRL_CSWI1_Pos_ctlModel,&iedModel_CTRL_CSWI1_Pos_t,currentTime,ctlModel);//stVal


			stVal = STVALCODEDENUM_INTERMEDIATE;
			if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_ON) {stVal |= STVALCODEDENUM_ON;}
			if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_OFF) {stVal |= STVALCODEDENUM_OFF;}

			IedServer_updateBitStringAttributeValueWithTime(iedServer, &iedModel_CTRL_CSWI1_Pos_stVal,&iedModel_CTRL_CSWI1_Pos_t,currentTime,stVal);

//  Quality (Pos)
			{
				int	Qual = QUALITY_VALIDITY_GOOD;
				int	Module	=	0;
				// �������� ���� ������������ �����������, � ������� ����������� ���� ������

				if (ucConfigBufSW[MB_rOffsetControlSW_ON] > 0 && ucConfigBufSW[MB_rOffsetControlSW_ON] < 17)		Module |= MB_bOffsetErrModule3a;
				if (ucConfigBufSW[MB_rOffsetControlSW_OFF] > 0 && ucConfigBufSW[MB_rOffsetControlSW_OFF] < 17)		Module |= MB_bOffsetErrModule3a;
				if (ucConfigBufSW[MB_rOffsetControlSW_ERROR] > 0 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] < 17)	Module |= MB_bOffsetErrModule3a;

				if (ucConfigBufSW[MB_rOffsetControlSW_ON] > 16 && ucConfigBufSW[MB_rOffsetControlSW_ON] < 34)		Module |= MB_bOffsetErrModule3b;
				if (ucConfigBufSW[MB_rOffsetControlSW_OFF] > 16 && ucConfigBufSW[MB_rOffsetControlSW_OFF] < 34)		Module |= MB_bOffsetErrModule3b;
				if (ucConfigBufSW[MB_rOffsetControlSW_ERROR] > 16 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] < 34)	Module |= MB_bOffsetErrModule3b;


				if ((Module & MB_bOffsetErrModule3a) && (ucMDiscInBuf[MB_offsetHardFaults] & MB_bOffsetErrModule3a)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
				if ((Module & MB_bOffsetErrModule3b) && (ucMDiscInBuf[MB_offsetHardFaults] & MB_bOffsetErrModule3b)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
				if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}

				IedServer_updateQualityWithTime(iedServer, &iedModel_CTRL_CSWI1_Pos_q,&iedModel_CTRL_CSWI1_Pos_t,currentTime,Qual);
			}
}

/*******************************************************
 * Set_XCBR ��������� ������������ �������
 *******************************************************/
int		Set_XCBR	(uint16_t QTnum, uint64_t currentTime )
{
	int	ret = false;

	uint32_t	Mod;
	uint32_t	ctlModel,stVal,ctlVal;
	uint32_t	Health = STVALINT32_OK;
	uint32_t 	CtrlD,CtrlD1;
	uint64_t 	diskreet;
	Quality		Qual = QUALITY_VALIDITY_GOOD;
	bool 		bolVal;


	if (ucMDiscInBuf[MB_offset_errorHard1] & MB_bOffset_errorSWAll) {Health = STVALINT32_Warning;}
	if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Health = STVALINT32_Warning;}


	if (IedServer_updateInt32AttributeValue(iedServer, &iedModel_CTRL_XCBR1_Health_stVal, Health))
	IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_Health_t, currentTime);

//pos
	stVal = STVALCODEDENUM_INTERMEDIATE;
	if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_ON) {stVal |= STVALCODEDENUM_ON;}
	if (ucMDiscInBuf[MB_offset_SW_status] & MB_b_SW_OFF) {stVal |= STVALCODEDENUM_OFF;}

	if (IedServer_updateBitStringAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_stVal, stVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_t, currentTime);
		// �������� ������ �����������
		AddToQueueMB(ModbusSentQueue, 	MB_Rd_ConfigSWCrash	,MB_Slaveaddr);
		ret = true;
	}

	//  Quality (Pos)
	{
		int	Qual = QUALITY_VALIDITY_GOOD;
		int	Module	=	0;
		// �������� ���� ������������ �����������, � ������� ����������� ���� ������

		if (ucConfigBufSW[MB_rOffsetControlSW_ON] > 0 && ucConfigBufSW[MB_rOffsetControlSW_ON] < 17)		Module |= MB_bOffsetErrModule3a;
		if (ucConfigBufSW[MB_rOffsetControlSW_OFF] > 0 && ucConfigBufSW[MB_rOffsetControlSW_OFF] < 17)		Module |= MB_bOffsetErrModule3a;
		if (ucConfigBufSW[MB_rOffsetControlSW_ERROR] > 0 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] < 17)	Module |= MB_bOffsetErrModule3a;

		if (ucConfigBufSW[MB_rOffsetControlSW_ON] > 16 && ucConfigBufSW[MB_rOffsetControlSW_ON] < 34)		Module |= MB_bOffsetErrModule3b;
		if (ucConfigBufSW[MB_rOffsetControlSW_OFF] > 16 && ucConfigBufSW[MB_rOffsetControlSW_OFF] < 34)		Module |= MB_bOffsetErrModule3b;
		if (ucConfigBufSW[MB_rOffsetControlSW_ERROR] > 16 && ucConfigBufSW[MB_rOffsetControlSW_ERROR] < 34)	Module |= MB_bOffsetErrModule3b;


		if ((Module & MB_bOffsetErrModule3a) && (ucMDiscInBuf[MB_offsetHardFaults] & MB_bOffsetErrModule3a)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
		if ((Module & MB_bOffsetErrModule3b) && (ucMDiscInBuf[MB_offsetHardFaults] & MB_bOffsetErrModule3b)) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
		if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}


		if (IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_Pos_q,Qual)){
			IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Pos_t, currentTime);
		}
	}
//Loc --------------------------------------------------------------------------------------------------
	bolVal = true;

	CtrlD = ucMUstavkiInBuf[MB_offset_BlockSDTU];	// ����� ��������������� ��������
	if (CtrlD>0) {
		diskreet = (uint64_t)ucMDiscInBuf[MB_offsetDiscreet_1];

		CtrlD1 = (CtrlD - 1)%2;	// �������� ����������
		CtrlD = (CtrlD - 1)/2;
		if (CtrlD1 > 0) { if ((diskreet & 1<<CtrlD) == 0) 	bolVal = false; }
		else			{ if ((diskreet & 1<<CtrlD)>0) 		bolVal = false; }
	} else{
		 bolVal = false;
	}

	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Loc_stVal, bolVal))
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Loc_t, currentTime);

	Qual = QUALITY_VALIDITY_GOOD;
	if ((ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki)>0) {Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;}
	if (IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_Loc_q, Qual)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_Loc_t, currentTime);
	}

// !Loc -------------------------------------------------------------------------------------------------
	//BlkOpn
		bolVal = false;
		if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkOpn_stVal, bolVal)){
			IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkOpn_t, currentTime);
		}

	//BlkCIs
		bolVal = true;
		if ((ucMDiscInBuf[MB_offsetErrorSW] & MB_bOffset_errorSWAll) == 0)
		{
			CtrlD = ucConfigBufSW[MB_rOffsetBlockSW_ON];			// ����� ��������������� ��������
			if (CtrlD>0) {
				diskreet = (uint64_t)ucMDiscInBuf[MB_offsetDiscreet_1];

				CtrlD1 = (CtrlD - 1)%2;
				CtrlD = (CtrlD - 1)/2;
				if (CtrlD1 > 0) { if ((diskreet & 1<<CtrlD) == 0) 	bolVal = false; }
				else			{ if ((diskreet & 1<<CtrlD)>0) 		bolVal = false; }
			}else{
				 bolVal = false;
			}
		}

	if (IedServer_updateBooleanAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_stVal, bolVal)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_t, currentTime);
	}

//OpCnt
	if (IedServer_updateUnsignedAttributeValue(iedServer, &iedModel_CTRL_XCBR1_OpCnt_stVal, ucSWCrash[MB_offset_Oper_cnt])){
		IedServer_updateUTCTimeAttributeValue(iedServer,&iedModel_CTRL_XCBR1_OpCnt_t, currentTime);
		ret = true;
	}

	Qual = QUALITY_VALIDITY_GOOD;
	IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_BlkOpn_q, Qual);

	if (ucMDiscInBuf[MB_offset_errorUstavki] & MB_bOffset_errorUstavki) Qual = QUALITY_VALIDITY_INVALID | QUALITY_DETAIL_FAILURE;
	if (IedServer_updateQuality(iedServer,&iedModel_CTRL_XCBR1_BlkCls_q, Qual)){
		IedServer_updateUTCTimeAttributeValue(iedServer, &iedModel_CTRL_XCBR1_BlkCls_t, currentTime);
	}

return	ret;
}
#endif

