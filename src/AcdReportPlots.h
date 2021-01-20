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
 * @brief Functions to make summary plots for calibrations and calibration trending
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdReportPlots.h,v 1.3 2008/08/06 17:46:09 echarles Exp $
 **/

namespace AcdReport {

  /// Save a plot to a gif, add the name of the gif to the list of saved plots
  void savePlot(TCanvas& c, TH1& plot, const char* outputPrefix, std::list<std::string>& plotNames);

  /// Save a two plot to a single gif, add the name of the gif to the list of saved plots
  void savePlots(TCanvas& c, TH1& plot1, TH1& plot2, const char* outputPrefix, std::list<std::string>& plotNames);

  /// Build a chain of calibration results for trending
  void makeTrendChain( const std::string& calType, 
		       const std::string& refFile, const std::list<std::string>& trendFiles,
		       TChain*& trendChain );			

  /// Make summary plots for a calibration, just calls one of the functions below
  Bool_t makeSummaryPlots( AcdCalibData::CALTYPE pType, const char* outputPrefix, TTree* inTree, 
			   std::list<std::string>& plotNames);
  
  /// Make delta w.r.t. reference plots for a calibration, just calls one of the functions below
  Bool_t makeDeltaPlots( AcdCalibData::CALTYPE pType, const char* outputPrefix, TTree* inTree, 
			 std::list<std::string>& plotNames);

  /// Make a generic 1D plot summarizing calibration values
  Bool_t makeSummaryPlot_Generic( TTree* inTree, TH1*& hist,
				  const TString& name, const TString& title,
				  const TString& varName, const TString& units,
				  const TString& cut,
				  UInt_t nBins, Float_t low, Float_t hi );

  /// Make a generic 2D plot summarizing calibration values by channel
  Bool_t makeSummaryPlot_GenericByChan( TTree* inTree, TH2*& hist,
					const TString& name, const TString& title,
					const TString& varName, const TString& units,
					const TString& cut,
					UInt_t nBins, Float_t low, Float_t hi );

  /// Make a generic 1D plot summarizing changes in calibration values w.r.t. reference
  Bool_t makeDeltaPlot_Generic( TTree* inTree, TH1*& hist,
				const TString& name, const TString& title,
				const TString& varName, const TString& units,
				const TString& cut,
				UInt_t nBins, Float_t low, Float_t hi );

  /// Make a generic 2D plot summarizing changes in calibration values w.r.t. reference, by channel
  Bool_t makeDeltaPlot_GenericByChan( TTree* inTree, TH2*& hist,
				      const TString& name, const TString& title,
				      const TString& varName, const TString& units,
				      const TString& cut,
				      UInt_t nBins, Float_t low, Float_t hi );

  /// Make a generic 2D plot summarizing relative changes in calibration values w.r.t. reference, by channel
  Bool_t makeDeltaPlot_Relative( TTree* inTree, TH1*& hist,
				 const TString& name, const TString& title,
				 const TString& varName, const TString& units,
				 const TString& cut,
				 UInt_t nBins, Float_t low, Float_t hi );

  /// Make a generic 2D plot calibration values v. reference values
  Bool_t makeCorrelPlot_Generic( TTree* inTree, TH2*& hist,
				 const TString& name, const TString& title,
				 const TString& varName, const TString& units,
				 const TString& cut,
				 UInt_t nBins, Float_t low, Float_t hi );
  
  /// Make a generic 2D trending plot, delta w.r.t. reference v. test phase
  Bool_t makeTrendPlot_Generic( TTree* inTree, TH2*& hist,
				const TString& name, const TString& title,
				const TString& varName, const TString& units,
				const TString& cut, Bool_t relative, 
				UInt_t nBins, Float_t low, Float_t hi );
 
  /// Make a generic 2D trending plot, delta w.r.t. reference v. time
  Bool_t makeTrendTimePlot_Generic( TTree* inTree, TH2*& hist,
				    const TString& name, 
				    TList& timeLines,
				    Double_t plotStart = 234893100.0,
				    Double_t plotSpan = 10000000 );

