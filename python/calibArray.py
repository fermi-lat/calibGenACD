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

class calibArray:
    """
    """
    def __init__(self,attName,attType,val=None):
        """
        """
        self.__name = attName
        self.__attType = attType
        if val is None:
            self.__val = Numeric.zeros((12,18),attType)
        else:
            self.__val = val

    def arrayType(self):
        """
        """
        return self.__attType

    def arrayName(self):
        """
        """
        return self.__name
        
    def val(self):
        """
        """
        return self.__val

