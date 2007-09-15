
#ifndef AcdCalibResult_h
#define AcdCalibResult_h

// local includes
#include "./AcdCalibEnum.h"

// ROOT
#include "Rtypes.h"

// stl includes
#include <map>
#include <vector>
#include <string>
#include <iostream>

// forward declares
class AcdHistCalibMap;
class DomElement;

class AcdCalibDescription {

public:
  static const AcdCalibDescription* getDesc(AcdCalib::CALTYPE calType) {
    return _descs[calType];
  }
protected:
  void addDesc(AcdCalib::CALTYPE calType, const AcdCalibDescription* desc);
private:
  static std::vector<const AcdCalibDescription*> _descs;
  
public:
  AcdCalibDescription(AcdCalib::CALTYPE calType, const std::string& calibTypeName, const std::string& txtFormat);
  virtual ~AcdCalibDescription() {;}

  inline const std::string& calibType() const {
    return _calibType;
  }
  inline const std::string& txtFormat() const {
    return _txtFormat;
  }
  inline AcdCalib::CALTYPE calType() const {
    return _calType;
  } 
  inline int nVar() const {
    return _varNames.size();
  }
  int getIndex(const std::string& name) const;
  const std::string& getVarName(int i) const;

  void addVarName(const std::string& name);

protected:
  AcdCalibDescription();

private:
  
  const AcdCalib::CALTYPE _calType;
  const std::string _calibType;
  const std::string _txtFormat;  
  std::vector<std::string> _varNames;

  ClassDef(AcdCalibDescription,1)

};


class AcdCalibResult {

public:

  enum STATUS { NOFIT = -1,
		OK = 0,	 
		MINUIT_FAILED = 4,
		PREFIT_FAILED = 5,
		USED_FALLBACK_1 = 6,
		USED_FALLBACK_2 = 7 };

public:

  AcdCalibResult(STATUS status, const AcdCalibDescription& desc);
  virtual ~AcdCalibResult(){;}

  void makeXmlNode(DomElement&  node ) const;
  void printTxtLine(std::ostream&  os ) const;
  Bool_t readTxt(std::istream& is);
  
  inline Float_t& operator[](int i) { return _vals[i]; }
  inline const Float_t& operator[](int i) const { return _vals[i]; }

  inline const AcdCalibDescription& desc() const {
    return *_desc;
  } 

  inline void setStatus(STATUS stat) { _status = stat; };
  inline STATUS getStatus() const { return _status; }

  bool getVal(const std::string& name, float& val);
  void setVal(const std::string& name, const float& val);
  int getIndex(const std::string& name);
  void setVals(const std::vector<float>& vals, STATUS status);
  void setVals(Float_t v1, Float_t v2, STATUS stat);
  void setVals(Float_t v1, Float_t v2, Float_t v3, STATUS stat);
  void setVals(Float_t v1, Float_t v2, Float_t v3, Float_t v4, STATUS stat);

protected:

  AcdCalibResult();
  bool setStatusInt(int stat);

  inline void sizeVals(int n) {
    _vals.resize(n);
  }

private:

  STATUS _status;
  const AcdCalibDescription* _desc;

  std::vector<float> _vals;

  ClassDef(AcdCalibResult,1);    // 
};


#endif

#ifdef AcdCalibResult_cxx

#endif // #ifdef AcdCalibResult_cxx
