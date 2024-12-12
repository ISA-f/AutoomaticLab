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
   uInt32 arg1;
   uInt32 arg2;
   uInt32 arg3;
   uInt32 arg4;
   uInt32 arg5;
   uInt32 arg6;
   uInt32 arg7;
   uInt32 arg8;
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
   uInt32 arg19;
   uInt32 arg20;
   } TD1;


//CIN MgErr CINRun(uInt32 *var1, TD1 *var2, uInt32 *Err);

//CIN MgErr CINRun(uInt32 *var1, TD1 *var2, uInt32 *Err) {
CIN MgErr CINRun(uInt32 *var1, TD1 *var2, int32 *Err);

CIN MgErr CINRun(uInt32 *var1, TD1 *var2, int32 *Err)
{


   /* ENTER YOUR CODE HERE */
   SLOT_PAR sl;
   *Err = GetSlotParam(var1,&sl);
   var2->arg1= sl.Base;
   var2->arg2= sl.BaseL;
   var2->arg3= sl.Base1;
   var2->arg4= sl.BaseL1;
   var2->arg5= sl.Mem;
   var2->arg6= sl.MemL;
   var2->arg7= sl.Mem1;
   var2->arg8= sl.MemL1;
   var2->arg9= sl.Irq;
   var2->arg10= sl.BoardType;
   var2->arg11= sl.DSPType;
   var2->arg12= sl.Dma;
   var2->arg13= sl.DmaDac;
   var2->arg14= sl.DTA_REG;
   var2->arg15= sl.IDMA_REG;
   var2->arg16= sl.CMD_REG;
   var2->arg17= sl.IRQ_RST;
   var2->arg18= sl.DTA_ARRAY;
   var2->arg19= sl.RDY_REG;
   var2->arg20= sl.CFG_REG;
   
   return noErr;
}
