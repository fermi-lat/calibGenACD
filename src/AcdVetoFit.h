
#ifndef AcdVetoFit_h
#define AcdVetoFit_h

#include "AcdCalibResult.h"
#include "AcdCalibMap.h"

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdVetoFitResult : public AcdCalibResult {

public:

  AcdVetoFitResult(Float_t veto, Float_t width, STATUS status);
  AcdVetoFitResult();

  inline Float_t veto() const { return _veto; }
  inline Float_t width() const { return _width; }
  
  inline void setVals(Float_t veto, Float_t width, STATUS status) {
    _veto = veto; _width = width;
    setStatus(status);
  }

  virtual void printXmlLine(ostream& os) const;
  virtual void printTxtLine(ostream& os) const;
  virtual Bool_t readTxt(istream& is);

private:
  Float_t _veto;
  Float_t _width;
  
  ClassDef(AcdVetoFitResult,1);
};


class AcdVetoFitMap : public AcdCalibMap {
public:
  AcdVetoFitMap();
  virtual ~AcdVetoFitMap();

  virtual AcdCalibResult* createHolder() const { return new AcdVetoFitResult; }

  virtual const char* calibType() const {
    return "ACD_Veto";
  }

  virtual const char* txtFormat() const {
    return "TILE PMT VETO WIDTH STATUS";
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

  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

private:
  
  ClassDef(AcdVetoFit,0) ;
};

#endif

#ifdef AcdVetoFit_cxx

#endif // #ifdef AcdVetoFit_cxx
