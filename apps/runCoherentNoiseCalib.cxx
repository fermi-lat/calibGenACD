#include <string>
#include <fstream>

#include "../src/AcdJobConfig.h"

#include "../src/AcdCalibUtil.h"
#include "../src/AcdCoherentNoise.h"
#include "../src/AcdCoherentNoiseFit.h"

#include "../src/AcdPadMap.h"
#include "../src/AcdCalibMap.h"

int main(int argn, char** argc) {

  // configure
  AcdJobConfig jc("runCoherentNoiseCalib.exe","This utility runs the coherent noise analysis");

  Int_t parseValue = jc.parse(argn,argc); 
  switch ( parseValue ) {
  case 0: // ok to proceed
    break;  
  case 1: // called -h option terminate processesing normally
    return 0; 
  default: 
    return parseValue;  // parse failed, return failure code
  }

  Bool_t okToContinue = jc.checkDigi();
  if ( ! okToContinue ) return 1; // no input, fail

  /// build filler & run over events
  AcdCoherentNoise r(jc.digiChain(), 529, 2529, jc.optval_b(), jc.config() );

  if ( jc.pedFileName() != "" ) {
    r.readCalib(AcdCalibData::PEDESTAL,jc.pedFileName().c_str());
  }
  r.go(jc.optval_n(),jc.optval_s());    

  r.fillHistograms();

  // do fits  
  AcdCoherentNoiseFitLibrary fitCoherent(AcdCoherentNoiseFitLibrary::Minuit);
  AcdCalibMap* fitMap = r.fit(fitCoherent,AcdCalibData::COHERENT_NOISE,AcdCalib::H_COHERENT_NOISE);
  
  // output
  std::string outputTxtFile = jc.outputPrefix() + "_CohNoise.txt";
  std::string outputXmlFile = jc.outputPrefix() + "_CohNoise.xml";
  std::string outputHistFile = jc.outputPrefix() + "_CohNoise.root";
  std::string outputRootFile = jc.outputPrefix() + "_CohNoiseFit.root";
  std::string outputPlotFile = jc.outputPrefix() + "_CohNoise_";
  
  r.writeHistograms(AcdCalib::H_COHERENT_NOISE, outputHistFile.c_str());
  fitMap->writeTxtFile(outputTxtFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitCoherent.algorithm(),r);
  fitMap->writeXmlFile(outputXmlFile.c_str(),jc.instrument().c_str(),jc.timeStamp().c_str(),fitCoherent.algorithm(),r);
  fitMap->writeResultsToTree(outputRootFile.c_str());
  AcdPadMap* padMap = AcdCalibUtil::drawStripCharts(*(r.getHistMap(AcdCalib::H_COHERENT_NOISE)),outputPlotFile.c_str());  
  AcdCalibUtil::saveCanvases(padMap->canvasList(),"",".gif");  

  return 0;
}







