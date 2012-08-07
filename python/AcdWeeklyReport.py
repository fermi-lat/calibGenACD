#!/usr/bin/env python
#
#                               Copyright 2007
#                                     by
#                        The Board of Trustees of the
#                     Leland Stanford Junior University.
#                            All rights reserved.
#

__facility__ = "calibGenACD"
__abstract__ = "Runs all the calibrations"
__author__    = "E. Charles"
__date__      = "$Date$"
__version__   = "$Revision$, $Author$"
__release__   = "$Name$"

#import LATTE.copyright_SLAC
import os, sys
import time
from optparse import OptionParser

ACDMONROOT = os.path.join(os.getenv("LATMonRoot"),'ACD','FLIGHT')
CALIBGENACD = os.getenv("CALIBGENACDROOT")
#CALIBGENACDBINDIR = os.path.join(CALIBGENACD,os.getenv('CMTCONFIG'))
CALIBGENACDBINDIR = os.path.join(os.getenv('RELEASE'),'bin',os.getenv('SCONS_TOTAL'))


CALIBTYPES = {'ped':('Ped','runPedestal.exe',1,['-P'],'digi'),
              'gain':('ElecGain','runMipCalib.exe',5,['ped'],'svac'),
              'veto':('ThreshVeto','runVetoCalib.exe',1,['ped'],'digi'),
              'range':('Range','runRangeCalib.exe',5,['ped','highPed'],'digi'),
              'cno':('ThreshHigh','runCnoCalib.exe',1,['highPed'],'digi'),
              'coherentNoise':('CoherentNoise','runCoherentNoiseCalib.exe',1,['-P','ped'],'digi'),
              'ribbon':('Ribbon','runRibbonCalib.exe',5,['ped'],'svac')}              
#              'highPed':('HighPed','runHighPed.exe',1,['-s 1000'],'meta'),
#              'carbon':('Carbon','runCarbonCalib.exe',60,['-G 6','highPed'],'svac'),
#              'cnoFit':('CnoFit','runCnoFitCalib.exe',0,[],'meta'),
#              'vetoFit':('VetoFit','runVetoFitCalib.exe',0,[],'meta'),
#              'highRange':('HighRange','runHighRangeCalib.exe',0,['ped','gain','highPed','carbon','range'],'meta'),
#              'check':('Check','runMeritCalib.exe',5,['ped','gain','highRange'],'svac')}

#CALIBTYPES = {'gain':('ElecGain','runMipCalib.exe',5,['ped'],'svac')}
  
if __name__=='__main__':
    # argument parsing

    usage = 'AcdWeeklyReport.py ACTION <options> digiFile svacFile'
    parser = OptionParser(usage)

    parser.add_option("-w", "--week",action="store",
                      dest="week",type="int",default=0,
                      help="Mission Week", metavar="INT")


    if len(sys.argv) == 1 or sys.argv[1] == '-h':
        parser.print_help()
        sys.exit()

    actionList = ['run','trend']
    action = sys.argv[1]    
    if action not in actionList:
        parser.print_help()
        print "Legal values for ACTION are %s"%str(actionList)
        sys.exit()     

    (options, args) = parser.parse_args(sys.argv[2:])

    if action == "run":
        if len(args) <> 2:
            parser.print_help()
            sys.exit()  
            
        if options.week==0:
            parser.print_help()
            sys.exit()  

    execName = os.path.join(CALIBGENACD,"python","AcdReportRun.py")
    trendName = os.path.join(CALIBGENACD,"python","AcdReportTrend.py")
    for cType,cData in CALIBTYPES.items():
        
        if action == "run":
            inputTable = ""
            if cData[4] == 'digi':
                inputTable = args[0]
            elif cData[4] == 'svac':
                inputTable = args[1]
            elif cData[4] == 'meta':
                pass
            else:
                parser.print_help()
                sys.exit()

            if inputTable <> "":
#                runString = "bsub -q xxl -o AcdReport_%s%d.log %s %s -w %d %s"%(cType,options.week,execName,cType,options.week,inputTable)
                runString = "%s %s -w %d %s"%(execName,cType,options.week,inputTable)
                print runString
 #               os.system(runString)

        elif action == "trend":
            if cType <> "check":
                runString = "%s %s"%(trendName,cType)
#                os.system(runString)
                print runString


