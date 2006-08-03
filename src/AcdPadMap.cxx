#include <fstream>
#include "AcdPadMap.h"

#include "TCanvas.h"
#include "TVirtualPad.h"

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ClassImp(AcdPadMap) ;

AcdPadMap::AcdPadMap(AcdMap::Config padConfig, const char* prefix)
  :m_config(padConfig),
   m_prefix(prefix)
{
  fill();
}

AcdPadMap::AcdPadMap()
  :m_config(AcdMap::LAT),
   m_prefix("")
{
  fill();
}

AcdPadMap::~AcdPadMap() 
{
 
}

void AcdPadMap::fill() {
  switch (m_config) {
  case AcdMap::LAT:
    fillForLat();
    break;
  case AcdMap::GARC_GAFE:
    fillForGarcGafe();
    break;
  case AcdMap::BEAM:
    fillForBeam();
    break;
  default:
    break;
  }
}

void AcdPadMap::fillForLat() {


  // top A & top B

  TCanvas* topA = new TCanvas(m_prefix+"top_A","top_A");
  TCanvas* topB = new TCanvas(m_prefix+"top_B","top_B");

  topA->Divide(5,5);
  topB->Divide(5,5);  

  UInt_t iRow(0);
  UInt_t iCol(0);

  UInt_t idx(0);
  for ( iRow = 0; iRow < 5; iRow++ ) {
    for ( iCol = 0; iCol < 5; iCol++ ) {
      idx++;     
      UInt_t idA = 10*iRow + iCol;
      UInt_t idB = 1000 + idA;

      TVirtualPad* vA = topA->cd(idx);
      TVirtualPad* vB = topB->cd(idx);	    

      m_map[idA] = vA;
      m_map[idB] = vB;
    }
  }
  m_canvasList.Add(topA);
  m_canvasList.Add(topB);


  // sides
  for ( UInt_t iFace(1); iFace < 5; iFace++ ) {
    char tmp[10];
    sprintf(tmp,"side_%i",iFace);
    TString nameA(m_prefix+tmp); nameA += "_A";
    TString nameB(m_prefix+tmp); nameB += "_B";
    
    TCanvas* cA = new TCanvas(nameA,nameA);
    TCanvas* cB = new TCanvas(nameB,nameB);
    cA->Divide(5,4);
    cB->Divide(5,4);
    idx = 0;
    for ( iRow = 0; iRow < 3; iRow++ ) {
      for ( iCol = 0; iCol < 5; iCol++ ) {
	idx++;
	UInt_t idA = 100*iFace + 10*iRow + iCol;
	UInt_t idB = 1000 + idA;

	TVirtualPad* vA = cA->cd(idx);
	TVirtualPad* vB = cB->cd(idx);	    
	m_map[idA] = vA;
	m_map[idB] = vB;
      }
    }
    //idx = 18;
    idx++;
    UInt_t idA = 100*iFace + 30;
    UInt_t idB = 1000 + idA;

    TVirtualPad* vA = cA->cd(idx);
    TVirtualPad* vB = cB->cd(idx);	    
    m_map[idA] = vA;
    m_map[idB] = vB;

    m_canvasList.Add(cA);
    m_canvasList.Add(cB);
  }


  // ribbons
  TCanvas* ribA = new TCanvas(m_prefix+"rib_A","rib_A");
  TCanvas* ribB = new TCanvas(m_prefix+"rib_B","rib_B");

  ribA->Divide(4,2);
  ribB->Divide(4,2);  

  idx = 0;
  for ( iRow = 5; iRow < 7; iRow++ ) {
    for ( iCol = 0; iCol < 4; iCol++ ) {
      idx++;     
      UInt_t idA = 100*iRow + iCol;
      UInt_t idB = 1000 + idA;
      
      TVirtualPad* vA = ribA->cd(idx);
      TVirtualPad* vB = ribB->cd(idx);	    
      m_map[idA] = vA;
      m_map[idB] = vB;      
    }
  }
  m_canvasList.Add(ribA);
  m_canvasList.Add(ribB);
}

void AcdPadMap::fillForGarcGafe() {
  return;
}

void AcdPadMap::fillForBeam() {

  TCanvas* c = new TCanvas(m_prefix+"BEAM","BEAM");

  c->Divide(5,2);
  
  //static const UInt_t tileIdList[6] = {130,120,110,100,1100,0};
  static const UInt_t tileKeyList[10] = {43,413,24,23,44,2000,2001,2002,42,2003};
  UInt_t idx(0);
  for ( UInt_t iTile(0); iTile < 10; iTile++ ) {
    idx++;    
    UInt_t id = tileKeyList[iTile];
    TVirtualPad* vP = c->cd(idx);
    m_map[id] = vP;
  }
  m_canvasList.Add(c);
}
