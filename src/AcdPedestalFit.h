
#ifndef AcdPedestalFit_h
#define AcdPedestalFit_h

#include "AcdCalibResult.h"
#include "AcdCalibMap.h"

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdPedestalFitResult : public AcdCalibResult {
public:
  AcdPedestalFitResult(Float_t mean, Float_t rms, STATUS status);
  AcdPedestalFitResult();
  virtual ~AcdPedestalFitResult() {;}
  inline Float_t mean() const { return _mean; }
  inline Float_t rms() const { return _rms; }
  inline void setVals(Float_t mean, Float_t rms, STATUS status) {
    _mean = mean; _rms = rms; 
    setStatus(status);
  }
  
  virtual void printXmlLine(ostream& os) const;
  virtual void printTxtLine(ostream& os) const;
  virtual Bool_t readTxt(istream& is);
  
private:
  Float_t _mean;
  Float_t _rms;
  ClassDef(AcdPedestalFitResult,1);
};


class AcdPedestalFitMap : public AcdCalibMap {
public:
  AcdPedestalFitMap(){;}
  virtual ~AcdPedestalFitMap();
  
  virtual AcdCalibResult* createHolder() const { return new AcdPedestalFitResult; }

  AcdPedestalFitResult* find(UInt_t key) {
    AcdCalibResult* res = get(key);
    if ( res == 0 ) return 0;
    AcdPedestalFitResult* fr = static_cast<AcdPedestalFitResult*>(res);
    return fr;
  }

  virtual const char* calibType() const {
    return "ACD_Pedestal";
  }

  virtual const char* txtFormat() const {
      return "TILE PMT PED RMS STATUS";
  }
  
private:  
  
  ClassDef(AcdPedestalFitMap,0) ;
};


class AcdPedestalFit {

public:

  AcdPedestalFit();

  virtual ~AcdPedestalFit() {;}
  
  virtual UInt_t fit(AcdPedestalFitResult& result, const TH1& hist);

  virtual UInt_t fitChannel(AcdPedestalFitMap& result, AcdHistCalibMap& input, UInt_t key);

  void fitAll(AcdPedestalFitMap& results, AcdHistCalibMap& hists);

  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

private:
  
  ClassDef(AcdPedestalFit,0) ;
};


#endif

#ifdef AcdPedestalFit_cxx

#endif // #ifdef AcdPedestalFit_cxx
