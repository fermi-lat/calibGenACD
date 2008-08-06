#ifndef AcdTrendCalib_h
#define AcdTrendCalib_h 

// base class
#include "AcdCalibBase.h"

// stl includes
#include <iostream>
#include <vector>
#include <list>

// Root includes
#include <TString.h>

// forward declares
class AcdHistCalibMap;
class TTree;
class TH2;
class TChain;

/** 
 * @class AcdTrendCalib
 *
 * @brief AcdCalibration class that trends calibrations results
 *
 * @author Eric Charles
 * $Header$
 */

class AcdTrendCalib : public AcdCalibBase {

public :
  
  enum ChannelSet { All = 0, 
		    Tiles = 1,
		    NoSkirt = 2,
		    Ribbons = 3 };

public :
  
  /// Standard ctor, where user provides the input data
  AcdTrendCalib(AcdCalibData::CALTYPE t, AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdTrendCalib();  

  /// Read in a file with calibration data
  Bool_t readCalib(const std::string& calibXmlFile, Bool_t isRef = kFALSE);

  /// Fill all the histograms
  Bool_t fillHistograms();

  /// Write all the outputs
  Bool_t writeOutputs(const std::string& outputPrefix, const std::string& timestamp);

protected:
  
  /// read in 1 event
  virtual Bool_t readEvent( ) { return kFALSE; }

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used) { used = kFALSE; }

  /// Make the summary histograms
  void makeSummaryHists();

  /// add a summary histogram
  void addSummaryHist(const char* name, const char* title, const char* units, Float_t min, Float_t max, ChannelSet cSet = All);

  /// Check to see if we should use a channel
  Bool_t useChannel(UInt_t id, ChannelSet cSet) const;

private:

  /// The number of calibrations to trend
  UInt_t                  m_nCalib;

  /// The reference calibration
  TTree*                  m_reference;

  /// The name of the file with the reference calibration
  TString                 m_refFileName;

  /// The trending calibrations
  TChain*                 m_calibs;

  /// The names of the vars to be trended
  std::list<std::string>  m_trendNames;

  /// The trending histograms
  AcdHistCalibMap*        m_trendHists;

  /// The trending summary histograms
  std::vector<TH2*>       m_summaryHists;           

  /// Which channels go into which summary histograms
  std::vector<ChannelSet> m_channels;

      
};

#endif
