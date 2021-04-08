/*****************************************************************************
* Model: tym_platform.qm
*
* This code has been generated by QM tool (see state-machine.com/qm).
* DO NOT EDIT THIS FILE MANUALLY. All your changes will be lost.
*
* This program is open source software: you can redistribute it and/or
* modify it under the terms of the GNU General Public License as published
* by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
* for more details.
*****************************************************************************/
/**
 *  @file      HdmiSrv.c
 *  @brief     This file contains the HDMI driver implementation.
 *  @author    Neo
 *  @date      24-Aug-2017
 *  @copyright Tymphany Ltd.
 */

#include "trace.h"
#include "product.config"
#include "HdmiSrv.h"
#include "HdmiSrv_priv.h"
#include "controller.h"
#include "HdmiDrv.h"
#include "HdmiSrv.Config"

#define CAST_ME cHdmiSrv* hdmiSrv = (cHdmiSrv *) me;

 /* local variables*/
static QEvt const *HdmiSrvQueueSto[HDMI_SRV_EVENT_Q_SIZE];     /**< HDMI Server Event queue */
static HdmiDrv HdmiDrvObj;

/*************************************************************
    Debug information
**************************************************************/
#ifdef HDMISRV_DEBUG
    #define TYMQP_DUMP_QUEUE_WITH_LOG(me, ...) TymQP_DumpQueue_WithLog((QActive *)(me), __func__, __VA_ARGS__)
    #define HDMISRV_DEBUG_MSG TP_PRINTF
#else
    #define TYMQP_DUMP_QUEUE_WITH_LOG(me, ...)
    #define HDMISRV_DEBUG_MSG(...)
    #undef   TP_PRINTF
    #define  TP_PRINTF(...)
#endif

/*****************************************************************************************************************
 *
 * Start-up / shut-down functions
 *
 *****************************************************************************************************************/
/* @brief       HDMI Server Startup process, register to controller and invoke by system
 * @param[in]   me          pointer to the base object of the Led Server object
 */
void HdmiSrv_StartUp(cPersistantObj *me)
{
    /* Add to QSPY directory - only does so if QSPY enabled.
    * Remember to make sure these items exist
    */
    QS_OBJ_DICTIONARY(hdmiSrv);
    QS_OBJ_DICTIONARY(HdmiSrv_Active);
    QS_OBJ_DICTIONARY(HdmiSrv_DeActive);

    /* start up the object and let it run. including the timer*/
    Server_Ctor((cServer*)me, Q_STATE_CAST(&HdmiSrv_Initial), HDMI_TIMEOUT_SIG,
                         HdmiSrvQueueSto, Q_DIM(HdmiSrvQueueSto), HDMI_SRV_ID);
}

/* @brief       Hdmi Server Shut Down process, register to controller and invoke by system
 * @param[in]   me          pointer to the base object of the Hdmi Server object
 */
void HdmiSrv_ShutDown(cPersistantObj *me)
{
    //cHdmiSrv *m = (cHdmiSrv*) me;

	HdmiDrv_Xtor(&HdmiDrvObj);

    Server_Xtor((cServer*)me);
}

/*****************************************************************************************************************
 *
 * State functions
 *
 *****************************************************************************************************************/

/* @brief       HDMI Server Initial state handler
 * @param[in]   me          pointer to Led Server object
 * @param[in]   e           N/A
 */
QState HdmiSrv_Initial(cHdmiSrv * const me, QEvt const * const e)
{
     /* suppress the compiler warning about unused parameter */
    (void)e;

    QActive_subscribe((QActive*)me, HDMI_STATE_SIG);
    HdmiDrv_Ctor(&HdmiDrvObj);
    return Q_TRAN(&HdmiSrv_DeActive);
}

/* @brief       ramp down the server
 * @param[in]   me          pointer to Led Server object
 * @param[in]   e           Event received
 */
QState HdmiSrv_PreActive(cHdmiSrv * const me, QEvt const * const e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
    {
        PersistantObj_RefreshTick((cPersistantObj*)me, HDMI_PREV_ACTIVE_MS);
        return Q_HANDLED();
    }
    case SYSTEM_ACTIVE_REQ_SIG:
    {
        CommonReqEvt* pReq = (CommonReqEvt*) e;
        CommonEvtResp((QActive*)me, pReq->sender, RET_SUCCESS, SYSTEM_ACTIVE_RESP_SIG);
        return Q_HANDLED();
    }
    case SYSTEM_SLEEP_REQ_SIG:
    {
        return Q_TRAN(HdmiSrv_DeActive);
    }
    case HDMI_TIMEOUT_SIG:
    {
        uint32 timeout = HDMI_PREV_ACTIVE_MS;

        if (!HdmiDrvObj.drvIsReady)
        {
            if (HdmiDrv_isDeviceReady())
            {
                timeout = HdmiDrv_Init(&HdmiDrvObj);
                if(0 == timeout)
                {
                   HdmiDrvObj.drvIsReady = TRUE;
                   return Q_TRAN(HdmiSrv_Active);
                }
            }
        }

        PersistantObj_RefreshTick((cPersistantObj*)me, timeout);
        return Q_HANDLED();
    }
    case Q_EXIT_SIG:
    {
        QTimeEvt_disarm(TIME_EVT_OF(me));
        return Q_HANDLED();
    }
    default:
        break;
    }
    return Q_SUPER(&QHsm_top);

}

