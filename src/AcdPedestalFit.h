
#ifndef AcdPedestalFit_h
#define AcdPedestalFit_h

// Base classes
#include "AcdCalibResult.h"
#include "AcdCalibFit.h"

// stl includes
#include <string>

// ROOT includes
#include "TH1.h"

// forward declares
class AcdHistCalibMap;


class AcdPedestalFitDesc : public AcdCalibDescription {

public:

  static const AcdPedestalFitDesc& ins();

private:

  static const std::string s_calibType;
  static const std::string s_txtFormat;

public:

  AcdPedestalFitDesc();
  virtual ~AcdPedestalFitDesc(){;};

private:
  
  ClassDef(AcdPedestalFitDesc,1);
};


class AcdPedestalFitLibrary : public AcdCalibFit {

public:

  enum FitType { None = 0, 
		 PeakValue = 1,
		 MeanValue = 2 };

public:

  AcdPedestalFitLibrary(FitType type)
    :AcdCalibFit(&AcdPedestalFitDesc::ins()),
    _type(type){}

  AcdPedestalFitLibrary(){}

  virtual ~AcdPedestalFitLibrary() {;}
  
  virtual Int_t fit(AcdCalibResult& result, const AcdCalibHistHolder& holder);

  inline FitType fitType() const { return _type; };
  inline void setFitType(FitType type) { _type = type; };

  virtual const char* algorithm() const {
    static const char* names[3] = {"None","PeakValue","MeanValue"};
    return names[_type];
  }

protected:

  UInt_t fitMean(AcdCalibResult& result, const AcdCalibHistHolder& holder);
  UInt_t fitPeak(AcdCalibResult& result, const AcdCalibHistHolder& holder);

private:
  
  FitType _type;

  ClassDef(AcdPedestalFitLibrary,0) ;
};


#endif

#ifdef AcdPedestalFit_cxx

#endif // #ifdef AcdPedestalFit_cxx
