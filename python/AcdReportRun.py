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
__date__      = "$Date: 2012/09/05 19:25:46 $"
__version__   = "$Revision: 1.6 $, $Author: brandt $"
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

os.system("echo $CALIBUTILROOT")
os.environ['CALIBUTILROOT'] = os.path.join(os.getenv("PARENT"), 'calibUtil')
os.system("echo $CALIBUTILROOT")

CALIBTYPES = {'ped':('Ped','runPedestal',1,['-P']),
              'gain':('ElecGain','runMipCalib',5,['ped']),
              'veto':('ThreshVeto','runVetoCalib',1,['ped']),
              'range':('Range','runRangeCalib',5,['ped','highPed']),
              'cno':('ThreshHigh','runCnoCalib',1,['highPed']),
              'coherentNoise':('CoherentNoise','runCoherentNoiseCalib',1,['-P','ped']),
              'ribbon':('Ribbon','runRibbonCalib',5,['ped']),
              'highPed':('HighPed','runHighPed',1,['-s 1000']),
              'carbon':('Carbon','runCarbonCalib',60,['-G 6','highPed']),
              'cnoFit':('CnoFit','runCnoFitCalib',0,[]),
              'vetoFit':('VetoFit','runVetoFitCalib',0,[]),
              'highRange':('HighRange','runHighRangeCalib',0,['ped','gain','highPed','carbon','range']),
              'check':('Check','runMeritCalib',1,['-m','ped','gain','highRange'])}

USABLERUNTYPES = {"Tack_scan0":0,
                  "Tack_scan1":0,
                  "Tack_scan2":0,
                  "Tack_scan2ver":0,
                  "Tack_scan3":0,
                  "Tack_scan3ver":0,
                  "Tack_scan4":0,
                  "Tack_scan4ver":0,
                  "Tack_scan5":0,
                  "TkrBuf_alternatingSplit":0,
                  "TkrBuf_taperTrcBuf":0,
                  "TkrThrMod_CalSplash":0,
                  "Tkr_orStretch16":0,
                  "Treq_TkrAcd":0,
                  "Treq_TkrCal_Gamma":0,
                  "Treq_TkrCal_Mip":0,
                  "bkgAlbedo":0,
                  "bkgNadir":0,
                  "bkgPrescaled":0,
                  "calibOps":0,
                  "calibOps_eng10ps25":0,
                  "conBkgNadir":0,
                  "conBkgPrescaled":0,
                  "conSciOps":0,
                  "conSciOps_DgnTkr":0,
                  "conSciOps_NoCal":0,
                  "fheCalib_evenHi":0,
                  "fheCalib_evenLo":0,
                  "fheCalib_oddHi":0,
                  "fheCalib_oddLo":0,
                  "filterSimple":0,
                  "filterTaper":0,
                  "fleCalib_evenHiNeg":0,
                  "fleCalib_evenHiPos":0,
                  "fleCalib_evenLoNeg":0,
                  "fleCalib_evenLoPos":0,
                  "fleCalib_oddHiNeg":0,
                  "fleCalib_oddHiPos":0,
                  "fleCalib_oddLoNeg":0,
                  "fleCalib_oddLoPos":0,
                  "hldCalib_Hi":0,
                  "hldCalib_Lo":0,
                  "hldCalib_Nom":0,
                  "lacCalib_LoNeg":0,
                  "lacCalib_LoPos":0,
                  "lacCalib_MidNeg":0,
                  "lacCalib_MidPos":0,
                  "nomSO_noSk_noCno_Lac1MeV":1,
                  "nomSO_noSk_noCno_Lac1p5MeV":1,
                  "nomSO_noSk_noCno_calLo":1,
                  "nomSO_noSk_noCno_optGccc":1,
                  "nomSO_noSk_noCno_optGccc_allEna":1,
                  "nomSciOps":1,
                  "nomSciOps_Emin5MeV":1,
                  "nomSciOps_DgnTkr":1,
                  "nomSciOps_altCno":1,
                  "nomSciOps_noCNO":1,
                  "nomSciOps_noSkirtCno":1,
                  "nomSciOps_noSkirtCno_noCno":1,
                  "nomSciOps_diagEna":1,
                  "nomSciOps_timing":1,
                  "nomSciOps_topCno":1,
                  "nomSciOps_uldLo":1,
                  "vetoCalib_Hi":0,
                  "vetoCalib_Lo":0,
                  "vetoCalib_Nom":0}

