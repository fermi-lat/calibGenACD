#define AcdCalibUtil_cxx

#include "AcdReportPlots.h"


#include <TCanvas.h>
#include <TLine.h>
#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <TMath.h>
#include <TList.h>
#include <TSystem.h>
#include <TString.h>
#include <TTreeFormula.h>
#include <TChain.h>
#include <cmath>


// Various constants

namespace AcdReport {
  // Number of bins in tile plots
  Int_t nBins_Tiles = 450;
  Int_t nBins_Chan  = 650;
  
  // Number of bins 
  Int_t nBins_Var   = 100;
  
  // Ranges
  Float_t min_Ped   = 0.;
  Float_t max_Ped   = 1000.;
  Float_t del_Ped   = 30.;
  
  // Names + Titles  
  TString name_mean      = "mean";
  TString name_peak      = "peak";
  TString name_veto      = "veto";
  TString name_cno       = "cno";
  TString name_ribbon    = "ribbon";
  TString name_width     = "width";
  TString name_loMax     = "low_max";
  TString name_hiMin     = "high_min";
  TString name_amp       = "amplitude";
  TString name_dec       = "decay";
  TString name_phase     = "phase";
  TString name_slope     = "slope";
  TString name_offset    = "offset";
  TString name_ped       = "ped";
  TString name_ped2      = "pedestal";
  TString name_sat       = "saturation";
  TString name_mip       = "mip";
  TString name_range     = "range";

  TString name_id        = "id";
  TString name_sum       = "sum";
  
  TString title_id       = "Channel ID";
  TString title_ped      = "Pedestal";
  TString title_pedHigh  = "High Range Pedestal";
  TString title_gain     = "MIP peak";
  TString title_carbon   = "Carbon peak";
  TString title_veto     = "Veto Threshold";
  TString title_cno      = "CNO Threshold";
  TString title_slope    = "DAC v PHA Slope";
  TString title_offset   = "DAC Offset";
  TString title_hrSlope  = "PHA v MIP Slope";
  TString title_hrSat    = "High Range Saturation";
  
  TString title_amp      = "Noise Amplitude";
  TString title_dec      = "Noise Decay Lifetime";
  TString title_phase    = "Noise Phase";

  TString title_loMax    = "Low Range Max";
  TString title_hiMin    = "Hi Range Min";
  TString title_range    = "Range Crossover";
  TString title_mip      = "MIP peak";

  TString units_rel      = " ";
  TString units_pha      = "(PHA)";
  TString units_mip      = "(mip)";
  TString units_decay    = "(ticks)";
  TString units_phase    = "(rad)";
  TString units_dac      = "(dac)";
  TString units_slope    = "(dac/PHA)";
  TString units_hrSlope  = "(MIP/PHA)";
  
  // Cuts
  TString cut_chan       = "id<700";
  TString cut_tile       = "id<500";
  TString cut_rib        = "id>=500 && id<700";
  TString cut_skirt      = "id<100 || ( id < 500 && ( (id%100) < 20 ) )";
  TString cut_side_p_rib = "(id>=500 && id<700) && (((id>550)*((id%2)==0) + (id<550)*((id%4)>1) + pmt)%2) == 0";
  TString cut_side_n_rib = "(id>=500 && id<700) && (((id>550)*((id%2)==0) + (id<550)*((id%4)>1) + pmt)%2) == 1";

  // Other
  TString gif_suffix = ".png";
  //  TString gif_suffix = ".pdf";

}


void AcdReport::savePlot(TCanvas& c, TH1& plot, const char* outputPrefix, std::list<std::string>& plotNames) {
  c.cd();
  plot.Draw();
  std::string gifName( outputPrefix );
  gifName += "_"; 
  gifName += plot.GetName();
  gifName += gif_suffix;
  c.Update(); c.SaveAs(gifName.c_str());
  plotNames.push_back(gifName);
}

void AcdReport::savePlots(TCanvas& c, TH1& plot1, TH1& plot2, const char* outputPrefix, std::list<std::string>& plotNames) {
  c.cd();
  plot1.Draw();
  plot2.SetLineColor(2);
  plot2.SetMarkerColor(2);
  plot2.Draw("same");
  std::string gifName( outputPrefix );
  gifName += "_"; 
  gifName += plot1.GetName();
  gifName += gif_suffix;
  c.Update(); c.SaveAs(gifName.c_str());
  plotNames.push_back(gifName);
}

void AcdReport::makeTrendChain( const std::string& calType, 
				const std::string& refFile, const std::list<std::string>& trendFiles,
				TChain*& trendChain ) {
  trendChain = new TChain(calType.c_str());
  TChain* refChain = new TChain(calType.c_str());
  for ( std::list<std::string>::const_iterator itr = trendFiles.begin();
	itr != trendFiles.end(); itr++ ) {
    trendChain->Add(itr->c_str());
    refChain->Add(refFile.c_str());
  }
  trendChain->AddFriend(refChain,"ref");
}

