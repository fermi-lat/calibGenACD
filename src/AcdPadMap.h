#ifndef AcdPadMap_h
#define AcdPadMap_h 

// base class headers
#include "TObject.h"

// local includes
#include "./AcdKey.h"

// stl includes
#include <iostream>
#include <map>

// ROOT includes
#include "TString.h"
#include "TList.h"

// forward declares
class TVirtualPad;

/** 
 * @class AcdPadMap
 *
 * @brief A simple tool to assoicate tile ids with TPads 
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdPadMap.h,v 1.6 2008/07/29 02:59:07 echarles Exp $
 */


class AcdPadMap : public TObject {

public :
  
  /// Standard c'tor
  AcdPadMap(AcdKey::Config, const char* prefix);
  
  /// Default c'tor
  AcdPadMap();

  /// Trivial d'tor
  virtual ~AcdPadMap();  

  /// return the canvas list
  TList& canvasList() { return m_canvasList; }

  /// get a particular pad
  TVirtualPad* getPad(UInt_t key) {
    std::map<UInt_t,TVirtualPad*>::iterator itr = m_map.find(key);
    return itr != m_map.end() ? itr->second : 0;      
  }  

protected:

  void fill();

  void fillForLat();

  void fillForGarcGafe();

  void fillForRibbons();

  void fillForBeam();

private:


  /// Type of padMap, see enums above
  AcdKey::Config m_config;

  /// prefix that goes on canvas names
  TString m_prefix;

  /// Owned list of canvas that this map controls
  TList m_canvasList;

  /// Map from histogram to Pad
  std::map<UInt_t,TVirtualPad*> m_map;
    
};

#endif
