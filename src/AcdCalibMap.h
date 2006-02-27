
#ifndef AcdCalibMap_h
#define AcdCalibMap_h

#include "AcdCalibResult.h"

#include <map>

class AcdHistCalibMap;
class AcdCalibBase;

class AcdCalibMap {
public:
  AcdCalibMap();
  virtual ~AcdCalibMap();
  
  void add(UInt_t key, AcdCalibResult& result);
  const AcdCalibResult* get(UInt_t key) const;
  AcdCalibResult* get(UInt_t key);

  Bool_t writeTxtFile(const char* fileName,
		      const char* instrument,
		      const char* timestamp,
		      const AcdCalibBase& calib);
  void writeTxt(ostream& os) const;

  Bool_t writeXmlFile(const char* fileName,
		      const char* instrument,
		      const char* timestamp,
		      const AcdCalibBase& calib) const;
  void writeXmlHeader(ostream& os, 
		      const char* instrument,
		      const char* timestamp,
		      const AcdCalibBase& calib) const;
  void writeXmlFooter(ostream& os) const;
  void writeXmlBody(ostream& os) const;
  
  virtual AcdCalibResult* createHolder() const { return 0; }

  Bool_t readTxtFile(const char* fileName);
  Bool_t readTxt(istream& is);

  virtual const char* calibType() {
    return "NONE";
  }

  virtual const char* txtFormat() {
    return "NONE";
  }

  const char* fileName() const {
    return m_fileName.c_str();
  }

private:  

  std::map<UInt_t,AcdCalibResult*> m_map;
  std::string m_fileName;

  ClassDef(AcdCalibMap,0) ;
};

#endif

#ifdef AcdCalibMap_cxx

#endif // #ifdef AcdCalibMap_cxx
