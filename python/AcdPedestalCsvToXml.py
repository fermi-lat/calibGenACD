#!/usr/bin/env python
''' Python DB API 2.0 driver lib. 
'''    

__facility__  = "Config System"
__abstract__  = "ACD pedestal calibration ancillary file XML readers"
__author__    = "E. Charles"
__date__      = "$Date$"
__version__   = "$Revision$, $Author$"
__release__   = "$Name$"

import sys,os
import numpy.oldnumeric as Numeric

from AcdTileMapLookUp import AcdTileMap
import AcdXmlUtil

class AcdPedestalCsvToXml:
  """
  """  
  def __init__(self):
    self.clear()

  def clear(self):
    """
    """
    self._loPed = Numeric.zeros((12,18),'f')
    self._loRms = Numeric.zeros((12,18),'f')
    self._hiPed = Numeric.zeros((12,18),'f')
    self._hiRms = Numeric.zeros((12,18),'f')
    

  def readCsvFiles(self,baseName):
    """
    """
    garcList = ("202","194","068","200","067","199","196","197","066","195","070","201")
    for garc in range(12):
        gId = garcList[garc]
        fileName = "%sGarc%s.csv"%(baseName,gId)
        setfile = open(fileName)
        inline = setfile.readline()
        start = False
        while inline<>'':
          splitList = inline.split(',')
          if start and splitList[0]<>'':
            gafe =int(splitList[0])
            loPed = float(splitList[1])
            loRms = float(splitList[2])
            hiPed = float(splitList[3])
            hiRms = float(splitList[4])
            self._loPed[garc,gafe]= loPed
            self._loRms[garc,gafe]= hiRms
            self._hiPed[garc,gafe]= loPed
            self._hiRms[garc,gafe]= hiRms
          if splitList[0].strip().lower()=='gafe':
            start=True                
          inline = setfile.readline()
            

  def writeXmlFile(self,lowRange,fileName=sys.stdout):
    """ 
    """
    if fileName is None:
      return False
    
    outDoc = AcdXmlUtil.makeDoc()
    topLvl = AcdXmlUtil.makeChildNode(outDoc,'acdCalib')

    genericNode = AcdXmlUtil.makeChildNode(topLvl,'generic')
    AcdXmlUtil.setAttribute(genericNode,'instrument','LAT')
    
    AcdXmlUtil.setAttribute(genericNode,'timestamp','Tue May  2 18:29:02 2006')
    AcdXmlUtil.setAttribute(genericNode,'calibType','ACD_Pedestal')
    AcdXmlUtil.setAttribute(genericNode,'algorithm','MeanValue')
    AcdXmlUtil.setAttribute(genericNode,'DTDVersion','v2r1')
    AcdXmlUtil.setAttribute(genericNode,'FMTVersion','v2r1')

    inputNode = AcdXmlUtil.makeChildNode(genericNode,'inputSample')
    AcdXmlUtil.setAttribute(inputNode,'startTime','060101120000')
    AcdXmlUtil.setAttribute(inputNode,'stopTime','060101120000')
    AcdXmlUtil.setAttribute(inputNode,'triggers','0')
    
    AcdXmlUtil.setAttribute(inputNode,'source','LCI')
    AcdXmlUtil.setAttribute(inputNode,'mode','LCI')

    dimNode = AcdXmlUtil.makeChildNode(topLvl,'dimension')
    AcdXmlUtil.setAttribute(inputNode,'nTile','108')    
    
    tileMap = AcdTileMap

    tileList = tileMap.tileDict.keys()
    tileList.sort()
    for tile in tileList:
      tileNode = AcdXmlUtil.makeChildNode(topLvl,'tile')
      AcdXmlUtil.setAttribute(tileNode,'tileId',tile)
      for pmt in ['A','B']:
        (garc,gafe) = tileMap.tileDict[tile][pmt]
        pedVal = 0.
        pedRms = 0.
        if lowRange:
            pedVal = self._loPed[garc][gafe]
            rmsVal = self._loRms[garc][gafe]
        else:
            pedVal = self._hiPed[garc][gafe]
            rmsVal = self._hiRms[garc][gafe]
        pmtNode = AcdXmlUtil.makeChildNode(tileNode,'pmt')
        if pmt == 'A':
          pmtId = 0
        elif pmt == 'B':
          pmtId = 1
        AcdXmlUtil.setAttribute(pmtNode,'iPmt',"%i"%pmtId)
        valNode = AcdXmlUtil.makeChildNode(pmtNode,'acdPed')
        AcdXmlUtil.setAttribute(valNode,'mean',"%.1f"%pedVal)
        AcdXmlUtil.setAttribute(valNode,'width',"%.1f"%rmsVal)
        AcdXmlUtil.setAttribute(valNode,'status',"0")
        pass
      pass

    AcdXmlUtil.xmlToStream(outDoc,fileName)


if __name__=='__main__':
  # argument parsing
  import sys
  reader = AcdPedestalCsvToXml()
  reader.readCsvFiles(sys.argv[1])
  reader.writeXmlFile(True,"lci_peds_low.xml")
  reader.writeXmlFile(False,"lci_peds_high.xml")
