# -*- python -*-
# $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/SConscript,v 1.13 2014/02/14 03:28:28 echarles Exp $ 
# Authors:  Eric Charles <echarles@slac.stanford.edu>
# Version: calibGenACD-06-00-00
Import('baseEnv')
Import('listFiles')
Import('packages')

if baseEnv['PLATFORM'] != "win32":
  progEnv = baseEnv.Clone()
  libEnv = baseEnv.Clone()

  libEnv.Tool('addLinkDeps', package='calibGenACD', toBuild='shared')
  calibGenACD = libEnv.SharedLibrary('calibGenACD',  listFiles(['src/*.cxx']))
  
  progEnv.Tool('calibGenACDLib')
  
  runPedestal = progEnv.Program('runPedestal',['apps/runPedestal.cxx'])
  runMipCalib = progEnv.Program('runMipCalib',['apps/runMipCalib.cxx'])
  runRangeCalib = progEnv.Program('runRangeCalib',['apps/runRangeCalib.cxx'])
  runVetoCalib = progEnv.Program('runVetoCalib',['apps/runVetoCalib.cxx'])
  runCnoCalib = progEnv.Program('runCnoCalib',['apps/runCnoCalib.cxx'])
  runCoherentNoiseCalib = progEnv.Program('runCoherentNoiseCalib',
                                          ['apps/runCoherentNoiseCalib.cxx'])
  runCarbonCalib=progEnv.Program('runCarbonCalib',['apps/runCarbonCalib.cxx'])
  runRibbonCalib=progEnv.Program('runRibbonCalib',['apps/runRibbonCalib.cxx'])
  runMeritCalib = progEnv.Program('runMeritCalib',['apps/runMeritCalib.cxx'])
  runHighPed = progEnv.Program('runHighPed',['apps/runHighPed.cxx'])
  runVetoFitCalib = progEnv.Program('runVetoFitCalib',
                                    ['apps/runVetoFitCalib.cxx'])
  runCnoFitCalib=progEnv.Program('runCnoFitCalib',['apps/runCnoFitCalib.cxx'])
  runHighRangeCalib = progEnv.Program('runHighRangeCalib',
                                      ['apps/runHighRangeCalib.cxx'])
  calibReport = progEnv.Program('calibReport',['apps/calibReport.cxx'])
  runCalibTrend = progEnv.Program('runCalibTrend',['apps/runCalibTrend.cxx'])
  makeResultTree=progEnv.Program('makeResultTree',['apps/makeResultTree.cxx'])

  runOverlayCalib = progEnv.Program('runOverlayCalib',['apps/runOverlayCalib.cxx'])
  runOverlayPedestal = progEnv.Program('runOverlayPedestal',['apps/runOverlayPedestal.cxx'])
  # runEfficCalib= progEnv.Program('runEfficCalib',['apps/runEfficCalib.cxx'])
  # runNoiseCalib= progEnv.Program('runNoiseCalib',['apps/runNoiseCalib.cxx'])


  progEnv.Tool('registerTargets', package = 'calibGenACD',
               libraryCxts = [[calibGenACD, libEnv]],
               binaryCxts  = [[runPedestal, progEnv],
                              [runMipCalib, progEnv],
                              [runRangeCalib, progEnv],
                              [runVetoCalib, progEnv],
                              [runCnoCalib, progEnv],
                              [runCoherentNoiseCalib, progEnv],
                              [runCarbonCalib, progEnv],
                              [runRibbonCalib, progEnv],
                              [runMeritCalib, progEnv],
                              [runHighPed, progEnv],
                              [runVetoFitCalib, progEnv],
                              [runCnoFitCalib, progEnv],
                              [runHighRangeCalib, progEnv],
                              [calibReport, progEnv],
                              [runCalibTrend, progEnv],
                              [makeResultTree, progEnv],
                              [runOverlayCalib, progEnv],
                              [runOverlayPedestal, progEnv] ],
               includes = listFiles(['calibGenACD/*.h']))
    




