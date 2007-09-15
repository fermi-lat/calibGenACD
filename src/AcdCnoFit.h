
#ifndef AcdCnoFit_h
#define AcdCnoFit_h

// Base classes
#include "AcdCalibResult.h"
#include "AcdCalibFit.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;


class AcdCnoFitDesc : public AcdCalibDescription {

public:

  static const AcdCnoFitDesc& ins();

private:

  static const std::string s_calibType; // "ACD_Cno";
  static const std::string s_txtFormat; //"TILE PMT CNO WIDTH STATUS";

public:

  AcdCnoFitDesc();
  virtual ~AcdCnoFitDesc(){;};

private:
  
  ClassDef(AcdCnoFitDesc,1);
};


class AcdCnoFitLibrary : public AcdCalibFit {

public:
  
  static Int_t findFirstBinAboveVal(const TH1& hist, Float_t val);

public:

  enum FitType { None = 0, 
		 Counting = 1,
		 Erf = 2 };

public:

  AcdCnoFitLibrary(FitType type)
    :AcdCalibFit(&AcdCnoFitDesc::ins()),
    _type(type){}

  AcdCnoFitLibrary(){}

  virtual ~AcdCnoFitLibrary() {;}
  
  virtual Int_t fit(AcdCalibResult& result, const AcdCalibHistHolder& holder);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  virtual const char* algorithm() const {
    static const char* names[3] = {"None","Counting","Erf"};
    return names[_type];
  }

protected:

  Int_t counting(AcdCalibResult& result, const TH1& hist);
  Int_t fitErf(AcdCalibResult& result, const TH1& hist);

private:
  
  FitType _type;

  ClassDef(AcdCnoFitLibrary,0) ;
};

#endif

#ifdef AcdCnoFit_cxx

#endif // #ifdef AcdCnoFit_cxx
