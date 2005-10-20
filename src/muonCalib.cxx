#include <fstream>
#include "TH1F.h"
#include "TF1.h"
#include "muonCalib.h"

#include <cassert>
#include <cmath>

using std::cout;
using std::endl;
using std::string;

muonCalib::muonCalib(TChain *digiChain, TChain *meritChain, 
		     const char *histFileName): 
  m_digiChain(0), m_meritChain(0), m_digiEvent(0),
  m_histFile(0), m_startEvent(0), m_reconDirZ(-9999)
{    
  m_histFile = new TFile(histFileName, "RECREATE");

  int size = g_nFace * g_nRow * g_nCol * g_nPmt * g_nRange;

  for(int iFace = 0; iFace != g_nFace; ++iFace) {
    for(int iRow = 0; iRow != g_nRow; ++iRow) {
      for(int iCol = 0; iCol != g_nCol; ++iCol) {
	for(int iPmt = 0; iPmt != g_nPmt; ++iPmt) {
	  for(int iRange = 0; iRange != g_nRange; ++iRange) {

	    int histId = calHistId(iFace, iRow, iCol, iPmt, iRange);

	    char pedName[]="ped00000";
	    sprintf(pedName, "ped%1d%1d%1d%1d%1d", iFace, iRow, iCol, iPmt,
		    iRange);
	    m_pedHists.push_back(new TH1F(pedName,pedName,200,0,400));
	    char rawName[]="raw00000";
	    sprintf(rawName, "raw%1d%1d%1d%1d%1d", iFace, iRow, iCol, iPmt,
		    iRange);
	    m_rawHists.push_back(new TH1F(rawName,rawName,220,0,2200));
	    char peakName[]="peak00000";
	    sprintf(peakName, "peak%1d%1d%1d%1d%1d", iFace, iRow, iCol, iPmt,
		    iRange);
	    m_gainHists.push_back(new TH1F(peakName,peakName,220,0,2200));

	    m_pedPeak[iFace][iRow][iCol][iPmt][iRange] = -9999;
	    m_pedRms[iFace][iRow][iCol][iPmt][iRange] = -9999;

	    m_gainPeak[iFace][iRow][iCol][iPmt][iRange] = -9999;
	    m_gainWidth[iFace][iRow][iCol][iPmt][iRange] = -9999;

	  }
	}
      }
    }
  }

  if (digiChain != 0) {
    m_digiEvent = 0;
    m_digiChain = digiChain;
    m_digiChain->SetBranchAddress("DigiEvent", &m_digiEvent);
    m_digiChain->SetBranchStatus("*",0);  // disable all branches
    // activate desired brances
    m_digiChain->SetBranchStatus("m_acd*",1);
    m_digiChain->SetBranchStatus("m_eventId", 1); 
    m_digiChain->SetBranchStatus("m_runId", 1);
  }
    
  if (meritChain != 0) {
    m_meritChain = meritChain;
    m_meritChain->SetBranchStatus("*",0);  // disable all branches
    m_meritChain->SetBranchAddress("VtxZDir", &m_reconDirZ);
    // activate desired branches
    m_meritChain->SetBranchStatus("VtxZDir", 1);
  }
    
}


muonCalib::~muonCalib() 
{
  if(m_histFile) {
    m_histFile->Write();
  
    for(unsigned i = 0; i < m_pedHists.size(); i++) {
      delete m_pedHists[i];
      delete m_rawHists[i];
      delete m_gainHists[i];
    }

    m_histFile->Close();

    delete m_histFile;
  }
    
  if (m_digiEvent) delete m_digiEvent;	
}

void muonCalib::fitGainHist()
{
  for(int iFace = 0; iFace != g_nFace; ++iFace) {
    for(int iRow = 0; iRow != g_nRow; ++iRow) {
      for(int iCol = 0; iCol != g_nCol; ++iCol) {
	for(int iPmt = 0; iPmt != g_nPmt; ++iPmt) {
	  for(int iRange = 0; iRange != g_nRange; ++iRange) {
	    int histId = calHistId(iFace, iRow, iCol, iPmt, iRange);
	    TH1F* h = m_gainHists[histId];
	    
	    if(h->GetEntries() == 0) continue;

	    float ave = h->GetMean();
	    float rms = h->GetRMS();
	    h->Fit("landau", "", "", ave-2*rms, ave+3*rms);
	    double* par = (h->GetFunction("landau"))->GetParameters();
	    float mean = *(par+1); float sigma=*(par+2);
	    cout << " mean=" << mean << "  sigma=" << sigma << endl;

	    m_gainPeak[iFace][iRow][iCol][iPmt][iRange] = mean;
	    m_gainWidth[iFace][iRow][iCol][iPmt][iRange] = sigma;

	  }
	}
      }
    }
  }

}

