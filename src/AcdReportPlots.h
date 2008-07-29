#ifndef AcdCalibPlots_h
#define AcdCalibPlots_h 

#include <Rtypes.h>

#include "CalibData/Acd/AcdCalibEnum.h"
#include <list>
#include <string>

class TCanvas;
class TString;
class TTree;
class TChain;
class TList;
class TH1;
class TH2;

/**
 * @brief Data for ACD calibrations
 *
 * @author Eric Charles
 * $Header$
 **/

namespace AcdReport {

  void savePlot(TCanvas& c, TH1& plot, const char* outputPrefix, std::list<std::string>& plotNames);

  void savePlots(TCanvas& c, TH1& plot1, TH1& plot2, const char* outputPrefix, std::list<std::string>& plotNames);

  void makeTrendChain( const std::string& calType, 
		       const std::string& refFile, const std::list<std::string>& trendFiles,
		       TChain*& trendChain );			

  Bool_t makeSummaryPlots( AcdCalibData::CALTYPE pType, const char* outputPrefix, TTree* inTree, 
			   std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots( AcdCalibData::CALTYPE pType, const char* outputPrefix, TTree* inTree, 
			 std::list<std::string>& plotNames );

  Bool_t makeSummaryPlot_Generic( TTree* inTree, TH1*& hist,
				  const TString& name, const TString& title,
				  const TString& varName, const TString& units,
				  const TString& cut,
				  UInt_t nBins, Float_t low, Float_t hi );

  Bool_t makeSummaryPlot_GenericByChan( TTree* inTree, TH2*& hist,
					const TString& name, const TString& title,
					const TString& varName, const TString& units,
					const TString& cut,
					UInt_t nBins, Float_t low, Float_t hi );

  Bool_t makeDeltaPlot_Generic( TTree* inTree, TH1*& hist,
				const TString& name, const TString& title,
				const TString& varName, const TString& units,
				const TString& cut,
				UInt_t nBins, Float_t low, Float_t hi );

  Bool_t makeDeltaPlot_GenericByChan( TTree* inTree, TH2*& hist,
				      const TString& name, const TString& title,
				      const TString& varName, const TString& units,
				      const TString& cut,
				      UInt_t nBins, Float_t low, Float_t hi );

  Bool_t makeDeltaPlot_Relative( TTree* inTree, TH1*& hist,
				 const TString& name, const TString& title,
				 const TString& varName, const TString& units,
				 const TString& cut,
				 UInt_t nBins, Float_t low, Float_t hi );

  Bool_t makeCorrelPlot_Generic( TTree* inTree, TH2*& hist,
				 const TString& name, const TString& title,
				 const TString& varName, const TString& units,
				 const TString& cut,
				 UInt_t nBins, Float_t low, Float_t hi );
  
  Bool_t makeTrendPlot_Generic( TTree* inTree, TH2*& hist,
				const TString& name, const TString& title,
				const TString& varName, const TString& units,
				const TString& cut, Bool_t relative, 
				UInt_t nBins, Float_t low, Float_t hi );
 
  Bool_t makeTrendTimePlot_Generic( TTree* inTree, TH2*& hist,
				    const TString& name, 
				    TList& timeLines,
				    Double_t plotStart = 234893100.0,
				    Double_t plotSpan = 10000000 );

  Bool_t makeSummaryPlots_Ped( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_Ped( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeSummaryPlots_Gain( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_Gain( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );
  
  Bool_t makeSummaryPlots_Veto( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_Veto( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeSummaryPlots_Range( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_Range( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeSummaryPlots_Cno( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_Cno( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames ); 
 
  Bool_t makeSummaryPlots_HighRange( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_HighRange( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeSummaryPlots_CohNoise( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_CohNoise( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeSummaryPlots_Ribbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_Ribbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeSummaryPlots_PedHigh( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_PedHigh( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeSummaryPlots_Carbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_Carbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeSummaryPlots_VetoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_VetoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );
 
  Bool_t makeSummaryPlots_CnoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  Bool_t makeDeltaPlots_CnoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

};

#endif