Bool_t AcdReport::makeSummaryPlots( AcdCalibData::CALTYPE pType, const char* outputPrefix, TTree* inTree, 
				    std::list<std::string>& plotNames ) {
  Bool_t ok(kFALSE);
  switch (pType) {
  case AcdCalibData::PEDESTAL:
    ok = makeSummaryPlots_Ped(inTree,outputPrefix,plotNames); break;    
  case AcdCalibData::GAIN:
    ok = makeSummaryPlots_Gain(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::VETO:
    ok = makeSummaryPlots_Veto(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::RANGE:
    ok = makeSummaryPlots_Range(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::CNO:
    ok = makeSummaryPlots_Cno(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::HIGH_RANGE:
    ok = makeSummaryPlots_HighRange(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::COHERENT_NOISE:
    ok = makeSummaryPlots_CohNoise(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::RIBBON:
    ok = makeSummaryPlots_Ribbon(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::PED_HIGH:
    ok = makeSummaryPlots_PedHigh(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::CARBON:
    ok = makeSummaryPlots_Carbon(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::VETO_FIT:
    ok = makeSummaryPlots_VetoFit(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::CNO_FIT:
    ok = makeSummaryPlots_CnoFit(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::MERITCALIB:
    ok = makeSummaryPlots_Check(inTree,outputPrefix,plotNames); break; 
  default:
    break;
  }
  if (!ok) return kFALSE;
  return kTRUE;
}


Bool_t AcdReport::makeDeltaPlots( AcdCalibData::CALTYPE pType, const char* outputPrefix, TTree* inTree, 
				  std::list<std::string>& plotNames ){
  Bool_t ok(kFALSE);
  switch (pType) {
  case AcdCalibData::PEDESTAL:
    ok = makeDeltaPlots_Ped(inTree,outputPrefix,plotNames); break;    
  case AcdCalibData::GAIN:
    ok = makeDeltaPlots_Gain(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::VETO:
    ok = makeDeltaPlots_Veto(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::RANGE:
    ok = makeDeltaPlots_Range(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::CNO:
    ok = makeDeltaPlots_Cno(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::HIGH_RANGE:
    ok = makeDeltaPlots_HighRange(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::COHERENT_NOISE:
    ok = makeDeltaPlots_CohNoise(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::RIBBON:
    ok = makeDeltaPlots_Ribbon(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::PED_HIGH:
    ok = makeDeltaPlots_PedHigh(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::CARBON:
    ok = makeDeltaPlots_Carbon(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::VETO_FIT:
    ok = makeDeltaPlots_VetoFit(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::CNO_FIT:
    ok = makeDeltaPlots_CnoFit(inTree,outputPrefix,plotNames); break; 
  case AcdCalibData::MERITCALIB:
    ok = kTRUE; break;
  default:
    break;
  }
  if (!ok) return kFALSE;
  return kTRUE; 
}


Bool_t AcdReport::makeSummaryPlot_Generic( TTree* inTree, TH1*& hist,
					   const TString& name, const TString& title,
					   const TString& varName, const TString& units,
					   const TString& cut,
					   UInt_t nBins, Float_t low, Float_t hi) {
  TString xTitle = title + " " + units;
  hist = new TH1F(name+"_1d",title,nBins,low,hi);
  hist->SetLineWidth(2);
  hist->SetXTitle(xTitle);
  inTree->Project(hist->GetName(),varName,cut);
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlot_GenericByChan( TTree* inTree, TH2*& hist,
						 const TString& name, const TString& title,
						 const TString& varName, const TString& units,
						 const TString& cut,
						 UInt_t nBins, Float_t low, Float_t hi ) {

  TString yTitle = title + " " + units;
  TString fullTitle = title + " v. Channel";
  hist = new TH2F(name+"_2d",fullTitle,650,0.,650.,nBins,low,hi);
  hist->SetLineWidth(2);
  hist->SetMarkerStyle(6); 
  hist->SetXTitle(title_id);
  hist->SetYTitle(yTitle);
  hist->SetTitle(fullTitle);
  inTree->Project(hist->GetName(),varName + ":id",cut); 
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlot_Generic( TTree* inTree, TH1*& hist,
					 const TString& name, const TString& title,
					 const TString& varName, const TString& units,
					 const TString& cut,
					 UInt_t nBins, Float_t low, Float_t hi) {

  TString fullTitle = "#Delta " + title;
  TString xTitle = title + " " + units;
  TString varExp = varName + " - old." + varName;
  hist = new TH1F(name+"_del_1d",fullTitle,nBins,low,hi);
  hist->SetLineWidth(2);
  hist->SetXTitle(xTitle);
  inTree->Project(hist->GetName(),varExp,cut);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlot_GenericByChan( TTree* inTree, TH2*& hist,
					       const TString& name, const TString& title,
					       const TString& varName, const TString& units,
					       const TString& cut,
					       UInt_t nBins, Float_t low, Float_t hi ) {

  TString yTitle = "#Delta " + title + " " + units;
  TString fullTitle = "#Delta " + title + " v. Channel";
  TString varExp = varName + " - old." + varName + ":id";
  hist = new TH2F(name+"_del_2d",title,650,0,650,nBins,low,hi);
  hist->SetLineWidth(2);
  hist->SetMarkerStyle(6); 
  hist->SetXTitle(title_id);
  hist->SetYTitle(yTitle);
  hist->SetTitle(fullTitle);
  inTree->Project(hist->GetName(),varExp,cut); 
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlot_Relative( TTree* inTree, TH1*& hist,
					  const TString& name, const TString& title,
					  const TString& varName, const TString& units,
					  const TString& cut,
 					  UInt_t nBins, Float_t low, Float_t hi ){
  TString fullTitle = "#Delta" + title + "/" + title;
  TString xTitle = fullTitle + " " + units;
  TString varExp = "(" + varName + " - old." + varName + ")/" + varName;
  hist = new TH1F(name+"_rel",fullTitle,nBins,low,hi);
  hist->SetLineWidth(2);
  hist->SetXTitle(xTitle);
  inTree->Project(hist->GetName(),varExp,cut);
  return kTRUE;
}

Bool_t AcdReport::makeCorrelPlot_Generic( TTree* inTree, TH2*& hist,
					  const TString& name, const TString& title,
					  const TString& varName, const TString& units,
					  const TString& cut,
					  UInt_t nBins, Float_t low, Float_t hi ) {
  TString xTitle = title + " " + units;
  TString yTitle = title + " " + units;
  TString fullTitle = title + ": new v. old";
  TString varExp = varName + ":old." + varName;
  hist = new TH2F(name+"_correl",fullTitle,nBins,low,hi,nBins,low,hi);
  hist->SetLineWidth(2);
  hist->SetMarkerStyle(6); 
  hist->SetXTitle(xTitle);
  hist->SetYTitle(yTitle);
  hist->SetTitle(fullTitle);
  inTree->Project(hist->GetName(),varExp,cut); 
  return kTRUE;
}

Bool_t AcdReport::makeTrendPlot_Generic( TTree* inTree, TH2*& hist,
					 const TString& name, const TString& title,
					 const TString& varName, const TString& units,
					 const TString& cut, Bool_t relative, 
					 UInt_t nBins, Float_t low, Float_t hi ) {
  TString xTitle = "Test Phase";
  TString yTitle = "#Delta" + title + " " + units;
  TString fullTitle = "Trend of " + title;
  Long64_t nBin = inTree->GetEntries();
  TString fullName = name+"_trend";
  fullName += relative ? "_rel" : "_abs"; 
  hist = new TH2F(fullName,fullTitle,nBin,0.,(Float_t)nBin,nBins,low,hi);
  TTreeFormula fVar("fVar",varName,inTree);
  TTreeFormula fRef("fRef","ref."+varName,inTree);
  TTreeFormula fCut("fCut",cut,inTree);
  for ( Long64_t i(0); i < nBin; i++ ) {
    inTree->LoadTree(i);
    for ( UInt_t idx(0); idx < 216; idx++ ) {
      Float_t cut = fCut.EvalInstance(idx);
      if ( cut < 0.5 ) continue;
      Float_t val = fVar.EvalInstance(idx);
      Float_t ref = fRef.EvalInstance(idx);
      val -= ref;
      if ( relative ) {
	if ( TMath::Abs(ref) < 1e-9 ) {
	  val = 0.;
	} else {
	  val /= ref;
	}
      }
      hist->Fill(i,val);
    }
  }
  return kTRUE;
}


Bool_t AcdReport::makeTrendTimePlot_Generic( TTree* inTree, TH2*& hist,
					     const TString& name,
					     TList& timeLines,
					     Double_t plotStart,
					     Double_t plotSpan ) {

  Long64_t nBin = inTree->GetEntries();
  TString fullTitle = "Test Phases for " + name;
  TString yTitle = "Test Phase";
  TString xTitle = "Date";
  hist = new TH2F(name+"_trendPhases",fullTitle,100,plotStart,plotStart+plotSpan,nBin,0,nBin);
  TTreeFormula fStartTime("fStart","startTime",inTree);
  TTreeFormula fEndTime("fEnd","endTime",inTree);
  for ( Long64_t i(0); i < nBin; i++ ) {
    inTree->LoadTree(i);
    TLine* l = new TLine( fStartTime.EvalInstance(), i, fEndTime.EvalInstance(), i);
    l->SetLineWidth(2);
    l->SetLineColor( (i % 2) == 0 ? 1 : 4 );
    timeLines.AddLast(l);
  }
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_Ped( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  if ( ! makeSummaryPlot_Generic(inTree,h1,name_sum,title_ped,name_mean,units_pha,cut_chan,100,0.,1000.) ) return kFALSE;
  savePlot(cnv,*h1,outputPrefix,plotNames);
  TH2* h2(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2,name_sum,title_ped,name_mean,units_pha,cut_chan,100,0.,1000.) ) return kFALSE;
  savePlot(cnv,*h2,outputPrefix,plotNames);
  return kTRUE;
} 

Bool_t AcdReport::makeDeltaPlots_Ped( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  if ( ! makeDeltaPlot_Generic(inTree,h1,name_sum,title_ped,name_mean,units_pha,cut_chan,100,-30.,30.) ) return kFALSE;
  savePlot(cnv,*h1,outputPrefix,plotNames);
  TH2* h2(0);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2,name_sum,title_ped,name_mean,units_pha,cut_chan,100,-30.,30.) ) return kFALSE;
  savePlot(cnv,*h2,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_Gain( TTree* inTree, const char* outputPrefix, 
					 std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  TH1* h1_rib(0);
  const TString& units = units_pha;
  Float_t max = 1500.;
  if ( ! makeSummaryPlot_Generic(inTree,h1,name_sum,
				 title_gain,name_peak,units,cut_chan,100,0.,max) ) return kFALSE;
  if ( ! makeSummaryPlot_Generic(inTree,h1_rib,name_sum+"_rib",
				 title_gain,name_peak,units,cut_rib,100,0.,max) ) return kFALSE;
  savePlots(cnv,*h1,*h1_rib,outputPrefix,plotNames);
  TH2* h2(0);
  TH2* h2_rib(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2,name_sum,
				       title_gain,name_peak,units,cut_tile,100,0.,max) ) return kFALSE;
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_rib,name_sum+"_rib",
				       title_gain,name_peak,units,cut_rib,100,0.,max) ) return kFALSE;  
  savePlots(cnv,*h2,*h2_rib,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_Gain( TTree* inTree, const char* outputPrefix, 
				       std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  TH1* h1_rib(0);
  const TString& units = units_pha;
  const TString& noUnits = units_rel;
  Float_t max = 1500.;
  if ( ! makeDeltaPlot_Relative(inTree,h1,name_sum,
				title_gain,name_peak,noUnits,cut_chan,100,-1.,1.) ) return kFALSE;
  if ( ! makeDeltaPlot_Relative(inTree,h1_rib,name_sum+"_rib",
				title_gain,name_peak,noUnits,cut_rib,100,-1.,1.) ) return kFALSE;
  savePlots(cnv,*h1,*h1_rib,outputPrefix,plotNames);
  TH2* h2(0);
  TH2* h2_rib(0);
  if ( ! makeCorrelPlot_Generic(inTree,h2,name_sum,
				title_gain,name_peak,units,cut_chan,100,0.,max) ) return kFALSE;
  if ( ! makeCorrelPlot_Generic(inTree,h2_rib,name_sum+"_rib",
				title_gain,name_peak,units,cut_rib,100,0.,max) ) return kFALSE;
  savePlots(cnv,*h2,*h2_rib,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_Veto( TTree* inTree, const char* outputPrefix, 
					 std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  TH1* h1_rib(0);
  const TString& units = units_pha;
  Float_t max = 1500.;
  if ( ! makeSummaryPlot_Generic(inTree,h1,name_sum,
				 title_veto,name_veto,units,cut_chan,100,0.,max) ) return kFALSE;
  if ( ! makeSummaryPlot_Generic(inTree,h1_rib,name_sum+"_rib",
				 title_veto,name_veto,units,cut_rib,100,0.,max) ) return kFALSE;
  savePlots(cnv,*h1,*h1_rib,outputPrefix,plotNames);
  TH2* h2(0);
  TH2* h2_rib(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2,name_sum,
				       title_veto,name_veto,units,cut_tile,100,0.,max) ) return kFALSE;
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_rib,name_sum+"_rib",
				       title_veto,name_veto,units,cut_rib,100,0.,max) ) return kFALSE;
  savePlots(cnv,*h2,*h2_rib,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_Veto( TTree* inTree, const char* outputPrefix, 
				       std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  TH1* h1_rib(0);
  const TString& units = units_pha;
  Float_t maxDel = 300.;
  if ( ! makeDeltaPlot_Generic(inTree,h1,name_sum,
			       title_veto,name_veto,units,cut_chan,100,-maxDel,maxDel) ) return kFALSE;
  if ( ! makeDeltaPlot_Generic(inTree,h1_rib,name_sum+"_rib",
			       title_veto,name_veto,units,cut_rib,100,-maxDel,maxDel) ) return kFALSE;
  savePlots(cnv,*h1,*h1_rib,outputPrefix,plotNames);
  TH2* h2(0);
  TH2* h2_rib(0);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2,name_sum,
				     title_veto,name_veto,units,cut_chan,100,-maxDel,maxDel) ) return kFALSE;
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_rib,name_sum+"_rib",
				     title_veto,name_veto,units,cut_rib,100,-maxDel,maxDel) ) return kFALSE;
  savePlots(cnv,*h2,*h2_rib,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_Range( TTree* inTree, const char* outputPrefix, 
					  std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1_lo(0);
  TH1* h1_hi(0);
  if ( ! makeSummaryPlot_Generic(inTree,h1_lo,name_sum+"_loMax",
				 title_loMax,name_loMax,units_pha,cut_chan,100,3600.,4100.) ) return kFALSE;
  savePlot(cnv,*h1_lo,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_Generic(inTree,h1_hi,name_sum+"_hiMin",
				 title_hiMin,name_hiMin,units_pha,cut_chan,100,0.,1000.) ) return kFALSE;
  savePlot(cnv,*h1_hi,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));
  TH2* h2_lo(0);
  TH2* h2_hi(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_lo,name_sum+"_loMax",
				       title_loMax,name_loMax,units_pha,cut_chan,100,3600.,4100.) ) return kFALSE;
  savePlot(cnv,*h2_lo,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_hi,name_sum+"_hiMin",
				       title_hiMin,name_hiMin,units_pha,cut_chan,100,0.,1000.) ) return kFALSE;
  savePlot(cnv,*h2_hi,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_Range( TTree* inTree, const char* outputPrefix, 
					std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1_lo(0);
  TH1* h1_hi(0);
  if ( ! makeDeltaPlot_Generic(inTree,h1_lo,name_sum+"_loMax",
			       title_loMax,name_loMax,units_pha,cut_chan,100,-200.,200.) ) return kFALSE;
  savePlot(cnv,*h1_lo,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_Generic(inTree,h1_hi,name_sum+"_hiMin",
			       title_hiMin,name_hiMin,units_pha,cut_chan,100,-200.,200.) ) return kFALSE;
  savePlot(cnv,*h1_hi,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));
  TH2* h2_lo(0);
  TH2* h2_hi(0);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_lo,name_sum+"_loMax",
				       title_loMax,name_loMax,units_pha,cut_chan,100,-200.,200.) ) return kFALSE;
  savePlot(cnv,*h2_lo,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_hi,name_sum+"_hiMin",
				     title_hiMin,name_hiMin,units_pha,cut_chan,100,-200.,200.) ) return kFALSE;
  savePlot(cnv,*h2_hi,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_Cno( TTree* inTree, const char* outputPrefix, 
					std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  const TString& units = units_pha;
  Float_t max = 300.;
  if ( ! makeSummaryPlot_Generic(inTree,h1,name_sum,
				 title_cno,name_cno,units,cut_skirt,100,0.,max) ) return kFALSE;
  savePlot(cnv,*h1,outputPrefix,plotNames);
  TH2* h2(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2,name_sum,
				       title_cno,name_cno,units,cut_skirt,100,0.,max) ) return kFALSE;
  savePlot(cnv,*h2,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_Cno( TTree* inTree, const char* outputPrefix, 
				      std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  const TString& units = units_pha;
  Float_t maxDel = 300.;
  if ( ! makeDeltaPlot_Generic(inTree,h1,name_sum,
			       title_cno,name_cno,units,cut_skirt,100,-maxDel,maxDel) ) return kFALSE;
  savePlot(cnv,*h1,outputPrefix,plotNames);
  TH2* h2(0);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2,name_sum,
				     title_cno,name_cno,units,cut_skirt,100,-maxDel,maxDel) ) return kFALSE;
  savePlot(cnv,*h2,outputPrefix,plotNames);
  return kTRUE;
} 
 
Bool_t AcdReport::makeSummaryPlots_HighRange( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
 TCanvas cnv;
  TH1* h1_ped(0);
  TH1* h1_slp(0);
  TH1* h1_sat(0);
  if ( ! makeSummaryPlot_Generic(inTree,h1_ped,name_sum+"_ped",
				 title_pedHigh,name_ped2,units_pha,cut_chan,100,0.,1000.) ) return kFALSE;
  savePlot(cnv,*h1_ped,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_Generic(inTree,h1_slp,name_sum+"_slope",
				 title_hrSlope,name_slope,units_hrSlope,cut_chan,100,0.,10.) ) return kFALSE;
  savePlot(cnv,*h1_slp,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_Generic(inTree,h1_sat,name_sum+"_sat",
				 title_hrSat,name_sat,units_pha,cut_chan,100,1500,2500 ) )return kFALSE;
  savePlot(cnv,*h1_sat,outputPrefix,plotNames);
  TH2* h2_ped(0);
  TH2* h2_slp(0);
  TH2* h2_sat(0);
  plotNames.push_back(std::string("br"));
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_ped,name_sum+"_ped",
				       title_pedHigh,name_ped2,units_pha,cut_chan,100,1.,1000.) ) return kFALSE;
  savePlot(cnv,*h2_ped,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_slp,name_sum+"_slope",
				       title_hrSlope,name_slope,units_hrSlope,cut_chan,100,0.,10.) ) return kFALSE;
  savePlot(cnv,*h2_slp,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_sat,name_sum+"_sat",
				       title_hrSat,name_sat,units_pha,cut_chan,100,1500,2500 ) ) return kFALSE;
  savePlot(cnv,*h2_sat,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_HighRange( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1_ped(0);
  TH1* h1_slp(0);
  TH1* h1_sat(0);
  if ( ! makeDeltaPlot_Generic(inTree,h1_ped,name_sum+"_ped",
			       title_pedHigh,name_ped2,units_pha,cut_chan,100,-20.,20.) ) return kFALSE;
  savePlot(cnv,*h1_ped,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_Generic(inTree,h1_slp,name_sum+"_slope",
			       title_hrSlope,name_slope,units_hrSlope,cut_chan,100,-5.,5.) ) return kFALSE;
  savePlot(cnv,*h1_slp,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_Generic(inTree,h1_sat,name_sum+"_sat",
			       title_hrSat,name_sat,units_pha,cut_chan,100,-100,100 ) )return kFALSE;
  savePlot(cnv,*h1_sat,outputPrefix,plotNames);
  TH2* h2_ped(0);
  TH2* h2_slp(0);
  TH2* h2_sat(0);
  plotNames.push_back(std::string("br"));
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_ped,name_sum+"_ped",
				     title_pedHigh,name_ped2,units_pha,cut_chan,100,-20.,20.) ) return kFALSE;
  savePlot(cnv,*h2_ped,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_slp,name_sum+"_slope",
				     title_hrSlope,name_slope,units_hrSlope,cut_chan,100,-5.,5.) ) return kFALSE;
  savePlot(cnv,*h2_slp,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_sat,name_sum+"_sat",
				     title_hrSat,name_sat,units_pha,cut_chan,100,-100,100 ) ) return kFALSE;
  savePlot(cnv,*h2_sat,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_CohNoise(  TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1_amp(0);
  TH1* h1_dec(0);
  TH1* h1_pha(0);
  if ( ! makeSummaryPlot_Generic(inTree,h1_amp,name_sum+"_amp",
				 title_amp,name_amp,units_pha,cut_chan,100,0.,50.) ) return kFALSE;
  savePlot(cnv,*h1_amp,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_Generic(inTree,h1_dec,name_sum+"_decay",
				 title_dec,name_dec,units_decay,cut_chan,100,0.,1500.) ) return kFALSE;
  savePlot(cnv,*h1_dec,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_Generic(inTree,h1_pha,name_sum+"_phase",
				 title_phase,name_phase,units_phase,cut_chan,100,0.,2.*TMath::Pi()) ) return kFALSE;
  savePlot(cnv,*h1_pha,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));
  TH2* h2_amp(0);
  TH2* h2_dec(0);
  TH2* h2_pha(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_amp,name_sum+"_amp",
				       title_amp,name_amp,units_pha,cut_chan,100,0.,50.) ) return kFALSE;
  savePlot(cnv,*h2_amp,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_dec,name_sum+"_decay",
				       title_dec,name_dec,units_decay,cut_chan,100,0.,1500.) ) return kFALSE;
  savePlot(cnv,*h2_dec,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_pha,name_sum+"_phase",
				       title_phase,name_phase,units_phase,cut_chan,100,0.,2.*TMath::Pi()) ) return kFALSE;
  savePlot(cnv,*h2_pha,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_CohNoise( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1_amp(0);
  TH1* h1_dec(0);
  TH1* h1_pha(0);
  if ( ! makeDeltaPlot_Generic(inTree,h1_amp,name_sum+"_amp",
			       title_amp,name_amp,units_pha,cut_chan,100,-20.,20.) ) return kFALSE;
  savePlot(cnv,*h1_amp,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_Generic(inTree,h1_dec,name_sum+"_decay",
			       title_dec,name_dec,units_decay,cut_chan,100,-500.,500.) ) return kFALSE;
  savePlot(cnv,*h1_dec,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_Generic(inTree,h1_pha,name_sum+"_phase",
			       title_phase,name_phase,units_phase,cut_chan,100,-1.*TMath::Pi(),TMath::Pi()) ) return kFALSE;
  savePlot(cnv,*h1_pha,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));
  TH2* h2_amp(0);
  TH2* h2_dec(0);
  TH2* h2_pha(0);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_amp,name_sum+"_amp",
				     title_amp,name_amp,units_pha,cut_chan,100,-20.,20.) ) return kFALSE;
  savePlot(cnv,*h2_amp,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_dec,name_sum+"_decay",
				     title_dec,name_dec,units_decay,cut_chan,100,-500.,500.) ) return kFALSE;
  savePlot(cnv,*h2_dec,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_pha,name_sum+"_phase",
				     title_phase,name_phase,units_phase,cut_chan,100,-1.*TMath::Pi(),TMath::Pi()) ) return kFALSE;
  savePlot(cnv,*h2_pha,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_Ribbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ) {  
  TH1* h0_1d = new TH1F("ribbon_bin0_1d","Ribbon Relative Signal, Bin0",50,-1.,1.);
  TH1* h1_1d = new TH1F("ribbon_bin1_1d","Ribbon Relative Signal, Bin1",50,-1.,1.);
  TH1* h2_1d = new TH1F("ribbon_bin2_1d","Ribbon Relative Signal, Bin2",50,-1.,1.);
  TH1* h4_1d = new TH1F("ribbon_bin4_1d","Ribbon Relative Signal, Bin4",50,-1.,1.);
  TH1* h5_1d = new TH1F("ribbon_bin5_1d","Ribbon Relative Signal, Bin5",50,-1.,1.);
  TH1* h6_1d = new TH1F("ribbon_bin6_1d","Ribbon Relative Signal, Bin6",50,-1.,1.);
  TH2* h0_2d = new TH2F("ribbon_bin0_2d","Ribbon Relative Signal, Bin0",8,-0.5,7.5,50,-1.,1.);
  TH2* h1_2d = new TH2F("ribbon_bin1_2d","Ribbon Relative Signal, Bin1",8,-0.5,7.5,50,-1.,1.);
  TH2* h2_2d = new TH2F("ribbon_bin2_2d","Ribbon Relative Signal, Bin2",8,-0.5,7.5,50,-1.,1.);
  TH2* h4_2d = new TH2F("ribbon_bin4_2d","Ribbon Relative Signal, Bin4",8,-0.5,7.5,50,-1.,1.);
  TH2* h5_2d = new TH2F("ribbon_bin5_2d","Ribbon Relative Signal, Bin5",8,-0.5,7.5,50,-1.,1.);
  TH2* h6_2d = new TH2F("ribbon_bin6_2d","Ribbon Relative Signal, Bin6",8,-0.5,7.5,50,-1.,1.);
  inTree->Project("ribbon_bin0_1d","log10(Frac_n3)",cut_side_p_rib);
  inTree->Project("+ribbon_bin0_1d","log10(Frac_p3)",cut_side_n_rib);
  h0_1d->SetXTitle("log10(Relative Signal)");  h0_1d->SetLineWidth(2); h0_1d->SetLineColor(1);   
  inTree->Project("ribbon_bin1_1d","log10(Frac_n2)",cut_side_p_rib);
  inTree->Project("+ribbon_bin1_1d","log10(Frac_p2)",cut_side_n_rib);
  h1_1d->SetXTitle("log10(Relative Signal)");  h1_1d->SetLineWidth(2); h1_1d->SetLineColor(2);   
  inTree->Project("ribbon_bin2_1d","log10(Frac_n1)",cut_side_p_rib);
  inTree->Project("+ribbon_bin2_1d","log10(Frac_p1)",cut_side_n_rib);
  h2_1d->SetXTitle("log10(Relative Signal)");  h2_1d->SetLineWidth(2); h2_1d->SetLineColor(3);   
  inTree->Project("ribbon_bin4_1d","log10(Frac_p1)",cut_side_p_rib);
  inTree->Project("+ribbon_bin4_1d","log10(Frac_n1)",cut_side_n_rib);
  h4_1d->SetXTitle("log10(Relative Signal)");  h4_1d->SetLineWidth(2); h4_1d->SetLineColor(4);   
  inTree->Project("ribbon_bin5_1d","log10(Frac_p2)",cut_side_p_rib);
  inTree->Project("+ribbon_bin5_1d","log10(Frac_n2)",cut_side_n_rib);
  h5_1d->SetXTitle("log10(Relative Signal)");  h5_1d->SetLineWidth(2); h5_1d->SetLineColor(5);   
  inTree->Project("ribbon_bin6_1d","log10(Frac_p3)",cut_side_p_rib);
  inTree->Project("+ribbon_bin6_1d","log10(Frac_n3)",cut_side_n_rib);
  h6_1d->SetXTitle("log10(Relative Signal)");  h6_1d->SetLineWidth(2); h6_1d->SetLineColor(6);   
  // 2D stuff
  inTree->Project("ribbon_bin0_2d","log10(Frac_n3):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin0_2d","log10(Frac_p3):(id%100)+(4*(id>550))",cut_side_n_rib);
  h0_2d->SetXTitle("log10(Relative Signal)");  h0_2d->SetYTitle("Ribbon Number");
  h0_2d->SetMarkerStyle(6); h0_2d->SetMarkerColor(1);   
  inTree->Project("ribbon_bin1_2d","log10(Frac_n2):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin1_2d","log10(Frac_p2):(id%100)+(4*(id>550))",cut_side_n_rib);
  h1_2d->SetXTitle("log10(Relative Signal)");  h1_2d->SetYTitle("Ribbon Number");
  h1_2d->SetMarkerStyle(6); h1_2d->SetMarkerColor(2);   
  inTree->Project("ribbon_bin2_2d","log10(Frac_n1):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin2_2d","log10(Frac_p1):(id%100)+(4*(id>550))",cut_side_n_rib);
  h2_2d->SetXTitle("log10(Relative Signal)");  h2_2d->SetYTitle("Ribbon Number");
  h2_2d->SetMarkerStyle(6); h2_2d->SetMarkerColor(3);   
  inTree->Project("ribbon_bin4_2d","log10(Frac_p1):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin4_2d","log10(Frac_n1):(id%100)+(4*(id>550))",cut_side_n_rib);
  h4_2d->SetXTitle("log10(Relative Signal)");  h4_2d->SetYTitle("Ribbon Number");
  h4_2d->SetMarkerStyle(6); h4_2d->SetMarkerColor(4);   
  inTree->Project("ribbon_bin5_2d","log10(Frac_p2):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin5_2d","log10(Frac_n2):(id%100)+(4*(id>550))",cut_side_n_rib);
  h5_2d->SetXTitle("log10(Relative Signal)");  h5_2d->SetYTitle("Ribbon Number");
  h5_2d->SetMarkerStyle(6); h5_2d->SetMarkerColor(5);   
  inTree->Project("ribbon_bin6_2d","log10(Frac_p3):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin6_2d","log10(Frac_n3):(id%100)+(4*(id>550))",cut_side_n_rib);
  h6_2d->SetXTitle("log10(Relative Signal)");  h6_2d->SetYTitle("Ribbon Number");
  h6_2d->SetMarkerStyle(6); h6_2d->SetMarkerColor(6);   
  TCanvas cnv;
  //std::string plot1DName(outputPrefix+name_sum+"_1d.gif");
  //std::string plot2DName(outputPrefix+name_sum+"_2d.gif");
  std::string plot1DName(outputPrefix+name_sum+"_1d.png");
  std::string plot2DName(outputPrefix+name_sum+"_2d.png");
  //std::string plot1DName(outputPrefix+name_sum+"_1d.pdf");
  //std::string plot2DName(outputPrefix+name_sum+"_2d.pdf");
  h0_1d->Draw(); h1_1d->Draw("same"); h2_1d->Draw("same"); 
  h4_1d->Draw("same"); h5_1d->Draw("same"); h6_1d->Draw("same"); 
  cnv.Update(); cnv.SaveAs(plot1DName.c_str());
  h0_2d->Draw(); h1_2d->Draw("same"); h2_2d->Draw("same"); 
  h4_2d->Draw("same"); h5_2d->Draw("same"); h6_2d->Draw("same"); 
  cnv.Update(); cnv.SaveAs(plot2DName.c_str());
  plotNames.push_back(plot1DName);
  plotNames.push_back(plot2DName);  
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_Ribbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ) {
  TH1* h0_1d = new TH1F("ribbon_bin0_del_1d","#Delta Ribbon Relative Signal, Bin0",50,-1.,1.);
  TH1* h1_1d = new TH1F("ribbon_bin1_del_1d","#Delta Ribbon Relative Signal, Bin1",50,-1.,1.);
  TH1* h2_1d = new TH1F("ribbon_bin2_del_1d","#Delta Ribbon Relative Signal, Bin2",50,-1.,1.);
  TH1* h4_1d = new TH1F("ribbon_bin4_del_1d","#Delta Ribbon Relative Signal, Bin4",50,-1.,1.);
  TH1* h5_1d = new TH1F("ribbon_bin5_del_1d","#Delta Ribbon Relative Signal, Bin5",50,-1.,1.);
  TH1* h6_1d = new TH1F("ribbon_bin6_del_1d","#Delta Ribbon Relative Signal, Bin6",50,-1.,1.);
  TH2* h0_2d = new TH2F("ribbon_bin0_del_2d","#Delta Ribbon Relative Signal, Bin0",8,-0.5,7.5,50,-1.,1.);
  TH2* h1_2d = new TH2F("ribbon_bin1_del_2d","#Delta Ribbon Relative Signal, Bin1",8,-0.5,7.5,50,-1.,1.);
  TH2* h2_2d = new TH2F("ribbon_bin2_del_2d","#Delta Ribbon Relative Signal, Bin2",8,-0.5,7.5,50,-1.,1.);
  TH2* h4_2d = new TH2F("ribbon_bin4_del_2d","#Delta Ribbon Relative Signal, Bin4",8,-0.5,7.5,50,-1.,1.);
  TH2* h5_2d = new TH2F("ribbon_bin5_del_2d","#Delta Ribbon Relative Signal, Bin5",8,-0.5,7.5,50,-1.,1.);
  TH2* h6_2d = new TH2F("ribbon_bin6_del_2d","#Delta Ribbon Relative Signal, Bin6",8,-0.5,7.5,50,-1.,1.);
  inTree->Project("ribbon_bin0_del_1d","log10(Frac_n3)-log10(old.Frac_n3)",cut_side_p_rib);
  inTree->Project("+ribbon_bin0_del_1d","log10(Frac_p3)-log10(old.Frac_p3)",cut_side_n_rib);
  h0_1d->SetXTitle("#Delta log10(Relative Signal)");  h0_1d->SetLineWidth(2); h0_1d->SetLineColor(1);   
  inTree->Project("ribbon_bin1_del_1d","log10(Frac_n2)-log10(old.Frac_n2)",cut_side_p_rib);
  inTree->Project("+ribbon_bin1_del_1d","log10(Frac_p2)-log10(old.Frac_p2)",cut_side_n_rib);
  h1_1d->SetXTitle("#Delta log10(Relative Signal)");  h1_1d->SetLineWidth(2); h1_1d->SetLineColor(2);   
  inTree->Project("ribbon_bin2_del_1d","log10(Frac_n1)-log10(old.Frac_n1)",cut_side_p_rib);
  inTree->Project("+ribbon_bin2_del_1d","log10(Frac_p1)-log10(old.Frac_p1)",cut_side_n_rib);
  h2_1d->SetXTitle("#Delta log10(Relative Signal)");  h2_1d->SetLineWidth(2); h2_1d->SetLineColor(3);   
  inTree->Project("ribbon_bin3_del_1d","log10(Frac_p1)-log10(old.Frac_p1)",cut_side_p_rib);
  inTree->Project("+ribbon_bin3_del_1d","log10(Frac_n1)-log10(old.Frac_n1)",cut_side_n_rib);
  h4_1d->SetXTitle("#Delta log10(Relative Signal)");  h4_1d->SetLineWidth(2); h4_1d->SetLineColor(4);   
  inTree->Project("ribbon_bin4_del_1d","log10(Frac_p2)-log10(old.Frac_p2)",cut_side_p_rib);
  inTree->Project("+ribbon_bin4_del_1d","log10(Frac_n2)-log10(old.Frac_n2)",cut_side_n_rib);
  h5_1d->SetXTitle("#Delta log10(Relative Signal)");  h4_1d->SetLineWidth(2); h5_1d->SetLineColor(5);   
  inTree->Project("ribbon_bin5_del_1d","log10(Frac_p3)-log10(old.Frac_p3)",cut_side_p_rib);
  inTree->Project("+ribbon_bin5_del_1d","log10(Frac_n3)-log10(old.Frac_n3)",cut_side_n_rib);
  h6_1d->SetXTitle("#Delta log10(Relative Signal)");  h6_1d->SetLineWidth(2); h6_1d->SetLineColor(6);   
  // 2D stuff
  inTree->Project("ribbon_bin0_del_2d","log10(Frac_n3)-log10(old.Frac_n3):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin0_del_2d","log10(Frac_p3)-log10(old.Frac_p3):(id%100)+(4*(id>550))",cut_side_n_rib);
  h0_2d->SetYTitle("#Delta log10(Relative Signal)");  h0_2d->SetXTitle("Ribbon Number");
  h0_2d->SetMarkerStyle(6); h0_2d->SetMarkerColor(1);   
  inTree->Project("ribbon_bin1_del_2d","log10(Frac_n2)-log10(old.Frac_n2):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin1_del_2d","log10(Frac_p2)-log10(old.Frac_p2):(id%100)+(4*(id>550))",cut_side_n_rib);
  h1_2d->SetYTitle("#Delta log10(Relative Signal)");  h1_2d->SetXTitle("Ribbon Number");
  h1_2d->SetMarkerStyle(6); h1_2d->SetMarkerColor(2);   
  inTree->Project("ribbon_bin2_del_2d","log10(Frac_n1)-log10(old.Frac_n1):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin2_del_2d","log10(Frac_p1)-log10(old.Frac_p1):(id%100)+(4*(id>550))",cut_side_n_rib);
  h2_2d->SetYTitle("#Delta log10(Relative Signal)");  h2_2d->SetXTitle("Ribbon Number");
  h2_2d->SetMarkerStyle(6); h2_2d->SetMarkerColor(3);   
  inTree->Project("ribbon_bin3_del_2d","log10(Frac_p1)-log10(old.Frac_p1):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin3_del_2d","log10(Frac_n1)-log10(old.Frac_n1):(id%100)+(4*(id>550))",cut_side_n_rib);
  h4_2d->SetYTitle("#Delta log10(Relative Signal)");  h4_2d->SetXTitle("Ribbon Number");
  h4_2d->SetMarkerStyle(6); h4_2d->SetMarkerColor(4);   
  inTree->Project("ribbon_bin4_del_2d","log10(Frac_p2)-log10(old.Frac_p2):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin4_del_2d","log10(Frac_n2)-log10(old.Frac_n2):(id%100)+(4*(id>550))",cut_side_n_rib);
  h5_2d->SetYTitle("#Delta log10(Relative Signal)");  h5_2d->SetXTitle("Ribbon Number");
  h5_2d->SetMarkerStyle(6); h5_2d->SetMarkerColor(5);   
  inTree->Project("ribbon_bin5_del_2d","log10(Frac_p3)-log10(old.Frac_p3):(id%100)+(4*(id>550))",cut_side_p_rib);
  inTree->Project("+ribbon_bin5_del_2d","log10(Frac_n3)-log10(old.Frac_n3):(id%100)+(4*(id>550))",cut_side_n_rib);
  h6_2d->SetYTitle("#Delta log10(Relative Signal)");  h6_2d->SetXTitle("Ribbon Number");
  h6_2d->SetMarkerStyle(6); h6_2d->SetMarkerColor(6);   
  TCanvas cnv;
  //std::string plot1DName(outputPrefix+name_sum+"_1d.gif");
  //std::string plot2DName(outputPrefix+name_sum+"_2d.gif");
  std::string plot1DName(outputPrefix+name_sum+"_1d.png");
  std::string plot2DName(outputPrefix+name_sum+"_2d.png");
  //std::string plot1DName(outputPrefix+name_sum+"_1d.pdf");
  //std::string plot2DName(outputPrefix+name_sum+"_2d.pdf");
  h0_1d->Draw(); h1_1d->Draw("same"); h2_1d->Draw("same"); 
  h4_1d->Draw("same"); h5_1d->Draw("same"); h6_1d->Draw("same"); 
  cnv.Update(); cnv.SaveAs(plot1DName.c_str());
  h0_2d->Draw(); h1_2d->Draw("same"); h2_2d->Draw("same"); 
  h4_2d->Draw("same"); h5_2d->Draw("same"); h6_2d->Draw("same"); 
  cnv.Update(); cnv.SaveAs(plot2DName.c_str());
  plotNames.push_back(plot1DName);
  plotNames.push_back(plot2DName);  
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_PedHigh( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  if ( ! makeSummaryPlot_Generic(inTree,h1,name_sum,
				 title_pedHigh,name_mean,units_pha,cut_chan,100,0.,1000.) ) return kFALSE;
  savePlot(cnv,*h1,outputPrefix,plotNames);
  TH2* h2(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2,name_sum,
				       title_pedHigh,name_mean,units_pha,cut_chan,100,0.,1000.) ) return kFALSE;
  savePlot(cnv,*h2,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_PedHigh( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  if ( ! makeDeltaPlot_Generic(inTree,h1,name_sum,
			       title_pedHigh,name_mean,units_pha,cut_chan,100,-30.,30.) ) return kFALSE;
  savePlot(cnv,*h1,outputPrefix,plotNames);
  TH2* h2(0);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2,name_sum,
				     title_pedHigh,
				     name_mean,units_pha,cut_chan,100,-30.,30.) ) return kFALSE;
  savePlot(cnv,*h2,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_Carbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  TH1* h1_rib(0);
  if ( ! makeSummaryPlot_Generic(inTree,h1,name_sum,
				 title_carbon,name_peak,units_pha,cut_chan,100,0.,400.) ) return kFALSE;
  if ( ! makeSummaryPlot_Generic(inTree,h1_rib,name_sum+"_rib",
				 title_carbon,name_peak,units_pha,cut_rib,100,0.,400.) ) return kFALSE;
  savePlots(cnv,*h1,*h1_rib,outputPrefix,plotNames);
  TH2* h2(0);
  TH2* h2_rib(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2,name_sum,
				       title_carbon,name_peak,units_pha,cut_tile,100,0.,400.) ) return kFALSE;
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_rib,name_sum+"_rib",
				       title_carbon,name_peak,units_pha,cut_rib,100,0.,400.) ) return kFALSE;  
  savePlots(cnv,*h2,*h2_rib,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_Carbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1(0);
  TH1* h1_rib(0);
  if ( ! makeDeltaPlot_Relative(inTree,h1,name_sum,
				title_carbon,name_peak,units_pha,cut_chan,100,-1.,1.) ) return kFALSE;
  if ( ! makeDeltaPlot_Relative(inTree,h1_rib,name_sum+"_rib",
				title_carbon,name_peak,units_pha,cut_rib,100,-1.,1.) ) return kFALSE;
  savePlots(cnv,*h1,*h1_rib,outputPrefix,plotNames);
  TH2* h2(0);
  TH2* h2_rib(0);
  if ( ! makeCorrelPlot_Generic(inTree,h2,name_sum,
				title_carbon,name_peak,units_pha,cut_chan,100,0.,400.) ) return kFALSE;
  if ( ! makeCorrelPlot_Generic(inTree,h2_rib,name_sum+"_rib",
				title_carbon,name_peak,units_pha,cut_chan,100,0.,400.) ) return kFALSE;
  savePlots(cnv,*h2,*h2_rib,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_VetoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ) {
  TCanvas cnv;
  TH1* h1_off(0);
  TH1* h1_slp(0);
  if ( ! makeSummaryPlot_Generic(inTree,h1_off,name_sum+"_offset",
				 title_offset,name_offset,units_dac,cut_chan,100,50.,70.) ) return kFALSE;
  savePlot(cnv,*h1_off,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_Generic(inTree,h1_slp,name_sum+"_slope",
				 title_slope,name_slope,units_slope,cut_chan,100,-0.05,0.) ) return kFALSE;
  savePlot(cnv,*h1_slp,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));
  TH2* h2_off(0);
  TH2* h2_slp(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_off,name_sum+"_offset",
				       title_offset,name_offset,units_dac,cut_chan,100,50.,70.) ) return kFALSE;
  savePlot(cnv,*h2_off,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_slp,name_sum+"_slope",
				       title_slope,name_slope,units_slope,cut_chan,100,-0.05,0.) ) return kFALSE;
  savePlot(cnv,*h2_slp,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_VetoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ){
  TCanvas cnv;
  TH1* h1_off(0);
  TH1* h1_slp(0);
  if ( ! makeDeltaPlot_Generic(inTree,h1_off,name_sum+"_offset",
			       title_offset,name_offset,units_dac,cut_chan,100,-10.,10.) ) return kFALSE;
  savePlot(cnv,*h1_off,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_Generic(inTree,h1_slp,name_sum+"_slope",
			       title_slope,name_slope,units_slope,cut_chan,100,-0.02,0.02) ) return kFALSE;
  savePlot(cnv,*h1_slp,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));
  TH2* h2_off(0);
  TH2* h2_slp(0);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_off,name_sum+"_offset",
				     title_offset,name_offset,units_dac,cut_chan,100,-10.,10.) ) return kFALSE;
  savePlot(cnv,*h2_off,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_slp,name_sum+"_slope",
				     title_slope,name_slope,units_slope,cut_chan,100,-0.02,0.02) ) return kFALSE;
  savePlot(cnv,*h2_slp,outputPrefix,plotNames);  
  return kTRUE;
}


Bool_t AcdReport::makeSummaryPlots_CnoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames  ){  
    TCanvas cnv;
  TH1* h1_off(0);
  TH1* h1_slp(0);
  if ( ! makeSummaryPlot_Generic(inTree,h1_off,name_sum+"_offset",
				 title_offset,name_offset,units_dac,cut_chan,100,50.,70.) ) return kFALSE;
  savePlot(cnv,*h1_off,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_Generic(inTree,h1_slp,name_sum+"_slope",
				 title_slope,name_slope,units_slope,cut_chan,100,-1.,0.) ) return kFALSE;
  savePlot(cnv,*h1_slp,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));
  TH2* h2_off(0);
  TH2* h2_slp(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_off,name_sum+"_offset",
				       title_offset,name_offset,units_dac,cut_chan,100,50.,70.) ) return kFALSE;
  savePlot(cnv,*h2_off,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_slp,name_sum+"_slope",
				       title_slope,name_slope,units_slope,cut_chan,100,-1.,0.) ) return kFALSE;
  savePlot(cnv,*h2_slp,outputPrefix,plotNames);
  return kTRUE;
}

