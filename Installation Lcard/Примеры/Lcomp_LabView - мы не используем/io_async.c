/* CIN source file */

#define LABVIEW_FW

#include "extcode.h"
#include <windows.h>
#include "ioctl.h"
#include "wlcomp.h"


/* Typedefs */

typedef struct {
   uInt32 elt1;
   uInt32 elt2;
   uInt32 elt3;
   uInt32 elt4;
   float64 elt5;
   uInt32 elt6;
   uInt32 elt7;
   uInt32 elt8;
   uInt32 elt9;
   uInt32 elt10;
   uInt32 elt11;
   uInt32 elt12;
   } TD1;

MgErr CINRun(uInt32 *hIfc, TD1 *arg1, uInt32 *arg2);

MgErr CINRun(uInt32 *hIfc, TD1 *arg1, uInt32 *arg2)
{

   /* Insert code here */
   WASYNC_PAR ap;

   ap.s_Type = arg1->elt1;
   ap.FIFO = arg1->elt2;
   ap.IrqStep = arg1->elt3;
   ap.Pages = arg1->elt4;

   ap.dRate = arg1->elt5;
   ap.Rate = arg1->elt6;
   ap.NCh = arg1->elt7;
   ap.Chn[0] = arg1->elt8;
   ap.Data[0] = arg1->elt9;
   ap.Chn[1] = arg1->elt10;
   ap.Data[1] = arg1->elt11;

   ap.Mode = arg1->elt12;
   
   *arg2= IoAsync(hIfc ,&ap);

   arg1->elt1 = ap.s_Type;
   arg1->elt2 = ap.FIFO;
   arg1->elt3 = ap.IrqStep;
   arg1->elt4 = ap.Pages;

   arg1->elt5 = ap.dRate;
   arg1->elt6 = ap.Rate;
   arg1->elt7 = ap.NCh;
   arg1->elt8 = ap.Chn[0];
   arg1->elt9 = ap.Data[0];
   arg1->elt10 = ap.Chn[1];
   arg1->elt11 = ap.Data[1];
   arg1->elt12= ap.Mode;

   return noErr;
}