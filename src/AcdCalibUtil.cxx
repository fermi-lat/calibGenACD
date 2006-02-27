#define AcdCalibUtil_cxx
#include "AcdCalibUtil.h"

#include "TString.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TPad.h"
#include "TH1.h"
#include "TF1.h"

#include "./AcdHistCalibMap.h"
#include "./AcdGainFit.h"

ClassImp(AcdCalibUtil) ;

AcdCalibUtil::AcdCalibUtil(){;}

AcdCalibUtil::~AcdCalibUtil(){;}

void AcdCalibUtil::saveCanvases(TList& cl, const char* filePrefix, const char* suffix) {
  TString pr(filePrefix);
  UInt_t n = cl.GetSize();
  for ( UInt_t i(0); i < n; i++ ) {
    TCanvas* canvas = (TCanvas*)(cl.At(i));
    TString name = pr + canvas->GetName();
    name += suffix;
    canvas->SaveAs(name);
  }
}

void AcdCalibUtil::drawMips(TList& cl, AcdHistCalibMap& h, AcdGainFitMap& gains, 
			    Bool_t onLog, const char* prefix) {

  // top A & top B
  TString pr(prefix);

  TCanvas* topA = new TCanvas(pr+"top_A","top_A");
  TCanvas* topB = new TCanvas(pr+"top_B","top_B");

  topA->Divide(5,5);
  topB->Divide(5,5);  

  UInt_t iRow(0);
  UInt_t iCol(0);

  Double_t ymin = onLog ? 1. : 0.;
  Double_t expand = onLog ? 0.5 : 0.;
  Double_t xmin, xmax, width;

  UInt_t idx(0);
  for ( iRow = 0; iRow < 5; iRow++ ) {
    for ( iCol = 0; iCol < 5; iCol++ ) {
      idx++;     
      UInt_t idA = 10*iRow + iCol;
      UInt_t idB = 1000 + idA;
      TH1* hA = h.getHist(idA);
      TH1* hB = h.getHist(idB);
      if ( hA == 0 || hB == 0 ) continue;
      
      TF1* fA = hA->GetFunction("pol7");
      TF1* fB = hB->GetFunction("pol7");
      if ( fA == 0 || fB == 0 ) {
	fA = hA->GetFunction("pol5");
	fB = hB->GetFunction("pol5");
	if ( fA == 0 || fB == 0 ) continue;
      }

      fA->GetRange(xmin,xmax);
      width = xmax - xmin;
      hA->SetAxisRange(xmin - expand* width, xmax + expand * width);
      hA->SetMinimum(ymin);     
      fB->GetRange(xmin,xmax);
      width = xmax - xmin;
      hB->SetAxisRange(xmin - expand* width, xmax + expand * width);
      hB->SetMinimum(ymin);
      AcdGainFitResult* rB = (AcdGainFitResult*)gains.get(idB);
      TVirtualPad* vA = topA->cd(idx);
      if ( onLog ) vA->SetLogy();
      hA->Draw();   
      AcdGainFitResult* rA = (AcdGainFitResult*)gains.get(idA);
      if ( rA != 0 ) {
	TLine* lA = new TLine( rA->peak(), hA->GetMinimum(),  rA->peak(), hA->GetMaximum());
	lA->SetLineColor(2);
	lA->Draw();
      } 
      TVirtualPad* vB = topB->cd(idx);	    
      if ( onLog ) vB->SetLogy();
      hB->Draw();      
      if ( rB != 0 ) {
	TLine* lB = new TLine( rB->peak(), hB->GetMinimum(),  rB->peak(), hB->GetMaximum());
	lB->SetLineColor(2);
	lB->Draw();
      }       
    }
  }
  cl.Add(topA);
  cl.Add(topB);


  // sides
  for ( UInt_t iFace(1); iFace < 5; iFace++ ) {
    char tmp[10];
    sprintf(tmp,"Side_%i",iFace);
    TString nameA(pr+tmp); nameA += "_A";
    TString nameB(pr+tmp); nameB += "_B";
    
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
	TH1* hA = h.getHist(idA);
	TH1* hB = h.getHist(idB);
	if ( hA == 0 || hB == 0 ) continue;
	
	TF1* fA = hA->GetFunction("pol7");
	TF1* fB = hB->GetFunction("pol7");
	if ( fA == 0 || fB == 0 ) {
	  fA = hA->GetFunction("pol5");
	  fB = hB->GetFunction("pol5");
	  if ( fA == 0 || fB == 0 ) continue;
	}

	fA->GetRange(xmin,xmax);
	width = xmax - xmin;
	hA->SetAxisRange(xmin - expand* width, xmax + expand * width);
	hA->SetMinimum(ymin);
	fB->GetRange(xmin,xmax);
	width = xmax - xmin;
	hB->SetMinimum(ymin);
	hB->SetAxisRange(xmin - expand* width, xmax + expand * width);
	AcdGainFitResult* rA = (AcdGainFitResult*)gains.get(idA);
	AcdGainFitResult* rB = (AcdGainFitResult*)gains.get(idB);
	TVirtualPad* vA = cA->cd(idx);
	if ( onLog ) vA->SetLogy();
	hA->Draw();
	if ( rA != 0 ) {
	  TLine* lA = new TLine( rA->peak(), hA->GetMinimum(),  rA->peak(), hA->GetMaximum());
	  lA->SetLineColor(2);
	  lA->Draw();
	} 
	TVirtualPad* vB = cB->cd(idx);	    
	if ( onLog ) vB->SetLogy();
	hB->Draw();  
	if ( rB != 0 ) {
	  TLine* lB = new TLine( rB->peak(), hB->GetMinimum(),  rB->peak(), hB->GetMaximum());
	  lB->SetLineColor(2);
	  lB->Draw();
	} 
      }
    }
    //idx = 18;
    idx++;
    UInt_t idA = 100*iFace + 30;
    UInt_t idB = 1000 + idA;
    TH1* hA = h.getHist(idA);
    TH1* hB = h.getHist(idB);

    if ( hA == 0 || hB == 0 ) continue;
    
    TF1* fA = hA->GetFunction("pol7");
    TF1* fB = hB->GetFunction("pol7");
    if ( fA == 0 || fB == 0 ) {
      fA = hA->GetFunction("pol5");
      fB = hB->GetFunction("pol5");
      if ( fA == 0 || fB == 0 ) continue;
    }

    fA->GetRange(xmin,xmax);
    width = xmax - xmin;  
    hA->SetAxisRange(xmin - expand* width, xmax + expand * width);
    hA->SetMinimum(ymin);
    fB->GetRange(xmin,xmax);
    width = xmax - xmin;   
    hB->SetAxisRange(xmin - expand* width, xmax + expand * width);
    hB->SetMinimum(ymin);
    AcdGainFitResult* rA = (AcdGainFitResult*)gains.get(idA);
    AcdGainFitResult* rB = (AcdGainFitResult*)gains.get(idB);
    TVirtualPad* vA = cA->cd(idx);
    if ( onLog ) vA->SetLogy();
    hA->Draw();
    if ( rA != 0 ) {
      TLine* lA = new TLine( rA->peak(), hA->GetMinimum(),  rA->peak(), hA->GetMaximum());
      lA->SetLineColor(2);
      lA->Draw();
    }     
    TVirtualPad* vB = cB->cd(idx);	    
    if ( onLog ) vB->SetLogy();
    hB->Draw();  
    if ( rB != 0 ) {
      TLine* lB = new TLine( rB->peak(), hB->GetMinimum(),  rB->peak(), hB->GetMaximum());
      lB->SetLineColor(2);
      lB->Draw();
    } 
    cl.Add(cA);
    cl.Add(cB);
  }
}


