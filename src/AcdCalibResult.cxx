#define AcdCalibResult_cxx

#include "AcdCalibResult.h"

#include "./AcdXmlUtil.h"
#include "./DomElement.h"

ClassImp(AcdCalibDescription) ;

std::vector<const AcdCalibDescription*> AcdCalibDescription::_descs(AcdCalib::NDESC,0);

void AcdCalibDescription::addDesc(AcdCalib::CALTYPE calType, const AcdCalibDescription* desc) {
  if ( _descs.size() == 0 ) {
    _descs.resize(AcdCalib::NDESC);
  }
  _descs[calType] = desc;
}

AcdCalibDescription::AcdCalibDescription(AcdCalib::CALTYPE calType, const std::string& calibTypeName, const std::string& txtFormat)
  :_calType(calType),
   _calibType(calibTypeName),
   _txtFormat(txtFormat){
  addDesc(calType,this);
}

AcdCalibDescription::AcdCalibDescription()
  :_calType(AcdCalib::NONE){;}


int AcdCalibDescription::getIndex(const std::string& name) const {
  for ( unsigned i(0); i < _varNames.size(); i++ ) {
    if ( _varNames[i] == name ) return i;
  }
  return -1;
}

const std::string& AcdCalibDescription::getVarName(int i) const {
  static const std::string noneName("NONE");
  if ( i < 0 || i >= (int)_varNames.size() ) return noneName;
  return _varNames[i];
}

void AcdCalibDescription::addVarName(const std::string& name) {
  _varNames.push_back(name);
}


ClassImp(AcdCalibResult) ;

AcdCalibResult::AcdCalibResult(STATUS status, const AcdCalibDescription& desc) 
  :_status(status),
   _desc(&desc){
  sizeVals(desc.nVar());
}

AcdCalibResult::AcdCalibResult()
  :_status(NOFIT),
   _desc(0){
}

bool AcdCalibResult::getVal(const std::string& name, float& val) {
  int idx = getIndex(name);
  if (idx < 0) return false;
  val = _vals[idx];
  return true;
}

void AcdCalibResult::setVal(const std::string& name, const float& val){
  int idx = getIndex(name);
  if (idx < 0) return;
  _vals[idx] = val;
}

void AcdCalibResult::setVals(Float_t v1, Float_t v2, STATUS stat) {
  (*this)[0] = v1;
  (*this)[1] = v2;
  setStatus(stat);
}

void AcdCalibResult::setVals(Float_t v1, Float_t v2, Float_t v3, STATUS stat){
  (*this)[0] = v1;
  (*this)[1] = v2;
  (*this)[2] = v3;
  setStatus(stat);
}

void AcdCalibResult::setVals(Float_t v1, Float_t v2, Float_t v3, Float_t v4, STATUS stat){
  (*this)[0] = v1;
  (*this)[1] = v2;
  (*this)[2] = v3;
  (*this)[3] = v4;
  setStatus(stat);
}

void AcdCalibResult::setVals(const std::vector<float>& vals, STATUS status) {
  if ( vals.size() == _vals.size() ) {
    for ( unsigned i(0); i < vals.size(); i++ ) {
      _vals[i] = vals[i];
    }
  } else {    
    return;
  }
  _status = status;
}

int AcdCalibResult::getIndex(const std::string& name){
  return _desc->getIndex(name);
}


void AcdCalibResult::makeXmlNode(DomElement&  node ) const {
  DomElement cNode = AcdXmlUtil::makeChildNode(node,_desc->calibType().c_str());
  for ( int i(0); i < _desc->nVar() ; i++ ) {
    AcdXmlUtil::addAttribute(cNode,_desc->getVarName(i).c_str(),_vals[i]);
  }  
}

void AcdCalibResult::printTxtLine(std::ostream&  os ) const {
  for ( int i(0); i < _desc->nVar() ; i++ ) {
    os << (float)_vals[i] << ' ';
  }
  os << getStatus();
}

Bool_t AcdCalibResult::readTxt(std::istream& is) {
  for ( int i(0); i < _desc->nVar() ; i++ ) {
    float aVal;
    is >> aVal;
    operator[](i) = aVal;
  }
  int stat;
  is >> stat;
  setStatusInt(stat);
  return kTRUE;
}
  
bool AcdCalibResult::setStatusInt(int stat) {
  switch (stat) {
  case NOFIT: setStatus(NOFIT); break;
  case OK: setStatus(OK); break;
  case MINUIT_FAILED: setStatus(MINUIT_FAILED); break;
  case PREFIT_FAILED: setStatus(PREFIT_FAILED); break;
  case USED_FALLBACK_1: setStatus(USED_FALLBACK_1); break;
  case USED_FALLBACK_2: setStatus(USED_FALLBACK_2); break;
  default:
    return false;
  }
  return true;
}
