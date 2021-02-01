#!/usr/bin/env python
#
#                               Copyright 2008
#                                     by
#                        The Board of Trustees of the
#                     Leland Stanford Junior University.
#                            All rights reserved.
#

__facility__ = "calibGenACD"
__abstract__ = "Extracts the DAC to PHA set point relationship of ACD veto"
__author__    = "E. Charles"
__date__      = "$Date: 2008/09/05 22:57:49 $"
__version__   = "$Revision: 1.3 $, $Author: echarles $"
__release__   = "$Name:  $"

import os, sys
import time
from optparse import OptionParser

ACDMONROOT = os.path.join(os.getenv("LATMonRoot"),'ACD','FLIGHT')

CALIBTYPES = {'Ped':'ped',
              'ElecGain':'gain',
              'ThreshVeto':'veto',
              'Range':'range',
              'ThreshHigh':'cno',
              'Ribbon':'ribbon',
              'HighPed':'highPed',
              'Carbon':'carbon',
              'CoherentNoise':'coherentNoise',
              'CnoFit':'cnoFit',
              'VetoFit':'vetoFit',
              'HighRange':'highRange',
              'Check':'check'}
              
def idFileType(inFileName):
    """
    """
    if inFileName.find('_check.') >= 0:
        return "Check"
    elif inFileName.find('_ped.') >= 0:
        return "Ped"
    elif inFileName.find('_gain.') >= 0:
        return "ElecGain"
    elif inFileName.find('_range.') >= 0:
        return "Range"
    elif inFileName.find('_veto.') >= 0:
        return "ThreshVeto"
    elif inFileName.find('_vetoFit.') >= 0:
        return "VetoFit"
    elif inFileName.find('_cno.') >= 0:
        return "ThreshHigh"
    elif inFileName.find('_cnoFit.') >= 0:
        return "CnoFit"
    elif inFileName.find('_pedHigh.') >= 0:
        return "HighPed"
    elif inFileName.find('_carbon.') >= 0:
        return "Carbon"
    elif inFileName.find('_ribbon.') >= 0:
        return "Ribbon"
    elif inFileName.find('_highRange.') >= 0:
        return "HighRange"
    elif inFileName.find('_coherentNoise') >= 0:
        return "CoherentNoise"
    return None

def getCalibInfo(inFileName):
    """
    """
    inFile = open(inFileName)
    inline = inFile.readline()
    while inline<>'':
        if inline.find('<inputSample') >= 0:
            fields = inline.split('"')
            startTime = float(fields[5])
            endTime =  float(fields[7])
            trig = int(fields[9])
            dur = endTime - startTime
            return "%-13.0f %-7.0f %-9d"%(startTime,dur,trig)
        inline = inFile.readline()
    inFile.close()
    return None

def addComment(inFileName,comment):
    """
    """
    outFileName = inFileName + ".bak"
    outFile = open(outFileName,'w')
    inFile = open(inFileName)
    inline = inFile.readline()
    while inline<>'':
        if inline.find('COMMENT__HOOK') >= 0:
            print "Adding generic comment %s"%comment
            outFile.write("      <li>%s</li>\n"%comment)
        outFile.write(inline)
        inline = inFile.readline()
    outFile.close()
    inFile.close()
    os.rename(outFileName,inFileName)

def addUse(inFileName,use):
    """
    """
    outFileName = inFileName + ".bak"
    outFile = open(outFileName,'w')
    inFile = open(inFileName)
    inline = inFile.readline() 
    while inline<>'':
        if inline.find('USE__HOOK') >= 0:
            print "Adding use comment %s"%use
            outFile.write("      <li>%s</li>\n"%use)
        outFile.write(inline)
        inline = inFile.readline()
    outFile.close()
    inFile.close()
    os.rename(outFileName,inFileName)

def addStore(calib,tag,comment,report):
    """
    """
    inFileName = os.path.join(ACDMONROOT,calib,'index.html')
    outFileName = inFileName + ".bak"
    outFile = open(outFileName,'w')
    inFile = open(inFileName)
    inline = inFile.readline()
    hook = "NEW__%s__CALIB__HOOK"%(calib)
    while inline<>'':
        if inline.find(hook) >= 0:
            print "Adding use %s %s %s"%(tag,report,comment)
            outFile.write("       <li>%s\n"%comment)
            outFile.write('         <a href="%s/%s">%s</a>\n'%(tag,report,tag))
            outFile.write("       </li>\n")
        outFile.write(inline)
        inline = inFile.readline()
    outFile.close()
    inFile.close()
    os.rename(outFileName,inFileName)

