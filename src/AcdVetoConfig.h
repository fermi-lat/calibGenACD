
#ifndef AcdVetoConfig_h
#define AcdVetoConfig_h

#include "Rtypes.h"

// stl includes
#include <string>

/** 
 * @class AcdVetoConfig
 *
 * @brief A set of calibration constants for all channels, 
 *
 * Also provides functions to read and write constants to XML and TXT formats
 *
 * All mapping is done using a decimal key:
 *    1000 * pmt + 100*face + 10*row + col
 *  
 *
 * @author Eric Charles
 * $Header$
 */



class AcdVetoConfig {

public:

  static Float_t settingAsFloat(UChar_t veto_dac, UChar_t veto_vernier);
  
  static void floatToDacVals(Float_t value, UChar_t& veto_dac, UChar_t& veto_vernier);

public:

  /// Build from a calibration description
  AcdVetoConfig( );
  /// Null c'tor
  virtual ~AcdVetoConfig();

  /// Get the settings for one channel
  void getSetting(UInt_t key, UChar_t& veto_dac, UChar_t& veto_vernier) const;
  void getSetting(UInt_t garc, UInt_t gafe, UChar_t& veto_dac, UChar_t& veto_vernier) const;

  ///
  void setSetting(UInt_t key, UChar_t veto_dac, UChar_t veto_vernier);
  void setSetting(UInt_t garc, UInt_t gafe, UChar_t veto_dac, UChar_t veto_vernier);
  
  /// Read calibration from an xml file, return kTRUE for success
  Bool_t readXmlFile(const char* fileName);

  /// Write calibration to an xml file
  Bool_t writeXmlFile(const char* fileName) const;  

  /// Return the name of the file associated with this calibration
  const char* fileName() const {
    return m_fileName.c_str();
  }

protected:


private:    

  UChar_t m_veto_dac[12][18];
  UChar_t m_veto_vernier[12][18];

  /// the name of the file associated with this calibration
  mutable std::string m_fileName;  

};

#endif

#ifdef AcdVetoConfig_cxx

#endif // #ifdef AcdVetoConfig_cxx
