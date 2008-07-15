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
from AcdROOT_LineFit import *
from AcdTileMapLookUp import AcdTileMap


class AcdOfflineVetoFitter:
    """ Class to fit the veto response function
    """
    def __init__(self):
        """ c'tor
        """
        self.__n = 0
        self.__offsetArray = Numeric.zeros((12,18),'f')
        self.__slopeArray = Numeric.zeros((12,18),'f')
        self.__carbons = Numeric.zeros((12,18),'f')
        self.__peds = Numeric.zeros((12,18),'f')
        self.__values = []
        self.__settings = []
        self.__theTileMap = AcdTileMap()        

    def nPoints(self):
        """ returns the number of points that have been loaded
        """
        return self.__n

    def __readSettingsFile(self, fileName):
        """ read a single file with ACD veto settings
        """
        (theFile,doc) = AcdXmlUtil.openXmlFileByName(fileName)
        theSettings = Numeric.zeros((12,18),'f')
        if theFile is None or doc is None:
            return False

        latcNode = None
        for cNode in doc.childNodes:
            if cNode.nodeName != "LATC_XML":
                continue
            latcNode = cNode
            break

        if latcNode is None:
            return False
        
        for aemNode in latcNode.childNodes:
            if aemNode.nodeType != aemNode.ELEMENT_NODE or aemNode.nodeName != "AEM":
                continue
            for arcNode in aemNode.childNodes:
                if arcNode.nodeType != arcNode.ELEMENT_NODE or arcNode.nodeName != "ARC":
                    continue
                arc = int(AcdXmlUtil.getAttribute(arcNode,"ID"))
                for afeNode in arcNode.childNodes:
                    if afeNode.nodeType != afeNode.ELEMENT_NODE or afeNode.nodeName != "AFE":
                        continue
                    hldVal = None
                    afe = int(AcdXmlUtil.getAttribute(afeNode,"ID"))
                    for regNode in afeNode.childNodes:
                        if regNode.nodeType != regNode.ELEMENT_NODE:
                            continue                        
                        if regNode.nodeName == "hld_dac":
                            (isOk,hldVal) = AcdXmlUtil.returnTextNodeValueInt(regNode)
                            if not isOk:
                                return False                     
                        else:
                            pass
                    if hldVal is None:
                        return False
                    floatVal = float(hldVal)
                    theSettings[arc,afe] = floatVal
                    pass
                pass
            pass
        self.__settings.append(theSettings)
        theFile.close()
        return True

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
                    if vetoNode.nodeType != vetoNode.ELEMENT_NODE or vetoNode.nodeName != "acdRange":
                        continue                    
                    pedVal = float(AcdXmlUtil.getAttribute(vetoNode,"high_min"))
                    pedVal -= 20;
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

    def readCarbons(self,fileName):
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
                    if vetoNode.nodeType != vetoNode.ELEMENT_NODE or vetoNode.nodeName != "acdCarbon":
                        continue                    
                    peakVal = float(AcdXmlUtil.getAttribute(vetoNode,"peak"))
                    peakVal -= 40;
                    pmtName = ""
                    if pmt == '0':
                        pmtName = 'A'
                    elif pmt == '1':
                        pmtName = 'B'
                    (garc,gafe) = self.__theTileMap.tileDict[tileName][pmtName]
                    self.__carbons[garc,gafe] = peakVal 
                    pass
                pass
            pass
        theFile.close()
        return True

    

    def __readSetPointFile(self, fileName):
        """ read a single file with the veto turn on points in PHA
        """
        (theFile,doc) = AcdXmlUtil.openXmlFileByName(fileName)
        theSetPoints = Numeric.zeros((12,18),'f')
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
                    if vetoNode.nodeType != vetoNode.ELEMENT_NODE or vetoNode.nodeName != "acdCno":
                        continue                    
                    vetoVal = float(AcdXmlUtil.getAttribute(vetoNode,"cno"))
                    pmtName = ""
                    if pmt == '0':
                        pmtName = 'A'
                    elif pmt == '1':
                        pmtName = 'B'
                    (garc,gafe) = self.__theTileMap.tileDict[tileName][pmtName]
                    theSetPoints[garc,gafe] = vetoVal + self.__peds[garc,gafe]
                    pass
                pass
            pass
        self.__values.append(theSetPoints)
        theFile.close()
        return True
        
    def readFilePair(self, settingFile, setPointFile):
        """ Read a pair of files, one with settings and one with veto turn on in PHA
        """        
        if not self.__readSettingsFile(settingFile):
            return False
        if not self.__readSetPointFile(setPointFile):
            return False
        self.__n += 1
        return True

    def parseAndFit(self, garc, gafe,fout=None):
        """ Fit the veto responce for a single channel
        """
        vals = [0.]*self.__n
        sets = [0.]*self.__n
        nUsed = 0
        for i in range(self.__n):
            if self.__values[i][garc,gafe] > 1:                
                vals[nUsed] = self.__values[i][garc,gafe]
                sets[nUsed] = self.__settings[i][garc,gafe]
                nUsed += 1
        #fitter = AcdROOT_LineFit("GARC_%s_%s"%(garc,gafe),120,0,1000,40,65)
        fitter = AcdROOT_LineFit("GARC_%s_%s"%(garc,gafe),120,0,360,40,65)
        if fout is None:
            saveRoot = False 
        else:
            saveRoot = True 
        fitter.fit(nUsed,vals,sets,saveRoot)
        return fitter

    def fitGarc(self,timestamp,garc,draw=False,fout=None):
        """
        """
        canvas = None
	if fout is not None:
	    fout.cd()
        if draw:
            canvasName = "cnoFits_%s_garc_%d"%(timestamp,garc)
            canvas = ROOT.TCanvas(canvasName)
            canvas.Divide(3,6)
        for gafe in range(18):
            fitter = self.parseAndFit(garc,gafe,fout)
            self.__offsetArray[garc,gafe] = fitter._fit[0]
            self.__slopeArray[garc,gafe] = fitter._fit[1]
            if draw:
                canvas.cd(gafe+1)
                #fitter._pad.DrawCopy()
                fitter.draw(self.__peds[garc,gafe],self.__carbons[garc,gafe],[0.56,1.00])
            if fout is not None:
                fitter.write()
        return canvas

    def findSettingForValue(self,value,garc,gafe):
        """
        """
        if value < 0:
            return 0
        retVal = (value * self.__slopeArray[garc,gafe]) + self.__offsetArray[garc,gafe]
        return retVal

    def writeSettings(self,value,fileName):
        """
        """
        doc = AcdXmlUtil.makeDoc()

        topNode = AcdXmlUtil.makeChildNode(doc,"configuration")
        ts = time.asctime()        
        AcdXmlUtil.addComment(topNode,"created by %s on %s"%("AcdROOT_FitCnoGarc",ts))

        latNode = AcdXmlUtil.makeChildNode(topNode,"GLAT")
        for garc in range(12):
            garcNode = AcdXmlUtil.makeChildNode(latNode,"GARC")
            AcdXmlUtil.setAttribute(garcNode,"ID",garc)
            for gafe in range(18):
                gafeNode = AcdXmlUtil.makeChildNode(garcNode,"GAFE")
                AcdXmlUtil.setAttribute(gafeNode,"ID",gafe)
                valInPha = -1
                if self.__carbons[garc,gafe] > 0 :
                    valInPha = self.__peds[garc,gafe] + (value * self.__carbons[garc,gafe])
                setting = self.findSettingForValue(valInPha,garc,gafe)
                valNode = AcdXmlUtil.makeChildNodeAndTextNode(gafeNode,"hld_dac","%d"%setting)
                pass
            pass
        AcdXmlUtil.xmlToStream(doc,fileName)
        print "creating file",fileName
        return
  

    def saveGarc(self,timestamp,garc,draw,fout):
        """
        """
        canvas = self.fitGarc(timestamp,garc,draw,fout)
        if draw:
            canvasFileName = "%s.gif"%(canvas.GetName())
            canvas.SaveAs(canvasFileName)
        return

    def doGarcs(self,timestamp,draw,fout):
        """
        """
        for garc in range(12):
            self.saveGarc(timestamp,garc,draw,fout)
            pass
        return

    def writeXml(self,timeString,filePairs):
        """
        """
        doc = AcdXmlUtil.makeDoc()

        topNode = AcdXmlUtil.makeChildNode(doc,"acdCalib")
        ts = time.asctime()
        
        AcdXmlUtil.addComment(topNode,"created by %s on %s"%("AcdROOT_FitVetoGarc",ts))

        genNode = AcdXmlUtil.makeChildNode(topNode,"generic")

        AcdXmlUtil.setAttribute(genNode,"DTDVersion","v2r1")
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

        for (settings,veto) in filePairs:
            inputSettingFileNode = AcdXmlUtil.makeChildNode(inputNode,"inputFile")
            AcdXmlUtil.setAttribute(inputSettingFileNode,"type","CnoSettings")
            AcdXmlUtil.setAttribute(inputSettingFileNode,"path",settings)           
            inputVetoFileNode = AcdXmlUtil.makeChildNode(inputNode,"inputFile")            
            AcdXmlUtil.setAttribute(inputVetoFileNode,"type","CnoCalib")
            AcdXmlUtil.setAttribute(inputVetoFileNode,"path",veto)
      
      

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
                valNode = AcdXmlUtil.makeChildNode(pmtNode,"acdCno")
                (garc,gafe) = self.__theTileMap.tileDict[tile][pmt]
                AcdXmlUtil.setAttribute(valNode,"slope","%7.6f"%self.__slopeArray[garc,gafe])
                AcdXmlUtil.setAttribute(valNode,"offset","%7.6f"%self.__offsetArray[garc,gafe])
                AcdXmlUtil.setAttribute(valNode,"status","0")
        fileName = 'AcdCnoParams_%s.xml'%(timeString)
        AcdXmlUtil.xmlToStream(doc,fileName)
        print "creating file",fileName
        return
        

