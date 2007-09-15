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


// this is just for stupid CINT
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



class AcdCoherentNoise : public AcdCalibBase {

public :
  
  // Standard ctor, where user provides the names of the input root files
  // and optionally the name of the output ROOT histogram file
  AcdCoherentNoise(TChain* digiChain, UInt_t loDT = 529, UInt_t hiDT = 2529, UInt_t nBins = 100, 
		   AcdMap::Config config = AcdMap::LAT);
  
  virtual ~AcdCoherentNoise();  

  void fillHistograms();
  
protected:

  Bool_t attachChains();

  // 
  void accumulate(Int_t deltaT, const AcdDigi& digi);

  // read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  virtual void useEvent(Bool_t& used);

private:

  // Local stash
  UInt_t m_loDT;
  UInt_t m_hiDT;
  UInt_t m_nBins;
  Float_t m_binSize;
  mutable Int_t m_currentBin;

  /// pointer to a ReconEvent
  DigiEvent* m_digiEvent;

  // The strip charts
  AcdHistCalibMap* m_histMap;

  // stashed values
  mutable std::map<UInt_t, AcdBinDataMap> m_vals;

  ClassDef(AcdCoherentNoise,0) ;
    
};

#endif
