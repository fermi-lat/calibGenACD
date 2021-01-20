#ifndef AcdCalibLoop_Svac_h
#define AcdCalibLoop_Svac_h 

// base classs
#include "AcdCalibBase.h"

// local includes
#include "./AcdGarcGafeHits.h"

// stl includes
#include <iostream>

/** 
 * @class AcdCalibLoop_Svac
 *
 * @brief AcdCalibration class that loop on Svac files
 *
 * This is used for the following calibrations:
 *   - Gains (MIP peaks)
 *   - Ribbon Attenuation
 *   - Carbon Peaks
 *   - Checking existing calibrations
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibLoop_Svac.h,v 1.10 2010/06/24 23:52:41 echarles Exp $
 */

class AcdCalibLoop_Svac : public AcdCalibBase {

public :

  /// Standard ctor, where user provides the input data
  AcdCalibLoop_Svac(AcdCalibData::CALTYPE t, TChain* svacChain, 		    
		    AcdKey::Config config = AcdKey::LAT);

  /// Special ctor, where user provides the input file with histograms
  AcdCalibLoop_Svac(AcdCalibData::CALTYPE t, const char* fileName, 		    
		    AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdCalibLoop_Svac();  
  
  
  AcdHistCalibMap* makeRatioPlots();

  /// Set the Z value for the GCR cut
  void setGCRCalibValue(Int_t calGCRSelect){
    m_calGCRSelect = calGCRSelect;
  }

  /// Set the flag to use the MIP values from the SVAC file
  void setMIPFromSvacValue(Bool_t mipFromSvac) {
    m_mipFromSvac = mipFromSvac;
  }

  /// Set up the merit calibration
  Bool_t fillMeritCalib();


protected:

  /// setup input data
  Bool_t attachChains();

  /// get reconstructed track direction 
  void getFitDir();
    
  /// fill a histogram, possibly using the pathlength correction
  void fillGainHistCorrect(unsigned id, float pathLength, unsigned iISect);

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId, Double_t& timeStamp);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);

  /// Run the GCR selection
  Bool_t runGCRSelect(Int_t whichZ);


  /// Get the range cross-over data
  Bool_t getRangeData(UInt_t key, Float_t& low_max, Float_t& high_min) const;

  /// get the high range pedestal for a channel
  float getPedsHigh(UInt_t key) const;

  /// Transform the data into a single range
  Bool_t singleRange(UInt_t key, Int_t pha, Int_t range, Float_t& val) const;

  /// Get the MIP values
  Bool_t getMipValues(UInt_t id, Float_t& mipsA, Float_t& mipsB);

  /// get the MIP value for one pmt
  Bool_t getMipValue(UInt_t key, UInt_t range, UInt_t pha, Float_t& mips);

  /// Called each event to do mapping from TILE:PMT to GARC:GAFE space
  void fillCnoData();  

private:


  /// flag for using the CAL to select a particular ion, by Z
  Int_t  m_calGCRSelect;

  /// flag to use the mip values from the svac file
  Bool_t m_mipFromSvac;

  /// PHA values by ID,PMT
  Int_t  m_AcdPha[604][2];
  /// Readout range used by ID,PMT
  Int_t  m_AcdRange[604][2];
  /// Veto bit setting by ID,PMT
  Int_t  m_AcdHitmap[604][2];
  /// Calibrated MIP equivalent signal seem by ID,PMT
  Float_t m_AcdMips[604][2];

  /// Number of ribbon fired
  Int_t  m_AcdRibbonCount;
  
  /// Number of intersection between tracks and ACD
  Int_t m_AcdNumTkrIntSec;
  /// Ids of tiles and ribbons crossed by tracks
  Int_t m_AcdTkrIntSecTileId[20];
  /// Ids of tracks that crossed tiles and ribbons
  Int_t m_AcdTkrIntSecTkrIndex[20];
  /// Path length of tracks through tile
  Float_t m_AcdTkrIntSecPathLengthInTile[20];    

  /// X position of intersections between tracks and ACD
  Float_t m_AcdTkrIntSecGlobalX[20];    
  /// Y position of intersections between tracks and ACD
  Float_t m_AcdTkrIntSecGlobalY[20];    
  /// Z position of intersections between tracks and ACD
  Float_t m_AcdTkrIntSecGlobalZ[20];    

  /// CNO bits
  Int_t m_GemCnoVector[12];

  /// The gem conditions word
  Int_t m_GemConditionsWord;

  /// Z directional cosine of best track
  Float_t m_Tkr1ZDir;
  /// Engine used to read event
  Int_t m_TrgEngineGem;
  /// Cal X-tal energies
  Float_t m_CalXtalEne[16][8][12];

  /// Time stamp for event
  Double_t m_timeStamp;

  /// Histograms for pedestals
  AcdHistCalibMap* m_pedHists;
  /// Histograms for MIP peak, carbon peaks, ribbon attenuation and gain checking)
  AcdHistCalibMap* m_peakHists;
  /// Histograms for range crossover calibration checking
  AcdHistCalibMap* m_rangeHists;
  /// Histograms for veto set point calibration checking
  AcdHistCalibMap* m_vetoHists;
  /// Histograms for cno set point calibration checking
  AcdHistCalibMap* m_cnoHists;

  /// Used to work in GARC:GAFE space instead of TILE:PMT space, Needed for CNO
  AcdGarcGafeHits m_garcGafeHits;  
 
        
};

#endif