/* @brief       HDMI Server Active state handler
 * @param[in]   me          pointer to Led Server object
 * @param[in]   e           Event received
 */
QState HdmiSrv_Active(cHdmiSrv * const me, QEvt const * const e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
    {
        PersistantObj_RefreshTick((cPersistantObj*)me, HDMI_TICK_MS);
        return Q_HANDLED();
    }
    case SYSTEM_ACTIVE_REQ_SIG:
    {
        CommonReqEvt* pReq = (CommonReqEvt*) e;
        CommonEvtResp((QActive*)me, pReq->sender, RET_SUCCESS, SYSTEM_ACTIVE_RESP_SIG);
        return Q_HANDLED();
    }
    case SYSTEM_SLEEP_REQ_SIG:
    {
        return Q_TRAN(HdmiSrv_DeActive);
    }
    case HDMI_INT_REQ_SIG:
    {
        HDMISRV_DEBUG_MSG("HDMI_INT_REQ_SIG\r\n");
        return Q_HANDLED();
    }
    case HDMI_REQ_SIG:  /* signal from MCU server to HDMI chip */
    {
        HdmiReqEvt* reqEvt = (HdmiReqEvt*)e;
        HDMISRV_DEBUG_MSG("HDMI_REQ_SIG id=%d\r\n", reqEvt->hdmiSettId);
        HdmiDrv_RequestHandler(reqEvt);
        return Q_HANDLED();
    }
    case HDMI_STATE_SIG:  /* signal from HDMI chip from MCU server */
    {
        HdmiGetCmd* pGetCmd = (HdmiGetCmd*)e;
        HDMISRV_DEBUG_MSG("HDMI_STATE_SIG id=%d\r\n", pGetCmd->hdmiSettId);
        HdmiDrv_GetHandler(pGetCmd);
        return Q_HANDLED();
    }
    case HDMI_TIMEOUT_SIG:
    {
        //HDMISRV_DEBUG_MSG("HDMI_TIMEOUT_SIG\r\n");
        HdmiDrv_UpdateStatus();
        PersistantObj_RefreshTick((cPersistantObj*)me, HDMI_TICK_MS);
        return Q_HANDLED();
    }
    case Q_EXIT_SIG:
    {
        QTimeEvt_disarm(TIME_EVT_OF(me));
        return Q_HANDLED();
    }
    default:
        break;
    }
    return Q_SUPER(&QHsm_top);
}

/* @brief       HDMI Server Deactive state handler
 * @param[in]   me          pointer to Led Server object
 * @param[in]   e           Event received
 */
QState HdmiSrv_DeActive(cHdmiSrv * const me, QEvt const * const e)
{
    switch (e->sig)
    {
    case Q_ENTRY_SIG:
    {
        return Q_HANDLED();
    }
    case SYSTEM_ACTIVE_REQ_SIG:
    {
        CommonReqEvt* pReq = (CommonReqEvt*) e;
        CommonEvtResp((QActive*)me, pReq->sender, RET_SUCCESS, SYSTEM_ACTIVE_RESP_SIG);
        return Q_TRAN(&HdmiSrv_PreActive);
    }
    case Q_EXIT_SIG:
    {
        QTimeEvt_disarm(TIME_EVT_OF(me));
        return Q_HANDLED();
    }

    default:
        break;
    }
    return Q_SUPER(&QHsm_top);
}


/*
* enter ISP mode
*/
void HdmiSrv_EnterISP(void)
{
    HdmiReqEvt* hdmiReqEvt = Q_NEW(HdmiReqEvt, HDMI_REQ_SIG);
    ASSERT(hdmiReqEvt);

    hdmiReqEvt->hdmiSettId = HDMI_ENTER_ISP_ID;
    SendToServer(HDMI_SRV_ID, (QEvt*)hdmiReqEvt);
}

