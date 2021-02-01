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
 *   application [options] <input>
 *
 *     INPUT file types
 *        *.root            : root files (including xrootd)
 *        *.lst, *.txt      : text files with list of root files
 *     
 *     OPTIONS for all jobs
 *        -h                : print this message
 *        -o <string>       : prefix to add to output files
 *        -x <fileName>     : refrence calibration, may be used to seed fits
 *        -n <int>          : run over <nEvents>
 *        -s <int>          : start with event <startEvent>
 *        -I (LAT | CU06)   : specify instument being calibrated [LAT]
 *
 *     OPTIONS for specific jobs (will be ignored by other jobs)
 *        -P                : use only periodic triggers
 *        -m                : use mips from SVAC file
 *        -1                : use only tiles with hits in only one PMT
 *        -2                : use only tiles with signals in both PMTs
 *        -G <int>          : use CAL GCR selection for a given Z
 *        -d <int>          : use only events with GemDeltaEventTime > value
 *        -p <fileName>     : use pedestals from this file
 *        -R <fileName>     : use range data from this file
 *        -g <fileName>     : use gains from this file
 *        -H <fileName>     : use high range pedestals from this file
 *        -C <fileName>     : use carbon peak calibration from this file
 *
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdJobConfig.h,v 1.14 2014/02/14 03:29:00 echarles Exp $
 */

class AcdJobConfig {

public :

  enum ReturnCode { Success = 0,
		    HelpMsg = 1,
		    IllegalOption = 2,
		    MissingInput = 3,
		    ProccessFail = 4,
		    OutputFail = 5 };

  /// Open a text file and get the list of root files (one per line), return kTRUE for success
  static Bool_t getFileList(const char* fileName, std::vector<std::string>& files);

public :
  
  // Standard c'tor
  AcdJobConfig(const char* appName, const char* desc);

  virtual ~AcdJobConfig();  

  ///  print the usage mesage
  void usage();
  
  ///  parse the arguments, return one of the ReturnCode enum
  Int_t parse(int argn, char** argc);

  /// return the name of the executable
  inline const std::string& theApp() const { return m_theApp; }

  /// return the output prefix (-o)
  inline const std::string& outputPrefix() const { return m_outputPrefix; }
  /// return the instrument being calibrated (-I)
  inline const std::string& instrument() const { return m_instrument; }
  /// return the timestamp 
  inline const std::string& timeStamp() const { return m_timeStamp; }
  
  /// return the name of the reference calibration file (-x)
  inline const std::string& refFileName() const { return m_refFileName; }

  /// return the name of the input histogram file (-i)
  inline const std::string& inFileName() const { return m_inFileName; }
  /// return the name of the pedestal calibration file (-p)
  inline const std::string& pedFileName() const { return m_pedFileName; }
  /// return the name of the gain calibration file (-g)
  inline const std::string& gainFileName() const { return m_gainFileName; }  
  /// return the name of the range calibration file (-R)
  inline const std::string& rangeFileName() const { return m_rangeFileName; }
  /// return the name of the high range pedestal calibration file (-H)
  inline const std::string& pedHighFileName() const { return m_pedHighFileName; } 
  /// return the name of the carbon peak calibration file (-C)
  inline const std::string& carbonFileName() const { return m_carbonFileName; } 
  /// return the name of the high range calibration file (-z)
  inline const std::string& highRangeFileName() const { return m_highRangeFileName; } 

  inline const std::string& eventList() const { return m_eventList; }

  /// return the input arguments after the options have been stripped off
  inline const std::list<std::string>& theArgs() const { return m_args; }

  /// return the config (LAT | CU06)
  inline AcdKey::Config config() const { return m_config; }

  /// return the number of events to use
  inline int optval_n() const { return m_optval_n; }
  /// return the number of events to use
  inline int optval_s() const { return m_optval_s; }
  /// return the flag to use only periodic triggers
  inline Bool_t optval_P() const { return m_optval_P; }
  /// return the flag to use the mip values from the svac file
  inline Bool_t optval_m() const { return m_optval_m; }
  /// return the flag to use only tiles with signals in only one PMT
  inline Bool_t optval_1() const { return m_optval_1; }
  /// return the flag to use only tiles with signals in both PMTs
  inline Bool_t optval_2() const { return m_optval_2; }
  /// return the value for Z to use in the Galatic Cosmic Ray calibration
  inline int optval_G() const { return m_optval_G; }  
  /// return the value to use for the GemDeltaEventTime cut
  inline int optval_d() const { return m_optval_d; }  
  

  /// return the TChain of digi trees
  inline TChain* digiChain() const { return m_digiChain;}
  /// return the TChain of svac trees
  inline TChain* svacChain() const { return m_svacChain;}
  /// return the TChain of merit trees
  inline TChain* meritChain() const { return m_meritChain;}
  /// return the TChain of recon trees
  inline TChain* reconChain() const { return m_reconChain;}
  /// return the TChain of overlay trees
  inline TChain* ovlChain() const { return m_ovlChain;}

  /// return the status of the Digi TChain
  Bool_t checkDigi() const;
  /// return the status of the Svac TChain
  Bool_t checkSvac() const;
  /// return the status of the Merit TChain
  Bool_t checkMerit() const;
  /// return the status of the Recon TChain
  Bool_t checkRecon() const;
  /// return the status of the Overlay TChain
  Bool_t checkOverlay() const;
      
protected:

  /// Take the arguments and add the files to the input TChains, return kTRUE for success
  Bool_t makeChain( ) const;
 
private:

  std::string m_theApp;
  std::string m_description;

  std::string m_outputPrefix;
  std::string m_instrument;
  std::string m_timeStamp;

  std::string m_refFileName;

  std::string m_inFileName;
  std::string m_pedFileName;
  std::string m_gainFileName;
  std::string m_rangeFileName;
  std::string m_pedHighFileName;
  std::string m_carbonFileName;
  std::string m_highRangeFileName;
  
  std::string m_eventList;

  std::list<std::string> m_args;

  AcdKey::Config m_config;
  int m_optval_n;
  int m_optval_s;
  Bool_t m_optval_P;
  Bool_t m_optval_m;
  Bool_t m_optval_1;
  Bool_t m_optval_2;
  int m_optval_G;
  int m_optval_d;

  mutable Bool_t  m_madeChain;
  mutable TChain* m_digiChain;
  mutable TChain* m_svacChain;
  mutable TChain* m_meritChain;
  mutable TChain* m_reconChain;
  mutable TChain* m_ovlChain;
    
};

#endif
