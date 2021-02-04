
#include "AcdTrendCalib.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TChain.h"

#include "AcdHistCalibMap.h"
#include "AcdCalibMap.h"
#include "AcdCalibUtil.h"
#include "AcdXmlUtil.h"

#include <TMath.h>
#include <TFile.h>
#include <TChain.h>

#include <cassert>
#include <cmath>
#include <fstream>

using std::cout;
using std::cerr;
using std::endl;
using std::string;


AcdTrendCalib::AcdTrendCalib(AcdCalibData::CALTYPE t, AcdKey::Config config)
  :AcdCalibBase(t,config),
   m_nCalib(0),
   m_nInputs(0),
   m_reference(0),
   m_calibs(0),
   m_trendHists(0) {
}


AcdTrendCalib::~AcdTrendCalib() {
}


Bool_t AcdTrendCalib::readCalib(const std::string& calibXmlFile, Bool_t isRef) {
  TString rootFileName(calibXmlFile.c_str());
  rootFileName.ReplaceAll(".xml","Pars.root");
  std::string treeName;
  static const std::string noneString("none");
  AcdXmlUtil::getCalibTreeName(treeName,calType());
  if ( isRef ) {
    if ( m_reference != 0 ) {
      std::cerr << "We already have a reference calibration " << std::endl;
      return kFALSE;
    }    
    TFile* tf = TFile::Open(rootFileName);
    if (tf == 0) {
      std::cerr << "Failed to open file " << rootFileName << std::endl;
      return kFALSE;
    }
    TObject* obj = tf->Get(treeName.c_str());
    if ( obj == 0 ) {
      std::cerr << "File " << rootFileName << " does not contain a tree " << treeName << std::endl;
      return kFALSE;
    }
    TTree* t = dynamic_cast<TTree*>(obj);
    if ( t  == 0 ) {
      std::cerr << "Object " << treeName << " in file " << rootFileName << " is not a TTree." << std::endl;
      return kFALSE;
    }    
    m_reference = t;
    m_refFileName = rootFileName;
  } else if ( calibXmlFile == noneString ) {    
    m_indexMap[m_nCalib] = -1;
    m_nCalib++;
  } else {
    //if ( rootFileName ==  m_refFileName ) return kTRUE;
    if ( m_calibs == 0 ) {
      m_calibs = new TChain(treeName.c_str());
    }
    if ( ! m_calibs->Add(rootFileName) ) {
      std::cerr << "Failed to add file " << rootFileName << " to chain" << std::endl;
      return kFALSE;
    }
    m_indexMap[m_nCalib] = m_nInputs;
    m_nInputs++;
    m_nCalib++;
  }
  return kTRUE; 
}