void HdmiSrv_SendPowerOnOff(bool on)
{
    HdmiReqEvt* hdmiReqEvt = Q_NEW(HdmiReqEvt, HDMI_REQ_SIG);
    ASSERT(hdmiReqEvt);

    hdmiReqEvt->hdmiSettId = HDMI_POWER_ID;
    hdmiReqEvt->param = on;
    SendToServer(HDMI_SRV_ID, (QEvt*)hdmiReqEvt);
}

void HdmiSrv_SendVolume(uint8_t volume)
{
    HdmiReqEvt* hdmiReqEvt = Q_NEW(HdmiReqEvt, HDMI_REQ_SIG);
    ASSERT(hdmiReqEvt);

    hdmiReqEvt->hdmiSettId = HDMI_VOLUME_ID;
    hdmiReqEvt->param = volume;
    SendToServer(HDMI_SRV_ID, (QEvt*)hdmiReqEvt);
}

void HdmiSrv_SendMute(bool mute)
{
    HdmiReqEvt* hdmiReqEvt = Q_NEW(HdmiReqEvt, HDMI_REQ_SIG);
    ASSERT(hdmiReqEvt);

    hdmiReqEvt->hdmiSettId = HDMI_MUTE_ID;
    hdmiReqEvt->param = mute;
    SendToServer(HDMI_SRV_ID, (QEvt*)hdmiReqEvt);
}

void HdmiSrv_SendTest(uint8_t param)
{
    HdmiReqEvt* hdmiReqEvt = Q_NEW(HdmiReqEvt, HDMI_REQ_SIG);
    ASSERT(hdmiReqEvt);

    hdmiReqEvt->hdmiSettId = HDMI_TEST_ID;
    hdmiReqEvt->param = param;
    SendToServer(HDMI_SRV_ID, (QEvt*)hdmiReqEvt);
}


/*
* cec_power on/off
*/
void HdmiSrv_UpdatedPowerOnOff(bool on)
{
    HdmiGetCmd* hdmiGetCmd = Q_NEW(HdmiGetCmd, HDMI_STATE_SIG);
    ASSERT(hdmiGetCmd);

    hdmiGetCmd->hdmiSettId = HDMI_CEC_POWER_ID;
    hdmiGetCmd->enable = on;
    QF_PUBLISH(&hdmiGetCmd->super, 0);
}

void HdmiSrv_UpdatedVolume(uint8_t soundBar_vol, uint8_t cec_volume)
{
    HdmiGetCmd* hdmiGetCmd = Q_NEW(HdmiGetCmd, HDMI_STATE_SIG);
    ASSERT(hdmiGetCmd);

    hdmiGetCmd->hdmiSettId = HDMI_CEC_VOLUME_ID;
    hdmiGetCmd->enable = TRUE;
    hdmiGetCmd->param = soundBar_vol;
    hdmiGetCmd->param2 = cec_volume;
    QF_PUBLISH(&hdmiGetCmd->super, 0);
}

void HdmiSrv_UpdatedMuteStatus(bool mute)
{
    HdmiGetCmd* hdmiGetCmd = Q_NEW(HdmiGetCmd, HDMI_STATE_SIG);
    ASSERT(hdmiGetCmd);

    hdmiGetCmd->hdmiSettId = HDMI_CEC_MUTE_ID;
    hdmiGetCmd->enable = mute;
    QF_PUBLISH(&hdmiGetCmd->super, 0);
}

void HdmiSrv_UpdatedSource(uint8_t source)
{
    HdmiGetCmd* hdmiGetCmd = Q_NEW(HdmiGetCmd, HDMI_STATE_SIG);
    ASSERT(hdmiGetCmd);

    hdmiGetCmd->hdmiSettId = HDMI_CEC_PORT_SOURCE_ID;
    hdmiGetCmd->enable = TRUE;
    hdmiGetCmd->param = source;
    QF_PUBLISH(&hdmiGetCmd->super, 0);
}

void HdmiSrv_UpdatedTxPluginStatus(bool plugin)
{
    HdmiGetCmd* hdmiGetCmd = Q_NEW(HdmiGetCmd, HDMI_STATE_SIG);
    ASSERT(hdmiGetCmd);

    hdmiGetCmd->hdmiSettId = HDMI_TX_PLUGIN_ID;
    hdmiGetCmd->enable = plugin;
    QF_PUBLISH(&hdmiGetCmd->super, 0);
}

void HdmiSrv_UpdatedRxPluginStatus(bool plugin)
{
    HdmiGetCmd* hdmiGetCmd = Q_NEW(HdmiGetCmd, HDMI_STATE_SIG);
    ASSERT(hdmiGetCmd);

    hdmiGetCmd->hdmiSettId = HDMI_RX_PLUGIN_ID;
    hdmiGetCmd->enable = plugin;
    QF_PUBLISH(&hdmiGetCmd->super, 0);
}

