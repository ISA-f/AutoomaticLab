/* --- circularBuf.c -------------------------------------------------------------------------------------- **
 *  Ğåàëèçàöèÿ êîëüöåâîãî áóôåğà.
 *  ÂÍÈÌÀÍÈÅ! Ğåàëèçàöèÿ áóôåğà òğåáóåò êğàòíîñòè ğàçìåğà ñòåïåíè 2!
 * 
 * -------------------------------------------------------------------------------------------------------- */

#include "circularBuf.h"

/************************************************************************************************
 *																                                *								
 *                                    FUNCTIONS DEFINITIONS      		    				    *
 *																								*																							
 ***********************************************************************************************/

/* Èíèöèàëèçàöèÿ áóôåğà.
 *
 */
void CIRBUF_Init(const CIRBUF_IndexType _dataSize, CIRBUF_Param* _pParam)

{
    _pParam->head = _pParam->tail = 0;
    _pParam->sizeMask = _dataSize - 1;
    _pParam->overflowMask = ~_pParam->sizeMask;
}

/* Î÷èñòêà áóôåğà
 *
 */
void CIRBUF_Flush(CIRBUF_Param* _pParam)
{
    CIRBUF_ENTER_CRITICAL_SECTION(_pParam);
    _pParam->head = _pParam->tail = 0;
    CIRBUF_LEAVE_CRITICAL_SECTION(_pParam);
}

/* Ïîëó÷åíèå êîëè÷åñòâà ñâîáîäíûõ ıëåìåíòîâ.
 * Ğàñõîæäåíèå ñòàğøèõ íå ìàñêèğóåìûõ áèòîâ èíäåêñîâ
 * ãîëîâû è õâîñòà óêàçûâàåò íà ôàêò/âîçìîæíîñòü ïîòåğè äàííûõ.
 * Åñëè ğàñõîæäåíèå çàôèêñèğîâàííî è èíäåêñ õâîñòà >= èíäåêñà
 * ãîëîâû - ğàçìåğ ñâîáîäíîãî ìåñòà ğàâåí 0.
 */
CIRBUF_IndexType CIRBUF_GetEmptySize(CIRBUF_Param* _pParam)
{
    CIRBUF_IndexType diff; 

    CIRBUF_ENTER_CRITICAL_SECTION(_pParam);
    diff = CIRBUF_CALCULATE_HEAD_TO_TAIL_DIFF(_pParam);
    CIRBUF_LEAVE_CRITICAL_SECTION(_pParam);

    if (diff & _pParam->overflowMask)
        return (0);
    else
        return (_pParam->sizeMask - diff + 1);
}

/* Ïîëó÷åíèå êîëè÷åñòâà ıëåìåíòîâ â áóôåğå.
 *
 */
CIRBUF_IndexType CIRBUF_GetDataSize(CIRBUF_Param* _pParam)
{
    CIRBUF_IndexType diff;

    CIRBUF_ENTER_CRITICAL_SECTION(_pParam);
    diff = CIRBUF_CALCULATE_HEAD_TO_TAIL_DIFF(_pParam);
    CIRBUF_LEAVE_CRITICAL_SECTION(_pParam);

    if (diff & _pParam->overflowMask)
        return (_pParam->sizeMask + 1);
    else 
        return (diff);
}

/* Ïîëó÷åíèå ñòàòóñà áóôåğà.
 * Âîçâğàùàåò:
 * - CIRBUF_BUFFER_EMPTY - åñëè áóôåğ ïóñò;
 * - CIRBUF_DATA_LOST - åñëè äîáàâëÿëñÿ ıëåìåíò íà ìåñòî íåïğî÷èòàííîãî;
 * - CIRBUF_BUFFER_FULL - åñëè áóôåğ ïîëîí;
 * - CIRBUF_NO_ERROR - âî âñåõ îñòàëüíûõ ñëó÷àÿõ.
 */
CIRBUF_Error CIRBUF_GetStatus(CIRBUF_Param* _pParam)
{
    CIRBUF_IndexType diff; 

    CIRBUF_ENTER_CRITICAL_SECTION(_pParam); 
    diff = CIRBUF_CALCULATE_HEAD_TO_TAIL_DIFF(_pParam);
    CIRBUF_LEAVE_CRITICAL_SECTION(_pParam);
        
    if (0 == diff)
        return (CIRBUF_BUFFER_EMPTY);
    else if ((diff - 1) & _pParam->overflowMask)
        return (CIRBUF_DATA_LOST);
    else if (diff & _pParam->sizeMask)
        return (CIRBUF_NO_ERROR);
    else
        return (CIRBUF_BUFFER_FULL);
}

/* Ïîëó÷åíèå êîëè÷åñòâà ïîìåùåííûõ ıëåìåíòîâ.
 *
 */
CIRBUF_IndexType CIRBUF_GetInDataTotal(CIRBUF_Param* _pParam)
{
    return (_pParam->tail);
}

/* Ïîëó÷åíèå êîëè÷åñòâà èçâëå÷åííûõ ıëåìåíòîâ.
 *
 */
CIRBUF_IndexType CIRBUF_GetOutDataTotal(CIRBUF_Param* _pParam)
{
    return (_pParam->head);
}



