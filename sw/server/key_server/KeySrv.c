/*
-------------------------------------------------------------------------------
TYMPHANY LTD





                  Key Server
                  -------------------------

                  SW Module Document




@file        KeySrv.c
@brief       This file defines the interfaces of the key server and the implementation of the interfaces.
@author      Bob.Xu
@date        2014-02-17
@copyright (c) Tymphany Ltd. All rights reserved.

Change History:
VERSION    : 1    DRAFT      2014-02-17     Bob.Xu
DESCRIPTION: First Draft. Generated by newclass.py
SCO/ERROR  :
-------------------------------------------------------------------------------
*/
#include "KeySrv_priv.h"
#include "bsp.h"
#include "trace.h"
#include "controller.h"
#include "KeySrv.Config"
#include "KeySrv.h"

//Q_DEFINE_THIS_FILE

/* Below are the protection macros in case if you missed to define some critical data */
#ifdef HAS_GPIO_KEY
#ifndef NUM_OF_GPIO_KEY
    #error "NUM_OF_GPIO_KEY is not defined, please define it in attachedDevices.h"
#endif
#endif

#ifdef HAS_LINEAR_ADC_KNOB_KEY
#ifndef NUM_OF_LINEAR_KNOB_KEY
    #error "NUM_OF_LINEAR_KNOB_KEY is not defined, please define it in attachedDevices.h"
#endif
#endif


#ifdef HAS_ADC_KEY
#ifndef NUM_OF_ADC_KEY
    #error "NUM_OF_ADC_KEY is not defined, please define it in attachedDevices.h"
#endif
#endif

/* End of macro protection */

#define CAST_ME cKeySrv * keySrv = (cKeySrv *) me;

/* Private functions */
static void KeySrv_AnalyseKeyEvt(cKeySrv *const me, uint8 i);
static void KeySrv_KeyDownHandler(cKeySrv *const me, uint8 i);
static void KeySrv_StartScanKey(cKeySrv *const me);
static void KeySrv_ResetKeytoDefault(cKeySrv *const me, uint8 i);
static void KeySrv_SendKeyEvt(cKeySrv *const me, uint8 keyObjIndex, eKeyEvent evt);
#ifdef KEY_SRV_HAS_DEBUG
static int16 KeySrv_GetRawData(cKeySrv *const me, KeyDebugReqEvt *req);
static void KeySrv_SendDebugResp( int16 data, eEvtReturn serviceState, KeyDebugReqEvt *req);
static void KeySrv_RunEvtSimulationCmd(cKeySrv *const me, KeyDebugReqEvt *req);
#endif
static void KeySrv_CreateKey(cKeySrv * keySrv);
#ifdef HAS_INTERRUPT_WAKE_UP_KEY
static void KeySrv_PostSysGoBackToSleep();
#endif
static void KeySrv_XtorKeyObjs(cKeySrv *const me);

#ifdef HAS_COMB_KEY
static void KeySrv_CombKeyInit();
static void KeySrv_AnalyseCombKeyEvt(cKeySrv *const me, cKeyDrv *pKeyObj,\
                                                tEvtSendCtr *pKeyEvtCtr);
static BOOL KeySrv_IsNormCombKey(tCombDef *pCombKeys);
static void KeySrv_RefreshCombKeyTimer();
static void KeySrv_SendCombKeyEvt(eKeyID keyId);
#endif

/* Internal event queue */
static QEvt const *KeyEvtQue[KEY_SRV_EVENT_Q_SIZE];

static QActive* pSysSleepRequester = NULL;

#ifdef HAS_INTERRUPT_WAKE_UP_KEY
static tWakeUpKeyIntInfoCtr wakeUpKeyIntInfoCtr =
{
    .debouceStart = FALSE,
    .isRealPress = TRUE
};
static eKeyID interrupGenerator = INVALID_KEY;
#endif

static uint32 previousTime = 0;
static uint32 currentTime  = 0;

/* internal signals */
enum InternalSignals
{
    TIMEOUT_SIG = MAX_SIG
};

/*****************************************************************************************************************
 *
 * Start-up / shut-down functions
 *
 *****************************************************************************************************************/
void KeySrv_StartUp(cPersistantObj *me)
{
    CAST_ME;
    /* Add to QSPY directory - only does so if QSPY enabled.
    * Remember to make sure these items exist
    */
    QS_OBJ_DICTIONARY(keySrv);
    QS_OBJ_DICTIONARY(KeySrv_DeActive);
    QS_OBJ_DICTIONARY(KeySrv_Active);
    
    keySrv->pConfig = &keySrvConfig;

    /* start up the object and let it run. including the timer*/
    Server_Ctor((cServer*)me, Q_STATE_CAST(&KeySrv_Initial), KEY_TIMEOUT_SIG,
                      KeyEvtQue, Q_DIM(KeyEvtQue), KEY_SRV_ID);
}

void KeySrv_ShutDown(cPersistantObj *me)
{
    /* Clean memory and shut-down. Called by the controller */    
    Server_Xtor((cServer*)me);
}

/******************************************************************************
 *
 * State functions
 *
 ******************************************************************************/
