/*
 * CIN source file 
 */

#define LABVIEW_FW

#include "extcode.h"
#include <windows.h>
#include "ioctl.h"
#include "wlcomp.h"


/*
 * typedefs
 */

typedef struct {
   int32 dimSize;
   uInt32 arg1[1];
   } TD2;
typedef TD2 **TD2Hdl;

typedef struct {
   uInt32 arg1;
   uInt32 arg2;
   uInt32 arg3;
   uInt32 arg4;
   uInt32 arg5;
   float64 arg6;
   float64 arg7;
   float64 arg8;
   uInt32 arg9;
   uInt32 arg10;
   uInt32 arg11;
   uInt32 arg12;
   uInt32 arg13;
   uInt32 arg14;
   uInt32 arg15;
   uInt32 arg16;
   uInt32 arg17;
   uInt32 arg18;
   TD2Hdl arg19;
   uInt32 arg20;
   uInt32 arg21;
   } TD1;
/* 
typedef struct W_ADC_PARAM_U_0
{
   USHORT s_Type;
   USHORT FIFO;
   USHORT IrqStep;
   USHORT Pages;

   USHORT AutoInit;

   double dRate;
   double dKadr;
   double dScale;
   USHORT Rate;
   USHORT Kadr;
   USHORT Scale;
   USHORT FPDelay;

   USHORT SynchroType;
   USHORT SynchroSensitivity;
   USHORT SynchroMode;
   USHORT AdChannel;
   USHORT AdPorog;
   USHORT NCh;
   USHORT Chn[128];
   USHORT IrqEna;
   USHORT AdcEna;
} WADC_PAR_0, *PWADC_PAR_0;
*/ 

CIN MgErr CINRun(uInt32 *var1, TD1 *var2, uInt32 *Err);

CIN MgErr CINRun(uInt32 *var1, TD1 *var2, uInt32 *Err)
{
   /* ENTER YOUR CODE HERE */
   SLOT_PAR sl;
   WDAQ_PAR ap;
   int32 i;
   TD2 *tmp;
   uInt32 sp_type;

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
         switch (var2->arg1)
         {
         case L_DAC_PARAM:
         {
            ap.t1.s_Type = var2->arg1; // L_ADC_PARAM
   
            ap.t1.FIFO = var2->arg2;
            ap.t1.IrqStep = var2->arg3;
            ap.t1.Pages = var2->arg4;
      
            ap.t1.AutoInit = var2->arg5;
            ap.t1.dRate = var2->arg6;

            ap.t1.DacNumber = var2->arg16;

            ap.t1.IrqEna = var2->arg20;
            ap.t1.DacEna = var2->arg21;

            sp_type=0;

            *Err = FillDAQparameters(var1, &ap, sp_type);

            var2->arg6 = ap.t1.dRate;


         } break;
         case L_ADC_PARAM:
         {
            ap.t3.s_Type = var2->arg1; // L_ADC_PARAM
   
            ap.t3.FIFO = var2->arg2;
            ap.t3.IrqStep = var2->arg3;
            ap.t3.Pages = var2->arg4;
      
            ap.t3.AutoInit = var2->arg5;
            ap.t3.dRate = var2->arg6;
            ap.t3.dKadr = var2->arg7;
            ap.t3.dScale = var2->arg8;
   
            ap.t3.SynchroType = var2->arg13;
            ap.t3.SynchroSensitivity = var2->arg14;
            ap.t3.SynchroMode = var2->arg15;
            ap.t3.AdChannel = var2->arg16;
            ap.t3.AdPorog = var2->arg17;
            ap.t3.NCh = var2->arg18;
   
            tmp= *(var2->arg19);
            for(i=0;i<128;i++) ap.t3.Chn[i] = tmp->arg1[i];

            ap.t3.IrqEna = var2->arg20;
            ap.t3.AdcEna = var2->arg21;

            sp_type=2;

            *Err = FillDAQparameters(var1, &ap, sp_type);

            var2->arg6 = ap.t3.dRate;
            var2->arg7 = ap.t3.dKadr;
         }
         }
         
      } break;

      case E2010:
      case E2010B:
      case L791:
      {
         switch (var2->arg1)
         {
         case L_DAC_PARAM:
         {
            ap.t2.s_Type = var2->arg1; // L_ADC_PARAM
   
            ap.t2.FIFO = var2->arg2;
            ap.t2.IrqStep = var2->arg3;
            ap.t2.Pages = var2->arg4;
      
            ap.t2.AutoInit = var2->arg5;
            ap.t2.dRate = var2->arg6;


            ap.t2.IrqEna = var2->arg20;
            ap.t2.DacEna = var2->arg21;

            sp_type=1;

            *Err = FillDAQparameters(var1, &ap, sp_type);

            var2->arg6 = ap.t2.dRate;


         } break;

         case L_ADC_PARAM:
         {
            ap.t4.s_Type = var2->arg1; // L_ADC_PARAM;

            ap.t4.FIFO = var2->arg2;
            ap.t4.IrqStep = var2->arg3;
            ap.t4.Pages = var2->arg4;


            ap.t4.AutoInit = var2->arg5;
            ap.t4.dRate = var2->arg6;
            ap.t4.dKadr = var2->arg7;
   //         ap.t3.dScale = var2->arg8;
   //         ap.t4.DM_Ena = var2->arg9;  !!! тут коллизия с новыми типами данных double - double разбит на 2 int32
 // поэтому датамаркер для 2010Ь не присваиваем....
            ap.t4.DM_Ena = 0;

            ap.t4.StartCnt = var2->arg13;
            ap.t4.StopCnt = var2->arg12;
            ap.t4.SynchroType = var2->arg13;
            ap.t4.SynchroMode = var2->arg14;
            ap.t4.AdPorog = var2->arg15;
            ap.t4.SynchroSrc = var2->arg16;
            ap.t4.AdcIMask = var2->arg17;
         
            ap.t4.NCh = var2->arg18;
            tmp= *(var2->arg19);
            for(i=0;i<128;i++) ap.t4.Chn[i] = tmp->arg1[i];
      
            ap.t4.IrqEna = var2->arg20;
            ap.t4.AdcEna = var2->arg21;

            sp_type=3;

            *Err = FillDAQparameters(var1, &ap, sp_type);

            var2->arg6 = ap.t4.dRate;
            var2->arg7 = ap.t4.dKadr;
         }
         }
      }
   }

   return noErr;
}