Float_t AcdCalibUtil::efficDivide(TH1& out, const TH1& top, const TH1& bot, Bool_t inEffic) {

  Int_t nt = top.GetNbinsX();
  Int_t nb = bot.GetNbinsX();
  if ( nt != nb ) return -1.;

  Float_t iT(0.);
  Float_t iB(0.);

  for ( UInt_t i(1); i <= nt; i++ ) {
    Float_t vT = top.GetBinContent(i);
    Float_t n = bot.GetBinContent(i);
    iT += vT;
    iB += n;
    if ( vT > n ) { 
      out.SetBinContent(i,0.0);
      out.SetBinError(i,0.0);
      continue;
    }
    if ( n < 0.5 )  {
      out.SetBinContent(i,0.0);
      out.SetBinError(i,0.0);
      continue;
    }
    // sneak in 1/10 an event to distinguish perfect channels from channels w/ no data.
    if ( vT < 0.1 ) {
      vT = 0.1;
    }
    Float_t p = vT / n;
    Float_t q = 1. - p;
    Float_t npq = n*p*q;
    Float_t err = TMath::Sqrt(npq) / n;
    out.SetBinContent(i, inEffic ? q : p);
    out.SetBinError(i,err);
  }

  out.SetMinimum(-0.05);
  out.SetMaximum(1.05);
  
  return fabs(iB) > 1e-9 ? iT / iB : 0.;
}


UShort_t AcdCalibUtil::gemId(UInt_t id) {
  UInt_t face = id / 100;
  UInt_t row = (id % 100 ) / 10;
  UInt_t col = (id % 10 );
  switch ( face ) {
  case 0:  return 64 + 5*row + col;  // top:      64 - 89
  case 1:  return 32 + 5*row + col;  // -x side:  32 - 47
  case 2:  return      5*row + col;  // -y side    0 - 15
  case 3:  return 48 + 5*row + col;  // +x side   48 - 63
  case 4:  return 16 + 5*row + col;  // +y side   16 - 31
  case 5:                                   // x ribbons 96 - 99
    return 96 + col;
  case 6:                                   // y ribbons 100- 103
    return 100 + col;
  }
  return 0xFFFF;
}


Float_t AcdCalibUtil::width(UInt_t id) {
  UInt_t face = id / 100;
  UInt_t row = (id % 100 ) / 10;
  //UInt_t col = (id % 10 );

  // stupid temp hack
  if ( id == 34 ) return 12.;

  switch ( face ) {
  case 0:
    return row == 2 ? 12. : 10.;
  case 1:
  case 2:
  case 3:
  case 4:
    return 10.;
  case 5:
  case 6:
    return 6.;
  }
  return -1.;
}