/* Initial state */
QState KeySrv_Initial(cKeySrv *const me, QEvt const * const e)
{
    (void)e; /* suppress the compiler warning about unused parameter */
    return Q_TRAN(&KeySrv_DeActive);
}

/* Waiting for power server state */
QState KeySrv_DeActive(cKeySrv *const me, QEvt const *const e)
{
    (void)me; /* suppress the compiler warning about unused parameter */
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
#ifdef HAS_INTERRUPT_WAKE_UP_KEY
            if(TRUE == wakeUpKeyIntInfoCtr.debouceStart)
            {
                wakeUpKeyIntInfoCtr.debouceStart = FALSE;
                KeySrv_PostSysGoBackToSleep();
            }
#endif
            if(NULL != pSysSleepRequester)
            {
                CommonEvtResp((QActive*)me, pSysSleepRequester,RET_SUCCESS, SYSTEM_SLEEP_RESP_SIG);
            }
            return Q_HANDLED();
        }
        case SYSTEM_ACTIVE_REQ_SIG:
        {
            /* clean pSysSleepRequester before going to active state */
            pSysSleepRequester = NULL;
            KeySrv_CreateKey(me);
            CommonReqEvt* pReq = (CommonReqEvt*)e;
            CommonEvtResp((QActive*)me, pReq->sender,RET_SUCCESS, SYSTEM_ACTIVE_RESP_SIG);
            return Q_TRAN(&KeySrv_PreActive);
        }
#ifdef HAS_INTERRUPT_WAKE_UP_KEY
        case WAKE_UP_KEY_INTERRUPT_SIG:
        {/* receive the power key interrupt signal*/
            /* clean pSysSleepRequester before going to active state */
            pSysSleepRequester = NULL;
            wakeUpKeyIntInfoCtr.debouceStart = TRUE;
            /*
             * we suppose it is a real press, the deboucing will update it if is
             * not a real press
             */
            wakeUpKeyIntInfoCtr.isRealPress = TRUE;
            KeyEvtFromDrv *pkeyIntEvt = (KeyEvtFromDrv*)e;
            interrupGenerator = pkeyIntEvt->keyId;
            KeySrv_CreateKey(me);
            return Q_TRAN(&KeySrv_PreActive);
        }
#endif
        default:break;
    }
    return Q_SUPER(&QHsm_top);
}



QState KeySrv_Active(cKeySrv *const me, QEvt const * const e)
{
    uint8 i;
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            for(i = 0; i < NUM_OF_ALL_KEY; i++)
            {
                /*ensure that all obj is in the defualt state*/
                me->pKeyObj[i]->keySimulationState = KEY_INVALIDE_STATE;
                me->keyEvtCtr[i].currEvtState = KEY_EVT_INVALID;
                KeySrv_ResetKeytoDefault(me, i);
                previousTime = getSysTime();
            }
            KeySrv_StartScanKey(me);
            PersistantObj_RefreshTick((cPersistantObj*)me, KEY_SERVER_TICK_MS);
            return Q_HANDLED();
        }
        case KEY_TIMEOUT_SIG:
        {
            currentTime = getSysTime();
            /**
             * Each time out, we do three things
             * First: Update all the keys' status based on peripherals' raw data
             * Second: analyse the key event based on the key status
             * Third: Scan peripherals' raw data for the next turn
             */
            for(i = 0; i < NUM_OF_ALL_KEY; i++)
            {
                ASSERT(me->pKeyObj[i]->KeyUpdateStatusCb) ;
                me->pKeyObj[i]->KeyUpdateStatusCb(me->pKeyObj[i]); /* First */
                KeySrv_AnalyseKeyEvt(me, i);/* Second */
            }
#ifdef HAS_COMB_KEY
            KeySrv_RefreshCombKeyTimer();
#endif
            previousTime  = currentTime;

            KeySrv_StartScanKey(me); /* Third */
            PersistantObj_RefreshTick((cPersistantObj*)me, KEY_SERVER_TICK_MS);
#ifdef HAS_INTERRUPT_WAKE_UP_KEY
            if(TRUE == wakeUpKeyIntInfoCtr.debouceStart)
            {
                if(FALSE == wakeUpKeyIntInfoCtr.isRealPress)
                {
                    KeySrv_XtorKeyObjs(me);
                    return Q_TRAN(&KeySrv_DeActive);
                }
            }
#endif
            return Q_HANDLED();
        }
#ifdef KEY_SRV_HAS_DEBUG
        case KEY_DEBUG_REQ_SIG:
        {
             int16 keyRawData = 0;
             KeyDebugReqEvt *pDebugReqEvt;
             pDebugReqEvt = (KeyDebugReqEvt*)e;
             switch(pDebugReqEvt->req)
             {
                 case DEBUG_RAW_DATA_REQ:
                 {
                    keyRawData = KeySrv_GetRawData(me,(KeyDebugReqEvt*)e);
                    if( keyRawData < 0 )
                    {
                        KeySrv_SendDebugResp(keyRawData,RET_FAIL,(KeyDebugReqEvt*)e);
                    }
                    else
                    {
                        KeySrv_SendDebugResp(keyRawData,RET_SUCCESS,(KeyDebugReqEvt*)e);
                    }
                    break;
                 }
                 case DEBUG_KEY_EVT_SIMULATION:
                 {
                     /* for production test */
                     KeySrv_RunEvtSimulationCmd(me,pDebugReqEvt);
                     break;
                 }
                 default:break;
             }
             return Q_HANDLED();
        }
