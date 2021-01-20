#ifndef AcdCalibBase_h
#define AcdCalibBase_h 


// local includes
#include "./AcdKey.h"
#include "./AcdCalibEnum.h"


#include "CalibData/Acd/AcdCalibEnum.h"


// stl includes
#include <map>
#include <vector>
#include <iostream>
#include <fstream>

// forward declares
class AcdHistCalibMap;
class AcdCalibMap;
class DomElement;
class AcdCalibFit;
class TChain;


/** 
 * @class AcdCalibEventStats
 *
 * @brief Holder to keep track of events we have processed
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibBase.h,v 1.21 2008/09/05 22:57:50 echarles Exp $
 */


class AcdCalibEventStats {
public :

  AcdCalibEventStats();
  virtual ~AcdCalibEventStats() {;};

  /// return the ID of the current event (during processing) 
  inline Int_t& evtId() { return m_evtId; }
  
  /// return the ID of the current run (during processing) 
  inline Int_t& runId() { return  m_runId; }

  /// return the ID of the first event processed 
  inline Int_t evtId_first() const { return m_evtId_first; }
  /// return the ID of the last event processed
  inline Int_t evtId_last() const { return m_evtId_last; }
  /// return the run ID of the first event processed
  inline Int_t runId_first() const { return m_runId_first; }
  /// return the run ID of the last event processed
  inline Int_t runId_last() const { return m_runId_last; }
  /// return the MET for the first event processed
  inline Double_t time_first() const { return m_time_first; }  
  /// return the MET for the last event processed
  inline Double_t time_last() const { return m_time_last; }

  /// return the index of the first event to process in the TChain
  inline Int_t startEvent() const { return m_startEvent; }

  /// return the index of the last event to process in the TChain
  inline Int_t last() const { return m_last; }

  /// return the number of events we ran over
  inline Int_t nTrigger() const { return m_nTrigger; }

  /// return the number of events that passed our prefilter
  inline Int_t nFilter() const { return m_nFilter; }

  /// return the number of events we used
  inline Int_t nUsed() const { return m_nUsed; }

  ///  print stuff every 1k events and keep track of the current event
  void logEvent(int ievent, Bool_t passedCut, Bool_t filtered, int runId, int evtId, Double_t timeStamp = 0.); 

  ///  reset all the counters 
  void resetCounters() {
    m_evtId_first = 0;
    m_evtId_last = 0;
    m_runId_first = 0;
    m_runId_last = 0;
    m_time_first = 0;
    m_time_last = 0;
    m_startEvent = 0;
    m_last = 0;
    m_nTrigger = 0;
    m_nFilter = 0;
    m_nUsed = 0;
  }

  ///  set the start and end evnts
  void setRange(Int_t startEvent, Int_t lastEvent) {
    m_startEvent = startEvent;
    m_last = lastEvent;
  }

  ///  cache some info about the first event
  void firstEvent() {
    m_evtId_first = m_evtId;
    m_runId_first = m_runId;
    m_time_first = m_evtTime;
  } 
  
  ///  cache some info about the last event
  void lastEvent() {
    m_evtId_last = m_evtId;
    m_runId_last = m_runId;
    m_time_last = m_evtTime;
  } 
  
private :
  
  /// The ID of the current event
  Int_t m_evtId;
  /// The ID of the current run
  Int_t m_runId;
  /// The timestamp (in MET) of the current event
  Double_t m_evtTime;

  /// The ID of the first event we saw
  Int_t m_evtId_first;
  /// The ID of the last event we saw  
  Int_t m_evtId_last;
  /// The run ID of the first event we saw
  Int_t m_runId_first;
  /// The run ID of the last event we saw
  Int_t m_runId_last;
  /// The timestamp (in MET) of the first event we saw
  Double_t m_time_first;
  /// The timestamp (in MET) of the last event we saw  
  Double_t m_time_last;


  /// starting event number in chain order
  Int_t m_startEvent;
  /// ending event number in chain order
  Int_t m_last;
  
  /// number of events we ran over
  Int_t m_nTrigger;
  /// number of events that passed the filter
  Int_t m_nFilter;
  /// number of events we used
  Int_t m_nUsed;

};


/** 
 * @class AcdCalibBase
 *
 * @brief AcdCalibration base class
 *
 * Mangages input data, calibration types and event looping.
 *
 * Basic strategy is to loop on events, filling histograms, 
 * then at the end of the event loop, do some fitting.
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibBase.h,v 1.21 2008/09/05 22:57:50 echarles Exp $
 */


class AcdCalibBase {

public :

