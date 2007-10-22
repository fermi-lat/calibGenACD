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
from calibArray import calibArray


class textNtupleReader:
    """
    """
    def __init__(self,attList):
        """
        """
        self.__attList = attList
        self.__arrayList = []
        for (attName,attType) in self.__attList:
            if len(attName) > 0 and attType != 'x':
                c = calibArray(attName,attType)
            else:
                c = None            
            self.__arrayList.append(c)
            if c is not None:
                self.__dict__[attName] = c.val()
            
    def readFile(self,fileName):
        """
        """
        f = open(fileName)
        aLine = f.readline()
        while len(aLine) > 0:
            fields = aLine.split()
            (garc,gafe) = (int(fields[0]),int(fields[1]))
            for i in range(2,len(fields)):
                (attName,attType) = self.__attList[i-2]
                castVal = None
                if attType == 'x' or len(attName) == 0:
                    continue
                if attType == 'f':
                    castVal = float( fields[i] )
                elif attType == 'i':
                    castVal = int( fields[i] )
                self.__dict__[attName][garc,gafe] = castVal
                pass
            aLine = f.readline()
            pass
        f.close()

    def dump(self):
        """
        """
        for (attName,attType) in self.__attList:
            print attName
            print self.__dict__[attName].val()
        


