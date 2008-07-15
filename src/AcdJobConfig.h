#ifndef AcdJobConfig_h
#define AcdJobConfig_h 

// local includes
#include "./AcdMap.h"

// ROOT includes
#include "Rtypes.h"

// stl includes
#include <iostream>
#include <string>
#include <vector>

// forward declares
class TChain;

/** 
 * @class AcdJobConfig
 *
 * @brief Tool to manage Job options for all calibGenACD applications
 *
 * Usage:
 *   application  -c <configFile>
 * 
 *   <configFile>      : name of xml file with job configuration
 *
 * 
 *   application [options] [input] -o <output>
 *
 *     INPUT
 *        -r <reconFiles>   : comma seperated list of recon ROOT files
 *        -d <digiFiles>    : comma seperated list of digi ROOT files
 *        -S <svacFiles>    : comma seperated list of svac ROOT files
 *        -m <meritFiles>   : comma seperated list of merit ROOT files
 *     NOTE:  Different calibrations jobs take diffenent types of input files
 *
 *        -o <output>       : prefix (path or filename) to add to output files"
 *     
 *     OPTIONS for all jobs
 *        -h                : print this message
 *        -n <nEvents>      : run over <nEvents>
 *        -s <startEvent>   : start with event <startEvent>
 *        -I (LAT | CU06)   : specify instument being calibrated [LAT]
 *
 *     OPTIONS for specific jobs (will be ignored by other jobs)
 *        -P                : use only periodic triggers
 *        -C                : use CAL GCR selection
 *        -L                : correct for pathlength in tile
 *        -b <bins>         : number of time bins in strip chart [300]
 *        -p <pedFile>      : use pedestals from this file
 *        -g <gainFile>     : use gains from this file
 *        -R <rangeFile>    : use range data from this file
 *
 *
 * @author Eric Charles
 * $Header$
 */

class AcdJobConfig {

public :
  
  // Standard c'tor
  AcdJobConfig(const char* appName, const char* desc);

  virtual ~AcdJobConfig();  

  void usage();
  
  Int_t parse(int argn, char** argc);

  inline const std::string& theApp() const { return m_theApp; }
  inline const std::string& path() const { return m_path; }

  inline const std::string& jobOptionXmlFile() const { return m_jobOptionXmlFile; }

  inline const std::string& inputFileName() const { return m_inputFileName; }
  inline const std::string& inputDigiFileStr() const { return m_inputDigiFileStr; }
  inline const std::string& inputReconFileStr() const { return m_inputReconFileStr; }
  inline const std::string& inputSvacFileStr() const { return m_inputSvacFileStr; }
  inline const std::string& inputMeritFileStr() const { return m_inputMeritFileStr; }
  inline const std::string& inputHistFile() const { return m_inputHistFile; }

  inline const std::string& outputPrefix() const { return m_outputPrefix; }
  inline const std::string& instrument() const { return m_instrument; }
  inline const std::string& timeStamp() const { return m_timeStamp; }

  inline const std::string& pedFileName() const { return m_pedFileName; }
  inline const std::string& gainFileName() const { return m_gainFileName; }  
  inline const std::string& rangeFileName() const { return m_rangeFileName; }
  inline const std::string& pedHighFileName() const { return m_pedHighFileName; }  

  inline AcdMap::Config config() const { return m_config; }
  inline int optval_n() const { return m_optval_n; }
  inline int optval_s() const { return m_optval_s; }
  inline int optval_b() const { return m_optval_b; }
  inline Bool_t optval_P() const { return m_optval_P; }
  inline Bool_t optval_L() const { return m_optval_L; }  
  inline Bool_t optval_C() const { return m_optval_C; }  

  inline TChain* digiChain() const { return m_digiChain;}
  inline TChain* reconChain() const { return m_reconChain;}
  inline TChain* svacChain() const { return m_svacChain;}
  inline TChain* meritChain() const { return m_meritChain;}

  Bool_t checkDigi() const;
  Bool_t checkRecon() const;
  Bool_t checkSvac() const;
  Bool_t checkMerit() const;
   
protected:

  TChain* makeChain(const char* name, const std::string& fileString) const;

  Bool_t getFileList(const char* fileName, std::vector<std::string>& files) const;
 
private:

  std::string m_theApp;
  std::string m_description;
  std::string m_path;

  std::string m_jobOptionXmlFile;

  std::string m_inputFileName;
  std::string m_inputDigiFileStr;
  std::string m_inputReconFileStr;
  std::string m_inputSvacFileStr;
  std::string m_inputMeritFileStr;
  std::string m_inputHistFile;

  std::string m_outputPrefix;
  std::string m_instrument;
  std::string m_timeStamp;

  std::string m_pedFileName;
  std::string m_gainFileName;
  std::string m_rangeFileName;
  std::string m_pedHighFileName;

  AcdMap::Config m_config;
  int m_optval_n;
  int m_optval_s;
  int m_optval_b;
  Bool_t m_optval_P;
  Bool_t m_optval_L;
  Bool_t m_optval_C;

  TChain* m_digiChain;
  TChain* m_reconChain;
  TChain* m_svacChain;
  TChain* m_meritChain;
    
};

#endif