Bool_t AcdTrendCalib::fillHistograms() {
  
  if ( m_reference == 0 ) {
    std::cerr << "No reference data" << std::endl;
    return kFALSE;
  }

  makeSummaryHists();

  m_trendHists = bookHists(AcdCalib::H_TREND,m_nCalib,0.,(Float_t)m_nCalib,m_summaryHists.size());

  UInt_t id_ref[216];
  UInt_t pmt_ref[216];
  Int_t status_ref[216];
  UInt_t id[216];
  UInt_t pmt[216];
  Int_t status[216];


  Float_t refVals[10][216];
  Float_t vals[10][216];
  
  m_reference->SetBranchStatus("*",1);
  m_calibs->SetBranchStatus("*",1);

  m_reference->SetBranchAddress("id",(id_ref));
  m_reference->SetBranchAddress("pmt",(pmt_ref));
  m_reference->SetBranchAddress("status",(status_ref));
  m_calibs->SetBranchAddress("id",(id));
  m_calibs->SetBranchAddress("pmt",(pmt));
  m_calibs->SetBranchAddress("status",(status));

  UInt_t iVal(0);
  UInt_t nAbs(0);
  for ( std::list<std::string>::const_iterator itr = m_trendNames.begin();
	itr != m_trendNames.end(); itr++, iVal++ ) {
    if ( itr->find("Rel") == std::string::npos ) {
      nAbs += 1;
      m_reference->SetBranchAddress(itr->c_str(),(refVals[iVal]));
      m_calibs->SetBranchAddress(itr->c_str(),(vals[iVal]));
    }
  }
  
  // Get the ref vals
  m_reference->GetEntry(0);
  

  // Loop on the calibs
  for ( UInt_t i(0); i < m_nCalib; i++) {

    Int_t input = m_indexMap[i];
    if ( input < 0 ) continue;
    m_calibs->GetEntry(input);
    
    // Loop on the channels
    for ( UInt_t j(0); j < 216; j++ ) {
      if ( id[j] >= 700 ) continue;
      if ( pmt[j] > 1 ) continue;
      if (( status[j] != 0 ) || (status_ref[j] != 0)) continue;

//    if (i < 161) continue;
      
//      int offset = (i > 161 && j > 108)? 1 : 0;
//      int offset = (i > 161)? 1 : 0;
      int offset = 0;


      if ( id[j] != id_ref[j+offset] ) {
	std::cerr << "Id numbers do no match " << i << ' ' << j << ' ' <<  id[j] << ' ' << id_ref[j] << std::endl << "Luckily (hopefully), Eric, Dave, and Terri fixed it."<< std::endl ;
//	return kFALSE;
	continue;
      }

      // Loop on the values
      for ( UInt_t k(0); k < m_trendNames.size(); k++ ) {
	if ( ! AcdKey::useChannel( id[j], m_channels[k] ) ) continue;
	Float_t delta = 0.;
	Float_t error = 1.;
	if (  k >= nAbs ) {
	  delta = vals[k-nAbs][j] - refVals[k-nAbs][j+offset];
	  if ( refVals[k-nAbs][j+offset] > 0.5 ) {
	    delta /= float(refVals[k-nAbs][j+offset]);
	  } else {
	    delta = 0.42;
	  }
	  error = 0.01;
	} else {
	  delta = vals[k][j] - refVals[k][j+offset];
	}
	fillHistBin(*m_trendHists,id[j],pmt[j],i+1,delta,error,k);
	m_summaryHists[k]->Fill((Float_t)i,delta);
      }
    }    
  }
  return kTRUE;
}


Bool_t AcdTrendCalib::writeOutputs(const std::string& outputPrefix, const std::string& /* timestamp */) {
  
  std::string suffix;
  AcdXmlUtil::getSuffix(suffix, calType() );  
  std::string histFile = outputPrefix + suffix + ".root";
  std::string histSummaryFile = outputPrefix + suffix + "_summary.root";
  std::string plotFile = outputPrefix + suffix + "_";

  if ( ! AcdCalibUtil::makeTrendPlots(*m_trendHists,m_summaryHists,plotFile.c_str()) ) {
    std::cerr << "Failed to write fits to " << plotFile << std::endl;
    return kFALSE;
  }

  if ( ! m_trendHists->writeHistograms( histFile.c_str() ) ) {
    std::cerr << "Failed to write histograms to " << histFile << std::endl;
    return kFALSE;
  }

  TFile* fSummary = TFile::Open(histSummaryFile.c_str(),"CREATE");
  if ( fSummary == 0 || fSummary->IsZombie() ) {
    std::cerr << "Failed to write summary histograms to " << histSummaryFile  << std::endl;
    return kFALSE;    
  }
  for ( std::vector<TH2*>::iterator itr = m_summaryHists.begin(); 
	itr != m_summaryHists.end(); itr++ ) {
    (*itr)->Write();
  }
  fSummary->Close();
  delete fSummary;

  return kTRUE;
}



