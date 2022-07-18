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
__date__      = "$Date: 2012/09/05 19:26:29 $"
__version__   = "$Revision: 1.5 $, $Author: brandt $"
__release__   = "$Name:  $"

#import LATTE.copyright_SLAC
import os, sys
import time
from optparse import OptionParser

ACDMONROOT = os.path.join(os.getenv("LATMonRoot"),'ACD','FLIGHT')
CALIBGENACD = os.path.join(os.getenv("RELEASE"), 'calibGenACD')
CALIBGENACDBINDIR = os.path.join(os.getenv("RELEASE"), 'bin', "%s-Optimized"%(os.getenv("SCONS_VARIANT")))
#CALIBGENACD = os.getenv("CALIBGENACDROOT")
#CALIBGENACDBINDIR = os.path.join(CALIBGENACD,os.getenv('CMTCONFIG'))

os.environ['CALIBUTILROOT'] = os.path.join(os.getenv("PARENT"), 'calibUtil')

CALIBTYPES = {
              'ped':('Ped','runPedestal',1,['-P'],'digi'),
               'gain':('ElecGain','runMipCalib',5,['ped'],'recon'),
               'veto':('ThreshVeto','runVetoCalib',1,['ped'],'digi'),
               'range':('Range','runRangeCalib',5,['ped','highPed'],'digi'),
               'cno':('ThreshHigh','runCnoCalib',1,['highPed'],'digi'),
               'coherentNoise':('CoherentNoise','runCoherentNoiseCalib',1,['-P','ped'],'digi'),

#              'ribbon':('Ribbon','runRibbonCalib',5,['ped'],'recon')
##              'highPed':('HighPed','runHighPed',1,['-s 1000'],'meta'),
##              'carbon':('Carbon','runCarbonCalib',60,['-G 6','highPed'],'recon')
##              'cnoFit':('CnoFit','runCnoFitCalib',0,[],'meta'),
##              'vetoFit':('VetoFit','runVetoFitCalib',0,[],'meta'),
##              'highRange':('HighRange','runHighRangeCalib',0,['ped','gain','highPed','carbon','range'],'meta'),
##              'check':('Check','runMeritCalib',5,['ped','gain','highRange'],'recon')}
}

#CALIBTYPES = {'gain':('ElecGain','runMipCalib',1,['ped'],'recon')}
#CALIBTYPES = {'ped':('Ped','runPedestal',1,['-P'],'digi')}
  
if __name__=='__main__':
    # argument parsing

    usage = 'AcdWeeklyReport.py ACTION <options> digiFile reconFile'
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
            elif cData[4] == 'recon':
               inputTable = args[0] + " " + args[1]
            elif cData[4] == 'meta':
                pass
            else:
                parser.print_help()
                sys.exit()

            singCommand = "singularity exec -B /afs,/gpfs,/nfs,/u /gpfs/slac/fermi/fs2/software/containers/slac-fermi.img.ext3 python "
            if inputTable <> "":
                runString = "bsub -W 20:00 -q long -R centos7  -o AcdReport_%s%d.log %s %s %s -w %d %s" % (cType, options.week, singCommand, execName, cType, options.week, inputTable)
#                runString = "bsub -W 20:00 -q xlong -Q '127' -R rhel60 -o AcdReport_%s%d.log %s %s -w %d %s"%(cType,options.week,execName,cType,options.week,inputTable)
#                runString = "bsub -q long -R fell -Q '127' -o AcdReport_%s%d.log %s %s -w %d %s"%(cType,options.week,execName,cType,options.week,inputTable)
#                runString = "%s %s -w %d %s"%(execName,cType,options.week,inputTable)
                print runString
                os.system(runString)

        elif action == "trend":
            if cType <> "check":
#                runString =  "%s %s"%(trendName,cType)
#                runString =  "bsub -W 20:00 -R rhel60 -o AcdTrend_%s.log %s %s"%(cType,trendName,cType)
                
                singCommand = "singularity exec -B /afs,/gpfs,/nfs,/u /gpfs/slac/fermi/fs2/software/containers/slac-fermi.img.ext3 python "
                runString = "bsub -W 20:00 -R centos7 -o AcdTrend_%s.log %s %s %s"%(cType, singCommand, trendName, cType)
                print runString
                os.system(runString)