void muonCalib::writeGainTxt(const char* fileName)
{
  std::ofstream gain(fileName); 

  for(int iFace = 0; iFace != g_nFace; ++iFace) {
    for(int iRow = 0; iRow != g_nRow; ++iRow) {
      for(int iCol = 0; iCol != g_nCol; ++iCol) {
	for(int iPmt = 0; iPmt != g_nPmt; ++iPmt) {
	  for(int iRange = 0; iRange != g_nRange; ++iRange) {
	    gain << iFace << " " << iRow << " " << iCol << " " << iPmt <<" " 
		 << iRange << " " 
		 << m_gainPeak[iFace][iRow][iCol][iPmt][iRange] << " "
		 << m_gainWidth[iFace][iRow][iCol][iPmt][iRange]
		 << endl;
	  }
	}
      }
    }
  }
}

void muonCalib::writeGainXml(const char* fileName)
{
  std::ofstream gain(fileName); 

  gain << "<!DOCTYPE calCalib SYSTEM \"$(CALIBUTILROOT)/xml/acdCalib_v0.dtd\" [] >" << endl;
  gain << "<acdCalib>" << endl;
  gain << "<generic instrument=\"" << m_instrument << "\" timestamp=\"" 
       << m_timeStamp << "\" calibType=\"CAL_Gain\" fmtVersion=\""
       << m_version << "\">" << endl;

  for(int iFace = 0; iFace != g_nFace; ++iFace) {

    gain << nSpaces(2) << "<face iFace=\"" << iFace << "\">" << endl; 

    for(int iRow = 0; iRow != g_nRow; ++iRow) {

      gain << nSpaces(4) << "<row iRow=\"" << iRow << "\">" << endl; 

      for(int iCol = 0; iCol != g_nCol; ++iCol) {

	gain << nSpaces(6) << "<col iCol=\"" << iCol << "\">" << endl; 

	for(int iPmt = 0; iPmt != g_nPmt; ++iPmt) {

	  gain << nSpaces(8) << "<pmt iPmt=\"" << iPmt << "\">" << endl; 

	  for(int iRange = 0; iRange != g_nRange; ++iRange) {

	    gain << nSpaces(10) << "<range iRange=\"" << iRange << "\">" << endl; 

	    gain << nSpaces(12) << "<acdGain peak=\""
		 << m_gainPeak[iFace][iRow][iCol][iPmt][iRange] 
		 << "\" width=\" "
		 << m_gainWidth[iFace][iRow][iCol][iPmt][iRange]
		 << "\"/>" << endl;

	    gain << nSpaces(10) << "</range>" << endl;
	  }
	  gain << nSpaces(8) << "</pmt>" << endl;
	}
	gain << nSpaces(6) << "</col>" << endl;
      }
      gain << nSpaces(4) << "</row>" << endl;
    }
    gain << nSpaces(2) << "</face>" << endl;
  }

  gain << "</acdCalib>" << endl;

}

void muonCalib::fitPedHist()
{
  for(int iFace = 0; iFace != g_nFace; ++iFace) {
    for(int iRow = 0; iRow != g_nRow; ++iRow) {
      for(int iCol = 0; iCol != g_nCol; ++iCol) {
	for(int iPmt = 0; iPmt != g_nPmt; ++iPmt) {
	  for(int iRange = 0; iRange != g_nRange; ++iRange) {
	    int histId = calHistId(iFace, iRow, iCol, iPmt, iRange);
	    TH1F* h = m_pedHists[histId];

	    if(h->GetEntries() == 0) continue;
	    
	    float av = h->GetMean(); 
	    float rms = h->GetRMS();
	    h->SetAxisRange(av-5*rms,av+5*rms);
	    av = h->GetMean(); rms = h->GetRMS();
	    h->SetAxisRange(av-5*rms,av+5*rms);
	    av = h->GetMean(); rms = h->GetRMS();
	    h->SetAxisRange(av-150,av+150);
		
	    m_pedPeak[iFace][iRow][iCol][iPmt][iRange] = av;
	    m_pedRms[iFace][iRow][iCol][iPmt][iRange] = rms;
	  }
	}
      }
    }
  }
}

void muonCalib::writePedTxt(const char* fileName)
{
  std::ofstream ped(fileName);

  for(int iFace = 0; iFace != g_nFace; ++iFace) {
    for(int iRow = 0; iRow != g_nRow; ++iRow) {
      for(int iCol = 0; iCol != g_nCol; ++iCol) {
	for(int iPmt = 0; iPmt != g_nPmt; ++iPmt) {
	  for(int iRange = 0; iRange != g_nRange; ++iRange) {

	      ped << iFace << " " << iRow << " " << iCol << " " << iPmt
		  << " " << iRange << " " 
		  << m_pedPeak[iFace][iRow][iCol][iPmt][iRange]
		  << " " << m_pedRms[iFace][iRow][iCol][iPmt][iRange]
		  << endl;

	  }
	}
      }
    }
  }
}

