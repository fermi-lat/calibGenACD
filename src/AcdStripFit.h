
#ifndef AcdStripFit_h
#define AcdStripFit_h

// Base classes
#include "AcdCalibResult.h"
#include "AcdCalibFit.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;


class AcdStripFitDesc : public AcdCalibDescription {

public:

  static const AcdStripFitDesc& ins();

private:
  
  static const std::string s_calibType; // "ACD_Strip";
  static const std::string s_txtFormat; // "TILE PMT MEAN RMS MIN MAX STATUS";

public:

  AcdStripFitDesc();
  virtual ~AcdStripFitDesc(){;};

private:
 
  ClassDef(AcdStripFitDesc,1);
};


class AcdStripFitLibrary : public AcdCalibFit {

public:

  enum FitType { None = 0, 
		 Minuit = 1 };

public:

  AcdStripFitLibrary(FitType type, AcdCalib::STRIPTYPE sType, unsigned nBin, float min, float max, float ref = 0., float scale = 1.)
    :AcdCalibFit(&AcdStripFitDesc::ins()),
    _type(type),
    _method(sType),
    _nBin(nBin),
    _min(min),
    _max(max),
    _ref(ref),
    _scale(scale){}

  AcdStripFitLibrary(){;}

  virtual ~AcdStripFitLibrary() {;}
  
  virtual Int_t fit(AcdCalibResult& result, const AcdCalibHistHolder& holder);

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
  
  ClassDef(AcdStripFitLibrary,0) ;
};


#endif

#ifdef AcdStripFit_cxx

#endif // #ifdef AcdStripFit_cxx
