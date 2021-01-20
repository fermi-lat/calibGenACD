#ifndef AcdCalibUtil_h
#define AcdCalibUtil_h 

// local includes
#include "AcdCalibEnum.h"

// ROOT includes
#include "TList.h"
#include <vector>

// forward declares
class AcdHistCalibMap;
class AcdCalibMap;
class AcdPadMap;
class TVirtualPad;
class TH1;
class TH2;

namespace CalibData {
  class AcdCalibObj;
}

/** 
 * @class AcdCalibUtil
 *
 * @brief Various tools to make nice plots of calibration curves
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdCalibUtil.h,v 1.14 2012/09/05 19:27:29 brandt Exp $
 */

class AcdCalibUtil {

public :


  /// Standard ctor
  AcdCalibUtil();
  
  /// Make some pretense of cleaning up
  virtual ~AcdCalibUtil();  
  
  static Bool_t makeFitPlots(AcdCalibMap& calib,
			     const char* filePrefix = "",
			     const char* suffix = ".png");
//			     const char* suffix = ".pdf");
  
  static Bool_t makeTrendPlots(AcdHistCalibMap& hTrends,
			       const std::vector<TH2*>& summaryHists,
			       const char* filePrefix = "",
     			       const char* suffix = ".png");
  //			       const char* suffix = ".pdf");
  
protected:

  /// save a list of canvases to files
  //  static void saveCanvases(TList& cl, const char* filePrefix = "", const char* suffix = ".pdf");
  static void saveCanvases(TList& cl, const char* filePrefix = "", const char* suffix = ".png");

  /// draw a single pedestal plot.
  static void drawPedPlot(TVirtualPad& vp, TH1& hist, CalibData::AcdCalibObj* res);

  /// draw a single range plot.
  static void drawRangePlot(TVirtualPad& vp, TH1& lowHist, TH1& hiHist, CalibData::AcdCalibObj* res);

  /// draw a single mip plot.
  static void drawMipPlot(TVirtualPad& vp, TH1& hist, CalibData::AcdCalibObj* res, Bool_t onLog);

  /// draw a single veto plot
  static void drawVetoPlot(TVirtualPad& vp, TH1& hVeto, TH1& hAll, CalibData::AcdCalibObj* res);

  /// draw a single veto plot
  static void drawCnoPlot(TVirtualPad& vp, TH1& hVeto, TH1& hAll, CalibData::AcdCalibObj* res);

  /// draw a single veto plot
  static void drawRangeCheckPlot(TVirtualPad& vp, TH1& lowHist, TH1& highHist, CalibData::AcdCalibObj* res);
  
  /// draw a single veto fit plot
  static void drawVetoFitPlot(TVirtualPad& vp, TH1& hVeto, CalibData::AcdCalibObj* res,
			      const std::vector<Float_t>& nomSettings);

  /// draw a single high range fit plot
  static void drawHighRangePlot(TVirtualPad& vp, TH1& hrData, TH1& hxData, CalibData::AcdCalibObj* res);

  /// draw a single trending plot
  static void drawTrendingPlot(TVirtualPad& vp, TH1& tData, const TH2& tRef);
 

  /// draw pedestal plots onto canvases
  static AcdPadMap* drawPeds(AcdHistCalibMap& hPeds,
			     AcdCalibMap& peds, const char* prefix = "");

  /// draw pedestal plots onto canvases
  static AcdPadMap* drawRanges(AcdHistCalibMap& hRanges,
			       AcdCalibMap& ranges, const char* prefix = "");

  /// draw veto plots onto canvases
  static AcdPadMap* drawVetos(AcdHistCalibMap& hVeto, 
			      AcdCalibMap& vetos, const char* prefix = "");

  /// draw cno plots onto canvases
  static AcdPadMap* drawCnos(AcdHistCalibMap& hVeto, 
			     AcdCalibMap& cnos, const char* prefix = "");  

  /// draw mip peaks onto canvases
  static AcdPadMap* drawMips(AcdHistCalibMap& h, 
			     AcdCalibMap& gains, 
			     Bool_t onLog = kTRUE, const char* prefix = "");

  /// draw mip peaks for ribbons onto canvases
  static AcdPadMap* drawRibbons(AcdHistCalibMap& h, 
				AcdCalibMap& ribbons, 
				Bool_t onLog = kTRUE, const char* prefix = "");  

  /// draw strip charts onto canvases
  static AcdPadMap* drawStripCharts(AcdHistCalibMap& h, const char* prefix = "");
 
  /// draw range checking plots onto canvases
  static AcdPadMap* drawRangeCheck(AcdHistCalibMap& hRanges, const char* prefix = "");

  /// draw veto fit plots onto canvases
  static AcdPadMap* drawVetoFits(AcdHistCalibMap& hVeto, 
				 AcdCalibMap& fits, const char* prefix = "", bool isVeto = true);

  /// draw high range fit plots onto canvases
  static AcdPadMap* drawHighRangeFits(AcdHistCalibMap& h, 
				      AcdCalibMap& fits, const char* prefix = "");

  /// draw the trend plots
  static AcdPadMap* drawTrends(AcdHistCalibMap& h, const TH2& rh, 
			       UInt_t idx, UInt_t nVar, const char* prefix = "");

public:

  /// switch from decimal tile ID (0-603) to GemID(0-127)
  static UShort_t gemId(UInt_t id);

  /// Divide one histogram into another to get an efficiency
  static Float_t efficDivide(TH1& out, const TH1& top, const TH1& bot, Bool_t inEffic, Float_t minBol = 0.5);

  /// supposed to return the width of a tile, 
  /// does something really weird with tile 34.
  /// thinks the ribbons are 6mm wide.
  static Float_t width(UInt_t id);

private:
    
};

#endif
