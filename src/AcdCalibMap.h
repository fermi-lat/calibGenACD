
#ifndef AcdCalibMap_h
#define AcdCalibMap_h

#include "Rtypes.h"

// stl includes
#include <map>
#include <string>
#include <iostream>

class AcdCalibDescription;
class AcdCalibResult;
class AcdHistCalibMap;
class AcdCalibBase;
class DomElement;

class AcdCalibMap {
public:

  AcdCalibMap(const AcdCalibDescription& desc);

  virtual ~AcdCalibMap();
  
  void add(UInt_t key, AcdCalibResult& result);
  const AcdCalibResult* get(UInt_t key) const;
  AcdCalibResult* get(UInt_t key);
  AcdCalibResult* makeNew() const;

  Bool_t writeTxtFile(const char* fileName,
		      const char* instrument,
		      const char* timestamp,
		      const char* algorithm,
		      const AcdCalibBase& calib);
  void writeTxt(std::ostream& os) const;

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
  
  Bool_t readTxtFile(const char* fileName);
  Bool_t readTxt(std::istream& is);

  const char* fileName() const {
    return m_fileName.c_str();
  }

  inline const std::map<UInt_t,AcdCalibResult*>& theMap() const { return m_map; }

protected:

  AcdCalibMap();

private:  

  const AcdCalibDescription* m_desc;
  
  std::map<UInt_t,AcdCalibResult*> m_map;
  std::string m_fileName;
  

  ClassDef(AcdCalibMap,0) ;
};

#endif

#ifdef AcdCalibMap_cxx

#endif // #ifdef AcdCalibMap_cxx
