/* CIN source file */

#define LABVIEW_FW

#include "extcode.h"
#include <windows.h>
#include "ioctl.h"
#include "791cmd.h"
#include "wlcomp.h"


MgErr CINRun(uInt32 *hIfc, uInt32 *StreamId, uInt32 *Sync);

MgErr CINRun(uInt32 *hIfc, uInt32 *StreamId, uInt32 *Sync)
	{
		ULONG Err;
		ULONG bt;
		ULONG sa;
		ULONG offset = 0;
      Err = GetParameter(hIfc, L_BOARD_TYPE, &bt);
		switch(*StreamId)
		{
		case L_STREAM_ADC:
			{
				if(bt==L791) offset = I_ADC_PCI_COUNT_L791;
				Err = GetParameter(hIfc, L_SYNC_ADDR_LO, &sa);
			} break;
		case L_STREAM_DAC:
			{
				if(bt==L791) offset = I_DAC_PCI_COUNT_L791;
				Err = GetParameter(hIfc, L_SYNC1_ADDR_LO, &sa);
			} break;
		}
		Err = GetSyncData(hIfc,sa,offset,Sync);
	/* Insert code here */

	return noErr;
	}