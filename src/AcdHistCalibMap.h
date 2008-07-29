
#ifndef AcdHistCalibMap_h
#define AcdHistCalibMap_h

// local includes
#include "AcdKey.h"

// ROOT includes
#include "TH1.h"
#include "TList.h"

// stl includes
#include <map>
#include <vector>
#include <string>

// forward declares
class TFile;

/** 
 * @class AcdCalibHistHolder
 *
 * @brief Group a bunch of histograms
 *
 * @author Eric Charles
 * $Header$
 */

class  AcdCalibHistHolder {
public:
  AcdCalibHistHolder(){;}  
  virtual ~AcdCalibHistHolder(){;}
  TH1* getHist(UInt_t idx) const;
  void addHist(TH1& hist);  
private:
  std::vector<TH1*> m_hists;     // NOT owned
};

/** 
 * @class AcdHistCalibMap
 *
 * @brief Map between channel ID and groups of histograms 
 *
 * All mapping is done using a decimal key:
 *    1000 * pmt + 100*face + 10*row + col
 *
 * @author Eric Charles
 * $Header$
 */


class AcdHistCalibMap {

public:

  /// Build by attaching a map to a TFile, 
  AcdHistCalibMap(TFile& file, AcdKey::Config config = AcdKey::LAT);

  /// Build with histogram details
  AcdHistCalibMap(const char* prefix, UInt_t nBins = 4096, Float_t lo = -0.5, Float_t hi = 4095.5, 
		  AcdKey::Config config = AcdKey::LAT, UInt_t n=1);
  
  virtual ~AcdHistCalibMap();
  
  /// Get a histogram by key and index
  TH1* getHist(UInt_t key, UInt_t idx=0);

  /// Get a histogram holder by key
  AcdCalibHistHolder* getHolder(UInt_t key);

  const std::map<UInt_t,AcdCalibHistHolder>& theMap() const {
    return m_map;
  }

  const TList& histograms() const { 
    return m_list;
  }
  
  void ls() {
    m_list.Print();
  }

  /// Write histograms to a file
  Bool_t writeHistograms(const char* newFileName );

  AcdKey::Config config() const { return m_config; }

protected:

  /// Make one histogram for each channel
  void bookHists(const char* prefix, UInt_t n=1);

private:

  AcdKey::Config         m_config;

  UInt_t                 m_bins;
  Float_t                m_lo;
  Float_t                m_hi;

  TList                  m_list;
  std::map<UInt_t,AcdCalibHistHolder>  m_map;
  
};

#endif

#ifdef AcdHistCalibMap_cxx


AcdHistCalibMap::AcdHistCalibMap(const char* prefix, 
				 UInt_t nBins, Float_t lo, Float_t hi, AcdKey::Config config, UInt_t n)
  :m_config(config),
  m_bins(nBins),m_lo(lo),m_hi(hi){
  bookHists(prefix,n);
}



AcdHistCalibMap::~AcdHistCalibMap()
{
  // m_list.Delete();
}

TH1* AcdHistCalibMap::getHist(UInt_t key, UInt_t idx) {
  std::map<UInt_t,AcdCalibHistHolder>::iterator itr = m_map.find(key);
  return itr == m_map.end() ? 0 : itr->second.getHist(idx);
}

AcdCalibHistHolder* AcdHistCalibMap::getHolder(UInt_t key) {
  std::map<UInt_t,AcdCalibHistHolder>::iterator itr = m_map.find(key);
  return itr == m_map.end() ? 0 : &(itr->second);
}

#endif // #ifdef AcdHistCalibMap_cxx
