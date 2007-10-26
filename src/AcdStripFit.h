
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
  
  class AcdStripFitDesc : public AcdCalibDescription {    
  public:    
    static const AcdStripFitDesc& instance() {
      static const AcdStripFitDesc desc;
      return desc;
    };        
  public:
    virtual ~AcdStripFitDesc(){;};    
  private:    
    AcdStripFitDesc()
      :AcdCalibDescription(AcdCalibData::TIME_PROF,"ACD_Cno"){
      addVarName("mean");
      addVarName("rms");
      addVarName("min");
      addVarName("max");      
    }
  };

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


class AcdStripFitLibrary : public AcdCalibFit {

public:

  enum FitType { None = 0, 
		 Minuit = 1 };

public:

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
  
  virtual Int_t fit(CalibData::AcdCalibObj& result, AcdCalibHistHolder& holder);

  Bool_t test(AcdCalibMap& results, Float_t lo, Float_t hi, const char* msg, const char* testName) const;

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  virtual const char* algorithm() const {
    static const char* names[2] = {"None","Minuit"};
    return names[_type];
  }

protected:

private:
  
  FitType _type;
  AcdCalib::STRIPTYPE _method;
  UInt_t _nBin;
  Float_t _min;
  Float_t _max;
  Float_t _ref;
  Float_t _scale;  
  
};


#endif

#ifdef AcdStripFit_cxx

#endif // #ifdef AcdStripFit_cxx
