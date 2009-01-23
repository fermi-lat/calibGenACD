# -*- python -*-
# $Header$ 
# Authors:  Eric Charles <echarles@slac.stanford.edu>
# Version: calibGenACD-04-01-03
Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('calibGenACDLib', depsOnly = 1)
calibGenACD = libEnv.SharedLibrary('calibGenACD',  listFiles(['src/*.cxx']))

progEnv.Tool('calibGenACDLib')

runPedestal = progEnv.Program('runPedestal',['apps/runPedestal.cxx'])
runMipCalib = progEnv.Program('runMipCalib',['apps/runMipCalib.cxx'])
runRangeCalib = progEnv.Program('runRangeCalib',['apps/runRangeCalib.cxx'])
runVetoCalib = progEnv.Program('runVetoCalib',['apps/runVetoCalib.cxx'])
runCnoCalib = progEnv.Program('runCnoCalib',['apps/runCnoCalib.cxx'])
runCoherentNoiseCalib = progEnv.Program('runCoherentNoiseCalib',['apps/runCoherentNoiseCalib.cxx'])
runCarbonCalib = progEnv.Program('runCarbonCalib',['apps/runCarbonCalib.cxx'])
runRibbonCalib = progEnv.Program('runRibbonCalib',['apps/runRibbonCalib.cxx'])
runMeritCalib = progEnv.Program('runMeritCalib',['apps/runMeritCalib.cxx'])
runHighPed = progEnv.Program('runHighPed',['apps/runHighPed.cxx'])
runVetoFitCalib = progEnv.Program('runVetoFitCalib',['apps/runVetoFitCalib.cxx'])
runCnoFitCalib = progEnv.Program('runCnoFitCalib',['apps/runCnoFitCalib.cxx'])
runHighRangeCalib = progEnv.Program('runHighRangeCalib',['apps/runHighRangeCalib.cxx'])
calibReport = progEnv.Program('calibReport',['apps/calibReport.cxx'])
runCalibTrend = progEnv.Program('runCalibTrend',['apps/runCalibTrend.cxx'])
makeResultTree = progEnv.Program('makeResultTree',['apps/makeResultTree.cxx'])
# runEfficCalib = progEnv.Program('runEfficCalib',['apps/runEfficCalib.cxx'])
# runNoiseCalib = progEnv.Program('runNoiseCalib',['apps/runNoiseCalib.cxx'])


progEnv.Tool('registerObjects', package = 'calibGenACD', libraries = [calibGenACD],
             binaries  = [runPedestal,
                          runMipCalib,
                          runRangeCalib,
                          runVetoCalib,
                          runCnoCalib,
                          runCoherentNoiseCalib,
                          runCarbonCalib,
                          runRibbonCalib,
                          runMeritCalib,
                          runHighPed,
                          runVetoFitCalib,
                          runCnoFitCalib,
                          runHighRangeCalib,
                          calibReport,
                          runCalibTrend,
                          makeResultTree],
             includes = listFiles(['calibGenACD/*.h']))




