#ifndef AcdCalibBase_h
#define AcdCalibBase_h 


// local includes
#include "./AcdMap.h"
#include "./AcdCalibEnum.h"


#include "CalibData/Acd/AcdCalibEnum.h"


// stl includes
#include <map>
#include <vector>
#include <iostream>

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
 * $Header$
 */


class AcdCalibEventStats {
public :

  AcdCalibEventStats();
  virtual ~AcdCalibEventStats() {;};

  // Acceses to the event and run ID
  inline Int_t& evtId() { return m_evtId; }
  inline Int_t& runId() { return  m_runId; }

  // get the index of the last event
  inline Int_t last() const { return m_last; }

  // get the id of the first and last events used
  inline Int_t evtId_first() const { return m_evtId_first; }
  inline Int_t evtId_last() const { return m_evtId_last; }
  inline Int_t runId_first() const { return m_runId_first; }
  inline Int_t runId_last() const { return m_runId_last; }
  inline Double_t time_first() const { return m_time_first; }
  inline Double_t time_last() const { return m_time_last; }

  /// starting event number in chain order
  inline Int_t startEvent() const { return m_startEvent; }
  
  /// number of events we ran over
  inline Int_t nTrigger() const { return m_nTrigger; }

  /// number of events that passed the filter
  inline Int_t nFilter() const { return m_nFilter; }

  /// number of events we used
  inline Int_t nUsed() const { return m_nUsed; }

  // print stuff every 1k events, keep track of the current event
  void logEvent(int ievent, Bool_t passedCut, Bool_t filtered, int runId, int evtId, Double_t timeStamp = 0.); 

  /// reset all the counters 
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

  /// set the start event
  void setRange(Int_t startEvent, Int_t lastEvent) {
    m_startEvent = startEvent;
    m_last = lastEvent;
  }

  /// cache the id of the first event
  void firstEvent() {
    m_evtId_first = m_evtId;
    m_runId_first = m_runId;
    m_time_first = m_evtTime;
  } 
  
  /// cache the id of the last event
  void lastEvent() {
    m_evtId_last = m_evtId;
    m_runId_last = m_runId;
    m_time_last = m_evtTime;
  } 

private :

  // the current run and event
  Int_t m_evtId;
  Int_t m_runId;
  Double_t m_evtTime;

  // store some information about what we ran over
  Int_t m_evtId_first;
  Int_t m_evtId_last;
  Int_t m_runId_first;
  Int_t m_runId_last;
  Double_t m_time_first;
  Double_t m_time_last;
  Int_t m_last;

  /// starting event number in chain order
  Int_t m_startEvent;
  
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
 * Mangage input data, calibration types and event looping.
 *
 * Basic strategy is to loop on events, filling histograms, 
 * then at the end of the event loop, do some fitting.
 *
 * @author Eric Charles
 * $Header$
 */


class AcdCalibBase {

public :

  /// Standard ctor, where user provides the names the output histogram files
  AcdCalibBase(AcdCalibData::CALTYPE t, AcdMap::Config config = AcdMap::LAT);
  
  virtual ~AcdCalibBase();
   
  // access functions

  /// get the maps of the histograms to be fit
  AcdHistCalibMap* getHistMap(AcdCalib::HISTTYPE hType);
  const AcdHistCalibMap* getHistMap(AcdCalib::HISTTYPE hType) const;

  /// Read the map of the histograms to be fit from a root file
  AcdHistCalibMap* readHistMap(AcdCalib::HISTTYPE hType, const char* fileName);

  /// get the results maps
  AcdCalibMap* getCalibMap(AcdCalibData::CALTYPE cType);
  const AcdCalibMap* getCalibMap(AcdCalibData::CALTYPE cType) const;
  
  /// get a particular chain
  TChain* getChain(AcdCalib::CHAIN chain);
  const TChain* getChain(AcdCalib::CHAIN chain) const;
  void setChain(AcdCalib::CHAIN chain, TChain* tchain) {
    m_chains[chain] = tchain;
  }

  /// Which type of calibration are we running
  inline AcdCalibData::CALTYPE calType() const { return m_calType; }

  /// Which instrument calibration?
  inline AcdMap::Config getConfig() const { return m_config; }

  /// read an input calibration (usually pedestals) from a file
  Bool_t readCalib(AcdCalibData::CALTYPE cType, const char* fileName);
  
  /// this writes the output histograms
  /// if newFileName is not set they will be written to the currently open file
  Bool_t writeHistograms(AcdCalib::HISTTYPE histType, const char* newFileName = 0);

  /// run the event loop
  void go(int numEvents = 0, int startEvent = 0);

  /// for writing output files
  virtual void writeXmlHeader(DomElement& node) const;
  virtual void writeTxtHeader(std::ostream& os) const;

  /// for writing the list of input files  
  virtual void writeXmlSources(DomElement& node) const;
  virtual void writeTxtSources(std::ostream& os) const;

  /// do the fitting
  AcdCalibMap* fit(AcdCalibFit& fitter, AcdCalibData::CALTYPE cType, AcdCalib::HISTTYPE hType);

protected:

  /// add a calibration
  void addCalibration(AcdCalibData::CALTYPE calibKey, AcdCalibMap& newCal);

  /// This opens the output file and fills books the output histograms
  AcdHistCalibMap* bookHists(AcdCalib::HISTTYPE histType, UInt_t nBin = 256, Float_t low = -0.5, Float_t hi = 4095.5, UInt_t nHist = 1);
  
  /// fill the histogram for id:pmtId with a value
  void fillHist(AcdHistCalibMap& histMap, int id, int pmtId, float val, UInt_t idx = 0);
  
  /// set a bin in a histogram 
  void fillHistBin(AcdHistCalibMap& histMap, int id, int pmtId, UInt_t binX, Float_t val, Float_t err, UInt_t idx=0);

  /// get the pedestal for a channel
  float getPeds(UInt_t key) const;
  
  const AcdCalibEventStats& eventStats() const { return  m_eventStats; }
  AcdCalibEventStats& eventStats() { return  m_eventStats; }

  /// Write the calibration data to a stream
  void writeCalibTxt(std::ostream& os, AcdCalibData::CALTYPE cType) const;

  /// Write the input chain data to a stream
  void writeChainTxt(std::ostream& os, AcdCalib::CHAIN chain) const;

  /// Write the calibration data to a DOM node
  void writeCalibXml(DomElement& node, AcdCalibData::CALTYPE cType) const;

  /// Write the input chain  data to a DOM node
  void writeChainXml(DomElement& node, AcdCalib::CHAIN chain) const;

  /// return the total number of events in the chains
  int getTotalEvents() const;

  /// read in 1 event
  virtual Bool_t readEvent(int /*ievent*/, Bool_t& /*filtered*/, 
			   int& /*runId*/, int& /*evtId*/, Double_t& /*timestamp*/) {
    return kFALSE;
  }
  
  /// Try to use an event for an calibration
  virtual void useEvent(Bool_t& /*used*/) {;}

private:


  /// which type of instrument
  AcdMap::Config m_config;
  
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