if __name__=='__main__':
    # argument parsing
    parser = OptionParser()

    parser.add_option("-t", "--time",action="store",
                      dest="time",type="string",default="",
                      help="Time stamp", metavar="TIME")

    parser.add_option("-d", "--draw",action="store_true",
                      dest="draw");
    
    parser.add_option("-i", "--input",action="append",
                      dest="input",type="string",
                      help="Input File Pairs", metavar="Settings:Values")

    parser.add_option("-p", "--peds",action="store",
                      dest="peds",type="string",default="",
                      help="Pedstal File", metavar="FILE")

    parser.add_option("-c", "--carbons",action="store",
                      dest="carbons",type="string",default="",
                      help="Carbon Peak File", metavar="FILE")

    
    (options, args) = parser.parse_args()

    # Latch the time
    timestring = options.time
    if timestring == "":
        timestring = "%.0f"%(time.mktime(time.localtime()) - time.mktime(time.strptime('Sun Dec 31 16:00:00 2000')))

    # do all the parsing 'n shit
    vetos = AcdOfflineVetoFitter()

    if options.peds != "":
        vetos.readPeds(options.peds)

    if options.carbons != "":
        vetos.readCarbons(options.carbons)    

    filePairs = []
    for aPair in options.input:
        files = aPair.split(':')
        if len(files) != 2:
            continue
        filePairs.append(files)

    for filePair in filePairs:
        if not vetos.readFilePair(filePair[0],filePair[1]):
            print "Failed to read %s %s"%(filePair[0],filePair[1])

   
    fout = ROOT.TFile("cno_fits.root","RECREATE")
    vetos.doGarcs(timestring,options.draw,fout)
    vetos.writeXml(timestring,filePairs)
    fout.Write()
    
    vetos.writeSettings(0.44,"ACD_Hld_AFE-Zeq4_calibGenACD.xml")
    vetos.writeSettings(0.70,"ACD_Hld_AFE-Zeq5_calibGenACD.xml")
    vetos.writeSettings(1.00,"ACD_Hld_AFE-Zeq6_calibGenACD.xml")