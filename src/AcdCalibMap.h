
#ifndef AcdCalibMap_h
#define AcdCalibMap_h

#include "AcdCalibResult.h"

#include <map>

class AcdHistCalibMap;

class AcdCalibMap {
public:
  AcdCalibMap();
  virtual ~AcdCalibMap();
  
  void add(UInt_t key, AcdCalibResult& result);
  const AcdCalibResult* get(UInt_t key) const;
  AcdCalibResult* get(UInt_t key);

  Bool_t writeTxtFile(const char* fileName) const;
  void writeTxt(ostream& os) const;

  Bool_t writeXmlFile(const char* fileName,
		      const char* system,
		      const char* instrument,
		      const char* timestamp,
		      const char* version) const;
  void writeXmlHeader(ostream& os, 
		      const char* system,
		      const char* instrument,
		      const char* timestamp,
		      const char* version) const;
  void writeXmlFooter(ostream& os) const;
  void writeXmlBody(ostream& os) const;
  
  virtual AcdCalibResult* createHolder() const { return 0; }

  Bool_t readTxtFile(const char* fileName);
  Bool_t readTxt(istream& is);

private:  

  std::map<UInt_t,AcdCalibResult*> m_map;
  ClassDef(AcdCalibMap,0) ;
};

#endif

#ifdef AcdCalibMap_cxx

#endif // #ifdef AcdCalibMap_cxx
