#include <fstream>
#include "AcdJobConfig.h"

#include <stdlib.h>
#include <string>
#include <fstream>

#include "TSystem.h"
#include "xmlBase/IFile.h"
#include "facilities/Util.h"
#include "facilities/Timestamp.h"
#ifdef WIN32
#include "facilities/XGetopt.h"
#endif
#include <time.h>
#include <TROOT.h>
#include <TPluginManager.h>
#include <TApplication.h>

#include "TChain.h"

AcdJobConfig::AcdJobConfig(const char* appName, const char* desc)
  :m_theApp(appName),
   m_description(desc),
   m_config(AcdKey::LAT),
   m_optval_n(0),
   m_optval_s(0),
   m_optval_P(kFALSE),
   m_optval_m(kFALSE),  
   m_optval_1(kFALSE),
   m_optval_2(kFALSE),
   m_optval_G(0),
   m_optval_d(0),
   m_madeChain(kFALSE),
   m_digiChain(0),
   m_svacChain(0),
   m_meritChain(0),
   m_reconChain(0),
   m_ovlChain(0)
{
  
}

AcdJobConfig::~AcdJobConfig() 
{
  delete m_digiChain;
  delete m_svacChain; 
  delete m_reconChain;
  delete m_meritChain;
  delete m_ovlChain;
}

void AcdJobConfig::usage() {

  using std::cout;
  using std::cerr;
  using std::endl;
  
  cout << endl
       << m_theApp << endl
       << m_description << endl
       << endl;
  
  cout << "Usage:" << endl
       << "\t" << m_theApp << " [options] <inputs>" << endl 
       << endl
       << "\t    INPUT file types" << endl
       << "\t       *.root        : root files (including xrootd)" << endl
       << "\t       *.lst, *.txt  : text files with list of root files" << endl
       << endl
       << "\tOPTIONS for all jobs" << endl
       << "\t   -h                : print this message" << endl
       << "\t   -o <string>       : prefix to add to output files" << endl
       << "\t   -x <fileName>     : " << endl
       << "\t   -n <int>          : run over <nEvents>" << endl
       << "\t   -s <int>          : start with event <startEvent>" << endl
       << "\t   -I (LAT | CU06)   : specify instument being calibrated [LAT]" << endl
       << endl
       << "\tOPTIONS for specific jobs (will be ignored by other jobs)"  << endl
       << "\t   -P                : use only periodic triggers" << endl
       << "\t   -m                : use the mip values from the svac file" << endl
       << "\t   -1                : use only tiles with signals in only 1 PMT" << endl
       << "\t   -2                : use only tiles with signals in both PMTs" << endl
       << "\t   -G <int>          : use CAL GCR selection for a given Z" << endl
       << "\t   -d <int>          : use use only events with GemDeltaEventTime > value" << endl
       << "\t   -i <fileName>     : use input histograms" << endl
       << "\t   -p <fileName>     : use pedestals from this file" << endl
       << "\t   -H <fileName>     : use high range pedestals from this file" << endl
       << "\t   -g <fileName>     : use gains from this file" << endl
       << "\t   -R <fileName>     : use range data from this file" << endl
       << "\t   -C <fileName>     : use carbon peak data from this file" << endl
       << "\t   -z <fileName>     : use high range calib from this file" << endl
       << "\t   -e <fileName>     : use event list for analysis" << endl
       << endl;
}
  
