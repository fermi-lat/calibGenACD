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
__date__      = "$Date: 2010/06/24 23:52:41 $"
__version__   = "$Revision: 1.1 $, $Author: echarles $"
__release__   = "$Name:  $"

#import LATTE.copyright_SLAC
import os, sys
import time
from optparse import OptionParser
from py_mootCore import MootQuery, vectorOfConstitInfo, ConstitInfo

DATACATBIN = "/afs/slac/g/glast/ground/bin/datacat"

def getDateStamp():
    """
    """
    return time.strftime("%y%m%d")

def callDatacat(group,dateStamp):
    """
    """

    if group == 'RECON':
      dataCatList = "%s_%s.list"%(group,dateStamp)
#      dataCatLine = "%s find --sort nMetStart --group %s /Data/Flight/Reprocess/P300/ > %s"%(DATACATBIN,group,dataCatList)
      dataCatLine = "%s find --sort nMetStart --group %s /Data/Flight/Level1/LPA/ > %s"%(DATACATBIN,group,dataCatList)
      print "Calling datacat for group %s on %s"%(group,dateStamp)
      os.system(dataCatLine)
      return dataCatList
    elif group == 'MERIT':
      dataCatList = "%s_%s.list"%(group,dateStamp)
#      dataCatLine = "%s find --sort nMetStart --group %s /Data/Flight/Reprocess/P301/ > %s"%(DATACATBIN,group,dataCatList)
      dataCatLine = "%s find --sort nMetStart --group %s /Data/Flight/Level1/LPA/ > %s"%(DATACATBIN,group,dataCatList)
      print "Calling datacat for group %s on %s"%(group,dateStamp)
      os.system(dataCatLine)
      return dataCatList
    else: 
      dataCatList = "%s_%s.list"%(group,dateStamp)
      dataCatLine = "%s find --sort nMetStart --group %s /Data/Flight/Level1/LPA/ > %s"%(DATACATBIN,group,dataCatList)
      print "Calling datacat for group %s on %s"%(group,dateStamp)
      os.system(dataCatLine)
      return dataCatList

def configInfo(metTime,mq):
    """
    """
    acqInfo = mq.getAcqSummaryInfo( int(metTime[1:]) )
    if acqInfo is None:
        return ("None",0)
    key = int(acqInfo.getConfigKey())
    configInfo = mq.getConfigInfo(key)
    if configInfo is None:
        return ("None",key)    
    return (configInfo.getName(),key)

def fmxKeys(mKey):
    """
    """
    mq = MootQuery(None)
    constits = vectorOfConstitInfo()
    ci = mq.getActiveFilters(mKey,constits,0)
    for ci in constits:
        print (ci.getKey(),ci.getFswId(),ci.getSchemaId(),ci.getSchemaVersionId(),ci.getInstanceId()  )
    
    
def utcDayAndWeek(metTime):
    """
    """
    unixSecs = float(metTime[1:])
    missionEpoch = time.mktime( time.strptime("Sun Dec 31 16:00:00 2000") )
    missionStart = time.mktime( time.strptime("Sun Jun  8 15:00:00 2008") )    
    utcTime = time.gmtime(unixSecs+missionEpoch)
    launchSecs = unixSecs+missionEpoch-missionStart
    week = int ( launchSecs / 604800 )
    day = "%02d%02d%02d"%(utcTime[0]-2000,utcTime[1],utcTime[2])
    return (day,week)

def parseNames(inFileName):
    """
    """
    outFileName = inFileName.replace("list","table")
    outFile = open(outFileName,'w')
    mq = MootQuery(None)
    inFile = open(inFileName)
    inline = inFile.readline()
    while inline<>'':
        w = inline.find('/r0')
        runNum = inline[w+2:w+12]
        (uDay,mWeek) = utcDayAndWeek(runNum)
        (configName,configKey) = configInfo(runNum,mq)
        outFile.write("%s %s %03d %-4d %s %s\n"%(runNum,uDay,mWeek,configKey,configName,inline.strip()))
        inline = inFile.readline()
    inFile.close()
    outFile.close()
    return None


if __name__=='__main__':
    # argument parsing

    usage = 'ParseFileList.py type'
    parser = OptionParser(usage)

    if len(sys.argv) == 1 or sys.argv[1] == '-h':
        parser.print_help()
        sys.exit()

    (options, args) = parser.parse_args(sys.argv[1:])

    if len(args) < 1:
        parser.print_help()
        sys.exit()        

    dateStamp = getDateStamp()

    for group in args:
        dataCatList = callDatacat(group,dateStamp)       
        #Latch the time
        parseNames(dataCatList)

    
    
