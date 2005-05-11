#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>
#include "muonCalib.h"
#include "TSystem.h"
#include "xml/IFile.h"
#include "facilities/Util.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ifstream;

int main(int argn, char** argc) {

  string path = ::getenv("CALIBGENACDROOT");

  string jobOptionXmlFile(path + "/src/muonCalibOption.xml");

  if(argn > 1) {
    jobOptionXmlFile = argc[1];
    cout << "Reading in user-specified options file: " << jobOptionXmlFile 
	 << std::endl << endl;
  }

  xml::IFile myFile(jobOptionXmlFile.c_str());

  string inputDigiFileStr;
  if (myFile.contains("parameters","digiFileList")) {
    inputDigiFileStr = myFile.getString("parameters", "digiFileList");
  }

  std::vector <std::string> token;
  facilities::Util::stringTokenize(inputDigiFileStr, ";, ", token);
  unsigned int nFiles = token.size();
  TChain* digiChain = new TChain("Digi");

  unsigned i;
  cout << "Input digi files:" << endl;
  for (i=0; i!=nFiles; ++i) {
    if (token[i]=="") break;
    digiChain->Add(token[i].c_str());
    cout << "   " << i+1 << ") " << token[i] << endl;
  }

  string inputReconFileStr;
  if (myFile.contains("parameters","reconFileList")) {
    inputReconFileStr = myFile.getString("parameters", "reconFileList");
  }

  token.resize(0);
  facilities::Util::stringTokenize(inputReconFileStr, ";, ", token);
  nFiles = token.size();
  TChain* reconChain = new TChain("Recon");

  cout << "Input recon files:" << endl;
  for (i=0; i!=nFiles; ++i) {
    if (token[i]=="") break;
    reconChain->Add(token[i].c_str());
    cout << "   " << i+1 << ") " << token[i] << endl;
  }

  string pedTextFile, pedXmlFile;
  if (myFile.contains("parameters","pedTextFile")) {
    pedTextFile = myFile.getString("parameters", "pedTextFile");
    cout << "pedestal text file: " << pedTextFile << endl;
  }
  if (myFile.contains("parameters","pedXmlFile")) {
    pedXmlFile = myFile.getString("parameters", "pedXmlFile");
    cout << "pedestal xml file: " << pedXmlFile << endl;
  }

  string gainTextFile, gainXmlFile;
  if (myFile.contains("parameters","gainTextFile")) {
    gainTextFile = myFile.getString("parameters", "gainTextFile");
    cout << "gain text file: " << gainTextFile << endl;
  }
  if (myFile.contains("parameters","gainXmlFile")) {
    gainXmlFile = myFile.getString("parameters", "gainXmlFile");
    cout << "gain xml file: " << gainXmlFile << endl;
  }

  string outputHistFile;
  if (myFile.contains("parameters","outputHistFile")) {
    outputHistFile = myFile.getString("parameters", "outputHistFile");
    cout << "output histogram root file: " << outputHistFile << endl;
  }

  string instrument;
  if (myFile.contains("parameters","instrument")) {
    instrument = myFile.getString("parameters", "instrument");
    cout << "instrument: " << instrument << endl;
  }

  string timeStamp;
  if (myFile.contains("parameters","timeStamp")) {
    timeStamp = myFile.getString("parameters", "timeStamp");
    cout << "timeStamp: " << timeStamp << endl;
  }

  string fmtVersion;
  if (myFile.contains("parameters","fmtVersion")) {
    timeStamp = myFile.getString("parameters", "fmtVersion");
    cout << "format version: " << fmtVersion << endl;
  }

  muonCalib r(digiChain, reconChain, 0, outputHistFile.c_str());
  r.setInstrument(instrument.c_str());
  r.setTimeStamp(timeStamp.c_str());
  r.setVersion(fmtVersion.c_str());

  r.setCalType(muonCalib::PEDESTAL);        
  r.go();
  r.fitPedHist();
  r.writePedTxt(pedTextFile.c_str());
  r.writePedXml(pedXmlFile.c_str());

  r.rewind();
  r.setCalType(muonCalib::GAIN);        
  r.go();    
  r.fitGainHist();
  r.writeGainTxt(gainTextFile.c_str());
  r.writeGainXml(gainXmlFile.c_str());

  delete digiChain;
  delete reconChain;

  return 0;
}







