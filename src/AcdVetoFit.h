
#ifndef AcdVetoFit_h
#define AcdVetoFit_h

#include "AcdCalibResult.h"
#include "AcdCalibMap.h"

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdVetoFitResult : public AcdCalibResult {

public:

  AcdVetoFitResult(Float_t veto, Float_t width, STATUS status, Int_t type);
  AcdVetoFitResult();

  inline Float_t veto() const { return _veto; }
  inline Float_t width() const { return _width; }
  inline Int_t type() const { return _type; }
  
  inline void setVals(Float_t veto, Float_t width, STATUS status, Int_t type) {
    _veto = veto; _width = width; _type = type;
    setStatus(status);
  }

  virtual void printXmlLine(ostream& os) const;
  virtual void printTxtLine(ostream& os) const;
  virtual Bool_t readTxt(istream& is);

private:
  Float_t _veto;
  Float_t _width;
  Int_t _type;
  
  ClassDef(AcdVetoFitResult,1);
};


class AcdVetoFitMap : public AcdCalibMap {
public:
  AcdVetoFitMap();
  virtual ~AcdVetoFitMap();

  virtual AcdCalibResult* createHolder() const { return new AcdVetoFitResult; }

  virtual const char* calibType() {
    return "ACD_ElecVeto";
  }

  virtual const char* txtFormat() {
    return "TILE PMT VETO WIDTH STATUS TYPE";
  }

private:  

  ClassDef(AcdVetoFitMap,0) ;
};


class AcdVetoFit {

public:

  AcdVetoFit();

  virtual ~AcdVetoFit();
  
  virtual Int_t fit(AcdVetoFitResult& result, const TH1& hist);

  virtual Int_t fitChannel(AcdVetoFitMap& result, AcdHistCalibMap& input, UInt_t key);

  void fitAll(AcdVetoFitMap& results, AcdHistCalibMap& hists);

private:
  
  ClassDef(AcdVetoFit,0) ;
};

#endif

#ifdef AcdVetoFit_cxx

#endif // #ifdef AcdVetoFit_cxx
