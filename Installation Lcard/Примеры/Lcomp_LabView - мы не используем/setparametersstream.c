/*
 * CIN source file 
 */

#define LABVIEW_FW

#include "extcode.h"
#include <windows.h>
#include "ioctl.h"
#include "wlcomp.h"



CIN MgErr CINRun(uInt32 *var1, uInt32 *var2, uInt32 *var3, int32 *var4,  uInt32 *Size, uInt32 *var7, uInt32 *var8, uInt32 *var9, uInt32 *Err);

CIN MgErr CINRun(uInt32 *var1, uInt32 *var2, uInt32 *var3, int32 *var4,  uInt32 *Size, uInt32 *var7, uInt32 *var8, uInt32 *var9, uInt32 *Err)
{

   /* ENTER YOUR CODE HERE */
   SLOT_PAR sl;
   WDAQ_PAR ap;
   int16 *data;
   uInt32 *sync;

   GetSlotParam(var1,&sl); // получим тип платы

   switch(sl.BoardType)
   {
      case PCIA:
      case PCIB:
      case PCIC:
      case E440:
      case E140:
      case E154:
      {
         switch(*var9)
         {
         case L_STREAM_ADC:
         {
            *Err = SetParametersStream(var1, &ap.t3, 2, Size, (void **)&data, (void **)&sync, *var9);
            *var2 = ap.t3.Pages;
            *var3 = ap.t3.IrqStep;
            *var4 = ap.t3.FIFO;
            *var7 = sync;
            *var8 = data;
         } break;
         case L_STREAM_DAC:
         {
            *Err = SetParametersStream(var1, &ap.t1, 0, Size, (void **)&data, (void **)&sync, *var9);
            *var2 = ap.t1.Pages;
            *var3 = ap.t1.IrqStep;
            *var4 = ap.t1.FIFO;
            *var7 = sync;
            *var8 = data;
         } break;
         }
      } break;

      case E2010B:
      case E2010:
      case L791:
      {
         switch(*var9)
         {
         case L_STREAM_ADC:
         {
            *Err = SetParametersStream(var1, &ap.t4, 3, Size, (void **)&data, (void **)&sync, *var9);
            *var2 = ap.t4.Pages;
            *var3 = ap.t4.IrqStep;
            *var4 = ap.t4.FIFO;
            *var7 = sync;
            *var8 = data;
         } break;
         case L_STREAM_DAC:
         {
            *Err = SetParametersStream(var1, &ap.t2, 1, Size, (void **)&data, (void **)&sync, *var9);
            *var2 = ap.t2.Pages;
            *var3 = ap.t2.IrqStep;
            *var4 = ap.t2.FIFO;
            *var7 = sync;
            *var8 = data;
         } break;
         }
      }
   }
   return noErr;
}
