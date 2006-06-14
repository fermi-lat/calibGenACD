
#ifndef AcdStripFit_h
#define AcdStripFit_h

#include "AcdCalibResult.h"
#include "AcdCalibMap.h"
#include "AcdHistCalibMap.h"

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdStripFitResult : public AcdCalibResult {

public:

  AcdStripFitResult(Float_t mean, Float_t rms, Float_t min, Float_t max, STATUS status);
  AcdStripFitResult();
  
  inline Float_t mean() const { return _mean; }
  inline Float_t rms() const { return _rms; }
  inline Float_t min() const { return _min; }
  inline Float_t max() const { return _max; }
  
  inline void setVals(Float_t mean, Float_t rms, Float_t min, Float_t max, STATUS status) {
    _mean = mean; _rms = rms; _min = min; _max = max;
    setStatus(status);
  }

  virtual void printXmlLine(ostream& os) const;
  virtual void printTxtLine(ostream& os) const;
  virtual Bool_t readTxt(istream& is);

private:
  Float_t _mean;
  Float_t _rms;
  Float_t _min;
  Float_t _max;
  
  ClassDef(AcdStripFitResult,1);
};


class AcdStripFitMap : public AcdCalibMap {
public:
  AcdStripFitMap();
  virtual ~AcdStripFitMap();

  virtual AcdCalibResult* createHolder() const { return new AcdStripFitResult; }

  Bool_t test(Float_t lo, Float_t hi, const char* msg, const char* testName) const;

  virtual const char* calibType() const {
    return "ACD_Strip";
  }

  virtual const char* txtFormat() const {
    return "TILE PMT MEAN RMS MIN MAX STATUS";
  }

private:  

  ClassDef(AcdStripFitMap,0) ;
};


class AcdStripFit {

public:

  AcdStripFit(const char* name, 
	      Int_t method = 0, 
	      Int_t nBin = 100, Float_t min = -50., Float_t max = 50.,
	      Float_t refVal = 0., Float_t scale = 1.);
  
  virtual ~AcdStripFit();
  
  virtual Int_t fit(AcdStripFitResult& result, const TH1& hist, TH1*& out);

  virtual Int_t fitChannel(AcdStripFitMap& result, AcdHistCalibMap& input, UInt_t key);

  void fitAll(AcdStripFitMap& results, AcdHistCalibMap& hists);

  virtual const char* algorithm() const {
    static const char* def("Default");
    return def;
  }

  inline AcdHistCalibMap& fittedHists() { return _fitted; }

private:

  AcdHistCalibMap _fitted;  

  Int_t m_method;
  Float_t m_refVal;
  Float_t m_scale;

  ClassDef(AcdStripFit,0) ;
};

#endif

#ifdef AcdStripFit_cxx

#endif // #ifdef AcdStripFit_cxx
