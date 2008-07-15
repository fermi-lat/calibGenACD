#ifndef AcdCalibUtil_h
#define AcdCalibUtil_h 

// local includes
#include "AcdCalibEnum.h"

// ROOT includes
#include "TList.h"

// forward declares
class AcdHistCalibMap;
class AcdCalibMap;
class AcdPadMap;
class TVirtualPad;
class TH1;

namespace CalibData {
  class AcdCalibObj;
}

/** 
 * @class AcdCalibUtil
 *
 * @brief Various tools to make nice plots of calibration curves
 *
 * @author Eric Charles
 * $Header$
 */

class AcdCalibUtil {

public :


  /// Standard ctor
  AcdCalibUtil();
  
  /// Make some pretense of cleaning up
  virtual ~AcdCalibUtil();  
   
  /// save a list of canvases to files
  static void saveCanvases(TList& cl, const char* filePrefix = "", const char* suffix = ".ps");

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

  /// draw pedestal plots onto canvases
  static AcdPadMap* drawPeds(AcdHistCalibMap& hPeds,
			     AcdCalibMap& peds, const char* prefix = "");

  /// draw pedestal plots onto canvases
  static AcdPadMap* drawRanges(AcdHistCalibMap& hRanges,
			       AcdCalibMap& ranges, const char* prefix = "");

  /// draw veto plots onto canvases
  static AcdPadMap* drawVetos(AcdHistCalibMap& hVeto, AcdHistCalibMap& hRaw,
			      AcdCalibMap& vetos, const char* prefix = "");

  /// draw cno plots onto canvases
  static AcdPadMap* drawCnos(AcdHistCalibMap& hVeto, AcdHistCalibMap& hRaw,
			     AcdCalibMap& cnos, const char* prefix = "");  

  /// draw mip peaks onto canvases
  static AcdPadMap* drawMips(AcdHistCalibMap& h, AcdCalibMap& gains, 
			     Bool_t onLog = kTRUE, const char* prefix = "");

  /// draw mip peaks for ribbons onto canvases
  static AcdPadMap* drawRibbons(AcdHistCalibMap& h, AcdCalibMap& ribbons, 
				Bool_t onLog = kTRUE, const char* prefix = "");  

  /// draw strip charts onto canvases
  static AcdPadMap* drawStripCharts(AcdHistCalibMap& h, const char* prefix = "");

  /// histogram the outliers 
  static void chi2Dist(const TH1& input, TH1*& output, Int_t method = AcdCalib::PLAIN, 
		       Float_t refVal = 0., Float_t scale = 1.);

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
