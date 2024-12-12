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
   LStrHandle arg1;
   LStrHandle arg2;
   LStrHandle arg3;
   LStrHandle arg4;
   int32 arg5;
   uInt16 arg6;
   } TD1;

CIN MgErr CINRun(uInt32 *var1, TD1 *var2, uInt32 *Err);

CIN MgErr CINRun(uInt32 *var1, TD1 *var2, uInt32 *Err)
{

   /* ENTER YOUR CODE HERE */
   SLOT_PAR sl;
   PLATA_DESCR_U2 pd;
	MgErr err;
   //PLATA_DESCR pd; // тут в LabView не все параметры передаютс€...вс€кие калибр. коэф. при желании можно самим дописать по аналогии...
   char str[255];

   GetSlotParam(var1,&sl);

   *Err = ReadPlataDescr(var1, &pd);

   switch(sl.BoardType)
   {
      case PCIA:
      case PCIB:
      case PCIC:
      {
			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg1), sizeof(pd.t1.SerNum));
			LStrLen(*(var2->arg1)) = sizeof(pd.t1.SerNum);
         CToPStr(pd.t1.SerNum,str); PToLStr(str,*(var2->arg1));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg2), sizeof(pd.t1.BrdName));
			LStrLen(*(var2->arg2)) = sizeof(pd.t1.BrdName);
         CToPStr(pd.t1.BrdName,str); PToLStr(str,*(var2->arg2));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg3), sizeof(pd.t1.Rev));
			LStrLen(*(var2->arg3)) = sizeof(pd.t1.Rev);
         str[0]=pd.t1.Rev; str[1]=0; CToPStr(str,str); PToLStr(str,*(var2->arg3));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg4), sizeof(pd.t1.DspType));
			LStrLen(*(var2->arg4)) = sizeof(pd.t1.DspType);
         CToPStr(pd.t1.DspType,str); PToLStr(str,*(var2->arg4));

         var2->arg5 = pd.t1.Quartz;

         var2->arg6 = pd.t1.IsDacPresent;
      } break;

      case E140:
      {
			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg1), sizeof(pd.t5.SerNum));
			LStrLen(*(var2->arg1)) = sizeof(pd.t5.SerNum);
         CToPStr(pd.t5.SerNum,str); PToLStr(str,*(var2->arg1));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg2), sizeof(pd.t5.BrdName));
			LStrLen(*(var2->arg2)) = sizeof(pd.t5.BrdName);
         CToPStr(pd.t5.BrdName,str); PToLStr(str,*(var2->arg2));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg3), sizeof(pd.t5.Rev));
			LStrLen(*(var2->arg3)) = sizeof(pd.t5.Rev);
         str[0]=pd.t5.Rev; str[1]=0; CToPStr(str,str); PToLStr(str,*(var2->arg3));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg4), sizeof(pd.t5.DspType));
			LStrLen(*(var2->arg4)) = sizeof(pd.t5.DspType);
         CToPStr(pd.t5.DspType,str); PToLStr(str,*(var2->arg4));

         var2->arg5 = pd.t5.Quartz;

         var2->arg6 = pd.t5.IsDacPresent;

      } break;

      case E154:
      {
			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg1), sizeof(pd.t7.SerNum));
			LStrLen(*(var2->arg1)) = sizeof(pd.t7.SerNum);
         CToPStr(pd.t7.SerNum,str); PToLStr(str,*(var2->arg1));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg2), sizeof(pd.t7.BrdName));
			LStrLen(*(var2->arg2)) = sizeof(pd.t7.BrdName);
         CToPStr(pd.t7.BrdName,str); PToLStr(str,*(var2->arg2));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg3), sizeof(pd.t7.Rev));
			LStrLen(*(var2->arg3)) = sizeof(pd.t7.Rev);
         str[0]=pd.t7.Rev; str[1]=0; CToPStr(str,str); PToLStr(str,*(var2->arg3));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg4), sizeof(pd.t7.DspType));
			LStrLen(*(var2->arg4)) = sizeof(pd.t7.DspType);
         CToPStr(pd.t7.DspType,str); PToLStr(str,*(var2->arg4));

         var2->arg5 = pd.t7.Quartz;
         var2->arg6 = pd.t7.IsDacPresent;
      } break;


      case E440:
      {
			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg1), sizeof(pd.t4.SerNum));
			LStrLen(*(var2->arg1)) = sizeof(pd.t4.SerNum);
         CToPStr(pd.t4.SerNum,str); PToLStr(str,*(var2->arg1));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg2), sizeof(pd.t4.BrdName));
			LStrLen(*(var2->arg2)) = sizeof(pd.t4.BrdName);
         CToPStr(pd.t4.BrdName,str); PToLStr(str,*(var2->arg2));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg3), sizeof(pd.t4.Rev));
			LStrLen(*(var2->arg3)) = sizeof(pd.t4.Rev);
         str[0]=pd.t4.Rev; str[1]=0; CToPStr(str,str); PToLStr(str,*(var2->arg3));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg4), sizeof(pd.t4.DspType));
			LStrLen(*(var2->arg4)) = sizeof(pd.t4.DspType);
         CToPStr(pd.t4.DspType,str); PToLStr(str,*(var2->arg4));

         var2->arg5 = pd.t4.Quartz;

         var2->arg6 = pd.t4.IsDacPresent;
      } break;

      case E2010B:
      case E2010:
      {
			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg1), sizeof(pd.t6.SerNum));
			LStrLen(*(var2->arg1)) = sizeof(pd.t6.SerNum);
         CToPStr(pd.t6.SerNum,str); PToLStr(str,*(var2->arg1));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg2), sizeof(pd.t6.BrdName));
			LStrLen(*(var2->arg2)) = sizeof(pd.t6.BrdName);
         CToPStr(pd.t6.BrdName,str); PToLStr(str,*(var2->arg2));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg3), sizeof(pd.t6.Rev));
			LStrLen(*(var2->arg3)) = sizeof(pd.t6.Rev);
         str[0]=pd.t6.Rev; str[1]=0; CToPStr(str,str); PToLStr(str,*(var2->arg3));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg4), sizeof(pd.t6.DspType));
			LStrLen(*(var2->arg4)) = sizeof(pd.t6.DspType);
         CToPStr(pd.t6.DspType,str); PToLStr(str,*(var2->arg4));

         var2->arg5 = pd.t6.Quartz;

         var2->arg6 = pd.t6.IsDacPresent;
      } break;

      case L791:
      {
			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg1), sizeof(pd.t3.SerNum));
			LStrLen(*(var2->arg1)) = sizeof(pd.t3.SerNum);
         CToPStr(pd.t3.SerNum,str); PToLStr(str,*(var2->arg1));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg2), sizeof(pd.t3.BrdName));
			LStrLen(*(var2->arg2)) = sizeof(pd.t3.BrdName);
         CToPStr(pd.t3.BrdName,str); PToLStr(str,*(var2->arg2));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg3), sizeof(pd.t3.Rev));
			LStrLen(*(var2->arg3)) = sizeof(pd.t3.Rev);
         str[0]=pd.t3.Rev; str[1]=0; CToPStr(str,str); PToLStr(str,*(var2->arg3));

			err = NumericArrayResize(uB, 1L, (UHandle *)&(var2->arg4), sizeof(pd.t3.DspType));
			LStrLen(*(var2->arg4)) = sizeof(pd.t3.DspType);
         CToPStr(pd.t3.DspType,str); PToLStr(str,*(var2->arg4));

         var2->arg5 = pd.t3.Quartz;

         var2->arg6 = pd.t3.IsDacPresent;
      } break;
   }

   return noErr;
}
