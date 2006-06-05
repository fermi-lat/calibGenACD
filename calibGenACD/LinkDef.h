/** @file LinkDef.h
    @brief for rootcint
 $Header$

*/
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class AcdCalibVersion;
#pragma link C++ class AcdCalibUtil;
#pragma link C++ class AcdMap;
#pragma link C++ class AcdHistCalibMap;

#pragma link C++ class AcdCalibResult;
#pragma link C++ class AcdCalibMap;

#pragma link C++ class AcdGainFitResult;
#pragma link C++ class AcdGainFitMap;
#pragma link C++ class AcdGainFit;
#pragma link C++ class AcdGainFitLibrary;

#pragma link C++ class AcdPedestalFitResult;
#pragma link C++ class AcdPedestalFitMap;
#pragma link C++ class AcdPedestalFit;
#pragma link C++ class AcdPedestalFitLibrary;

#pragma link C++ class AcdVetoFitResult;
#pragma link C++ class AcdVetoFitMap;
#pragma link C++ class AcdVetoFit;
#pragma link C++ class AcdVetoFitLibrary;

#pragma link C++ class AcdCalibBase;
#pragma link C++ class AcdMuonBenchCalib;
#pragma link C++ class AcdMuonRoiCalib;
#pragma link C++ class AcdMuonTkrCalib;
#pragma link C++ class AcdMuonSvacCalib;
#pragma link C++ class AcdMeritCalib;
#pragma link C++ class AcdStripChart;

#endif
