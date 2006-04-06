
#ifndef AcdCalibResult_h
#define AcdCalibResult_h

#include "TH1.h"
#include <map>

class AcdHistCalibMap;

class AcdCalibResult {

public:

  enum STATUS { NOFIT = -1,
		OK = 0,	 
		MINUIT_FAILED = 4,
		PREFIT_FAILED = 5,
		USED_FALLBACK_1 = 6,
		USED_FALLBACK_2 = 7 };

public:

  AcdCalibResult(STATUS status);
  AcdCalibResult();
  virtual ~AcdCalibResult(){;}

  virtual void printXmlLine(ostream& /* os */) const {;};
  virtual void printTxtLine(ostream& /* os */) const {;};
  virtual Bool_t readTxt(istream& /* is */) { return kFALSE; };
  
  inline void setStatus(STATUS stat) { _status = stat; };
  inline STATUS getStatus() const { return _status; }

private:

  STATUS _status;

  ClassDef(AcdCalibResult,1);    // 
};


#endif

#ifdef AcdCalibResult_cxx

#endif // #ifdef AcdCalibResult_cxx
