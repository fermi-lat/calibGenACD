#
#                               Copyright 2007
#                                     by
#                        The Board of Trustees of the
#                     Leland Stanford Junior University.
#                            All rights reserved.
#

__facility__  = "calibGenACD"
__abstract__  = "ROOT based least squares fit to line"
__author__    = "E. Charles"
__date__      = "$Date$"
__version__   = "$Revision$, $Author$"
__release__   = "$Name$"

import os
import ROOT
from math import *
import numarray


def linear_fit(n, x, y):
    """ Simple Least Squares linear fit

    @param n number of points
    @param x array of x values
    @param y array of y values
    @return [offset,slope,linearRegression]
    """
    
    retVal = [0,0,0]
    if n < 2 :
        return retVal
    
    sumx = sumy = sumx2 = sumy2 = sumxy =0;
    for i in range(n):
        sumx += x[i]
        sumy += y[i]
        sumx2 += (x[i] * x[i])
        sumy2 += (y[i] * y[i])
        sumxy += (x[i] * y[i])

    sxx = sumx2 - sumx * sumx / n
    syy = sumy2 - sumy * sumy / n
    sxy = sumxy - sumx * sumy / n

    if abs(sxx) < 0.000001:
        return retVal

    slope = sxy / sxx
    intercept = (sumy / n) - (slope * sumx / n)

    regress = 1
    if syy != 0:
        regress = sxy / sqrt(sxx * syy);

    retVal[0] = intercept
    retVal[1] = slope
    retVal[2] = regress

    return retVal

class AcdROOT_LineFit:
    """ Interface to ROOT and plotting
    """    
    def __init__(self,name,bins=120,low=0,hi=1500,min=40.,max=65.):
        """ c'tor

        @param name of the histogram
        @param bins number of x bins
        @param low lowest bin edge
        @param hi highest bin edge
        @param min minimum y value
        @param max maximum y value        
        """
        self._name = name
        self._pad = ROOT.TH1F(self._name,self._name,bins,low,hi)
        self._min = min;
        self._max = max
        self._pad.SetMaximum(max)
        self._pad.SetMinimum(min)
        self._doneFit = False
        pass

    def setPoints(self,nPoint,array):
        """ Add points to Histogram

        @param nPoint number of points
        @param array of the points [x0,y0],[x1,y1],...
        """
        self._doneFit = False
        self._nPoint = nPoint
        self._xVals = [0.]*nPoint
        self._yVals = [0.]*nPoint
        self._polyMarker = ROOT.TPolyMarker().Clone()
        self._polyMarker.SetMarkerStyle(8)
        for i in range(nPoint):
            self._xVals[i] = float(i)
            self._yVals[i] = array[i]
            self._polyMarker.SetNextPoint(self._xVals[i],self._yVals[i])

    def fit(self,nPoint,xVals,yVals,addPoints=False):
        """ Fit points to line
        
        Also draw a Line on the Root histogram
        
        @param nPoint number of points
        @param xVals array of x values
        @param yVals array of y values
        """
        self._nPoint = nPoint
        self._xVals = xVals
        self._yVals = yVals
        self._polyMarker = ROOT.TPolyMarker().Clone()
        self._polyMarker.SetMarkerStyle(8)
        for i in range(nPoint):
            self._polyMarker.SetNextPoint(xVals[i],yVals[i])
            if addPoints:
                bin = self._pad.FindBin(xVals[i])
                self._pad.SetBinContent(bin,yVals[i])
                self._pad.SetBinError(bin,0.1)
        self._fit = linear_fit(nPoint,xVals,yVals)
        self._doneFit = True
        x0 = xVals[0]
        x1 = xVals[nPoint-1]
        y0 = self._fit[0] + (self._fit[1] * xVals[0])
        y1 = self._fit[0] + (self._fit[1] * x1)
        self._line = ROOT.TLine(x0,y0,x1,y1).Clone()
        self._line.SetLineWidth(3)
        self._line.SetLineColor(2)


    def draw(self,ped,mips,sLines):
        """ Draw the histogram and fitted line and points
        """
        self._pad.DrawCopy()
        if self._doneFit:
            self._line.Draw("l same")
        self._polyMarker.Draw("same")
        if mips > 0:
            self._pedLine = ROOT.TLine(ped,self._min,ped,self._max).Clone()
            self._pedLine.SetLineColor(3)
            self._pedLine.Draw("l same")
            
            self._mipLine = ROOT.TLine(ped+mips,self._min,ped+mips,self._max).Clone()
            self._mipLine.SetLineColor(4)
            self._mipLine.Draw("l same")

            for sL in sLines:                
                self._sLine = ROOT.TLine(ped+sL*mips,self._min,ped+sL*mips,self._max).Clone()
                self._sLine.SetLineColor(7)
                self._sLine.Draw("l same")            
            

    def drawPoints(self):
        """ Draw only the points
        """
        self._polyMarker.Draw("same")

    def dump(self):
        """ Print the fit values
        """
        print "%6.4f %6.4f"%(self._fit[0],self._fit[1])
   
    def write(self):
        """
        """
        self._pad.Write()
