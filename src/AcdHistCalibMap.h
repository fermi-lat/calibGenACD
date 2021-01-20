
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
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdHistCalibMap.h,v 1.10 2008/08/01 00:08:25 echarles Exp $
 */

class  AcdCalibHistHolder {
public:
  AcdCalibHistHolder(){;}  
  virtual ~AcdCalibHistHolder(){;}
  /// return the ith histogram
  TH1* getHist(UInt_t idx) const;
  ///  add a histogram to the set
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
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdHistCalibMap.h,v 1.10 2008/08/01 00:08:25 echarles Exp $
 */

class AcdHistCalibMap {

public:

  /// Build by attaching a map to a TFile, 
  AcdHistCalibMap(TFile& file, AcdKey::Config config = AcdKey::LAT);

  /// Build with histogram details
  AcdHistCalibMap(const char* prefix, UInt_t nBins = 4096, Float_t lo = -0.5, Float_t hi = 4095.5, 
		  AcdKey::Config config = AcdKey::LAT, UInt_t n=1);
  
  virtual ~AcdHistCalibMap();
  
  /// return a histogram by key and index
  TH1* getHist(UInt_t key, UInt_t idx=0);

  /// return a set of histograms by key
  AcdCalibHistHolder* getHolder(UInt_t key);

  /// return the entire map
  const std::map<UInt_t,AcdCalibHistHolder>& theMap() const {
    return m_map;
  }

  /// return the list of all the histograms
  const TList& histograms() const { 
    return m_list;
  }
  
  ///  write histograms to a file
  Bool_t writeHistograms(const char* newFileName );

  /// return the configuration (LAT or CU06)
  AcdKey::Config config() const { return m_config; }

protected:

  /// Make one histogram for each channel
  void bookHists(const char* prefix, UInt_t n=1);

private:

  /// The configuration (LAT or CU06)
  AcdKey::Config         m_config;

  /// Number of bins in each histogram
  UInt_t                 m_bins;
  /// Low value of each histogram
  Float_t                m_lo;
  /// High value of each histogram
  Float_t                m_hi;

  /// All the histograms
  TList                  m_list;
  /// Map for channel key to set of histograms
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
