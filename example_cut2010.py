#! /usr/bin/env python
# -*- coding: utf-8 -*-

import logging
import time

from lcomp.lcomp import LCOMP
from lcomp.ldevioctl import (E140, E154, E440, E2010, E2010B, L791, L_ADC_PARAM,
                             L_ASYNC_ADC_INP, L_ASYNC_DAC_OUT, L_ASYNC_TTL_CFG,
                             L_ASYNC_TTL_INP, L_ASYNC_TTL_OUT, L_EVENT_ADC_BUF,
                             L_STREAM_ADC, L_USER_BASE, WASYNC_PAR, WDAQ_PAR)
from lcomp.device import e140, e154, e440, e2010, l791

logging.basicConfig(level=logging.INFO)

from scipy.stats import linregress
from numpy import arange

if __name__ == "__main__":
  #  with LCOMP(slot=0) as ldev:     # либо ldev.OpenLDevice() в начале и ldev.CloseLDevice() в конце
        ldev = LCOMP(slot = 0)
        ldev.OpenLDevice()
        print("Bios?")
        a = input()
        if(a=="1"):
                print("LoadBios: {}".format(ldev.LoadBios("E440")))
                # для E2010 биос "e2010", для E2010B биос "e2010m",для E440 биос "E440"
        print("PlataTest: {}".format(ldev.PlataTest()))

        slPar = ldev.GetSlotParam()
        print("GetSlotParam: {}".format(slPar))
        print("    Base:      {}".format(slPar.Base))
        print("    BaseL:     {}".format(slPar.BaseL))
        print("    Base1:     {}".format(slPar.Base1))
        print("    BaseL1:    {}".format(slPar.BaseL1))
        print("    Mem:       {}".format(slPar.Mem))
        print("    MemL:      {}".format(slPar.MemL))
        print("    Mem1:      {}".format(slPar.Mem1))
        print("    MemL1:     {}".format(slPar.MemL1))
        print("    Irq:       {}".format(slPar.Irq))
        print("    BoardType: {}".format(slPar.BoardType))
        print("    DSPType:   {}".format(slPar.DSPType))
        print("    Dma:       {}".format(slPar.Dma))
        print("    DmaDac:    {}".format(slPar.DmaDac))
        print("    DTA_REG:   {}".format(slPar.DTA_REG))
        print("    IDMA_REG:  {}".format(slPar.IDMA_REG))
        print("    CMD_REG:   {}".format(slPar.CMD_REG))
        print("    IRQ_RST:   {}".format(slPar.IRQ_RST))
        print("    DTA_ARRAY: {}".format(slPar.DTA_ARRAY))
        print("    RDY_REG:   {}".format(slPar.RDY_REG))
        print("    CFG_REG:   {}".format(slPar.CFG_REG))

        plDescr = ldev.ReadPlataDescr()
        print("ReadPlataDescr: {}".format(plDescr))

        if slPar.BoardType in (E2010, E2010B):
            print("    SerNum:       {}".format(plDescr.t6.SerNum))
            print("    BrdName:      {}".format(plDescr.t6.BrdName))
            print("    Rev:          {}".format(plDescr.t6.Rev))
            print("    DspType:      {}".format(plDescr.t6.DspType))
            print("    IsDacPresent: {}".format(bool(plDescr.t6.IsDacPresent)))
            print("    Quartz:       {}".format(plDescr.t6.Quartz))
            # print("    KoefADC:      {}".format(list(plDescr.t6.KoefADC)))
            # print("    KoefDAC:      {}".format(list(plDescr.t6.KoefDAC)))

        # Потоковый вывод с АЦП

        buffer_size = ldev.RequestBufferStream(size=131072, stream_id=L_STREAM_ADC)    # Желательно, чтобы размер буфера был
        print("RequestBufferStream: {}".format(buffer_size))                           # кратен числу используемых каналов NCh

        if slPar.BoardType in (E2010, E2010B):
            adcPar = WDAQ_PAR()

            adcPar.t4.s_Type = L_ADC_PARAM
            adcPar.t4.FIFO = 4096
            adcPar.t4.IrqStep = 4096
            adcPar.t4.Pages = 32
            adcPar.t4.AutoInit = 1
            adcPar.t4.dRate = 1000.0
            adcPar.t4.dKadr = 0.001
            adcPar.t4.SynchroType = e2010.INT_START_TRANS
            adcPar.t4.SynchroSrc = e2010.INT_CLK_TRANS
            adcPar.t4.AdcIMask = e2010.SIG_0 | e2010.V30_0  # | e2010.SIG_1 | e2010.V10_1 # | e2010.SIG_2 | e2010.V03_2 # | e2010.GND_3
            adcPar.t4.NCh = 1   # 1 - 4
            adcPar.t4.Chn[0] = e2010.CH_0
            # adcPar.t4.Chn[1] = e2010.CH_1
            # adcPar.t4.Chn[2] = e2010.CH_2
            # adcPar.t4.Chn[3] = e2010.CH_3
            adcPar.t4.IrqEna = 1
            adcPar.t4.AdcEna = 1

            print("FillDAQparameters: {}".format(ldev.FillDAQparameters(adcPar.t4)))
            print("    s_Type:      {}".format(adcPar.t4.s_Type))
            print("    FIFO:        {}".format(adcPar.t4.FIFO))
            print("    IrqStep:     {}".format(adcPar.t4.IrqStep))
            print("    Pages:       {}".format(adcPar.t4.Pages))
            print("    AutoInit:    {}".format(adcPar.t4.AutoInit))
            print("    dRate:       {}".format(adcPar.t4.dRate))
            print("    dKadr:       {}".format(adcPar.t4.dKadr))
            print("    Reserved1:   {}".format(adcPar.t4.Reserved1))
            print("    DigRate:     {}".format(adcPar.t4.DigRate))
            print("    DM_Ena:      {}".format(adcPar.t4.DM_Ena))
            print("    Rate:        {}".format(adcPar.t4.Rate))
            print("    Kadr:        {}".format(adcPar.t4.Kadr))
            print("    StartCnt:    {}".format(adcPar.t4.StartCnt))
            print("    StopCnt:     {}".format(adcPar.t4.StopCnt))
            print("    SynchroType: {}".format(adcPar.t4.SynchroType))
            print("    SynchroMode: {}".format(adcPar.t4.SynchroMode))
            print("    AdPorog:     {}".format(adcPar.t4.AdPorog))
            print("    SynchroSrc:  {}".format(adcPar.t4.SynchroSrc))
            print("    AdcIMask:    {}".format(adcPar.t4.AdcIMask))
            print("    NCh:         {}".format(adcPar.t4.NCh))
            # print("    Chn:         {}".format(list(adcPar.t4.Chn)))
            print("    IrqEna:      {}".format(adcPar.t4.IrqEna))
            print("    AdcEna:      {}".format(adcPar.t4.AdcEna))

            data_ptr, syncd = ldev.SetParametersStream(adcPar.t4, buffer_size)
            print("SetParametersStream: {}, {}".format(data_ptr, syncd))
            print("    Pages:   {}".format(adcPar.t4.Pages))
            print("    IrqStep: {}".format(adcPar.t4.IrqStep))
            print("    FIFO:    {}".format(adcPar.t4.FIFO))
            print("    Rate:    {}".format(adcPar.t4.dRate))

        print("EnableCorrection: {}".format(ldev.EnableCorrection(True)))

        print("InitStartLDevice: {}".format(ldev.InitStartLDevice()))
        print("StartLDevice: {}".format(ldev.StartLDevice()))

        print("Read data from buffer ...")

        Linterface = ldev.Get_LDEV2_Interface()
        print(Linterface)

        
        print("Buffer fulfillment start time: ", time.time())

        while syncd() < buffer_size:            # ждем, пока заполнится буфер
                pass
        print("Buffer fulfillment finish time: ", time.time())

        print("Data ready ...")
        print(type(syncd))
        if slPar.BoardType in (E2010, E2010B):
            x = e2010.GetDataADC(adcPar.t4, plDescr, data_ptr, buffer_size)

        print("x: ", type(x), syncd())
        
        start = time.time()
        x[0].tofile("channel-1-e440v2.log", sep="\n") # индекс соответствует номеру канала из Chn
        end = time.time()
        print("Time ToFile: ", end - start)

        start = time.time()
        s = x[0].mean()
        end = time.time()
        print("Mean: ", s)
        print("Mean time: ", end - start)

        start = time.time()
        d = x[0].var()
        end = time.time()
        print("Var: ", d)
        print("Var time: ", end - start)

        start = time.time()
        slope = linregress(arange(len(x[0])), x[0])
        end = time.time()
        print("Slope: ", slope)
        print("Slope time: ", end - start)
        # x[1].tofile("channel-2.log", sep="\n")
        # x[2].tofile("channel-3.log", sep="\n")

        print("StopLDevice: {}".format(ldev.StopLDevice()))

        
        # Асинхронные операции ввода/вывода

        asp = WASYNC_PAR()

        asp.s_Type = L_ASYNC_DAC_OUT
        asp.Mode = 0
        asp.Data[0] = 512
        if ldev.IoAsync(asp):
            print("IoAsync (L_ASYNC_DAC_OUT): {}".format(asp.Data[0]))

        asp.s_Type = L_ASYNC_ADC_INP
        asp.Chn[0] = 0x00
        if ldev.IoAsync(asp):
            print("IoAsync (L_ASYNC_ADC_INP): {}".format(asp.Data[0]))

        asp.s_Type = L_ASYNC_TTL_CFG
        asp.Mode = 1
        if ldev.IoAsync(asp):
            print("IoAsync (L_ASYNC_TTL_CFG): {}".format(asp.Data[0]))

        asp.s_Type = L_ASYNC_TTL_INP
        asp.Chn[0] = 0x00
        if ldev.IoAsync(asp):
            print("IoAsync (L_ASYNC_TTL_INP): {}".format(asp.Data[0]))

        asp.s_Type = L_ASYNC_TTL_OUT
        asp.Data[0] = 0xA525
        if ldev.IoAsync(asp):
            print("IoAsync (L_ASYNC_TTL_OUT): {}".format(asp.Data[0]))
        print(type(asp))
        
        # Проверка работоспособности других функций

        print("SetParameter (L_USER_BASE): {}".format(ldev.SetParameter(name=L_USER_BASE, value=123)))
        print("GetParameter (L_USER_BASE): {}".format(ldev.GetParameter(name=L_USER_BASE)))
        print("ReadFlashWord: {}".format(ldev.ReadFlashWord(address=L_USER_BASE)))
        print("EnableFlashWrite: {}".format(ldev.EnableFlashWrite(False)))
        print("SendCommand: {}".format(ldev.SendCommand(cmd=0)))
        print("SetLDeviceEvent: {}".format(ldev.SetLDeviceEvent(event=0, event_id=L_EVENT_ADC_BUF)))

        print("GetWord_DM: {}".format(ldev.GetWord_DM(address=0x0400)))
        print("GetWord_PM: {}".format(ldev.GetWord_PM(address=0)))
        print("GetArray_DM: {}".format(ldev.GetArray_DM(address=0x1080, count=2)))
        print("GetArray_PM: {}".format(ldev.GetArray_PM(address=0, count=2)))

        print("inbyte: {}".format(ldev.inbyte(offset=0)))
        print("inword: {}".format(ldev.inword(offset=0)))
        print("indword: {}".format(ldev.indword(offset=0)))
        print("inmbyte: {}".format(ldev.inmbyte(offset=0)))
        print("inmword: {}".format(ldev.inmword(offset=0)))
        print("inmdword: {}".format(ldev.inmdword(offset=0)))

        print("Get_LDEV2_Interface: {}".format(ldev.Get_LDEV2_Interface()))
        print("InitStartLDeviceEx: {}".format(ldev.InitStartLDeviceEx(stream_id=L_STREAM_ADC)))
        print("StartLDeviceEx: {}".format(ldev.StartLDeviceEx(stream_id=L_STREAM_ADC)))
        print("StopLDeviceEx: {}".format(ldev.StopLDeviceEx(stream_id=L_STREAM_ADC)))
        print("Release_LDEV2_Interface: {}".format(ldev.Release_LDEV2_Interface()))
        
        ldev.CloseLDevice()
