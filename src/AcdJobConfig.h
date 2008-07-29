#ifndef AcdJobConfig_h
#define AcdJobConfig_h 

// local includes
#include "./AcdKey.h"

// ROOT includes
#include "Rtypes.h"

// stl includes
#include <iostream>
#include <string>
#include <vector>
#include <list>

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
 *        -f <histFile>     : file with histograms
 *        -i <inputFile>    : generic input file
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
 *        -H <pedHighFile>  : use high range pedestals from this file
 *        -g <gainFile>     : use gains from this file
 *        -R <rangeFile>    : use range data from this file
 *
 *
 * @author Eric Charles
 * $Header$
 */

class AcdJobConfig {

public :

  enum ReturnCode { Success = 0,
		    HelpMsg = 1,
		    IllegalOption = 2,
		    MissingInput = 3,
		    ProccessFail = 4,
		    OutputFail = 5 };

public :
  
  // Standard c'tor
  AcdJobConfig(const char* appName, const char* desc);

  virtual ~AcdJobConfig();  

  void usage();
  
  Int_t parse(int argn, char** argc);

  inline const std::string& theApp() const { return m_theApp; }

  inline const std::string& outputPrefix() const { return m_outputPrefix; }
  inline const std::string& instrument() const { return m_instrument; }
  inline const std::string& timeStamp() const { return m_timeStamp; }

  inline const std::string& refFileName() const { return m_refFileName; }

  inline const std::string& pedFileName() const { return m_pedFileName; }
  inline const std::string& gainFileName() const { return m_gainFileName; }  
  inline const std::string& rangeFileName() const { return m_rangeFileName; }
  inline const std::string& pedHighFileName() const { return m_pedHighFileName; } 
  inline const std::string& carbonFileName() const { return m_carbonFileName; } 

  inline const std::list<std::string>& theArgs() const { return m_args; }

  inline AcdKey::Config config() const { return m_config; }
  inline int optval_n() const { return m_optval_n; }
  inline int optval_s() const { return m_optval_s; }
  inline Bool_t optval_P() const { return m_optval_P; }
  inline Bool_t optval_L() const { return m_optval_L; }  
  inline Bool_t optval_G() const { return m_optval_G; }  

  inline TChain* digiChain() const { return m_digiChain;}
  inline TChain* svacChain() const { return m_svacChain;}

  Bool_t checkDigi() const;
  Bool_t checkSvac() const;
   
protected:

  Bool_t makeChain( ) const;

  Bool_t getFileList(const char* fileName, std::vector<std::string>& files) const;
 
private:

  std::string m_theApp;
  std::string m_description;

  std::string m_outputPrefix;
  std::string m_instrument;
  std::string m_timeStamp;

  std::string m_refFileName;

  std::string m_pedFileName;
  std::string m_gainFileName;
  std::string m_rangeFileName;
  std::string m_pedHighFileName;
  std::string m_carbonFileName;
  
  std::list<std::string> m_args;

  AcdKey::Config m_config;
  int m_optval_n;
  int m_optval_s;
  Bool_t m_optval_P;
  Bool_t m_optval_L;
  Bool_t m_optval_G;

  mutable TChain* m_digiChain;
  mutable TChain* m_svacChain;
    
};

#endif
