#define AcdCalibResult_cxx

#include "AcdCalibResult.h"

ClassImp(AcdCalibResult) ;

AcdCalibResult::AcdCalibResult(STATUS status) 
  :_status(status){
}

AcdCalibResult::AcdCalibResult()
  :_status(NOFIT){
}