Bool_t AcdReport::makeDeltaPlots_CnoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames  ){
  TCanvas cnv;
  TH1* h1_off(0);
  TH1* h1_slp(0);
  if ( ! makeDeltaPlot_Generic(inTree,h1_off,name_sum+"_offset",
			       title_offset,name_offset,units_dac,cut_chan,100,-10.,10.) ) return kFALSE;
  savePlot(cnv,*h1_off,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_Generic(inTree,h1_slp,name_sum+"_slope",
			       title_slope,name_slope,units_slope,cut_chan,100,-0.5,0.5) ) return kFALSE;
  savePlot(cnv,*h1_slp,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));
  TH2* h2_off(0);
  TH2* h2_slp(0);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_off,name_sum+"_offset",
				     title_offset,name_offset,units_dac,cut_chan,100,-10.,10.) ) return kFALSE;
  savePlot(cnv,*h2_off,outputPrefix,plotNames);
  if ( ! makeDeltaPlot_GenericByChan(inTree,h2_slp,name_sum+"_slope",
				     title_slope,name_slope,units_slope,cut_chan,100,-0.5,0.5) ) return kFALSE;
  savePlot(cnv,*h2_slp,outputPrefix,plotNames);  
  return kTRUE;
}

Bool_t AcdReport::makeSummaryPlots_Check( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames  ){  
  TCanvas cnv;
  TH1* h1_ped(0);
  TH1* h1_mip(0);
  if ( ! makeSummaryPlot_Generic(inTree,h1_ped,name_sum+"_ped",
				 title_ped,name_ped,units_mip,cut_chan,100,-0.2,0.2) ) return kFALSE;
  savePlot(cnv,*h1_ped,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_Generic(inTree,h1_mip,name_sum+"_mip",
				 title_mip,name_mip,units_mip,cut_tile,100,0.,2.0) ) return kFALSE;
  savePlot(cnv,*h1_mip,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));
  TH1* h1_veto(0);
  TH1* h1_cno(0);
  TH1* h1_range(0);
  if ( ! makeSummaryPlot_Generic(inTree,h1_veto,name_sum+"_veto",
				 title_veto,name_veto,units_mip,cut_tile,100,0.,2.) ) return kFALSE;
  savePlot(cnv,*h1_veto,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_Generic(inTree,h1_cno,name_sum+"_cno",
				 title_cno,name_cno,units_mip,cut_skirt,100,0.,50.) ) return kFALSE;
  savePlot(cnv,*h1_cno,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_Generic(inTree,h1_range,name_sum+"_range",
				 title_range,name_range,units_mip,cut_chan,100,-5.,5.) ) return kFALSE;
  savePlot(cnv,*h1_range,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));

  TH2* h2_ped(0);
  TH2* h2_mip(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_ped,name_sum+"_ped",
				 title_ped,name_ped,units_mip,cut_chan,100,-0.2,0.2) ) return kFALSE;
  savePlot(cnv,*h2_ped,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_mip,name_sum+"_mip",
				 title_mip,name_mip,units_mip,cut_chan,100,0.,2.) ) return kFALSE;
  savePlot(cnv,*h2_mip,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));
  TH2* h2_veto(0);
  TH2* h2_cno(0);
  TH2* h2_range(0);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_veto,name_sum+"_veto",
				 title_veto,name_veto,units_mip,cut_chan,100,0.,2.) ) return kFALSE;
  savePlot(cnv,*h2_veto,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_cno,name_sum+"_cno",
				 title_cno,name_cno,units_mip,cut_chan,100,0.,50.) ) return kFALSE;
  savePlot(cnv,*h2_cno,outputPrefix,plotNames);
  if ( ! makeSummaryPlot_GenericByChan(inTree,h2_range,name_sum+"_range",
				 title_range,name_range,units_mip,cut_chan,100,-5.,5.) ) return kFALSE;
  savePlot(cnv,*h2_range,outputPrefix,plotNames);
  plotNames.push_back(std::string("br"));

  return kTRUE;
}
