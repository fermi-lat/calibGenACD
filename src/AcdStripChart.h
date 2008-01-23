#ifndef AcdStripChart_h
#define AcdStripChart_h 

// base class
#include "AcdCalibBase.h"

// stl includes
#include <iostream>
#include <set>

// forward declares
class AcdDigi;
class DigiEvent;

/** 
 * @class AcdStripChart
 *
 * @brief AcdCalibration to fill time series strip charts
 *
 * FIXME more here
 *
 * @author Eric Charles
 * $Header$
 */

class AcdStripChart : public AcdCalibBase {

public :

  /// Standard ctor, where user provides the input data
  AcdStripChart(TChain* digiChain, UInt_t nBins, const char* timeStampFile = "timestamps.txt");
  
  virtual ~AcdStripChart();  

protected:

  /// setup input data
  Bool_t attachChains();

  ///
  void accumulate(int ievent, const AcdDigi& digi);

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);


private:

  // Local stash
  UInt_t m_nBins;
  UInt_t m_nEvtPerBin;
  mutable UInt_t m_currentCount;
  mutable UInt_t m_currentBin;

  /// pointer to a ReconEvent
  DigiEvent* m_digiEvent;

  // The strip charts
  AcdHistCalibMap* m_phaStrip;
  AcdHistCalibMap* m_hitStrip;
  AcdHistCalibMap* m_vetoStrip;

  // stashed values
  mutable std::map<UInt_t,std::multiset<Double_t> > m_vals;

  mutable std::ostream* m_timeStampLog;
    
};

#endif
