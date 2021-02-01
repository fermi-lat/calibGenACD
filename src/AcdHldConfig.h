
#ifndef AcdHldConfig_h
#define AcdHldConfig_h

#include "Rtypes.h"

// stl includes
#include <string>

/** 
 * @class AcdHldConfig
 *
 * @brief The register values for the LAT ACD HLD thresholds (aka CNO)
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdHldConfig.h,v 1.2 2008/08/01 00:08:25 echarles Exp $
 */



class AcdHldConfig {

public:

  ///  convert register value to a float 
  static Float_t settingAsFloat(UChar_t hld_dac);
  
  ///  convert register value from a float
  static void floatToDacVals(Float_t value, UChar_t& hld_dac);

public:

  /// Build from a calibration description
  AcdHldConfig( );
  /// Null c'tor
  virtual ~AcdHldConfig();

  ///  get the settings for one channel by key
  void getSetting(UInt_t key, UChar_t& hld_dac) const;
  ///  get the settings for one channel by garc,gafe
  void getSetting(UInt_t garc, UInt_t gafe, UChar_t& hld_dac) const;

  ///  set the settings for one channel by key
  void setSetting(UInt_t key, UChar_t hld_dac);
  ///  set the settings for one channel by garc,gafe
  void setSetting(UInt_t garc, UInt_t gafe, UChar_t hld_dac);
  
  ///  read calibration from an xml file, return kTRUE for success
  Bool_t readXmlFile(const char* fileName);

  ///  write calibration to an xml file
  Bool_t writeXmlFile(const char* fileName) const;  

  /// return the name of the file associated with this calibration
  const char* fileName() const {
    return m_fileName.c_str();
  }

protected:


private:    

  /// The hld_dac register settings by garc,gafe
  UChar_t m_hld_dac[12][18];

  /// the name of the file associated with this calibration
  mutable std::string m_fileName;  

};

#endif
