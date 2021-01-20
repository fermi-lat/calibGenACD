#ifndef AcdCoherentNoise_h
#define AcdCoherentNoise_h 

// base class
#include "AcdCalibBase.h"

// stl includes
#include <iostream>
#include <set>
#include <map>
#include <vector>

// forward declares
class AcdDigi;
class DigiEvent;

/** 
 * @class AcdBinDataMap
 *
 * @brief a class to keep CINT from chocking 
 * 
 * CINT died on std::map<UInt_t, std::vector< std::multiset<Double_t> > > 
 * So this takes care of that.
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCoherentNoise.h,v 1.8 2008/08/01 00:08:25 echarles Exp $
 */

class AcdBinDataMap {
public:
  AcdBinDataMap(Int_t val=10)
    :m_val(val){
    // make damn sure
    m_val.resize(val);
  }
  AcdBinDataMap(const AcdBinDataMap& other)
    :m_val(other.val()){
  }
  AcdBinDataMap& operator=(const AcdBinDataMap& other) {
    if ( this != &other ) {
      m_val = other.val();
    }
    return *this;
  }
  virtual ~AcdBinDataMap() {;}
  inline const std::vector< std::multiset<Double_t> >& val() const { return m_val; }
  inline std::vector< std::multiset<Double_t> >& val() { return m_val; }
private:
  std::vector< std::multiset<Double_t> > m_val;
};



/** 
 * @class AcdCoherentNoise
 *
 * @brief AcdCalibration to fit coherent readout noise
 *
 * This Event Loop looks only at periodic triggers and accumulates the PHA values 
 * according to the time since the last read out event (GemDeltaEventTime)
 * 
 * At the end of the loop each of the histograms filled as a profile with the mean and error for 
 * each of the time bins
 *
 * Those profile plots are then fit to extract the coherent noise parameters
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCoherentNoise.h,v 1.8 2008/08/01 00:08:25 echarles Exp $
 */

class AcdCoherentNoise : public AcdCalibBase {

public :
  
  /// Standard ctor, where user provides the input data
  AcdCoherentNoise(TChain* digiChain, UInt_t loDT = 529, UInt_t hiDT = 2529, UInt_t nBins = 100, 
		   AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdCoherentNoise();  

  /// Fill the profile histograms at the end of the event loop
  void fillHistograms();
  
protected:

  /// setup input data
  Bool_t attachChains();

  /// Add data to histogram 
  void accumulate(Int_t deltaT, const AcdDigi& digi);

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId, Double_t& timestamp);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);

private:

  /// Low end of time distribution, used to calculate binning
  UInt_t m_loDT;
  /// High end of time distribution, used to calculate binning
  UInt_t m_hiDT;
  /// Number of time bins
  UInt_t m_nBins;
  /// Bin size, used to calculate binning
  Float_t m_binSize;

  /// Time bin for current event, cached for efficiency
  mutable Int_t m_currentBin;

  /// pointer to a DigiEvent
  DigiEvent* m_digiEvent;

  /// The strip charts
  AcdHistCalibMap* m_histMap;

  /// stashed PHA value for each channel
  mutable std::map<UInt_t, AcdBinDataMap> m_vals;

};

#endif
