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
__date__      = "$Date$"
__version__   = "$Revision$, $Author$"
__release__   = "$Name$"

#import LATTE.copyright_SLAC
import os
import time
import numpy.oldnumeric as Numeric
from optparse import OptionParser

import ROOT

import AcdXmlUtil
from AcdTileMapLookUp import AcdTileMap


class AcdHighRangeCalib:
    """ Class to fit the veto response function
    """
    def __init__(self):
        """ c'tor
        """
        self.__saturationValue = 2000.;
        self.__carbonInMips = 33.7779472161;
        self.__slopeArray = Numeric.zeros((12,18),'f')
        self.__carbon = Numeric.zeros((12,18),'f')
        self.__peds = Numeric.zeros((12,18),'f')
        self.__theTileMap = AcdTileMap()        

    def readPeds(self,fileName):
        """ read a single file with the veto turn on points in PHA
        """
        (theFile,doc) = AcdXmlUtil.openXmlFileByName(fileName)
        if theFile is None or doc is None:
            return False

        calibNode = None
        for cNode in doc.childNodes:
            if cNode.nodeName != "acdCalib":
                continue
            calibNode = cNode
            break

        if calibNode is None:
            return False
        
        for tileNode in calibNode.childNodes:
            if tileNode.nodeType != tileNode.ELEMENT_NODE or tileNode.nodeName != "tile":
                continue
            tile = int(AcdXmlUtil.getAttribute(tileNode,"tileId"))
            if tile >= 700:
                tileName = "NA%02d"%(tile-700)
            else:
                tileName = "%03d"%(tile)
                       
            for pmtNode in tileNode.childNodes:
                if pmtNode.nodeType != pmtNode.ELEMENT_NODE or pmtNode.nodeName != "pmt":
                    continue
                pmt = AcdXmlUtil.getAttribute(pmtNode,"iPmt")
                for vetoNode in pmtNode.childNodes:
                    if vetoNode.nodeType != vetoNode.ELEMENT_NODE or vetoNode.nodeName != "acdHighPed":
                        continue                    
                    pedVal = float(AcdXmlUtil.getAttribute(vetoNode,"mean"))
                    pmtName = ""
                    if pmt == '0':
                        pmtName = 'A'
                    elif pmt == '1':
                        pmtName = 'B'
                    (garc,gafe) = self.__theTileMap.tileDict[tileName][pmtName]
                    self.__peds[garc,gafe] = pedVal
                    pass
                pass
            pass
        theFile.close()
        return True

    def readCarbon(self,fileName):
        """ read a single file with the veto turn on points in PHA
        """
        (theFile,doc) = AcdXmlUtil.openXmlFileByName(fileName)
        if theFile is None or doc is None:
            return False

        calibNode = None
        for cNode in doc.childNodes:
            if cNode.nodeName != "acdCalib":
                continue
            calibNode = cNode
            break

        if calibNode is None:
            return False
        
        for tileNode in calibNode.childNodes:
            if tileNode.nodeType != tileNode.ELEMENT_NODE or tileNode.nodeName != "tile":
                continue
            tile = int(AcdXmlUtil.getAttribute(tileNode,"tileId"))
            if tile >= 700:
                tileName = "NA%02d"%(tile-700)
            else:
                tileName = "%03d"%(tile)
                       
            for pmtNode in tileNode.childNodes:
                if pmtNode.nodeType != pmtNode.ELEMENT_NODE or pmtNode.nodeName != "pmt":
                    continue
                pmt = AcdXmlUtil.getAttribute(pmtNode,"iPmt")
                for vetoNode in pmtNode.childNodes:
                    if vetoNode.nodeType != vetoNode.ELEMENT_NODE or vetoNode.nodeName != "acdGain":
                        continue                    
                    peakVal = float(AcdXmlUtil.getAttribute(vetoNode,"peak"))
                    pmtName = ""
                    if pmt == '0':
                        pmtName = 'A'
                    elif pmt == '1':
                        pmtName = 'B'
                    (garc,gafe) = self.__theTileMap.tileDict[tileName][pmtName]
                    self.__carbon[garc,gafe] = peakVal
                    pass
                pass
            pass
        theFile.close()
        return True


    def calc(self):
        """
        """
        self.__slopeArray = self.__carbon / self.__carbonInMips            
        return

    def writeXml(self,timeString,pedFile,carbonFile):
        """
        """
        doc = AcdXmlUtil.makeDoc()

        topNode = AcdXmlUtil.makeChildNode(doc,"acdCalib")
        ts = time.asctime()
        
        AcdXmlUtil.addComment(topNode,"created by %s on %s"%("AcdHighRangeCalib.py",ts))

        genNode = AcdXmlUtil.makeChildNode(topNode,"generic")

        AcdXmlUtil.setAttribute(genNode,"DTDVersion","v3")
        AcdXmlUtil.setAttribute(genNode,"algorithm","LineFit")
        AcdXmlUtil.setAttribute(genNode,"calibType","ACD_VetoCalib")
        AcdXmlUtil.setAttribute(genNode,"fmtVersion","v2r1")
        
        AcdXmlUtil.setAttribute(genNode,"instrument","LAT")
        AcdXmlUtil.setAttribute(genNode,"timestamp",ts)

        inputNode = AcdXmlUtil.makeChildNode(genNode,"inputSample")
        AcdXmlUtil.setAttribute(inputNode,"mode","Normal")
        AcdXmlUtil.setAttribute(inputNode,"source","Orbit")
        AcdXmlUtil.setAttribute(inputNode,"startTime","0")
        AcdXmlUtil.setAttribute(inputNode,"stopTime","0")
        AcdXmlUtil.setAttribute(inputNode,"triggers","0")

        pedFileNode = AcdXmlUtil.makeChildNode(inputNode,"inputFile")
        AcdXmlUtil.setAttribute(pedFileNode,"type","Ped")
        AcdXmlUtil.setAttribute(pedFileNode,"path",pedFile)           
        carbonFileNode = AcdXmlUtil.makeChildNode(inputNode,"inputFile")            
        AcdXmlUtil.setAttribute(carbonFileNode,"type","AcdCarbon")
        AcdXmlUtil.setAttribute(carbonFileNode,"path",carbonFile)
      
        dimNode = AcdXmlUtil.makeChildNode(topNode,"dimension")
        AcdXmlUtil.setAttribute(dimNode,"nTile","108")
  
        tileList = self.__theTileMap.getTileNameList()
        for tile in tileList:            
            tileNode = AcdXmlUtil.makeChildNode(topNode,"tile")
            AcdXmlUtil.setAttribute(tileNode,"tileId",tile)
            for pmt in ['A','B']:
                pmtNode =  AcdXmlUtil.makeChildNode(tileNode,"pmt")
                if pmt == 'A':
                    iPmt = 0
                elif pmt == 'B':
                    iPmt = 1
                AcdXmlUtil.setAttribute(pmtNode,"iPmt",iPmt)
                valNode = AcdXmlUtil.makeChildNode(pmtNode,"acdVeto")
                (garc,gafe) = self.__theTileMap.tileDict[tile][pmt]
                AcdXmlUtil.setAttribute(valNode,"slope","%7.6f"%self.__slopeArray[garc,gafe])
                AcdXmlUtil.setAttribute(valNode,"pedestal","%7.6f"%self.__peds[garc,gafe])
                AcdXmlUtil.setAttribute(valNode,"saturation","%7.6f"%self.__saturationValue)
                AcdXmlUtil.setAttribute(valNode,"status","0")
        fileName = 'AcdHighRangeCalib_%s.xml'%(timeString)
        AcdXmlUtil.xmlToStream(doc,fileName)
        print "creating file",fileName
        return
        

if __name__=='__main__':
    # argument parsing
    parser = OptionParser()

    parser.add_option("-t", "--time",action="store",
                      dest="time",type="string",default="",
                      help="Time stamp", metavar="TIME")
    

    parser.add_option("-p", "--peds",action="store",
                      dest="peds",type="string",default="",
                      help="Pedstal File", metavar="FILE")

    parser.add_option("-c", "--carbon",action="store",
                      dest="carbon",type="string",default="",
                      help="Carbon Peak File", metavar="FILE")

    
    (options, args) = parser.parse_args()

    # Latch the time
    timestring = options.time
    if timestring == "":
        timestring = "%.0f"%(time.mktime(time.localtime()) - time.mktime(time.strptime('Sun Dec 31 16:00:00 2000')))

    # do all the parsing 'n shit
    calib = AcdHighRangeCalib()


    if options.peds != "":
        calib.readPeds(options.peds)

    if options.carbon != "":
        calib.readCarbon(options.carbon)    

    calib.calc()
    calib.writeXml(timestring,options.peds,options.carbon)
    
