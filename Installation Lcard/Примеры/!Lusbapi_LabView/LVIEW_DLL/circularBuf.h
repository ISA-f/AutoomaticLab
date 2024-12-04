/* --- circularBuf.h -------------------------------------------------------------------------------------- **
 *  Реализация кольцевого буфера.
 *  ВНИМАНИЕ! Реализация буфера требует чтобы его размер был кратен степени 2!
 * 
 * -------------------------------------------------------------------------------------------------------- */
#ifndef CIRBUF_H_
#define	CIRBUF_H_

#include <stddef.h>
#include <limits.h>
#include "circularBufCfg.h"

/************************************************************************************************
 *																                                *								
 *                                    TYPES DECLARATIONS       			    				    *
 *																								*																							
 ***********************************************************************************************/

typedef enum 
{
    CIRBUF_NO_ERROR,
    CIRBUF_BUFFER_EMPTY,
    CIRBUF_BUFFER_FULL,
    CIRBUF_DATA_LOST,
    CIRBUF_OPERATION_INCOMPLETE
} CIRBUF_Error;

typedef struct
{
    CIRBUF_IndexType head;
    CIRBUF_IndexType tail;
    CIRBUF_IndexType sizeMask;
    CIRBUF_IndexType overflowMask;
    void (*EnterCriticalSection)(void);
    void (*LeaveCriticalSection)(void);
} CIRBUF_Param;


/************************************************************************************************
 *																                                *								
 *                                    FUNCTIONS DECLARATIONS      		    				    *
 *																								*																							
 ***********************************************************************************************/
#ifdef  __cplusplus
extern "C" 
{
void CIRBUF_Init(const CIRBUF_IndexType _dataSize, CIRBUF_Param* _pParam);
CIRBUF_IndexType CIRBUF_GetEmptySize(CIRBUF_Param* _pParam);
CIRBUF_IndexType CIRBUF_GetDataSize(CIRBUF_Param* _pParam);
CIRBUF_Error CIRBUF_GetStatus(CIRBUF_Param* _pParam);
CIRBUF_IndexType CIRBUF_GetInDataTotal(CIRBUF_Param* _pParam);
CIRBUF_IndexType CIRBUF_GetOutDataTotal(CIRBUF_Param* _pParam);
void CIRBUF_Flush(CIRBUF_Param* _pParam);
}
#endif 

/************************************************************************************************
 *																                                *								
 *                                 SERVICE MACROS DEFINITIONS      		    				    *
 *																								*																							
 ***********************************************************************************************/

/* Вход в критическую секцию.
 *
 */
#define CIRBUF_ENTER_CRITICAL_SECTION(PARAM) if ((PARAM)->EnterCriticalSection != NULL) (PARAM)->EnterCriticalSection()

/* Выход из критической секции.
 *
 */
#define CIRBUF_LEAVE_CRITICAL_SECTION(PARAM) if ((PARAM)->LeaveCriticalSection != NULL) (PARAM)->LeaveCriticalSection()

/* Вычисление модуля разности между головой и хвостом */
#define CIRBUF_CALCULATE_HEAD_TO_TAIL_DIFF(PPARAM)  (PPARAM->tail >= PPARAM->head) ? (PPARAM->tail - PPARAM->head) :\
                                                    ((CIRBUF_IndexType)(CIRBUF_INDEX_MAX - PPARAM->head + PPARAM->tail + 1))


/************************************************************************************************
 *																                                *								
 *                                  DATA MACROS DEFINITIONS      		    				    *
 *																								*																							
 ***********************************************************************************************/

/* Извлечение элемента из головы буфера.
 * Внимание! Наличие данных не проверяется!
 */
#define CIRBUF_POP_FRONT(BUF, PARAM, DATA)\
{\
    CIRBUF_ENTER_CRITICAL_SECTION(&PARAM);\
    DATA = BUF[(PARAM.head++) & PARAM.sizeMask];\
    CIRBUF_LEAVE_CRITICAL_SECTION(&PARAM);\
}

