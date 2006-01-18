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

  // this writes the output histograms if newFileName is not set, 
  // they will be writing to the currently open file
  Bool_t writeHistograms(CALTYPE type, const char* newFileName = 0);

  // get the id of the first and last events used
  Int_t evtId_first() const { return m_evtId_first; }
  Int_t evtId_last() const { return m_evtId_last; }
  Int_t runId_first() const { return m_runId_first; }
  Int_t runId_last() const { return m_runId_last; }

  /// starting event number in chain order
  Int_t startEvent() const { return m_startEvent; }
  
  /// number of events we ran over
  Int_t nTrigger() const { return m_nTrigger; }

  /// number of events we used
  Int_t nUsed() const { return m_nUsed; }

  /// run the event loop
  void go(int numEvents = 0, int startEvent = 0);

protected:

  // This opens the output file and fills books the output histograms
  Bool_t bookHists(const char* fileName,
		   UInt_t nBinGain = 256, Float_t lowGain = -0.5, Float_t hiGain = 4095.5,
		   UInt_t nBinPed = 4096, Float_t lowPed = -0.5, Float_t hiPed = 4095.5);  

  // filling various histogram depending on m_calType
  void fillPedestalHist(int id, int pmtId, int pha);
  void fillGainHist(int id, int pmtId, float phaCorrect);
  void fillUnpairedHist(int id, int pmtId, int pha);
  
  // print stuff every 1k events, keep track of the current event
  void logEvent(int ievent, Bool_t passedCut,int runId,int evtId); 

  // reset all the counters
  void resetCounters() {
    m_evtId_first = 0;
    m_evtId_last = 0;
    m_runId_first = 0;
    m_runId_last = 0;
    m_startEvent = 0;
    m_nTrigger = 0;
    m_nUsed = 0;
  }

  // cache the id of the first event
  void firstEvent(Int_t runId, Int_t evtId) {
    m_evtId_first = evtId;
    m_runId_first = runId;
  } 
  
  // cache the id of the last event
  void lastEvent(Int_t runId, Int_t evtId) {
    m_evtId_last = evtId;
    m_runId_last = runId;
  } 

  // return the total number of events in the chains
  virtual int getTotalEvents() const { return 0; } 

  // read in 1 event
  virtual Bool_t readEvent(int /*ievent*/, Bool_t& /*filtered*/, 
			   int& /*runId*/, int& /*evtId*/) {
    return kFALSE;
  }

  virtual void useEvent(Bool_t& /*used*/) {;}

private:

  /// the current run and event
  Int_t m_evtId;
  Int_t m_runId;

  /// store some information about what we ran over
  Int_t m_evtId_first;
  Int_t m_evtId_last;
  Int_t m_runId_first;
  Int_t m_runId_last;

  /// starting event number in chain order
  Int_t m_startEvent;
  
  /// number of events we ran over
  Int_t m_nTrigger;

  /// number of events we used
  Int_t m_nUsed;


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