#endif
         case SYSTEM_SLEEP_REQ_SIG:
        {
            CommonReqEvt *pReqEvt = (CommonReqEvt*)e;
            pSysSleepRequester = pReqEvt->sender;
            KeySrv_XtorKeyObjs(me);
            return Q_TRAN(KeySrv_DeActive);
        }
        case SYSTEM_ACTIVE_REQ_SIG:
        {
            /*
             * keySrv has already been activated by the wake up key interrupt
             * simply send a response to indicate that keySrv is ready to serve.
             */
            CommonReqEvt* pReq = (CommonReqEvt*)e;
            CommonEvtResp((QActive*)me, pReq->sender,RET_SUCCESS, SYSTEM_ACTIVE_RESP_SIG);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG:
        {
            QTimeEvt_disarm(TIME_EVT_OF(me));
            return Q_HANDLED();
        }
        default: break;
    }
    return Q_SUPER(&QHsm_top);
}

static bool CheckInitSeqDone(cKeySrv * me)
{
    uint8 i;
    const tKeyboard *pKeyboard;
    tKeyboardDevice * pKeyboardConfig;
    uint16 index = 0xFFFF;
    eDeviceType prevDevType = INVALID_DEV_TYPE;

    for(i = 0; i < (me->pConfig->keyboardNum); i++)
    {
        pKeyboard = &(me->pConfig->keyboardSet[i]);
        if (pKeyboard->deviceType == prevDevType)
        {
            index++;
        }
        else
        {
            index = 0;
        }
        pKeyboardConfig = (tKeyboardDevice*) getDevicebyIdAndType(KEYBOARD_DEV_ID,pKeyboard->deviceType,&index);
        prevDevType = pKeyboard->deviceType;
        ASSERT(pKeyboardConfig);
        if(TRUE == pKeyboardConfig->isNeedInitSeq)
        {
            if(FALSE == pKeyboardConfig->isInitSeqDone)
            {
                return FALSE;
            }
        }
    }
    return TRUE;
}

/* Waiting for init state */
QState KeySrv_PreActive(cKeySrv *const me, QEvt const *const e)
{
    uint8 i;
    const tKeyboard *pKeyboard;
    tKeyboardDevice * pKeyboardConfig;
    uint16 index = 0xFFFF;
    eDeviceType prevDevType = INVALID_DEV_TYPE;
    
    (void)me; /* suppress the compiler warning about unused parameter */
    switch (e->sig)
    {
        case Q_ENTRY_SIG:
        {
            index = 0xFFFF;
            prevDevType = INVALID_DEV_TYPE;
            for(i = 0; i < (me->pConfig->keyboardNum); i++)
            {
                pKeyboard = &(me->pConfig->keyboardSet[i]);
                if (pKeyboard->deviceType == prevDevType)
                {
                    index++;
                }
                else
                {
                    index = 0;
                }
                pKeyboardConfig = (tKeyboardDevice*) getDevicebyIdAndType(KEYBOARD_DEV_ID,pKeyboard->deviceType,&index);
                prevDevType = pKeyboard->deviceType;
                ASSERT(pKeyboardConfig);
                if(TRUE == pKeyboardConfig->isNeedInitSeq && FALSE == pKeyboardConfig->isInitSeqDone)
                {
                    ASSERT(pKeyboardConfig->KeyboardInitSeqStartCb);
                    pKeyboardConfig->KeyboardInitSeqStartCb(pKeyboardConfig);
                }
            }

//            if(FALSE == CheckInitSeqDone(me))
//            {
//                PersistantObj_RefreshTick((cPersistantObj*)me, KEY_SERVER_TICK_MS);
//                return Q_HANDLED();
//            }
//            else
//            {
//                return Q_TRAN(&KeySrv_Active);
//            }
            
            PersistantObj_RefreshTick((cPersistantObj*)me, KEY_SERVER_TICK_MS);
        }

        case KEY_TIMEOUT_SIG:
        {
            index = 0xFFFF;
            prevDevType = INVALID_DEV_TYPE;
            for(i = 0; i < (me->pConfig->keyboardNum); i++)
            {
                pKeyboard = &(me->pConfig->keyboardSet[i]);
                if (pKeyboard->deviceType == prevDevType)
                {
                    index++;
                }
                else
                {
                    index = 0;
                }
                pKeyboardConfig = (tKeyboardDevice*) getDevicebyIdAndType(KEYBOARD_DEV_ID,pKeyboard->deviceType,&index);
                prevDevType = pKeyboard->deviceType;
                ASSERT(pKeyboardConfig);
                if(TRUE == pKeyboardConfig->isNeedInitSeq && FALSE == pKeyboardConfig->isInitSeqDone)
                {
                    ASSERT(pKeyboardConfig->KeyboardInitSeqProcessCb);
                    if(TRUE == pKeyboardConfig->KeyboardInitSeqProcessCb(pKeyboardConfig))
                    {
                        pKeyboardConfig->isInitSeqDone = TRUE;
                    }
                }
            }

            if(FALSE == CheckInitSeqDone(me))
            {
                PersistantObj_RefreshTick((cPersistantObj*)me, KEY_SERVER_TICK_MS);
                return Q_HANDLED();
            }
            else
            {
                return Q_TRAN(&KeySrv_Active);
            }
        }
        default:break;
    }
    return Q_SUPER(&QHsm_top);
}

