
#ifndef AcdCnoFit_h
#define AcdCnoFit_h

#include "AcdCalibResult.h"
#include "AcdCalibMap.h"

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdCnoFitResult : public AcdCalibResult {

public:

  AcdCnoFitResult(Float_t cno, Float_t width, STATUS status);
  AcdCnoFitResult();

  inline Float_t cno() const { return _cno; }
  inline Float_t width() const { return _width; }
  
  inline void setVals(Float_t cno, Float_t width, STATUS status) {
    _cno = cno; _width = width;
    setStatus(status);
  }

  virtual void makeXmlNode(DomElement& node) const;
  virtual void printTxtLine(ostream& os) const;
  virtual Bool_t readTxt(istream& is);

private:
  Float_t _cno;
  Float_t _width;
  
  ClassDef(AcdCnoFitResult,1);
};


class AcdCnoFitMap : public AcdCalibMap {
public:
  AcdCnoFitMap();
  virtual ~AcdCnoFitMap();

  virtual AcdCalibResult* createHolder() const { return new AcdCnoFitResult; }

  virtual const char* calibType() const {
    return "ACD_Cno";
  }

  virtual const char* txtFormat() const {
    return "TILE PMT CNO WIDTH STATUS";
  }

private:  

  ClassDef(AcdCnoFitMap,0) ;
};


class AcdCnoFit {

public:

  AcdCnoFit();

  virtual ~AcdCnoFit();
  
  virtual Int_t fit(AcdCnoFitResult& result, const TH1& hist);

  virtual Int_t fitChannel(AcdCnoFitMap& result, AcdHistCalibMap& input, UInt_t key);

  void fitAll(AcdCnoFitMap& results, AcdHistCalibMap& hists);

  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

private:
  
  ClassDef(AcdCnoFit,0) ;
};

#endif

#ifdef AcdCnoFit_cxx

#endif // #ifdef AcdCnoFit_cxx
