#define AcdHistCalibMap_cxx
#include "AcdHistCalibMap.h"

#include "TFile.h"

ClassImp(AcdHistCalibMap) ;

AcdHistCalibMap::AcdHistCalibMap(TFile& file, AcdMap::Config config) 
  :m_config(config),m_bins(0),m_lo(0.),m_hi(0.){
  TList* l = file.GetListOfKeys();
  if ( l != 0 ) {
    TListIter iter(l);
    while ( TObject* key = iter() ) {
      TObject* obj = file.Get(key->GetName());
      TH1* h1 = dynamic_cast<TH1*>(obj);
      if ( h1 == 0 ) continue;
      m_list.Add(obj);
      UInt_t id = h1->GetUniqueID();
      m_map[id] = h1;
    }
  }
}

void AcdHistCalibMap::bookHists(const char* prefix) {

  if ( m_config == AcdMap::LAT ) {
    for ( UInt_t iPmt(0); iPmt < AcdMap::nPmt; iPmt++ ) {    
      for ( UInt_t iFace(0); iFace < AcdMap::nFace; iFace++ ) {
	UInt_t nRow = AcdMap::getNRow(iFace);
	for ( UInt_t iRow(0); iRow < nRow; iRow++ ) {
	  UInt_t nCol = AcdMap::getNCol(iFace,iRow);
	  for ( UInt_t iCol(0); iCol < nCol; iCol++ ) {
	    UInt_t key = AcdMap::makeKey(iPmt,iFace,iRow,iCol);
	    std::string suffix;
	    AcdMap::makeSuffix(suffix,iPmt,iFace,iRow,iCol);
	    std::string histName(prefix); histName += suffix;
	    TH1F* theHist = new TH1F(histName.c_str(),histName.c_str(),m_bins,m_lo,m_hi);
	    theHist->SetUniqueID(key);
	    m_map[key] = theHist;
	    m_list.Add(theHist);
	  }
	}
      }
    }
  } else if ( m_config == AcdMap::BEAM ) {
    //static const UInt_t tileKeyList[16] = {314,304,424,603,414,404,44,34,24,423,413,403,43,33,23,42};
    //for ( UInt_t i(0); i < 16; i++ ) {
    static const UInt_t tileKeyList[10] = {0,100,110,120,130,1000,1100,1110,1120,1130};
    for ( UInt_t i(0); i < 10 ; i++ ) {
      UInt_t id = tileKeyList[i];
      if ( id >= 2000 ) continue;
      UInt_t pmt = AcdMap::getPmt(id);
      UInt_t face = AcdMap::getFace(id);
      UInt_t row = AcdMap::getRow(id);
      UInt_t col = AcdMap::getCol(id);
      std::string suffix;
      AcdMap::makeSuffix(suffix,pmt,face,row,col);
      std::string histName(prefix); histName += suffix;
      TH1F* theHist = new TH1F(histName.c_str(),histName.c_str(),m_bins,m_lo,m_hi);
      theHist->SetUniqueID(id);
      m_map[id] = theHist;
      m_list.Add(theHist);      
    }
  }
}

Bool_t AcdHistCalibMap::writeHistograms(const char* newFileName ) {
  TFile * histFile(0);
  if ( newFileName != 0 ) {
    histFile = TFile::Open(newFileName, "RECREATE");
    if ( histFile == 0 ) return kFALSE;
  }
  if( histFile == 0 ) return kFALSE;
  histograms().Write();
  histFile->Close();
  return kTRUE;
}