void KeySrv_SendEvtToKeySrv(eKeyID keyId, eKeyEvent evt)
{
    KeyEvtFromDrv *pEvt = Q_NEW(KeyEvtFromDrv,REMOTE_KEY_SIG);
    pEvt->keyId = keyId;
    pEvt->keyEvent = evt;
    SendToServer(KEY_SRV_ID, (QEvt*)pEvt);
}

/******************************************************************************
 *
 * private functions
 *
 ******************************************************************************/
static void KeySrv_StartScanKey(cKeySrv *const me)
{
    uint8 i ;
    const tKeyboard *pKeyboard;
    for(i = 0; i < (me->pConfig->keyboardNum); i++)
    {
        pKeyboard = &(me->pConfig->keyboardSet[i]);
        if(NULL != pKeyboard->pKeySet[0].KeyStartScanCb)
        {
            pKeyboard->pKeySet[0].KeyStartScanCb((&(pKeyboard->pKeySet[0])));
        }
    }
}

/**
*
* |-----------|------------------------|-----------------------------|
* |<-boucing->|<---sampling period---->|<--Evt anlys based on timer->|
* Igore the boucing period, the key state is unstable during the boucing period.
* During the sampling period, any up event indicate an unstable key press and it
* is not treat as an key down event
*/
static void KeySrv_AnalyseKeyEvt(cKeySrv *const me, uint8 i)
{
    uint32 debounceTime = me->pConfig->timing.debounceTime;

    /* for production test */
    if (KEY_DOWN == me->pKeyObj[i]->keySimulationState)
    {
        /* Overwrite the key state */
        me->pKeyObj[i]->keyState = KEY_DOWN;
#ifdef HAS_COMB_KEY
        (me->numOfKeyDown)++;
        KeySrv_AnalyseCombKeyEvt(me,me->pKeyObj[i],&(me->keyEvtCtr[i]));
#endif
    }
    else if (KEY_UP == me->pKeyObj[i]->keySimulationState)
    {
        /**
         * do not set pKeyObj->keyState to KEY_UP here, if key is up in reality,
         * the state will be freshed in the next loop automatically.
         * setting pKeyObj->keyState to KEY_UP may invoke the server to send a
         * fake key up even key is pressed in reality. This case exists when
         * simulation and real key pressed are processed simutaniously.
         */
        me->pKeyObj[i]->keySimulationState = KEY_INVALIDE_STATE;
    }
    /* End of production test */

    /*upgrade timer */
    me->keyTimer[i] += (currentTime - previousTime);
    switch(me->keyAnlysStage[i])
    {
        case KEY_UP:
        {
            if(me->pKeyObj[i]->keyState == KEY_DOWN) /* key is down */
            {
                me->keyAnlysStage[i] = KEY_DEBOUNCING;
            }
            me->keyTimer[i] = 0;
            break;
        }
        case KEY_DEBOUNCING:
        {
            if(me->keyTimer[i] < (debounceTime - (DEBOUNCE_SAMP_NUM*KEY_SERVER_TICK_MS)))
            {
                return;
            }
            else if(me->pKeyObj[i]->keyState == KEY_DOWN)
            {
                if(me->keyTimer[i] >= debounceTime)
                {
                    me->keyAnlysStage[i] = KEY_DOWN;
                }
            }
            else /* not a real key down */
            {
                /*
                 * (TRUE == wakeUpKeyIntInfoCtr.debouceStart) means
                 * system just changed from DeActive state to ActiveState caused by the
                 * wake up key interrupt.
                 */
#ifdef HAS_INTERRUPT_WAKE_UP_KEY
                if((interrupGenerator == me->pKeyObj[i]->keyID) &&\
                        (TRUE == wakeUpKeyIntInfoCtr.debouceStart))
                {
                    wakeUpKeyIntInfoCtr.isRealPress = FALSE;
                }
#endif
                me->keyTimer[i] = 0;
                me->keyAnlysStage[i] = KEY_UP;
            }
            break;
        }
        case KEY_DOWN:
        {
            KeySrv_KeyDownHandler(me,i);
            break;
        }
        default:break;
    }
}

