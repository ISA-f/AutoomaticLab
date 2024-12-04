/* --- circularBufCfg.h ----------------------------------------------------------------------------------- **
 *  Файл конфигурации кольцевого буфера.
 *
 * -------------------------------------------------------------------------------------------------------- */
#ifndef CIRBUFCFG_H_
#define	CIRBUFCFG_H_

#include <windows.h>

/* Тип индексных переменных.
 * Внимание! Тип должен быть беззнаковым!
 */
#define CIRBUF_INDEX_MAX 			ULONG_MAX 
typedef DWORD CIRBUF_IndexType;
  
#endif /*#ifndef CIRBUFCFG_H_*/
