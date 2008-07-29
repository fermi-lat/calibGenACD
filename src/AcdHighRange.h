#ifndef AcdHighRange_h
#define AcdHighRange_h 

// base class
#include "AcdCalibBase.h"

// stl includes
#include <iostream>

// forward declares

/** 
 * @class AcdHighRange
 *
 * @brief AcdCalibration to fit High Range light yield
 *
 * FIXME more here
 *
 * @author Eric Charles
 * $Header$
 */

class TH1;

class AcdHighRange : public AcdCalibBase {

public :
  
  /// Standard ctor, where user provides the input data
  AcdHighRange(TChain* svacChain, AcdKey::Config config = AcdKey::LAT);
  
  virtual ~AcdHighRange();  

  Bool_t convertTxt(const char* txtFileName);

  void fillHistFromPars(Float_t oldPars[5], TH1& theHist, Float_t maxVal);


protected:

  /// setup input data
  Bool_t attachChains();

  /// read in 1 event
  virtual Bool_t readEvent(int ievent, Bool_t& filtered, 
			   int& runId, int& evtId);    

  /// Try to use an event for calibration
  virtual void useEvent(Bool_t& used);

private:

  // Local stash

  /// The data to fit
  AcdHistCalibMap* m_histMap;

};

#endif
