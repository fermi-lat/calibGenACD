
#ifndef AcdHldConfig_h
#define AcdHldConfig_h

#include "Rtypes.h"

// stl includes
#include <string>

/** 
 * @class AcdHldConfig
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



class AcdHldConfig {

public:

  static Float_t settingAsFloat(UChar_t hld_dac);
  
  static void floatToDacVals(Float_t value, UChar_t& hld_dac);

public:

  /// Build from a calibration description
  AcdHldConfig( );
  /// Null c'tor
  virtual ~AcdHldConfig();

  /// Get the settings for one channel
  void getSetting(UInt_t key, UChar_t& hld_dac) const;
  void getSetting(UInt_t garc, UInt_t gafe, UChar_t& hld_dac) const;

  ///
  void setSetting(UInt_t key, UChar_t hld_dac);
  void setSetting(UInt_t garc, UInt_t gafe, UChar_t hld_dac);
  
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

  UChar_t m_hld_dac[12][18];

  /// the name of the file associated with this calibration
  mutable std::string m_fileName;  

};

#endif

#ifdef AcdHldConfig_cxx

#endif // #ifdef AcdHldConfig_cxx
