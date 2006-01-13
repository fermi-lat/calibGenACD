#ifndef AcdCalibBase_h
#define AcdCalibBase_h 

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TH1.h"
#include "TChain.h"
#include "TMath.h"
#include "TCollection.h"  // Declares TIter
#include <iostream>

class AcdHistCalibMap;
class AcdPedestalFit;
class AcdPedestalFitMap;
class AcdGainFit;
class AcdGainFitMap;
class DigiEvent;

class AcdCalibBase {

public :

  enum CALTYPE{PEDESTAL, GAIN, UNPAIRED};

public :

  // Standard ctor, where user provides the names the output histogram files
  AcdCalibBase();
  
  virtual ~AcdCalibBase();  
   
  // access functions

  // get the maps of the histograms to be fit
  inline AcdHistCalibMap* rawMap() { return m_rawMap; }
  inline AcdHistCalibMap* peakMap() { return m_peakMap; }
  inline AcdHistCalibMap* unPairedMap() { return m_unpairedMap; }
  
  // get the results maps
  inline AcdPedestalFitMap* getPedestals() { return m_pedestals; }
  inline AcdGainFitMap* getGains() { return m_gains; } 
   

  // trivial setting functions

  // Which type of calibration are we running
  inline CALTYPE calType() const { return m_calType; }
  inline void setCalType(CALTYPE t) { m_calType = t; }

  // read the pedestals from a file
  Bool_t readPedestals(const char* fileName);
  
  // these two just call down to the fitAll() routines in the fitters
  AcdPedestalFitMap* fitPedestals(AcdPedestalFit& fitter);
  AcdGainFitMap* fitGains(AcdGainFit& fitter);

  // this is here in case we can correct for the path length
  virtual Float_t reconCorrection(UInt_t /* face */ ) { 
    return 1.;
  }

   // this writes the output histograms if newFileName is not set, they will be writing to the currently open file
  Bool_t writeHistograms(CALTYPE type, const char* newFileName = 0);

protected:

  // This opens the output file and fills books the output histograms
  Bool_t bookHists(const char* fileName,
		   UInt_t nBinGain = 256, Float_t lowGain = -0.5, Float_t hiGain = 4095.5,
		   UInt_t nBinPed = 4096, Float_t lowPed = -0.5, Float_t hiPed = 4095.5);  

  // filling various histogram depending on m_calType
  void fillPedestalHist(int id, int pmtId, int pha);
  void fillGainHist(int id, int pmtId, float phaCorrect);
  void fillUnpairedHist(int id, int pmtId, int pha);

  // select events used to calibrate gain
  virtual Bool_t failCuts() { return kFALSE; }
  
private:

  // Which type of calibration are we getting the histograms for?
  CALTYPE m_calType;
  
  // These are the histograms
  AcdHistCalibMap* m_rawMap;
  AcdHistCalibMap* m_peakMap;
  AcdHistCalibMap* m_unpairedMap;

  // These are the results of the fits
  AcdPedestalFitMap* m_pedestals;
  AcdGainFitMap* m_gains;

  // This is the output file
  TFile* m_histFile;

  ClassDef(AcdCalibBase,0) ;
    
};

#endif