def getDirName(calibName):
    """
    """
    if CALIBTYPES.has_key(calibName):
        return CALIBTYPES[calibName][0]
    return None

def getExeName(calibName):
    """
    """
    if CALIBTYPES.has_key(calibName):
        return os.path.join(CALIBGENACDBINDIR,CALIBTYPES[calibName][1])
    return None

def getNRuns(calibName):
    """
    """
    if CALIBTYPES.has_key(calibName):
        return CALIBTYPES[calibName][2]
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

def writeRunList(runs):
    """
    """
    for aRun in runs:
        print "%s"%(aRun[5].strip())

def getCalibFileName(calibName,baseName):
    """
    """
    calibDirName = getDirName(calibName)
    return os.path.join(ACDMONROOT,calibDirName,baseName)

def getRefFileName(calibName,refDict):
    """
    """
    return getCalibFileName(calibName,refDict[calibName])

def getArgs(calibName,useDict):
    """
    """
    theArgs = ""
    if CALIBTYPES.has_key(calibName):
        argList = CALIBTYPES[calibName][3]
    else:
        return None
  
    for arg in argList:
        if arg == 'ped':
            theArgs += " -p %s"%getRefFileName(arg,useDict)
        elif arg == 'gain':
            theArgs += " -g %s"%getRefFileName(arg,useDict)
        elif arg == 'range':
            theArgs += " -R %s"%getRefFileName(arg,useDict)
        elif arg == 'highPed':
            theArgs += " -H %s"%getRefFileName(arg,useDict)
        elif arg == 'carbon':
            theArgs += " -C %s"%getRefFileName(arg,useDict)
        elif arg == 'highRange':
            theArgs += " -R %s"%getRefFileName(arg,useDict)
        else:
            theArgs += " %s"%arg
    return theArgs

def readRunsTable(inFileName):
    """
    """
    inFile = open(inFileName)
    inline = inFile.readline()
    runsData = []
    while inline<>'':
        fields = inline.split(' ')
        if len(fields) <> 6:
            return None
        runsData.append(fields)
        inline = inFile.readline()
    inFile.close()
    return runsData

def makeRunString(calibName,runs,tag):
    """
    """
    runStr = ""
    if len(runs) < 10:
        for aRun in runs:
            runStr += "%s "%(aRun[5].strip())
        return runStr
    runStr = "%s_%s.lst"%(calibName,tag)
    runFile = open(runStr,'w')
    for aRun in runs:
        runFile.write(aRun[5])
    runFile.close()
    return runStr

def buildCalibCommand(calibName,refDict,useDict,runs,tag):
    """
    """
    if len(runs) >= 1:
        outprefix = "r%s"%(runs[0][0])
    elif calibName in ['cnoFit','vetoFit']:
        outprefix = "scan_%s"%tag
    elif calibName in ['highRange']:
        outprefix = "hr_%s"%tag    
    execName = getExeName(calibName)
    refNameStr = "-x %s"%getRefFileName(calibName,refDict)
    
    theArgs = getArgs(calibName,useDict)
    runStr = makeRunString(calibName,runs,tag)
    execLine = "%s%s %s -o %s %s"%(execName,theArgs,refNameStr,outprefix,runStr)
#    execLine = "%s%s %s -o %s %s -e eventList.txt"%(execName,theArgs,refNameStr,outprefix,runStr)
    return (execLine,outprefix)

def buildReportCommand(calib,comment,outPrefix,tag):
    """
    """
    reportExec = os.path.join(CALIBGENACD,'python','AcdReportUtil.py')
    reportLine = '%s store -c "%s" -T %s %s_%s.xml'%(reportExec,comment,tag,outPrefix,calib)
    return reportLine

