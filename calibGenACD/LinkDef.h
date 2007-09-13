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
#pragma link C++ class AcdCalibHistHolder;
#pragma link C++ class AcdHistCalibMap;

#pragma link C++ class AcdCalibResult;
#pragma link C++ class AcdCalibMap;

#pragma link C++ class AcdPadMap;
#pragma link C++ class AcdJobConfig;

#pragma link C++ class AcdGarcGafeHits;

#pragma link C++ class AcdGainFitResult;
#pragma link C++ class AcdGainFitMap;
#pragma link C++ class AcdGainFit;
#pragma link C++ class AcdGainFitLibrary;

#pragma link C++ class AcdPedestalFitResult;
#pragma link C++ class AcdPedestalFitMap;
#pragma link C++ class AcdPedestalFit;
#pragma link C++ class AcdPedestalFitLibrary;

#pragma link C++ class AcdCnoFitResult;
#pragma link C++ class AcdCnoFitMap;
#pragma link C++ class AcdCnoFit;
#pragma link C++ class AcdCnoFitLibrary;

#pragma link C++ class AcdRangeFitResult;
#pragma link C++ class AcdRangeFitMap;
#pragma link C++ class AcdRangeFit;
#pragma link C++ class AcdRangeFitLibrary;

#pragma link C++ class AcdVetoFitResult;
#pragma link C++ class AcdVetoFitMap;
#pragma link C++ class AcdVetoFit;
#pragma link C++ class AcdVetoFitLibrary;

#pragma link C++ class AcdStripFitResult;
#pragma link C++ class AcdStripFitMap;
#pragma link C++ class AcdStripFit;

#pragma link C++ class AcdCoherentNoiseFitResult;
#pragma link C++ class AcdCoherentNoiseFitMap;
#pragma link C++ class AcdCoherentNoiseFit;

#pragma link C++ class AcdCalibBase;

#pragma link C++ class AcdCalibLoop_Bench;
#pragma link C++ class AcdCalibLoop_Digi;
#pragma link C++ class AcdCalibLoop_Merit;
#pragma link C++ class AcdCalibLoop_Recon;
#pragma link C++ class AcdCalibLoop_Svac;

#pragma link C++ class AcdStripChart;
#pragma link C++ class AcdCoherentNoise;

#endif
