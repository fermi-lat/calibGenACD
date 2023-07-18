#!/usr/bin/env python
#
#                               Copyright 2007
#                                     by
#                        The Board of Trustees of the
#                     Leland Stanford Junior University.
#                            All rights reserved.
#

__facility__ = "calibGenACD"
__abstract__ = "Extracts the DAC to PHA set point relationship of ACD veto"
__author__    = "E. Charles"
__date__      = "$Date: 2012/09/05 19:26:29 $"
__version__   = "$Revision: 1.3 $, $Author: brandt $"
__release__   = "$Name:  $"

#import LATTE.copyright_SLAC
import os, sys
import time
from optparse import OptionParser
import datetime

ACDMONROOT = os.path.join(os.getenv("LATMonRoot"),'ACD','FLIGHT')
CALIBGENACD = os.path.join(os.getenv("RELEASE"), 'calibGenACD')
CALIBGENACDBINDIR = os.path.join(os.getenv("RELEASE"), 'bin', "%s-Optimized"%(os.getenv("SCONS_VARIANT")))
#CALIBGENACD = os.getenv("CALIBGENACDROOT")
#CALIBGENACDBINDIR = os.path.join(CALIBGENACD,os.getenv('CMTCONFIG'))

os.environ['CALIBUTILROOT'] = os.path.join(os.getenv("PARENT"), 'calibUtil')

CALIBTYPES = {'ped':('Ped','runPedestal',1,['-P']),
              'gain':('ElecGain','runMipCalib',5,['ped']),
              'veto':('ThreshVeto','runVetoCalib',1,['ped']),
              'range':('Range','runRangeCalib',1,['ped','highPed']),
              'cno':('ThreshHigh','runCnoCalib',1,['highPed']),
              'coherentNoise':('CoherentNoise','runCoherentNoiseCalib',1,['-P','ped']),
              'ribbon':('Ribbon','runRibbonCalib',5,['ped']),
              'highPed':('HighPed','runHighPed',1,['-s 1000']),
              'carbon':('Carbon','runCarbonCalib',60,['-G 6','highPed']),
              'cnoFit':('CnoFit','runCnoFitCalib',0,[]),
              'vetoFit':('VetoFit','runVetoFitCalib',0,[]),
              'highRange':('HighRange','runHighRangeCalib',0,['ped','gain','highPed','carbon','range']),
              'check':('Check','runMeritCalib',5,['ped','gain','highRange'])}

def getDirName(calibName):
    """
    """
    if CALIBTYPES.has_key(calibName):
        return CALIBTYPES[calibName][0]
    return None

def getRefFiles(inFileName):
    """
    """
    inFile = open(inFileName)
    inline = inFile.readline()
    refFiles = {}
    while inline<>'':
        fields = inline.split(' ')
        if len(fields) <> 2:
            return None
        refFiles[fields[0]] = fields[1].strip()
        inline = inFile.readline()
    inFile.close()
    return refFiles

def getCalibFileName(calibName,baseName):
    """
    """
    calibDirName = getDirName(calibName)
    return os.path.join(ACDMONROOT,calibDirName,baseName)

def getRefFileName(calibName,refDict):
    """
    """
    return getCalibFileName(calibName,refDict[calibName])

def buildTrendCommand(calibName,refDict):
    """
    """
    outPrefStr = "-o trend"
    execName = os.path.join(CALIBGENACDBINDIR,"runCalibTrend")
    refNameStr = "-x %s"%getRefFileName(calibName,refDict)
    calibDirName = getDirName(calibName)
    inputName = os.path.join(ACDMONROOT,calibDirName,"calibs.lst");
    execLine = "%s %s %s %s"%(execName,refNameStr,outPrefStr,inputName)
    return execLine


  
if __name__=='__main__':
    # argument parsing

    usage = 'AcdReportRun.py CALIB [options]'
    parser = OptionParser(usage)

    if len(sys.argv) == 1 or sys.argv[1] == '-h':
        parser.print_help()
        sys.exit()

    calib = sys.argv[1]
    if calib not in CALIBTYPES:
        parser.print_help()
        print "CALIB must be one of %s"%str(CALIBTYPES.keys())
        sys.exit()        

    (options, args) = parser.parse_args(sys.argv[2:])

    if len(args) <> 0:
        parser.print_help()
        sys.exit()  

    refDict = getRefFiles( os.path.join(ACDMONROOT,'ref.new.txt') )
#    refDict = getRefFiles( os.path.join(ACDMONROOT,'ref.txt') )

    execLine = buildTrendCommand(calib,refDict)

    print execLine
    os.system(execLine)

    toDir = os.path.join(ACDMONROOT,getDirName(calib),"trend")
    
    todayString = datetime.date.today().strftime("%y%m%d")
    saveDir = os.path.join( toDir, "plots_before_%s" % todayString )
    
    sysCom = "mkdir -p %s" % saveDir
    os.system(sysCom)
    
    sysCom = "mv %s/trend_*  %s"%(toDir, saveDir)
    os.system(sysCom)

    sysCom = "mv trend_%s* %s"%(calib,toDir)
    os.system(sysCom)