static void KeySrv_KeyDownHandler(cKeySrv *const me, uint8 i)
{
    uint32 longPressTime     = me->pConfig->timing.longPressTime;
    uint32 veryLongPressTime = me->pConfig->timing.veryLongPressTime;
    uint32 repeatStartTime   = me->pConfig->timing.repeatStartTime;
    uint32 repeatHoldTime    = me->pConfig->timing.repeatHoldTime[me->pKeyObj[i]->keyID];
    if(!(me->keyEvtCtr[i].keyDownEvtSent))
    {
        me->keyEvtCtr[i].keyDownEvtSent = TRUE;
        KeySrv_SendKeyEvt(me, i, KEY_EVT_DOWN);

#ifdef HAS_COMB_KEY
        /*
         *  Comb key analysis
         *  Note: KeySrv_AnalyseCombKeyEvt(me,pKeyObj,pKeyEvtCtr);
         *  must be the last step
         */
        me->keyEvtCtr[i].currEvtState = KEY_EVT_DOWN;
        (me->numOfKeyDown)++;
        KeySrv_AnalyseCombKeyEvt(me,me->pKeyObj[i],&(me->keyEvtCtr[i]));
#endif
#ifdef HAS_INTERRUPT_WAKE_UP_KEY
        if((interrupGenerator == me->pKeyObj[i]->keyID) && \
                (TRUE == wakeUpKeyIntInfoCtr.debouceStart))
        {
            /*
             * The wake up key interrup passed the deboucing, it is a real key
             * down event. Interrrup key deboucing is terminated from now on.
             */
            wakeUpKeyIntInfoCtr.debouceStart = FALSE;
        }
#endif
    }

    if(me->pKeyObj[i]->keyState == KEY_DOWN) /* key is still down */
    {
        if(me->keyTimer[i] > longPressTime && me->keyTimer[i] < veryLongPressTime)
        {
           /**
             * The life time of the key holding signal
             * start from now until a pressed signal is posted
             */
            if(!(me->keyEvtCtr[i].holdEvtSent))
            {
                /**
                 * post KEY_EVT_HOLD only once, otherwise it will
                 * generate KEY_EVT_HOLD every TIMEOUT_SIG.
                 */
                KeySrv_SendKeyEvt(me, i, KEY_EVT_HOLD);
                me->keyEvtCtr[i].holdEvtSent = TRUE;                
                me->keyEvtCtr[i].currEvtState = KEY_EVT_HOLD;
#ifdef HAS_COMB_KEY
                /* Comb key analysis */                
                KeySrv_AnalyseCombKeyEvt(me,me->pKeyObj[i],&(me->keyEvtCtr[i]));
#endif
            }
        }
        else if(me->keyTimer[i] > veryLongPressTime)
        {
            if(!(me->keyEvtCtr[i].veryLongholdEvtSent))
            {
                /**
                * post KEY_EVT_VERY_LONG_HOLD only once
                */
                KeySrv_SendKeyEvt(me, i, KEY_EVT_VERY_LONG_HOLD);
                me->keyEvtCtr[i].veryLongholdEvtSent = TRUE;
                me->keyEvtCtr[i].currEvtState = KEY_EVT_VERY_LONG_HOLD;
#ifdef HAS_COMB_KEY
                /* Comb key analysis */                
                KeySrv_AnalyseCombKeyEvt(me,me->pKeyObj[i],&(me->keyEvtCtr[i]));
#endif
            }
        }
        /*
         * It shouldn't be an "else if" since repeat should be sent once the key 
         * is hold, no matter it's long press hold or every long press hold
        */
        uint32 timeToSendRepeat = (repeatStartTime + repeatHoldTime + \
                    me->keyEvtCtr[i].repeatEvtSentNum * repeatHoldTime);
        if((repeatHoldTime > 0) && me->keyTimer[i] > timeToSendRepeat)
        {
            /*
             * If repeatHoldTime[x] > 0, then after sent KEY_EVT_HOLD,
             * post KEY_EVT_REPEAT in every repeatHoldTime[x] until key up,
             * If repeatHoldTime[x] = 0, it means that it is not required
             * to send KEY_EVT_REPEAT for the specific key.
             */
            KeySrv_SendKeyEvt(me, i, KEY_EVT_REPEAT);
            me->keyEvtCtr[i].repeatEvtSentNum++;
        }

    }
    else if(me->pKeyObj[i]->keyState == KEY_UP)/* Key Up */
    {
        /* First: send a press evt */
        if(me->keyTimer[i] < longPressTime)
        {
            me->keyEvtCtr[i].currEvtState = KEY_EVT_SHORT_PRESS;
            KeySrv_SendKeyEvt(me, i, KEY_EVT_SHORT_PRESS);
        }
        else if(me->keyTimer[i] >= longPressTime && me->keyTimer[i] < veryLongPressTime)
        {
            me->keyEvtCtr[i].currEvtState = KEY_EVT_LONG_PRESS;
            KeySrv_SendKeyEvt(me, i, KEY_EVT_LONG_PRESS);
        }
        else
        {
            me->keyEvtCtr[i].currEvtState = KEY_EVT_VERY_LONG_PRESS;
            KeySrv_SendKeyEvt(me, i, KEY_EVT_VERY_LONG_PRESS);
        }
        /* Second: Send a key up evt */
        KeySrv_SendKeyEvt(me, i, KEY_EVT_UP);
#ifdef HAS_COMB_KEY
        /* Comb key analysis */
        (me->numOfKeyDown)--;
        KeySrv_AnalyseCombKeyEvt(me,me->pKeyObj[i],&(me->keyEvtCtr[i]));
#endif
        /* A key complete key action, clean all the flags */
        KeySrv_ResetKeytoDefault(me, i);
    }
    else
    { /*Get invalid state, reset the key and clean all the flags*/
        KeySrv_ResetKeytoDefault(me, i);        
    }
}

