
#ifndef AcdVetoFit_h
#define AcdVetoFit_h

#include "AcdCalibResult.h"
#include <string>

#include "AcdCalibFit.h"
#include "TH1.h"
class AcdHistCalibMap;

class AcdVetoFitDesc : public AcdCalibDescription {

public:
  static const AcdVetoFitDesc& ins();

private:

 static const std::string s_calibType; // "ACD_Veto";
  static const std::string s_txtFormat; //"TILE PMT VETO WIDTH STATUS";

public:
  AcdVetoFitDesc();
  virtual ~AcdVetoFitDesc(){;};

private:
  
  ClassDef(AcdVetoFitDesc,1);
};



class AcdVetoFitLibrary : public  AcdCalibFit{

public:
  
  static Int_t findFirstBinAboveVal(const TH1& hist, Float_t val);

public:

  enum FitType { None = 0, 
		 Counting = 1,
		 Erf = 2 };

public:

  AcdVetoFitLibrary(FitType type)
    :AcdCalibFit(&AcdVetoFitDesc::ins()),
    _type(type){}

  AcdVetoFitLibrary(){}

  virtual ~AcdVetoFitLibrary() {;}
  
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

  ClassDef(AcdVetoFitLibrary,0) ;
};

#endif

#ifdef AcdVetoFit_cxx

#endif // #ifdef AcdVetoFit_cxx
