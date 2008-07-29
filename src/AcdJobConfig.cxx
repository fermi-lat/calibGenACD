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

#include "TChain.h"

AcdJobConfig::AcdJobConfig(const char* appName, const char* desc)
  :m_theApp(appName),
   m_description(desc),
   m_config(AcdKey::LAT),
   m_optval_n(0),
   m_optval_s(0),
   m_optval_P(kFALSE),
   m_optval_L(kFALSE),
   m_optval_G(kFALSE),
   m_digiChain(0),
   m_svacChain(0)
{

}

AcdJobConfig::~AcdJobConfig() 
{
  delete m_digiChain;
  delete m_svacChain; 
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
       << "\tNOTE:  Different calibrations jobs take diffenent types of input files" << endl
       << endl
       << "\tOPTIONS for all jobs" << endl
       << "\t   -h                : print this message" << endl
       << "\t   -n <nEvents>      : run over <nEvents>" << endl
       << "\t   -s <startEvent>   : start with event <startEvent>" << endl
       << "\t   -I (LAT | CU06)   : specify instument being calibrated [LAT]" << endl
       << "\t   -t <timeString>   : force a timestamp" << endl
       << "\t   -x <fileName>     : reference calibration file for comparision" << endl
       << "\t   -o <output>       : prefix (path or filename) to add to output files" << endl
       << endl
       << "\tOPTIONS for specific jobs (will be ignored by other jobs)"  << endl
       << "\t   -p <fileName>     : use pedestals from this file" << endl
       << "\t   -H <fileName>     : use high range pedestals from this file" << endl
       << "\t   -g <fileName>     : use gains from this file" << endl
       << "\t   -R <fileName>     : use range data from this file" << endl
       << "\t   -C <fileName>     : use carbon peak data from this file" << endl
       << "\t   -P                : use only periodic triggers" << endl
       << "\t   -G                : use CAL GCR selection" << endl
       << "\t   -L                : correct for pathlength in tile" << endl
       << endl;
}
  
Int_t AcdJobConfig::parse(int argn, char** argc) {

  // init XROOTD for kicks
  gROOT->GetPluginManager()->AddHandler("TSystem", "^root", "TXNetSystem","Netx");
  
  using std::cout;
  using std::cerr;
  using std::endl;
  using std::string;
    
  // parse options
  char* endPtr;  
  int opt;
  char* optString = "hn:s:I:t:x:o:p:H:g:R:C:PGL";

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
    case 't':
      m_timeStamp = string(optarg);
      break;
    case 'x':
      m_refFileName = string(optarg);
      break;
    case 'o':   //  output
      m_outputPrefix = string(optarg);
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
    case 'C':   // carobon peak pedestals
      m_carbonFileName = string(optarg);
      break;      
    case 'P':   // periodic only
      m_optval_P = kTRUE;
      break;
    case 'G':   // use CAL GCR selection
      m_optval_G = kTRUE;
      break;
    case 'L':   // pathlength correction
      m_optval_L = kTRUE;
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
  if ( m_timeStamp == "" ) {
    facilities::Timestamp ts;
    m_timeStamp = ts.getString();
    m_timeStamp += " UTC";
  }
  
  if ( m_instrument == std::string("LAT") || 
       m_instrument == std::string("") ) {
    m_config = AcdKey::LAT;
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
  if ( m_optval_G ) {
    std::cout << "Using CAL GCR selection" << std::endl;
  }
  if ( m_optval_L ) {
    std::cout << "Using pathlength correction" << std::endl;
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

  return Success;
}


Bool_t AcdJobConfig::makeChain( ) const {

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
    } else {
      std::cerr << "File " << token << " not a Digi or Svac file" << std::endl;
      return kFALSE;
    }
    chain->Add(token.c_str());
    std::cout << "   " << iFile+1 << ") " << token << std::endl;
  }
  return kTRUE;  
}
 

Bool_t AcdJobConfig::checkDigi() const {
  if ( ! makeChain() ) return kFALSE;
  if ( m_digiChain == 0 ) {
    std::cerr << "This job requires digi ROOT files as input." << std::endl
	      << "\tuse -d <file> option to specify them." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}

Bool_t AcdJobConfig::checkSvac() const {
  if ( ! makeChain() ) return kFALSE;
  if ( m_svacChain == 0 ) {
    std::cerr << "This job requires svac ROOT files as input." << std::endl
	      << "\tuse -S <file> option to specify them." << std::endl
	      << std::endl;
    return kFALSE;
  }
  return kTRUE;
}
 
Bool_t AcdJobConfig::getFileList(const char* fileName, std::vector<std::string>& files) const {
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