static void KeySrv_ResetKeytoDefault(cKeySrv *const me, uint8 i)
{
    me->pKeyObj[i]->keyState = KEY_UP;
    me->keyTimer[i] = 0;
    me->keyEvtCtr[i].holdEvtSent = FALSE;
    me->keyEvtCtr[i].keyDownEvtSent = FALSE;
    me->keyEvtCtr[i].veryLongholdEvtSent = FALSE;
    me->keyEvtCtr[i].repeatEvtSentNum = 0;
    me->keyAnlysStage[i] = KEY_UP;
}

#ifdef KEY_SRV_HAS_DEBUG
static int16 KeySrv_GetRawData(cKeySrv *const me, KeyDebugReqEvt *req)
{
    uint8 i;
    int16 rtnValue = TP_ACCESS_ERROR;
    for(i = 0; i < NUM_OF_ALL_KEY; i++)
    {
        if(req->keyId == me->pKeyObj[i]->keyID)
        {
            rtnValue = me->pKeyObj[i]->KeyGetRawDataCb(me->pKeyObj[i]);
            ASSERT(rtnValue>=0);
            break;
        }
    }
    ASSERT(TP_ACCESS_ERROR != rtnValue);
    return rtnValue;
}

/* producation test */
static void KeySrv_RunEvtSimulationCmd(cKeySrv *const me, KeyDebugReqEvt *req)
{
    uint8 i;
    for(i = 0; i < NUM_OF_ALL_KEY; i++)
    {
        if(req->keyId == me->pKeyObj[i]->keyID)
        {
            if(KEY_EVT_UP == req->keyEvent)
            {
                me->pKeyObj[i]->keySimulationState = KEY_UP;
            }
            else if(KEY_EVT_DOWN == req->keyEvent)
            {
                me->pKeyObj[i]->keySimulationState = KEY_DOWN;
            }
            break;
        }
    }
}

static void KeySrv_SendDebugResp( int16 data, eEvtReturn serviceState, KeyDebugReqEvt *req)
{
    if (req->sender != NULL)
    {
        KeyDebugRespEvt *pte = Q_NEW(KeyDebugRespEvt, KEY_DEBUG_RESP_SIG);
        pte->keyId = req->keyId;
        pte->rawData = data;
        pte->serviceState = serviceState;
        QACTIVE_POST(req->sender, (QEvt*)pte, 0);
    }
}
#endif

static void KeySrv_SendKeyEvt(cKeySrv *const me, uint8 keyObjIndex, eKeyEvent evt)
{
    KeyStateEvt *pte = Q_NEW(KeyStateEvt, KEY_STATE_SIG);
    ASSERT(pte);

    pte->keyId = me->pKeyObj[keyObjIndex]->keyID;
    pte->keyEvent = evt;
#ifdef HAS_PARAM_KEY
    pte->param = me->pKeyObj[keyObjIndex]->param;
#endif
    QF_PUBLISH(&pte->super, 0);
}

void KeySrv_SendKeyEvt_Direct(eKeyEvent evt, eKeyID keyId)
{
    KeyStateEvt *pte = Q_NEW(KeyStateEvt, KEY_STATE_SIG);
    ASSERT(pte);

    pte->keyId = keyId;
    pte->keyEvent = evt;
#ifdef HAS_PARAM_KEY
    pte->param = 0;
#endif
    QF_PUBLISH(&pte->super, 0);
}

static void KeySrv_CreateKey(cKeySrv * keySrv)
{
    const tKeyboard *pKeyboard;
    uint8 i = 0;
    uint8 j = 0;
    uint8 arrrayPosition = 0;
    uint16 index = 0xFFFF;
    eDeviceType prevDevType = INVALID_DEV_TYPE;

    if(keySrv->isReady)
    {
        /* if all key has been  created, simply return*/
        return;
    }
    tKeyboardDevice * pKeyboardConfig;
    for(i = 0; i < (keySrv->pConfig->keyboardNum); i++)
    {
        /* create all key objs */
        pKeyboard = &(keySrv->pConfig->keyboardSet[i]);

        //TODO: use index in getDevicebyIdAndType

        if (pKeyboard->deviceType == prevDevType)
        {
            index++;
        }
        else
        {
            index = 0;
        }

        pKeyboardConfig = (tKeyboardDevice*) getDevicebyIdAndType(KEYBOARD_DEV_ID,pKeyboard->deviceType, &index);
        prevDevType = pKeyboard->deviceType;

        ASSERT(pKeyboardConfig);
        KeyDrv_Ctor((pKeyboard->pKeySet), pKeyboardConfig, pKeyboard->pKeyboardAttachedObj);

        /* put all key pointers to pKeyObj array */
        uint8 *pObjBaseAddress;
        pObjBaseAddress = (uint8*)(&(pKeyboard->pKeySet[0]));
        cKeyDrv *pKeyObj;
        for(j = 0; j < pKeyboardConfig->keyNum; j++)
        {
            pKeyObj = (cKeyDrv*)pObjBaseAddress;
            keySrv->pKeyObj[arrrayPosition] = pKeyObj;
            arrrayPosition++;
            pObjBaseAddress += pKeyboard->sizeOfKeyType;
        }
    }
#ifdef HAS_COMB_KEY
    keySrv->numOfKeyDown = 0;
    KeySrv_CombKeyInit();
#endif
    keySrv->isReady = TRUE;
}

