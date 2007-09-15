#ifndef AcdCalibUtil_h
#define AcdCalibUtil_h 

// local includes
#include "AcdCalibEnum.h"

// ROOT includes
#include "TList.h"

// forward declares
class AcdHistCalibMap;
class AcdCalibMap;
class AcdCalibResult;
class AcdPadMap;
class TVirtualPad;
class TH1;


class AcdCalibUtil {

public :


  // Standard ctor
  AcdCalibUtil();
  
  virtual ~AcdCalibUtil();  
   
  // save a list of canvases to files
  static void saveCanvases(TList& cl, const char* filePrefix = "", const char* suffix = ".ps");

  // draw a single mip plot.
  static void drawMipPlot(TVirtualPad& vp, TH1& hist, AcdCalibResult* res, Bool_t onLog);

  // draw a single veto plot
  static void drawVetoPlot(TVirtualPad& vp, TH1& hVeto, TH1& hAll, AcdCalibResult* res);

  // draw a single veto plot
  static void drawCnoPlot(TVirtualPad& vp, TH1& hVeto, TH1& hAll, AcdCalibResult* res);

  // draw veto plots onto canvases
  static AcdPadMap* drawVetos(AcdHistCalibMap& hVeto, AcdHistCalibMap& hRaw,
			      AcdCalibMap& vetos, const char* prefix = "");

  // draw veto plots onto canvases
  static AcdPadMap* drawCnos(AcdHistCalibMap& hVeto, AcdHistCalibMap& hRaw,
			     AcdCalibMap& vetos, const char* prefix = "");

  // draw mip peaks onto canvases
  static AcdPadMap* drawMips(AcdHistCalibMap& h, AcdCalibMap& gains, 
			     Bool_t onLog = kTRUE, const char* prefix = "");

  // draw strip charts onto canvases
  static AcdPadMap* drawStripCharts(AcdHistCalibMap& h, const char* prefix = "");

  // histogram the outliers 
  static void chi2Dist(const TH1& input, TH1*& output, Int_t method = AcdCalib::PLAIN, 
		       Float_t refVal = 0., Float_t scale = 1.);

  static UShort_t gemId(UInt_t id);

  static Float_t efficDivide(TH1& out, const TH1& top, const TH1& bot, Bool_t inEffic, Float_t minBol = 0.5);

  static Float_t width(UInt_t id);

private:

  ClassDef(AcdCalibUtil,0) ;
    
};

#endif