/* Извлечение элемента из хвоста буфера.
 * Внимание! Наличие данных не проверяется!
 */
#define CIRBUF_POP_BACK(BUF, PARAM, DATA)\
{\
    CIRBUF_ENTER_CRITICAL_SECTION(&PARAM);\
    DATA = BUF[(--PARAM.tail) & PARAM.sizeMask];\
    CIRBUF_LEAVE_CRITICAL_SECTION(&PARAM);\
}

/* Помещение элемента в голову буфера.
 * Внимание! Переполнение буфера не проверяется!
 */
#define CIRBUF_PUSH_FRONT(BUF, PARAM, DATA)\
{\
    CIRBUF_ENTER_CRITICAL_SECTION(&PARAM);\
    BUF[(--PARAM.head) & PARAM.sizeMask] = DATA;\
    CIRBUF_LEAVE_CRITICAL_SECTION(&PARAM);\
}

/* Помещение элемента в хвост буфера.
 * Внимание! Переполнение буфера не проверяется!
 */
#define CIRBUF_PUSH_BACK(BUF, PARAM, DATA)\
{\
   CIRBUF_ENTER_CRITICAL_SECTION(&PARAM);\
   BUF[(PARAM.tail++) & PARAM.sizeMask] = DATA;\
   CIRBUF_LEAVE_CRITICAL_SECTION(&PARAM);\
}

/* Помещение множества элементов в хвост буфера.
 * Внимание! Переполнение буфера не проверяется!
 */
#define CIRBUF_PUSH_MULTIPLE_BACK(BUF, PARAM, PDATA, QNT)\
{\
    CIRBUF_IndexType idx;\
    CIRBUF_ENTER_CRITICAL_SECTION(&PARAM);\
    for (idx = 0; idx < QNT; idx++)\
        BUF[(PARAM.tail++) & PARAM.sizeMask] = PDATA[idx];\
    CIRBUF_LEAVE_CRITICAL_SECTION(&PARAM);\
}

/* Помещение множества элементов в голову буфера.
 * Внимание! Переполнение буфера не проверяется!
 */
#define CIRBUF_PUSH_MULTIPLE_FRONT(BUF, PARAM, PDATA, QNT)\
{\
    CIRBUF_IndexType idx;\
    CIRBUF_ENTER_CRITICAL_SECTION(&PARAM);\
    for (idx = 0; idx < QNT; idx++)\
        BUF[(--PARAM.head) & PARAM.sizeMask] = PDATA[idx];\
    CIRBUF_LEAVE_CRITICAL_SECTION(&PARAM);\
}

/* Извлечение множества элементов из головы буфера.
 * Внимание! Необходимо следить за тем, чтобы буфер
 * назначения оказался достаточным, чтобы вместить данные.
 */
#define CIRBUF_POP_MULTIPLE_FRONT(BUF, PARAM, PDST, QNT)\
{\
    CIRBUF_IndexType idx = 0;\
    CIRBUF_ENTER_CRITICAL_SECTION(&PARAM);\
    while (PARAM.head != PARAM.tail && idx < QNT)\
    {\
        PDST[idx++] = BUF[(PARAM.head++) & PARAM.sizeMask];\
    }\
    CIRBUF_LEAVE_CRITICAL_SECTION(&PARAM);\
}

/* Извлечение множества элементов из хвоста буфера.
 * Внимание! Необходимо следить за тем, чтобы буфер
 * назначения оказался достаточным, чтобы вместить данные.
 */
#define CIRBUF_POP_MULTIPLE_BACK(BUF, PARAM, PDST, QNT)\
{\
    CIRBUF_IndexType idx = 0;\
    CIRBUF_ENTER_CRITICAL_SECTION(&PARAM);\
    while (PARAM.head != PARAM.tail && idx < QNT)\
    {\
        PDST[idx++] = BUF[(--PARAM.tail) & PARAM.sizeMask];\
    }\
    CIRBUF_LEAVE_CRITICAL_SECTION(&PARAM);\
}

#endif /*#ifndef CIRBUF_H_*/