#ifdef HAS_INTERRUPT_WAKE_UP_KEY
static void KeySrv_PostSysGoBackToSleep()
{
    QEvt* pEvt = Q_NEW(QEvt, POWER_MCU_SLEEP_SIG);
    QF_PUBLISH(pEvt, 0);
}
#endif

static void KeySrv_XtorKeyObjs(cKeySrv *const me)
{
    /* distroy all key objects */
    const tKeyboard *pKeyboard;
    uint8 i;
    uint16 attached_device_index = 0;
    for(i = 0; i < (me->pConfig->keyboardNum); i++)
    {
        tKeyboardDevice * pKeyboardConfig;
        pKeyboardConfig = (tKeyboardDevice*) getDevicebyId(KEYBOARD_DEV_ID,&attached_device_index);
        attached_device_index++;
        pKeyboard = &(me->pConfig->keyboardSet[i]);
        KeyDrv_Xtor((pKeyboard->pKeySet),pKeyboardConfig);
    }
    me->isReady = FALSE;
}

#ifdef HAS_COMB_KEY
static void KeySrv_CombKeyInit()
{
    uint8 i = 0;
    uint8 j = 0;
    for(i = 0; i < ArraySize(combKeysDef); i++)
    {
        for(j = 0; j < combKeysDef[i].numOfKeyElem; j++)
        {
            combKeysDef[i].pCombKeyElems[j].matchFlag = COMB_KEY_FLAG_RESET;
        }
        combKeysDef[i].timeElapse = 0;
    }
}

static void KeySrv_ResetCombKeyFlag(tCombDef *pCombKey)
{
    uint8 i = 0;
    for(i = 0; i < pCombKey->numOfKeyElem; i++)
    {
        pCombKey->pCombKeyElems[i].matchFlag = FALSE;
    }
}

