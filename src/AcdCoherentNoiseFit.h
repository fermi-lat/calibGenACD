#ifndef AcdCoherentNoiseFit_h
#define AcdCoherentNoiseFit_h

#include "AcdCalibResult.h"
#include "AcdCalibMap.h"
#include "AcdHistCalibMap.h"

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdCoherentNoiseFitResult : public AcdCalibResult {

public:

  AcdCoherentNoiseFitResult(Float_t min, Float_t minTime, Float_t max, Float_t maxTime, 
			    const char* fitPars, STATUS status);
  AcdCoherentNoiseFitResult();
  
  inline Float_t max() const { return _max; }
  inline Float_t maxTime() const { return _maxTime; }
  inline Float_t min() const { return _min; }
  inline Float_t minTime() const { return _minTime; }
  inline const TString& fitPars() const { return _fitPars; }
  
  inline void setVals(Float_t min, Float_t minTime, Float_t max, Float_t maxTime, 
		      const char* fitPars, STATUS status){
    _min = min; _minTime = minTime; _max = max; _maxTime = maxTime;
    _fitPars = fitPars;
    setStatus(status);
  }

  virtual void printXmlLine(ostream& os) const;
  virtual void printTxtLine(ostream& os) const;
  virtual Bool_t readTxt(istream& is);

private:
  Float_t _min;
  Float_t _minTime;
  Float_t _max;
  Float_t _maxTime;
  TString _fitPars;

  ClassDef(AcdCoherentNoiseFitResult,1);
};


class AcdCoherentNoiseFitMap : public AcdCalibMap {
public:
  AcdCoherentNoiseFitMap();
  virtual ~AcdCoherentNoiseFitMap();

  virtual AcdCalibResult* createHolder() const { return new AcdCoherentNoiseFitResult; }

  virtual const char* calibType() const {
    return "ACD_CoherentNoise";
  }

  virtual const char* txtFormat() const {
    return "TILE PMT MIN MIN_TIME MAX MAX_TIME FIT_PARAMETERS STATUS";
  }

private:  

  ClassDef(AcdCoherentNoiseFitMap,0) ;
};


class AcdCoherentNoiseFit {

public:

  AcdCoherentNoiseFit(const char* name, 
		      Int_t method = 0);
  
  virtual ~AcdCoherentNoiseFit();
  
  virtual Int_t fit(AcdCoherentNoiseFitResult& result, const TH1& hist);

  virtual Int_t fitChannel(AcdCoherentNoiseFitMap& result, AcdHistCalibMap& input, UInt_t key);

  void fitAll(AcdCoherentNoiseFitMap& results, AcdHistCalibMap& hists);

  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

private:

  Int_t m_method;

  ClassDef(AcdCoherentNoiseFit,0) ;
};

#endif

#ifdef AcdCoherentNoiseFit_cxx

#endif // #ifdef AcdCoherentNoiseFit_cxx
