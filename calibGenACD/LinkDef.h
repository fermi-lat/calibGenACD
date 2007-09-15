/** @file LinkDef.h
    @brief for rootcint
 $Header$

*/
#ifdef __CINT__

#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

#pragma link C++ class AcdCalibVersion;
#pragma link C++ class AcdMap;
#pragma link C++ class AcdPadMap;
#pragma link C++ class AcdCalibHistHolder;
#pragma link C++ class AcdHistCalibMap;

#pragma link C++ class AcdCalibDescription;
#pragma link C++ class AcdCalibResult;
#pragma link C++ class AcdCalibMap;
#pragma link C++ class AcdCalibFit;

#pragma link C++ class AcdCalibUtil;

#pragma link C++ class AcdJobConfig;

#pragma link C++ class AcdGarcGafeHits;

#pragma link C++ class AcdGainFitDesc;
#pragma link C++ class AcdGainFitLibrary;

#pragma link C++ class AcdPedestalFitDesc;
#pragma link C++ class AcdPedestalFitLibrary;

#pragma link C++ class AcdCnoFitDesc;
#pragma link C++ class AcdCnoFitLibrary;

#pragma link C++ class AcdRangeFitDesc;
#pragma link C++ class AcdRangeFitLibrary;

#pragma link C++ class AcdVetoFitDesc;
#pragma link C++ class AcdVetoFitLibrary;

#pragma link C++ class AcdStripFitDesc;
#pragma link C++ class AcdStripFitLibrary;

#pragma link C++ class AcdCoherentNoiseFitDesc;
#pragma link C++ class AcdCoherentNoiseFitLibrary;

#pragma link C++ class AcdCalibEventStats;
#pragma link C++ class AcdCalibBase;

#pragma link C++ class AcdCalibLoop_Bench;
#pragma link C++ class AcdCalibLoop_Digi;
#pragma link C++ class AcdCalibLoop_Merit;
#pragma link C++ class AcdCalibLoop_Recon;
#pragma link C++ class AcdCalibLoop_Svac;

#pragma link C++ class AcdStripChart;
#pragma link C++ class AcdCoherentNoise;

#endif