def setRef(calib,inFileName,tag,timeString,action,comment):
    """
    """
    relName = inFileName.replace(os.path.join(ACDMONROOT,calib),"")
    indexFileName = os.path.join(ACDMONROOT,calib,'index.html')
    outFileName = indexFileName + ".bak"    
    outFile = open(outFileName,'w')
    inFile = open(indexFileName)
    inline = inFile.readline()
    while inline<>'':
        outFile.write(inline)
        hookString = "%s__%s__CALIB__HOOK"%(action.upper(),calib)
        if inline.find(hookString) >= 0:
            print "Adding reference %s %s"%(relName[1:],tag)
            outFile.write('          <a href="%s">%s</a>\n'%(relName[1:],tag))
        inline = inFile.readline()
    outFile.close()
    inFile.close()
    os.rename(outFileName,indexFileName)

    if action in ['ref','calib']:
        inFile = open(os.path.join(ACDMONROOT,"%s.txt"%action))
        outFile = open(os.path.join(ACDMONROOT,"%s.bak"%action),'w')
        inline = inFile.readline()
        while inline<>'':
            fields = inline.split(' ')
            if len(fields) <> 2:
                return None
            if fields[0]== CALIBTYPES[calib]:
                outFile.write("%s %s\n"%(fields[0],relName[1:].replace(".html",".xml")))
            else:
                outFile.write(inline)
            inline = inFile.readline()
        inFile.close()
        outFile.close()
        os.rename(os.path.join(ACDMONROOT,"%s.bak"%action),os.path.join(ACDMONROOT,"%s.txt"%action))

    if action == "ref":
        addUse(inFileName,"Set as reference on %s"%timeString)
    elif action == "calib":
        addUse(inFileName,"Set to use in making calibration on %s"%timeString)
    elif action == "config":    
        addUse(inFileName,"Added to moot on %s: %s"%(timeString,comment))
    elif action == "offline":    
        addUse(inFileName,"Added to offline on %s: %s"%(timeString,comment))
    elif action == "sim":    
        addUse(inFileName,"Added to sim on %s: %s"%(timeString,comment))


if __name__=='__main__':
    # argument parsing

    usage = 'AcdReportUtil.py ACTION [options] input'
    parser = OptionParser(usage)

    parser.add_option("-t", "--time",action="store",
                      dest="time",type="string",default="",
                      help="Time stamp", metavar="TIME")

    parser.add_option("-T", "--tag",action="store",
                      dest="tag",type="string",
                      help="Output tag", metavar="string")    
    
    parser.add_option("-c", "--comment",action="store",
                      dest="comment",type="string",default="",
                      help="Add a comment to the report", metavar="string")


    if len(sys.argv) == 1 or sys.argv[1] == '-h':
        parser.print_help()
        sys.exit()

    action = sys.argv[1]
    actions = ['comment','use','store','ref','config','calib','offline','sim']
    if action not in actions:
        parser.print_help()
        print "ACTION must be one of %s"%(str(actions))
        sys.exit()        

    (options, args) = parser.parse_args(sys.argv[2:])

    if len(args) <> 1:
        parser.print_help()
        sys.exit()        

    input = args[0]

    # Latch the time
    timestring = options.time
    if timestring == "":
        timestring = time.asctime(time.localtime())

    idFt = idFileType(input)
    if idFt is None:
        print "Can't get file type for %s"%options.input
        sys.exit()
        
    print "Input file of type %s"%idFt

    if action == 'comment':
        addComment(input,options.comment)
    elif action == 'use':
        addUse(input,options.comment)
    elif action in ['ref','calib','config','offline','sim']:
        setRef(idFt,input,options.tag,timestring,action,options.comment)
    elif action == 'store':        
        cInfo = getCalibInfo(input)
        if cInfo is None:
            print "Couldn't find calibration info in %s"%input            
            sys.exit()
        if options.comment == '':
            print "You should provide a comment with importing a calibration"
            parser.print_help()
            sys.exit()
        if options.tag == '':
            print "You should provide a tag with importing a calibration"
            parser.print_help()
            sys.exit()            
        acqStr = "%-20s %s %s"%(options.tag,cInfo,options.comment)

        htmlName = input.replace('.xml','.html')
        addUse(htmlName,"stored at %s"%timestring )
        addComment(htmlName,options.comment)
        
        acqFileName = os.path.join(ACDMONROOT,idFt,'acquire_notes.txt')
        calibLstFileName = os.path.join(ACDMONROOT,idFt,'calibs.lst')        
        toDir = os.path.join(ACDMONROOT,idFt,options.tag)
        genInName = input.replace('.xml','*')
        
        if not os.path.exists(toDir): #HF
            os.mkdir(toDir) 
        sysCom = "mv %s %s"%(genInName,toDir)
        os.system(sysCom)

        acqFile = open(acqFileName,'a')
        acqFile.write("%s\n"%acqStr)
        acqFile.close()

        calibLstFile = open(calibLstFileName,'a')        
        calibLstFile.write("%s\n"%os.path.join(toDir,input))
        calibLstFile.close()
        
        addStore(idFt,options.tag,options.comment,htmlName)
    
