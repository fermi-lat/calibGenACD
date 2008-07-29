
#ifndef AcdStripFit_h
#define AcdStripFit_h

// Base classes
#include "CalibData/Acd/AcdCalibObj.h"
#include "CalibData/CalibModel.h"
#include "AcdCalibFit.h"
#include "AcdCalibEnum.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;

namespace CalibData {

  /** 
   * @class AcdStripFitDesc
   *
   * @brief Description of strip chart calibration
   * 
   * This calibration consists of:
   *  - mean  = the mean Y value of the bins
   *  - rms   = the rms of the Y value of the bins
   *  - min   = the min Y value of the bins
   *  - max   = the max Y value of the bins
   *
   * @author Eric Charles
   */

  class AcdStripFitDesc : public AcdCalibDescription {    
  public:    
    /// Get this description
    static const AcdStripFitDesc& instance() {
      static const AcdStripFitDesc desc;
      return desc;
    };        
  public:
    /// Trivial D'ctor
    virtual ~AcdStripFitDesc(){;};    
  private:    
    /// This is a singleton
    AcdStripFitDesc()
      :AcdCalibDescription(AcdCalibData::TIME_PROF,"ACD_TileProfile"){
      addVarName("mean");
      addVarName("rms");
      addVarName("min");
      addVarName("max");      
    }
  };

  /** 
   * @class AcdStripResult
   *
   * @brief A strip chart test result for 1 PMT.
   * 
   * This calibration consists of:
   *  - mean  = the mean Y value of the bins
   *  - rms   = the rms of the Y value of the bins
   *  - min   = the min Y value of the bins
   *  - max   = the max Y value of the bins
   *
   * @author Eric Charles
   */

  class AcdStripResult : public AcdCalibObj {
  public:
    static const CLID& calibCLID() {
      static const CLID clidNode = 0;
      return clidNode;
    }
  public:
    AcdStripResult(const AcdCalibDescription& desc, const std::vector<float>& vals, STATUS status=NOFIT) :
      AcdCalibObj(status,vals,desc){
      assert( desc.calibType() == AcdCalibData::TIME_PROF );
      setVals(vals,status);
    }
    virtual ~AcdStripResult() {}

  };

};

/** 
 * @class AcdStripFitLibrary
 *
 * @brief ACD strip chart (aka time series) fitting library
 *
 * Algorithms are:
 * - Minuit:
 *    - Not really minuit at all
 *
 * @author Eric Charles
 */

class AcdStripFitLibrary : public AcdCalibFit {

public:

  enum FitType { None = 0, 
		 Minuit = 1 };

public:

  /// Standard c'tor, specify fit type, number of time bins, mix and max value, etc.
  AcdStripFitLibrary(FitType type, AcdCalib::STRIPTYPE sType, unsigned nBin, float min, float max, float ref = 0., float scale = 1.)
    :AcdCalibFit(&CalibData::AcdStripFitDesc::instance()),
    _type(type),
    _method(sType),
    _nBin(nBin),
    _min(min),
    _max(max),
    _ref(ref),
    _scale(scale){}

  AcdStripFitLibrary(){;}

  virtual ~AcdStripFitLibrary() {;}

  /// Do the fit, return the status
  virtual Int_t fit(CalibData::AcdCalibObj& result, AcdCalibHistHolder& holder,
		    CalibData::AcdCalibObj* ref = 0);

  /// Test all histograms for discreet jumps, print warning and error on std::cerr
  Bool_t test(AcdCalibMap& results, Float_t lo, Float_t hi, const char* msg, const char* testName) const;

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  /// return the name of the algorithm
  virtual const char* algorithm() const {
    static const char* names[2] = {"None","Minuit"};
    return names[_type];
  }

protected:

private:

  /// Algorithm to use
  FitType _type;
  /// Method to use to test for outliers
  AcdCalib::STRIPTYPE _method;
  /// Number of bins in strip charts
  UInt_t _nBin;
  /// Min y value
  Float_t _min;
  /// Max y value
  Float_t _max;
  /// Reference y value
  Float_t _ref;
  /// Scale factor for y axis
  Float_t _scale;  
  
};


#endif

#ifdef AcdStripFit_cxx

#endif // #ifdef AcdStripFit_cxx