void muonCalib::writePedXml(const char* fileName)
{
  std::ofstream ped(fileName);

  ped << "<!DOCTYPE calCalib SYSTEM \"acdCalib_v0.dtd\" [] >" << endl;
  ped << "<acdCalib>" << endl;
  ped << "<generic instrument=\"" << m_instrument << "\" timestamp=\"" 
       << m_timeStamp << "\" calibType=\"ACD_Ped\" fmtVersion=\""
       << m_version << "\">" << endl;

  for(int iFace = 0; iFace != g_nFace; ++iFace) {

    ped << nSpaces(2) << "<face iFace=\"" << iFace << "\">" << endl; 

    for(int iRow = 0; iRow != g_nRow; ++iRow) {

      ped << nSpaces(4) << "<row iRow=\"" << iRow << "\">" << endl; 

      for(int iCol = 0; iCol != g_nCol; ++iCol) {

	ped << nSpaces(6) << "<col iCol=\"" << iCol << "\">" << endl; 

	for(int iPmt = 0; iPmt != g_nPmt; ++iPmt) {

	  ped << nSpaces(8) << "<pmt iPmt=\"" << iPmt << "\">" << endl; 

	  for(int iRange = 0; iRange != g_nRange; ++iRange) {

	    ped << nSpaces(10) << "<range iRange=\"" << iRange << "\">" << endl;
	    ped << nSpaces(12) << "<acdPed peak=\""
		<< m_pedPeak[iFace][iRow][iCol][iPmt][iRange]
		<< "\" width=\" "
		<< m_pedRms[iFace][iRow][iCol][iPmt][iRange]
		<< "\"/>" << endl;
	    ped << nSpaces(10) << "</range>" << endl;

	  }
	  ped << nSpaces(8) << "</pmt>" << endl;
	}
	ped << nSpaces(6) << "</col>" << endl;
      }
      ped << nSpaces(4) << "</row>" << endl;
    }
    ped << nSpaces(2) << "</face>" << endl;
  }
  ped << "</acdCalib>" << endl;
}

void muonCalib::digiAcd() 
{
  const TObjArray* acdDigiCol = m_digiEvent->getAcdDigiCol();
  if (!acdDigiCol) return;

  int nAcdDigi = acdDigiCol->GetLast() + 1;
  for(int i = 0; i != nAcdDigi; ++i) {

    AcdDigi* acdDigi = static_cast<AcdDigi*>(acdDigiCol->At(i));

    assert(acdDigi != 0);

    const AcdId& id = acdDigi->getId();
    int face = id.getFace();
    int row = id.getRow();
    int col = id.getColumn();
    
    int pmt0 = acdDigi->getPulseHeight(AcdDigi::A);
    fillPhaHist(face, row, col, 0, 0,pmt0);

    int pmt1 = acdDigi->getPulseHeight(AcdDigi::B);
    fillPhaHist(face, row, col, 1, 0,pmt1);

  }
}

void muonCalib::fillPhaHist(int face, int row, int col, int pmtId, int range,
			    int pha)
{
  int histId = calHistId(face, row, col, pmtId, range); 
  if (m_calType == PEDESTAL) {
    m_pedHists[histId]->Fill(pha);
    m_rawHists[histId]->Fill(pha);
  }

  if(m_calType == GAIN && pha > m_pedPeak[face][row][col][1][0] + 5.*m_pedRms[face][row][col][1][0] && m_reconDirZ > -9999) { // require to be reconstructed

    if(face == 0) { // tiles at top
      float corrPha = (pha-m_pedPeak[face][row][col][1][0]) * (-m_reconDirZ);
      m_gainHists[histId]->Fill(corrPha);
    }
    else { // tiles at side
      float corrPha = (pha-m_pedPeak[face][row][col][1][0]) * sqrt(1.- m_reconDirZ*m_reconDirZ);
      m_gainHists[histId]->Fill(corrPha);
    }
  }
}

void muonCalib::go(int numEvents)
{        
  // determine how many events to process
  int nEntries = (int) m_digiChain->GetEntries();
  cout << "Number of events in the digi chain: " << nEntries << endl;
  int nMax = TMath::Min(numEvents+m_startEvent,nEntries);
  cout << "Number of events used: " << nMax-m_startEvent << endl;
  if (m_startEvent == nEntries) {
    cout << " all events in file read" << endl;
    return;
  }
  if (nEntries <= 0) return;
    
  // BEGINNING OF EVENT LOOP
  for (Int_t ievent=m_startEvent; ievent!=nMax; ++ievent) {
        
    if(m_digiEvent) m_digiEvent->Clear();
               
    if(m_digiChain) m_digiChain->GetEvent(ievent);
    if(m_meritChain) m_meritChain->GetEvent(ievent);

    if(m_digiEvent) {
      int digiEventId = m_digiEvent->getEventId(); 
      int digiRunNum = m_digiEvent->getRunId();
      if(digiEventId%1000 == 0) {
	cout <<" run " << digiRunNum << " event " << digiEventId 
	     << endl;
      }

      if(m_calType == GAIN) getFitDir();
      
      if(m_calType == GAIN && failCuts() ) continue;
 
      digiAcd();

    }
        
  }  // end analysis code in event loop
    
}

void muonCalib::getFitDir()
{

    if ( fabs(m_reconDirZ) < 0.0001 ) m_reconDirZ = -9999;

}

bool muonCalib::failCuts() 
{
  if(m_reconDirZ == -9999) return true;
  else return false;
}

string muonCalib::nSpaces(int n) const 
{
  string temp;
  for(int i = 0; i != n; ++i) {
    temp += ' ';
  }
  return temp;
}
