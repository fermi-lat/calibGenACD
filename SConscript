# -*- python -*-
# $Header$ 
# Authors:  Eric Charles <echarles@slac.stanford.edu>
# Version: calibGenACD-04-00-00
Import('baseEnv')
Import('listFiles')
Import('packages')
progEnv = baseEnv.Clone()
libEnv = baseEnv.Clone()

libEnv.Tool('calibGenACDLib', depsOnly = 1)
calibGenACD = libEnv.SharedLibrary('calibGenACD',  listFiles(['src/*.cxx']))

progEnv.Tool('calibGenACDLib')
progEnv.Tool('addLibrary', library = baseEnv['rootLibs'])

runPedestal = progEnv.Program('runPedestal',[ 'apps/runPedestal.cxx'])
runMipCalib = progEnv.Program('runMipCalib',[ 'apps/runMipCalib.cxx'])
runRangeCalib = progEnv.Program('runRangeCalib',[ 'apps/runRangeCalib.cxx'])
runVetoCalib = progEnv.Program('runVetoCalib',[ 'apps/runVetoCalib.cxx'])
runCnoCalib = progEnv.Program('runCnoCalib',[ 'apps/runCnoCalib.cxx'])
runCoherentNoiseCalib = progEnv.Program('runCoherentNoiseCalib',[ 'apps/runCoherentNoiseCalib.cxx'])
runCarbonCalib = progEnv.Program('runCarbonCalib',[ 'apps/runCarbonCalib.cxx'])
convertXmlToTxt = progEnv.Program('convertXmlToXml',[ '../apps/convertXmlToTxt.cxx'])

progEnv.Tool('registerObjects', package = 'calibGenACD', libraries = [calibGenACD], binaries  = [runPedestal, runMipCalib,
                                                                                                 runRangeCalib,runVetoCalib, runCnoCalib,
                                                                                                 runCoherentNoiseCalib, runCarbonCalib,
                                                                                                 convertXmlToTxt], includes = listFiles(['calibGenACD/*.h']))
