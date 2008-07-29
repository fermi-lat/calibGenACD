#define AcdHistCalibMap_cxx
#include "AcdHistCalibMap.h"

#include <TFile.h>
#include <TH2.h>

TH1* AcdCalibHistHolder::getHist(UInt_t idx) const {
  return idx < m_hists.size() ?  m_hists[idx] : 0;
}

void AcdCalibHistHolder::addHist(TH1& hist) {
  m_hists.push_back(&hist);
}

AcdHistCalibMap::AcdHistCalibMap(TFile& file, AcdKey::Config config) 
  :m_config(config),
   m_bins(0),m_lo(0.),m_hi(0.){
  TList* l = file.GetListOfKeys();
  if ( l != 0 ) {
    TListIter iter(l);
    while ( TObject* key = iter() ) {
      TObject* obj = file.Get(key->GetName());
      TH1* h1 = dynamic_cast<TH1*>(obj);
      if ( h1 == 0 ) continue;
      m_list.Add(obj);
      UInt_t id = h1->GetUniqueID();
      m_map[id].addHist(*h1);
    }
  }

}


void AcdHistCalibMap::bookHists(const char* prefix, UInt_t n) {

if ( m_config == AcdKey::LAT || m_config == AcdKey::RIBBONS ) {
    for ( UInt_t iPmt(0); iPmt < AcdKey::nPmt; iPmt++ ) {    
      for ( UInt_t iFace(0); iFace < AcdKey::nFace; iFace++ ) {
	if ( m_config == AcdKey::RIBBONS && iFace < 5 ) continue;
	UInt_t nRow = AcdKey::getNRow(iFace);
	for ( UInt_t iRow(0); iRow < nRow; iRow++ ) {
	  UInt_t nCol = AcdKey::getNCol(iFace,iRow);
	  for ( UInt_t iCol(0); iCol < nCol; iCol++ ) {	    
	    UInt_t key = AcdKey::makeKey(iPmt,iFace,iRow,iCol);
	    for ( UInt_t idx(0); idx < n; idx++ ) {	      
	      std::string suffix;
	      AcdKey::makeSuffix(suffix,iPmt,iFace,iRow,iCol,idx);
	      std::string histName(prefix); histName += suffix;
	      TH1F* theHist = new TH1F(histName.c_str(),histName.c_str(),m_bins,m_lo,m_hi);
	      theHist->SetUniqueID(key + (10000*idx));
	      m_map[key].addHist(*theHist);
	      m_list.Add(theHist);
	    }
	  }
	}
      }
    }
  } else if ( m_config == AcdKey::BEAM ) {
    static const UInt_t tileKeyList[10] = {0,100,110,120,130,1000,1100,1110,1120,1130};
    for ( UInt_t i(0); i < 10 ; i++ ) {
      UInt_t id = tileKeyList[i];
      if ( id >= 2000 ) continue;
      UInt_t pmt = AcdKey::getPmt(id);
      UInt_t face = AcdKey::getFace(id);
      UInt_t row = AcdKey::getRow(id);
      UInt_t col = AcdKey::getCol(id);
      for ( UInt_t idx(0); idx < n; idx++ ) {
	std::string suffix;
	AcdKey::makeSuffix(suffix,pmt,face,row,col,idx);
	std::string histName(prefix); histName += suffix;
	TH1F* theHist = new TH1F(histName.c_str(),histName.c_str(),m_bins,m_lo,m_hi);
	theHist->SetUniqueID(id);
	m_map[id].addHist(*theHist);
	m_list.Add(theHist);      
      }
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