Int_t AcdJobConfig::parse(int argn, char** argc) {

  using std::cout;
  using std::cerr;
  using std::endl;
  using std::string;
    
  // parse options
  char* endPtr;  
  int opt;
  char* optString = "hn:s:I:i:x:o:p:H:g:R:C:Pm12G:d:z:e:";

  m_outputPrefix = "test";

#ifdef WIN32
  while ( (opt = facilities::getopt(argn, argc, optString)) != EOF ) {
#else
  while ( (opt = getopt(argn, argc, optString)) != EOF ) {
#endif
    switch (opt) {
    case 'h':   // help      
      usage();
      return HelpMsg;
    case 'n':   // number of events
      m_optval_n = strtoul( optarg, &endPtr, 0 );
      break;
    case 's':   // start event
      m_optval_s = strtoul( optarg, &endPtr, 0 );
      break;
    case 'I':   // Instrument
      m_instrument = string(optarg);
      break;
    case 'x':
      m_refFileName = string(optarg);
      break;
    case 'o':   //  output
      m_outputPrefix = string(optarg);
      break;
    case 'i':   // Input histograms
      m_inFileName = string(optarg);
      break;
    case 'p':   // Pedestals
      m_pedFileName = string(optarg);
      break;
    case 'H':   // High range pedestals
      m_pedHighFileName = string(optarg);
      break;      
    case 'g':   // gains
      m_gainFileName = string(optarg);
      break;
    case 'R':   // Ranges
      m_rangeFileName = string(optarg);
      break;
    case 'C':   // carbon peak pedestals
      m_carbonFileName = string(optarg);
      break;      
    case 'z':   // high range calibration
      m_highRangeFileName = string(optarg);
      break;      
    case 'm':   // periodic only
      m_optval_m = kTRUE;
      break;
    case 'P':   // periodic only
      m_optval_P = kTRUE;
      break;
    case '1':   // require that only one PMT have a signal in tiles
      m_optval_1 = kTRUE;
      break;
    case '2':   // require signals in both PMTs in tiles
      m_optval_2 = kTRUE;
      break;
    case 'G':   // use CAL GCR selection
      m_optval_G = strtoul( optarg, &endPtr, 0 );
      break;
    case 'd':   // use GemDeltaEventTime cut
      m_optval_d = strtoul( optarg, &endPtr, 0 );
      break;
    case 'e': // use event list
      m_eventList = string(optarg);
      break;
    case '?':
      usage();
      return IllegalOption;
    default:
      cerr << opt << " not parsable..." << endl;
      cerr << "Try " << m_theApp << " -h" << endl;
      return IllegalOption;
    }

    // This is just to keep emacs from getting confused 
#ifdef WIN32
  }
#else
  }
#endif
  
  
  for ( int iarg(optind); iarg < argn; iarg++ ) {
    m_args.push_back( std::string(argc[iarg]) );
  }
  
  // timestamp  
  facilities::Timestamp ts;
  m_timeStamp = ts.getString();
  m_timeStamp += " UTC";
  
  if ( m_instrument == std::string("LAT") || 
       m_instrument == std::string("") ) {
    m_config = AcdKey::LAT;
    m_instrument = "LAT";
  } else if ( m_instrument == std::string("CU06") ) {
    m_config = AcdKey::BEAM;
  } else {
    std::cerr << "Invalid instrument: " << m_instrument << std::endl
	      << "Valid choice are LAT and CU06" << std::endl;
    return IllegalOption;
  }

  std::cout << "output file prefix: " << m_outputPrefix << std::endl;
  std::cout << "instrument: " << m_instrument << std::endl;
  std::cout << "timestamp: " << m_timeStamp << std::endl;
  if ( m_refFileName != "" ) {   
    std::cout << "Reference file: " << m_refFileName << std::endl;
  }
  if ( m_optval_P ) {
    std::cout << "Using only periodic triggers" << std::endl;
  }
  if ( m_optval_m ) {
    std::cout << "Taking mip values from svac file" << std::endl;
  }
  if ( m_optval_1 ) {
    std::cout << "Taking only signals with only one PMT in tiles" << std::endl;
  }
  if ( m_optval_2 ) {
    std::cout << "Requiring signals in both PMTs for tiles" << std::endl;
  }
  if ( m_optval_G != 0 ) {
    std::cout << "Using CAL GCR selection for Z = " << m_optval_G << std::endl;
  }
  if ( m_optval_d != 0 ) {
    std::cout << "Using GemDeltaEventTime cut d = " << m_optval_d << std::endl;
  }
  if ( m_inFileName != "" ) {
    std::cout << "Input file: " << m_inFileName << std::endl;
  }
  if ( m_pedFileName != "" ) {
    std::cout << "pedestal file: " << m_pedFileName << std::endl;
  }
  if ( m_gainFileName != "" ) {   
    std::cout << "gain file: " << m_gainFileName << std::endl;
  }
  if ( m_rangeFileName != "" ) {   
    std::cout << "range file: " << m_rangeFileName << std::endl;
  }
  if ( m_pedHighFileName != "" ) {   
    std::cout << "highRangePed file: " << m_pedHighFileName << std::endl;
  }
  if ( m_carbonFileName != "" ) {   
    std::cout << "carbon file: " << m_carbonFileName << std::endl;
  }
  if ( m_highRangeFileName != "" ) {   
    std::cout << "highRange file: " << m_highRangeFileName << std::endl;
  }
  if ( m_eventList != "" ) {   
    std::cout << "event list file: " << m_eventList << std::endl;
  }

  static TApplication* theApp = new TApplication("runMipCalib.exe",&argn,argc);
  // init XROOTD for kicks
  gROOT->GetPluginManager()->AddHandler("TSystem", "^root", "TXNetSystem","Netx");
  

  return Success;
}