def getRunsFromDay(runsData,day,nRuns):
    """
    """
    runs = []
    tag = ""
    for aRun in runsData:
        rDay = aRun[1]
        tag = rDay
        if rDay <> day:
            continue
        rType = aRun[4]
        if USABLERUNTYPES.has_key(rType):
            if USABLERUNTYPES[rType] < 1:
                continue
        else:
            print "Unknown run type %s skipped"%(rType)
        if len(runs) >= nRuns:
            return (tag,runs)
        runs.append(aRun)
    if len(runs) < nRuns:
        print "Only found %d good runs in day %s, not %d"%(len(runs),day,nRuns)
    return (tag,runs)

def getRunsFromWeek(runsData,week,nRuns):
    """
    """
    runs = []
    tag = ""
    for aRun in runsData:
        tag = aRun[1]
#        print tag
        rWeek = int(aRun[2],10)
        if rWeek <> week:
            continue
        rType = aRun[4]
        if USABLERUNTYPES.has_key(rType):
            if USABLERUNTYPES[rType] < 1:
                continue
        else:
            print "Unknown run type %s skipped"%(rType)       
        if len(runs) >= nRuns:
            return (tag,runs)
        runs.append(aRun)
    if len(runs) < nRuns:
        print "Only found %d good runs in week %d, not %d"%(len(runs),week,nRuns)
    return (tag,runs)
        
  
if __name__=='__main__':
    # argument parsing

    usage = 'AcdReportRun.py CALIB [options] input'
    parser = OptionParser(usage)

    parser.add_option("-w", "--week",action="store",
                      dest="week",type="int",default=0,
                      help="Mission Week", metavar="INT")

    parser.add_option("-d", "--day",action="store",
                      dest="day",type="string",default="",
                      help="Day", metavar="YYMMDD")

    parser.add_option("-T", "--Tag",action="store",
                      dest="Tag",type="string",default="",
                      help="Tag", metavar="YYMMDD")

    if len(sys.argv) == 1 or sys.argv[1] == '-h':
        parser.print_help()
        sys.exit()

    calib = sys.argv[1]
    if calib not in CALIBTYPES:
        parser.print_help()
        print "CALIB must be one of %s"%str(CALIBTYPES.keys())
        sys.exit()        

    (options, args) = parser.parse_args(sys.argv[2:])

#    print args

#    if len(args) <> 1:
#        parser.print_help()
#        sys.exit()  

#    refDict = getRefFiles( os.path.join(ACDMONROOT,'ref.txt') )
    refDict = getRefFiles( os.path.join(ACDMONROOT,'ref.new.txt') )
    useDict = getRefFiles( os.path.join(ACDMONROOT,'calib.new.txt') )

    if len(args) > 1:
      runsTableDigi = readRunsTable(args[0])
      runsTableRecon = readRunsTable(args[1])

      if options.week:            
          (tag,runsDigi) = getRunsFromWeek(runsTableDigi,options.week,getNRuns(calib))
          (tag,runsRecon) = getRunsFromWeek(runsTableRecon,options.week,getNRuns(calib))
          comment = "Data from Mission week %d"%(options.week)
      elif options.day:            
          (tag,runsDigi) = getRunsFromDay(runsTableDigi,options.day,getNRuns(calib))
          (tag,runsRecon) = getRunsFromDay(runsTableRecon,options.day,getNRuns(calib))
          comment = "Data from %s"%(options.day)
      else:
          parser.print_help()
          sys.exit()

      runs = runsDigi + runsRecon

      (execLine,outPrefix) = buildCalibCommand(calib,refDict,useDict,runs,tag)
      reportLine = buildReportCommand(calib,comment,outPrefix,tag)



    else:
      runsTable = readRunsTable(args[0])

      if options.week:            
          (tag,runs) = getRunsFromWeek(runsTable,options.week,getNRuns(calib))
          comment = "Data from Mission week %d"%(options.week)
      elif options.day:            
          (tag,runs) = getRunsFromDay(runsTable,options.day,getNRuns(calib))
          comment = "Data from %s"%(options.day)
      else:
          parser.print_help()
          sys.exit()
    
    (execLine,outPrefix) = buildCalibCommand(calib,refDict,useDict,runs,tag)
    reportLine = buildReportCommand(calib,comment,outPrefix,tag)

    #os.system("echo $CALIBUTILROOT")

    print execLine
    os.system(execLine)
    print reportLine
    os.system(reportLine)


    

        


