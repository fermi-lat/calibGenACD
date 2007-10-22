#!/usr/bin/env python
#
#                               Copyright 2007
#                                     by
#                        The Board of Trustees of the
#                     Leland Stanford Junior University.
#                            All rights reserved.
#

__facility__  = "calibGenACD"
__abstract__  = "XML utilities for the ACD calibrations system"
__author__    = "E. Charles"
__date__      = "$Date$"
__version__   = "$Revision$, $Author$"
__release__   = "$Name$"


import Numeric

import AcdXmlUtil
from calibArray import calibArray

from AcdTileMapLookUp import *

acdMap = AcdTileMap()

class AcdCalibBase:
    """
    """
    def __init__(self,calibName,nodeName,arrayList):
        """
        """
        self.__name = calibName
        self.__nodeName = nodeName
        self.__arrayList = arrayList        
        for anArray in self.__arrayList:
            if len(anArray.arrayName()) > 0 and anArray.arrayType() != 'x':
                self.__dict__[anArray.arrayName()] = anArray
     
    def writeTileNode(self,domNode,tileName):
        """
        """
        tileNode = AcdXmlUtil.makeChildNode(domNode,'tile')
        AcdXmlUtil.setAttribute(tileNode,'tileId',tileName)
        
        garcGafeA =acdMap .tileDict[tileName]['A']

        pmtANode = AcdXmlUtil.makeChildNode(tileNode,'pmt')
        AcdXmlUtil.setAttribute(pmtANode,'iPmt',0)

        calibNode = AcdXmlUtil.makeChildNode(pmtANode,self.__nodeName)
        for x in self.__arrayList:
            AcdXmlUtil.setAttribute(calibNode,x.arrayName(),x.val()[garcGafeA])

        garcGafeB = acdMap.tileDict[tileName]['B']
        
        pmtANode = AcdXmlUtil.makeChildNode(tileNode,'pmt')
        AcdXmlUtil.setAttribute(pmtANode,'iPmt',1)

        calibNode = AcdXmlUtil.makeChildNode(pmtANode,self.__nodeName)
        for x in self.__arrayList:
            AcdXmlUtil.setAttribute(calibNode,x.arrayName(),x.val()[garcGafeB])

        return tileNode

 
    def writeXml(self,fileName):
        """
        """
        doc = AcdXmlUtil.makeDoc()
        topNode = AcdXmlUtil.makeChildNode(doc,'acdCalib')

        self.makeGenericNode( topNode )
        self.makeDimensionNode( topNode )

        for tile in acdMap.getTileNameList():            
            self.writeTileNode( topNode, tile )

        AcdXmlUtil.xmlToStream( doc, fileName )


    def makeGenericNode(self, domNode ):
        """
        """
        genNode = AcdXmlUtil.makeChildNode(domNode,'generic')
        
        AcdXmlUtil.setAttribute(genNode,'instrument','LAT')
        AcdXmlUtil.setAttribute(genNode,'timestamp','0')        
        AcdXmlUtil.setAttribute(genNode,'calibType',self.__name)
        AcdXmlUtil.setAttribute(genNode,'algorithm','MeanValue')
        AcdXmlUtil.setAttribute(genNode,'DTDVersion','v1r1')
        AcdXmlUtil.setAttribute(genNode,'fmtVersion','v1r1')
                
        inNode = AcdXmlUtil.makeChildNode(genNode,'inputSample')
        AcdXmlUtil.setAttribute(inNode,'startTime','0')
        AcdXmlUtil.setAttribute(inNode,'stopTime','0')
        AcdXmlUtil.setAttribute(inNode,'triggers','0')   
        AcdXmlUtil.setAttribute(inNode,'source','0')
        AcdXmlUtil.setAttribute(inNode,'mode','0')

                
    def makeDimensionNode(self, domNode ):
        """
        """
        dimNode = AcdXmlUtil.makeChildNode(domNode,'dimension')
        AcdXmlUtil.setAttribute(dimNode,'nTile','108')

                
if __name__ == '__main__':
    """
    """
    from tntReader import textNtupleReader
    
    baseName = "/afs/slac.stanford.edu/g/glast/ground/releases/monitor//ACD/CPT/060930_0_SA_Dry_2_cfg_1/"
    pedName = "AcdPedestal_Garc.tnt"
    hiName = "AcdTciHighRange.tnt"
    regName = "AcdTciRegRange.tnt"

    pedAttList = [('low_mean','f'),('low_rms','f'),('hi_mean','f'),('hi_rms','f')]
    hiAttList = [('low_slope','f'),('low_offset','f'),('','x'),('hi_slope','f'),('hi_offset','f'),('','x')]
    regAttList = [('','x'),('','x'),('','x'),('','x'),('','x'),('','x'),('','x'),('low_max','f'),('hi_min','f')]


    peds = textNtupleReader(pedAttList)
    peds.readFile( baseName+pedName )

    hiRange = textNtupleReader(hiAttList)
    hiRange.readFile( baseName+hiName )

    regRange = textNtupleReader(regAttList)
    regRange.readFile( baseName+regName )
    
    lowScale = regRange.low_max - peds.low_mean
    hiScale = regRange.hi_min - peds.hi_mean
    hiSat = hiRange.hi_slope * 100. + hiRange.hi_offset

    scaleRatio = lowScale/hiScale

    srArray = calibArray('slope','f',scaleRatio)
    pedArray = calibArray('pedestal','f',peds.hi_mean)
    satArray = calibArray('saturation','f',hiSat)

    aL = [srArray,pedArray,satArray]

    calib = AcdCalibBase('ACD_Ped','acdPed',aL)

    calib.writeXml(None)
    
    