  /// Standard ctor, where user provides the names the output histogram files
  AcdCalibBase(AcdCalibData::CALTYPE t, AcdKey::Config config = AcdKey::LAT);
  
  /// D'tor is a simple cleanup
  virtual ~AcdCalibBase();

   
  // access functions

  /// return the maps of the histograms to be fit for a given type of histogram
  AcdHistCalibMap* getHistMap(AcdCalib::HISTTYPE hType);
  /// return the maps of the histograms to be fit for a given type of histogram
  const AcdHistCalibMap* getHistMap(AcdCalib::HISTTYPE hType) const;

  ///  read and return the map of the histograms to be fit from a root file
  AcdHistCalibMap* readHistMap(AcdCalib::HISTTYPE hType, const char* fileName);

  /// return the calibration results for a given calibration type
  AcdCalibMap* getCalibMap(AcdCalibData::CALTYPE cType);
  /// return the calibration results for a given calibration type
  const AcdCalibMap* getCalibMap(AcdCalibData::CALTYPE cType) const;
  
  /// return the input data TChain of a given type
  TChain* getChain(AcdCalib::CHAIN chain);
  /// return the input data TChain of a given type
  const TChain* getChain(AcdCalib::CHAIN chain) const;

  ///  set the input data TChain of a given type
  void setChain(AcdCalib::CHAIN chain, TChain* tchain) {
    m_chains[chain] = tchain;
  }

  /// return which type of calibration are we running
  inline AcdCalibData::CALTYPE calType() const { return m_calType; }

  /// return which instrument are we calibrating (LAT or CU)
  inline AcdKey::Config getConfig() const { return m_config; }

  ///  read and return an input calibration of a given type from a file
  Bool_t readCalib(AcdCalibData::CALTYPE cType, const char* fileName);
  
  ///  run the event loop
  void go(int numEvents = 0, int startEvent = 0);

  void go_list(std::vector<int> EvtRecon);

  ///  fit all the histograms
  AcdCalibMap* fit(AcdCalibFit& fitter, AcdCalibData::CALTYPE cType, AcdCalib::HISTTYPE hType, 
		   const char* referenceFile = 0, AcdKey::ChannelSet cSet = AcdKey::All);

protected:

  ///  feed input data info into a CalibMap so it can be stored with the calibration
  void giveInfoToCalib(AcdCalibMap& theMap);

  /// return the event stats
  const AcdCalibEventStats& eventStats() const { return  m_eventStats; }
  /// return the event stats
  AcdCalibEventStats& eventStats() { return  m_eventStats; }

  ///  add a calibration
  void addCalibration(AcdCalibData::CALTYPE calibKey, AcdCalibMap& newCal);

  ///  book the output histograms
  AcdHistCalibMap* bookHists(AcdCalib::HISTTYPE histType, 
			     UInt_t nBin = 256, Float_t low = -0.5, Float_t hi = 4095.5, UInt_t nHist = 1);
  
  ///  fill the histogram for id:pmtId:idx with a value
  void fillHist(AcdHistCalibMap& histMap, int id, int pmtId, float val, UInt_t idx = 0);
  
  ///  set a bin in the histogram for id:pmtId:idx with val and err
  void fillHistBin(AcdHistCalibMap& histMap, int id, int pmtId, UInt_t binX, Float_t val, Float_t err, UInt_t idx=0);

  /// return the pedestal value for a channel
  float getPeds(UInt_t key) const;
  
  /// return the total number of events in input chain
  int getTotalEvents() const;

  ///  read in 1 event
  virtual Bool_t readEvent(int /*ievent*/, Bool_t& /*filtered*/, 
			   int& /*runId*/, int& /*evtId*/, Double_t& /*timestamp*/) {
    return kFALSE;
  }
  
  ///  try to use an event for an calibration
//  virtual void useEvent(Bool_t& /*used*/, std::ofstream& /*outfile*/, int /*ievent*/) {;}
  virtual void useEvent(Bool_t& /*used*/) {;}

private:

  /// which type of instrument
  AcdKey::Config m_config;
  
  /// Which type of calibration are we getting the histograms for?
  AcdCalibData::CALTYPE m_calType;
  
  /// Keep track of events 
  AcdCalibEventStats m_eventStats;

  /// These are the histograms, mapped by hist type
  /// index is given by AcdCalib::HISTTYPE
  std::vector<AcdHistCalibMap*> m_histMaps;

  /// These are the results of the fits, mapped by the calib type
  /// index is given by AcdCalib::CALTYPE
  std::vector<AcdCalibMap*> m_fitMaps;

  /// These are the various chains used by the analysis
  /// index is given by AcdCalib::CHAIN
  std::vector<TChain*> m_chains;

};

#endif