void AcdTrendCalib::makeSummaryHists() {
  
  switch ( calType() ) {
  case AcdCalibData::PEDESTAL:
    addSummaryHist("mean","Trend of Pedestal Mean","(PHA)",-25.,25.);
    break;   
  case AcdCalibData::GAIN:
    addSummaryHist("peak","Trend of MIP Peak","(PHA)",-400.,400.);
    addSummaryHist("width","Trend of MIP Width","(PHA)",-200.,200.);
    addSummaryHist("RelPeak","Trend of MIP Peak (relative to reference)","",-0.50,0.50);
    addSummaryHist("RelWidth","Trend of MIP Width (relative to reference)","",-0.50,0.50);
    break;
  case AcdCalibData::VETO:
    addSummaryHist("veto","Trend of Veto Threshold","(PHA)",-200.,200.,AcdKey::Tiles);
    break;
  case AcdCalibData::RANGE:
    addSummaryHist("low_max","Trend of Max Low Range PHA","(PHA)",-200.,200.);
    addSummaryHist("high_min","Trend of Min High Range PHA","(PHA)",-50.,50.);
    break;
  case AcdCalibData::CNO:
    addSummaryHist("cno","Trend of CNO Threshold","(PHA)",-25.,25.,AcdKey::NoSkirt);
    break;
  case AcdCalibData::HIGH_RANGE:
    addSummaryHist("pedestal","Trend of High Range Pedestal","(PHA)",-25.,25.);
    addSummaryHist("slope","Trend of High Range PHA/mip","(PHA/mip)",-2.5,2.5);
    addSummaryHist("saturation","Trend of High Range Max PHA","(PHA)",-500.,500.);
    break;
  case AcdCalibData::COHERENT_NOISE:
    addSummaryHist("amplitude","Trend of Noise Amplitude","(PHA)",-10.,10.);
    addSummaryHist("decay","Trend of Noise Decay Rate","(1/ticks)",-500.,500.);
    addSummaryHist("phase","Trend of Noise Phase","(rad)",-2.,2.);
    break;
  case AcdCalibData::RIBBON:
    break;
  case AcdCalibData::PED_HIGH:
    addSummaryHist("mean","Trend of High Range Pedestal","(PHA)",-25.,25.);
    break;
  case AcdCalibData::CARBON:
    addSummaryHist("peak","Trend of Carbon Peak","(PHA)",-100.,100.);
    addSummaryHist("width","Trend of Carbon Peak Width","(PHA)",-100.,100.);
    break;
  case AcdCalibData::VETO_FIT:
    addSummaryHist("slope","Trend of veto pha/dac slope","(PHA/dac)",-0.1,0.1,AcdKey::Tiles);
    addSummaryHist("offset","Trend of veto pha/dac offest","(dac)",-10.,10.,AcdKey::Tiles);
    break;
  case AcdCalibData::CNO_FIT:
    addSummaryHist("slope","Trend of hld pha/dac slope","(PHA/dac)",-0.1,0.1,AcdKey::NoSkirt);
    addSummaryHist("offset","Trend of pha/dac offest","(dac)",-10.,10.,AcdKey::NoSkirt);
    break;
  case AcdCalibData::MERITCALIB:
    addSummaryHist("ped","Trend of pedestals","(mip)",-0.1,0.1);
    addSummaryHist("mip","Trend of MIP peaks","(mip)",-1.,1.,AcdKey::Tiles);
    addSummaryHist("veto","Trend of veto Threshold","(mip)",-1.,1.,AcdKey::Tiles);
    addSummaryHist("cno","Trend of CNO Threshold","(mip)",-25.,25.,AcdKey::NoSkirt);
    addSummaryHist("range","Trend of Range Crossover Gap","log10(mip)",-2.,2.);
    break;
  default:
    break;
  }

  for ( std::vector<TH2*>::const_iterator itr = m_summaryHists.begin();
	itr != m_summaryHists.end(); itr++ ) {
    m_trendNames.push_back( std::string( (*itr)->GetName() ) );
  }

}

void AcdTrendCalib::addSummaryHist(const char* name, const char* title, const char* units,
				   Float_t min, Float_t max, AcdKey::ChannelSet cSet) {
  TH2F* h =  new TH2F(name,title,m_nCalib,0,(Float_t)m_nCalib,40,min,max);
  h->SetXTitle("Test Phase");
  TString yTitle("#Delta ");
  yTitle += name;
  yTitle += " ";
  yTitle += units;
  h->SetYTitle(yTitle);
  m_summaryHists.push_back(h);
  m_channels.push_back(cSet);
}