Bool_t AcdJobConfig::makeChain( ) const {
  
  if ( m_madeChain ) return kTRUE;

  std::vector <std::string> tokens;
  for ( std::list<std::string>::const_iterator itr = m_args.begin();
	itr != m_args.end(); itr++ ) {
    const std::string fileString = *itr;
    if ( fileString.find(".txt") != fileString.npos ||
	 fileString.find(".lst") != fileString.npos ) {
      if ( ! getFileList(fileString.c_str(),tokens) ) return kFALSE; 
    } else {
      tokens.push_back(fileString);
    }
  }
  
  unsigned int nFiles = tokens.size();  
  TChain* chain(0);
  for ( unsigned int iFile(0); iFile < nFiles; iFile++ ) {
    const std::string& token = tokens[iFile];
    if (token=="") continue;
    if (token.find("digi.root") != token.npos ) {
      if ( m_digiChain == 0 ) {
	m_digiChain = new TChain("Digi");
      }
      chain = m_digiChain;
    } else if (token.find("svac.root") != token.npos ) {
      if ( m_svacChain == 0 ) {
	m_svacChain = new TChain("Output");
      }
      chain = m_svacChain;
    } else if (token.find("merit.root") != token.npos ) {
      if ( m_meritChain == 0 ) {
	m_meritChain = new TChain("MeritTuple");
      }
      chain = m_meritChain;
    } else if (token.find("recon.root") != token.npos ) {
      if ( m_reconChain == 0 ) {
	m_reconChain = new TChain("Recon");
      }
      chain = m_reconChain;
    } else if (token.find("Overlay") != token.npos ) {
      if ( m_ovlChain == 0 ) {
	m_ovlChain = new TChain("Overlay");
      }
      chain = m_ovlChain;      
    } else {
      std::cerr << "File " << token << " not a Recognized file" << std::endl;
      return kFALSE;
    }
    chain->Add(token.c_str());
    std::cout << "   " << iFile+1 << ") " << token << std::endl;
  }

  m_madeChain = kTRUE;
  return kTRUE;  
}
 

Bool_t AcdJobConfig::checkDigi() const {
  if ( ! makeChain() ) return kFALSE;
  if ( m_digiChain == 0 ) {
    std::cerr << "This job requires digi ROOT files as input." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}

Bool_t AcdJobConfig::checkSvac() const {
  if ( ! makeChain() ) return kFALSE;
  if ( m_svacChain == 0 ) {
    std::cerr << "This job requires svac ROOT files as input." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}
 
Bool_t AcdJobConfig::checkMerit() const {
  if ( ! makeChain() ) return kFALSE;
  if ( m_meritChain == 0 ) {
    std::cerr << "This job requires merit ROOT files as input." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}

Bool_t AcdJobConfig::checkRecon() const {
  if ( ! makeChain() ) return kFALSE;
  if ( m_reconChain == 0 ) {
    std::cerr << "This job requires recon ROOT files as input." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}

Bool_t AcdJobConfig::checkOverlay() const {
  if ( ! makeChain() ) return kFALSE;
  if ( m_ovlChain == 0 ) {
    std::cerr << "This job requires overlay ROOT files as input." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}


Bool_t AcdJobConfig::getFileList(const char* fileName, std::vector<std::string>& files) {
   std::ifstream infile(fileName);   
   if ( ! infile.good() ) return kFALSE;
   char formatLine[512];
   while ( ! infile.eof() ) {
     infile.getline(formatLine,512);
     if ( !infile.eof() ) {
       if ( !infile.good() ) return kFALSE;     
     }
     std::string aLine(&formatLine[0]);
     if ( aLine.size() < 2 ) continue;
     if ( aLine[0] == '#' ) continue;
     files.push_back(aLine);
   }
   return kTRUE;
 }