  /// Make the summary plots for the pedestal calibration: Mean and width
  Bool_t makeSummaryPlots_Ped( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the delta plots for the pedestal calibration: Mean and width
  Bool_t makeDeltaPlots_Ped( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the summary plots for the MIP calibration: Peak and width
  Bool_t makeSummaryPlots_Gain( TTree* inTree, const char* outputPrefix, 
				std::list<std::string>& plotNames );

  /// Make the delta plots for the MIP calibration: Peak and width
  Bool_t makeDeltaPlots_Gain( TTree* inTree, const char* outputPrefix, 
			      std::list<std::string>& plotNames );
  
  /// Make the summary plots for the veto threshold calibration: veto turn on point
  Bool_t makeSummaryPlots_Veto( TTree* inTree, const char* outputPrefix, 
				std::list<std::string>& plotNames );

  /// Make the delta plots for the veto threshold calibration: veto turn on point
  Bool_t makeDeltaPlots_Veto( TTree* inTree, const char* outputPrefix, 
			      std::list<std::string>& plotNames );

  /// Make the summary plots for the range crossover calibration: highest low range value, lowest high range value
  Bool_t makeSummaryPlots_Range( TTree* inTree, const char* outputPrefix, 
				 std::list<std::string>& plotNames );

  /// Make the delta plots for the range crossover calibration: highest low range value, lowest high range value
  Bool_t makeDeltaPlots_Range( TTree* inTree, const char* outputPrefix, 
			       std::list<std::string>& plotNames );

  /// Make the summary plots for the cno threshold calibration: cno turn on point
  Bool_t makeSummaryPlots_Cno( TTree* inTree, const char* outputPrefix, 
			       std::list<std::string>& plotNames );

  /// Make the delta plots for the cno threshold calibration: cno turn on point
  Bool_t makeDeltaPlots_Cno( TTree* inTree, const char* outputPrefix, 
			     std::list<std::string>& plotNames ); 

  /// Make the summary plots for the high range calibration: pedestal, slope, saturation value
  Bool_t makeSummaryPlots_HighRange( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the delta plots for the high range calibration: pedestal, slope, saturation value
  Bool_t makeDeltaPlots_HighRange( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the summary plots for the coherent noise calibration: amplitude, decay rate, phase
  Bool_t makeSummaryPlots_CohNoise( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the delta plots for the coherent noise calibration: amplitude, decay rate, phase
  Bool_t makeDeltaPlots_CohNoise( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the summary plots for the ribbon attenuation calibration: attenuation by bin
  Bool_t makeSummaryPlots_Ribbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the delta plots for the ribbon attenuation calibration: attenuation by bin
  Bool_t makeDeltaPlots_Ribbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the summary plots for the high range pedestal calibration: Mean and width
  Bool_t makeSummaryPlots_PedHigh( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the summary plots for the high range pedestal calibration: Mean and width
  Bool_t makeDeltaPlots_PedHigh( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the summary plots for the Carbon Peak calibration: Peak and width
  Bool_t makeSummaryPlots_Carbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the summary plots for the Carbon Peak calibration: Peak and width
  Bool_t makeDeltaPlots_Carbon( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the summary plots for the Veto setting calibration, slope and offset of PHA v. dac setting
  Bool_t makeSummaryPlots_VetoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the delta plots for the Veto setting calibration, slope and offset of PHA v. dac setting
  Bool_t makeDeltaPlots_VetoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );
 
  /// Make the summary plots for the CNO setting calibration, slope and offset of PHA v. dac setting
  Bool_t makeSummaryPlots_CnoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

  /// Make the delta plots for the CNO setting calibration, slope and offset of PHA v. dac setting
  Bool_t makeDeltaPlots_CnoFit( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );

    /// Make the summary plots for the checking calibration, pedestals, mips, thresholds, range crossover
  Bool_t makeSummaryPlots_Check( TTree* inTree, const char* outputPrefix, std::list<std::string>& plotNames );


};

#endif

