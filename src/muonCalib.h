#ifndef muonCalib_h
#define muonCalib_h 

#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TMath.h"
#include "TCollection.h"  // Declares TIter
#include "digiRootData/DigiEvent.h"
#include "reconRootData/ReconEvent.h"
#include "mcRootData/McEvent.h"
#include <iostream>

class muonCalib {

public :

  enum CALTYPE{PEDESTAL, GAIN};
 
  enum {g_nFace=6, g_nRow=4, g_nCol=4, g_nPmt=2, g_nRange=2};
    
// Standard ctor, where user provides the names of the input root files
// and optionally the name of the output ROOT histogram file
 muonCalib(TChain *digiChain, TChain *recChain = 0, TChain *mcChain = 0, 
	   const char *histFileName="Histograms.root");

 ~muonCalib();  

 // reset for next Go to start at beginning of file 
 void rewind() { m_startEvent = 0; }; 

 // Fit pedestal histograms
 void fitPedHist();

 // Fit pedestal subtracted signal distribution with a Landau function
 void fitGainHist();

 // write pedestal peak and RMS values to a text file
 void writePedTxt(const char* fileName);

 // write pedestal peak and RMS values to a xml file
 void writePedXml(const char* fileName);

 // write pedestal subtracted peak and width values to a text file
 void writeGainTxt(const char* fileName);

 // write pedestal subtracted peak and width values to a xml file
 void writeGainXml(const char* fileName);

 /// process events
 void go(int numEvents=100000); 

 void setCalType(CALTYPE t) { m_calType = t; }

 int calHistId(int face, int row, int col, int pmt, int range)
   {
     return face*g_nRow*g_nCol*g_nPmt*g_nRange + row*g_nCol*g_nPmt*g_nRange + 
       col*g_nPmt*g_nRange + pmt*g_nRange + range;
   }

 void setInstrument(const char* p) { m_instrument = p; }
 void setTimeStamp(const char* p) { m_timeStamp = p; }
 void setVersion(const char* p) { m_version = p; }

 private:
 // filling various histogram depending on m_calType
 void fillPhaHist(int face, int row, int col, int pmtId, int range,
		  int pha);

 // get reconstruction direction 
 void getFitDir();

 // select events used to calibrate gain
 bool failCuts();

 // process digi data in ACD
 void digiAcd();

 // return a string consists of n spaces
 std::string nSpaces(int n) const;

 CALTYPE m_calType;

 TObjArray* m_pedHists;
 TObjArray* m_rawHists;
 TObjArray* m_gainHists;

 float m_gainPeak[g_nFace][g_nRow][g_nCol][g_nPmt][g_nRange];
 float m_gainWidth[g_nFace][g_nRow][g_nCol][g_nPmt][g_nRange];
 float m_pedPeak[g_nFace][g_nRow][g_nCol][g_nPmt][g_nRange];
 float m_pedRms[g_nFace][g_nRow][g_nCol][g_nPmt][g_nRange];
     
 /// Optional TChain input
 TChain      *m_digiChain, *m_recChain, *m_mcChain;

 /// pointer to a DigiEvent
 DigiEvent* m_digiEvent;

 /// pointer to a ReconEvent
 ReconEvent* m_reconEvent;

 /// Pointer to a McEvent
 McEvent* m_mcEvent;

 /// name of the output histogram ROOT file
 TFile* m_histFile; 

 /// starting event number
 Int_t m_startEvent;

 // reconstructed event direction along the z axis, defaulted as -9999
 float m_reconDirZ;

 std::string m_instrument;

 std::string m_timeStamp;

 std::string m_version;
    
};

#endif