static void KeySrv_AnalyseCombKeyEvt(cKeySrv *const me, cKeyDrv *pKeyObj,\
                                        tEvtSendCtr *pKeyEvtCtr)
{
    uint8 i = 0;
    uint8 j = 0;
    for(i = 0; i < ArraySize(combKeysDef); i++)
    {
        BOOL keyIdMatched = FALSE;
        for(j = 0; j < combKeysDef[i].numOfKeyElem; j++)
        {
            if(SEQ_KEY_PRESS_COMB_TYPE == combKeysDef[i].combKeyType)
            {
                if(pKeyObj->keyID == combKeysDef[i].pCombKeyElems[j].keyId)
                {
                    keyIdMatched = TRUE;
                    /* check if time elapse more that the time defined. */
                    if(combKeysDef[i].timeElapse >= combKeysDef[i].resetTime)
                    {
                        KeySrv_ResetCombKeyFlag(&combKeysDef[i]);
                        combKeysDef[i].timeElapse = 0;
                    }
                    if(combKeysDef[i].pCombKeyElems[j].matchFlag == TRUE)
                    {
                        continue;
                    }
                    /* update the matchFlag */
                    if(j == 0 && (pKeyEvtCtr->currEvtState >= KEY_EVT_UP))
                    {
                        /* hit the first key*/
                        combKeysDef[i].pCombKeyElems[j].matchFlag = TRUE;
                        /* start the timer */
                        combKeysDef[i].timeElapse = KEY_SERVER_TICK_MS;
                    }
                    else if((pKeyEvtCtr->currEvtState >= KEY_EVT_UP)
                            &&(combKeysDef[i].pCombKeyElems[j-1].matchFlag))
                    {
                        /* key matched, not the first one */
                        combKeysDef[i].pCombKeyElems[j].matchFlag = TRUE;
                        /* reset timer */
                        combKeysDef[i].timeElapse = KEY_SERVER_TICK_MS;
                    }
                    /* The last key is matched, it is time to send an EVT */
                    if(j == (combKeysDef[i].numOfKeyElem - 1) && combKeysDef[i].pCombKeyElems[j].matchFlag)
                    {
                        KeySrv_ResetCombKeyFlag(&combKeysDef[i]);
                        combKeysDef[i].timeElapse = 0;
                        KeySrv_SendCombKeyEvt(combKeysDef[i].combKeyId);
                    }
                    else if(j < (combKeysDef[i].numOfKeyElem - 1))
                    {
                        break;
                    }
                }
            }
            else if(SEQ_KEY_SHIFT_COMB_TYPE == combKeysDef[i].combKeyType)
            {
                if(pKeyObj->keyID == combKeysDef[i].pCombKeyElems[j].keyId)
                {
                    /* update the matchFlag */
                    if(j == 0 && (pKeyEvtCtr->currEvtState == combKeysDef[i].pCombKeyElems[j].combKeyTrigEvt))
                    {
                        /* hit the first key*/
                        combKeysDef[i].pCombKeyElems[j].matchFlag = TRUE;
                    }
                    else if(j == 0 && (pKeyEvtCtr->currEvtState >= KEY_EVT_UP))
                    {
                        /* Any press of the first key will reset the comb Key */
                        KeySrv_ResetCombKeyFlag(&combKeysDef[i]);
                    }
                    else if(combKeysDef[i].pCombKeyElems[j-1].matchFlag &&
                            (pKeyEvtCtr->currEvtState == combKeysDef[i].pCombKeyElems[j].combKeyTrigEvt))
                    {
                        /* key matched, not the first one */
                        combKeysDef[i].pCombKeyElems[j].matchFlag = TRUE;
                    }
                    else if(combKeysDef[i].pCombKeyElems[j-1].matchFlag &&
                            (pKeyEvtCtr->currEvtState >= KEY_EVT_UP))
                    {
                        if(combKeysDef[i].pCombKeyElems[j].combKeyTrigEvt < KEY_EVT_UP)
                        {
                            /*
                             * Got a key up evt, if the trigEvt is defined hold
                             * It is time to reset the flag since the key is released
                             */
                            combKeysDef[i].pCombKeyElems[j].matchFlag = FALSE;
                        }
                    }

                    /* The last key is matched, it is time to send an EVT */
                    if(j == (combKeysDef[i].numOfKeyElem - 1) && combKeysDef[i].pCombKeyElems[j].matchFlag)
                    {
                        KeySrv_SendCombKeyEvt(combKeysDef[i].combKeyId);
                        /* Comb key evt has been sent, reset the flag */
                        combKeysDef[i].pCombKeyElems[j].matchFlag = FALSE;
                    }
                }
            }
            else if (NORMAL_KEY_COMB_TYPE == combKeysDef[i].combKeyType)
            {
                if(pKeyObj->keyID == combKeysDef[i].pCombKeyElems[j].keyId)
                {
                    if(pKeyEvtCtr->currEvtState == combKeysDef[i].pCombKeyElems[j].combKeyTrigEvt)
                    {
                        combKeysDef[i].pCombKeyElems[j].matchFlag = TRUE;
                    }
                    else if(pKeyEvtCtr->currEvtState >= KEY_EVT_UP)
                    {
                        /*
                         * This if condition means key EVT could be
                         * KEY_EVT_UP, KEY_EVT_SHORT_PRESS,
                         * KEY_EVT_LONG_PRESS, KEY_EVT_VERY_LONG_PRESS,
                         */
                        combKeysDef[i].pCombKeyElems[j].matchFlag = FALSE;
                    }
#ifdef HAS_NOT_STRICT_NORMAL_COMBO
                    if(KeySrv_IsNormCombKey(&combKeysDef[i]))
#else
                    if(KeySrv_IsNormCombKey(&combKeysDef[i])&& (me->numOfKeyDown == combKeysDef[i].numOfKeyElem))
#endif
                    {
                        /*
                         * the condition (me->numOfKeyDown == pCombKeys->numOfKeyElem)
                         * is for a special case: comb key is composed
                         * of two keys A and B, if we have A down, C down,then B down,
                         * without this condition will post a comb key EVT
                         */
                        KeySrv_SendCombKeyEvt(combKeysDef[i].combKeyId);
                    }
                }
            }
        }
        if(!keyIdMatched && combKeysDef[i].combKeyType == SEQ_KEY_PRESS_COMB_TYPE)
        {
            /* means user pressed a wrong key */
            KeySrv_ResetCombKeyFlag(&combKeysDef[i]);
            combKeysDef[i].timeElapse = 0;
        }
    }
}
static BOOL KeySrv_IsNormCombKey(tCombDef *pCombKeys)
{
    uint8 i = 0;
    BOOL truthValue = TRUE;
    for(i = 0; i < pCombKeys->numOfKeyElem; i++)
    {
        truthValue &= pCombKeys->pCombKeyElems[i].matchFlag;
    }
    return truthValue;
}
static void KeySrv_RefreshCombKeyTimer()
{
    uint8 i = 0;
    for(i = 0; i < ArraySize(combKeysDef); i++)
    {
        if(combKeysDef[i].combKeyType != NORMAL_KEY_COMB_TYPE && combKeysDef[i].timeElapse != 0)
        {
            combKeysDef[i].timeElapse += (currentTime - previousTime);
        }
    }
}
static void KeySrv_SendCombKeyEvt(eKeyID keyId)
{
    KeyStateEvt *pte = Q_NEW(KeyStateEvt, KEY_STATE_SIG);
    ASSERT(pte);
    pte->keyId = keyId;
    pte->keyEvent = COMB_KEY_EVT;

    QF_PUBLISH(&pte->super, 0);
}
#endif

#ifdef HAS_LINEAR_ADC_KNOB_KEY
void KeySrv_ResetKnobKeyIndex(void)
{
    cLinearKnobKeyDrv *p_key_drv;
    uint32_t i;

    p_key_drv = (cLinearKnobKeyDrv *)knobKeySet;
    for(i=0; i<NUM_OF_LINEAR_KNOB_KEY; i++)
    {
        p_key_drv->preIndex = 111;  // same as STARTUP_PRE_INDEX
        p_key_drv ++;
    }
}
#endif
