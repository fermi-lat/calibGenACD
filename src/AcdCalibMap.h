
#ifndef AcdCalibMap_h
#define AcdCalibMap_h

#include "AcdCalibResult.h"

#include <map>

class AcdHistCalibMap;
class AcdCalibBase;
class DomElement;

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
		      const char* algorithm,
		      const AcdCalibBase& calib);
  void writeTxt(ostream& os) const;

  Bool_t writeXmlFile(const char* fileName,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm,
		      const AcdCalibBase& calib) const;
  void writeXmlHeader(DomElement& node,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm,
		      const AcdCalibBase& calib) const;
  void writeXmlFooter(DomElement& node) const;
  void writeXmlBody(DomElement& node) const;
  
  virtual AcdCalibResult* createHolder() const { return 0; }

  Bool_t readTxtFile(const char* fileName);
  Bool_t readTxt(istream& is);

  virtual const char* calibType() const {
    return "NONE";
  }

  virtual const char* txtFormat() const {
    return "NONE";
  }

  const char* fileName() const {
    return m_fileName.c_str();
  }

protected:

  inline const std::map<UInt_t,AcdCalibResult*>& theMap() const { return m_map; }

private:  

  std::map<UInt_t,AcdCalibResult*> m_map;
  std::string m_fileName;

  ClassDef(AcdCalibMap,0) ;
};

#endif

#ifdef AcdCalibMap_cxx

#endif // #ifdef AcdCalibMap_cxx
