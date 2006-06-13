
#ifndef AcdHistCalibMap_h
#define AcdHistCalibMap_h

#include "TH1.h"
#include "TList.h"
#include "AcdMap.h"

#include <map>
#include <string>
class TFile;

class AcdHistCalibMap {

public:

  AcdHistCalibMap(TFile& file);

  AcdHistCalibMap(const char* prefix, UInt_t nBins = 4096, Float_t lo = -0.5, Float_t hi = 4095.5);
  
  virtual ~AcdHistCalibMap();
  
  TH1* getHist(UInt_t key);

  const TList& histograms() const { 
    return m_list;
  }
  
  void ls() {
    m_list.Print();
  }

  Bool_t writeHistograms(const char* newFileName );


protected:

  void bookHists(const char* prefix);

private:

  UInt_t                 m_bins;
  Float_t                m_lo;
  Float_t                m_hi;

  TList                  m_list;
  std::map<UInt_t,TH1*>  m_map;
  
  ClassDef(AcdHistCalibMap,0) ;

};

#endif

#ifdef AcdHistCalibMap_cxx


AcdHistCalibMap::AcdHistCalibMap(const char* prefix, UInt_t nBins, Float_t lo, Float_t hi)
  :m_bins(nBins),
   m_lo(lo),
   m_hi(hi)
{  
  bookHists(prefix);
}

AcdHistCalibMap::~AcdHistCalibMap()
{
  // m_list.Delete();
}

TH1* AcdHistCalibMap::getHist(UInt_t key) {
  std::map<UInt_t,TH1*>::iterator itr = m_map.find(key);
  return itr == m_map.end() ? 0 : itr->second;
}

#endif // #ifdef AcdHistCalibMap_cxx
