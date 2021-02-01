
#ifndef AcdVetoConfig_h
#define AcdVetoConfig_h

#include "Rtypes.h"

// stl includes
#include <string>

/** 
 * @class AcdVetoConfig
 *
 * @brief The register values for the LAT ACD veto thresholds
 *
 * @author Eric Charles
 * $Header: /nfs/slac/g/glast/ground/cvs/calibGenACD/src/AcdVetoConfig.h,v 1.2 2008/08/01 00:08:26 echarles Exp $
 */



class AcdVetoConfig {

public:

  ///  convert register value to a float using veto_dac + (veto_vernier/32)
  static Float_t settingAsFloat(UChar_t veto_dac, UChar_t veto_vernier);

  ///  convert register value from a float using veto_dac + (veto_vernier/32)
  static void floatToDacVals(Float_t value, UChar_t& veto_dac, UChar_t& veto_vernier);

public:

  /// Build from a calibration description
  AcdVetoConfig( );
  /// Null c'tor
  virtual ~AcdVetoConfig();

  ///  get the settings for one channel by key
  void getSetting(UInt_t key, UChar_t& veto_dac, UChar_t& veto_vernier) const;
  ///  get the settings for one channel by garc,gafe
  void getSetting(UInt_t garc, UInt_t gafe, UChar_t& veto_dac, UChar_t& veto_vernier) const;

  ///  set the settings for one channel by key
  void setSetting(UInt_t key, UChar_t veto_dac, UChar_t veto_vernier);
  ///  set the settings for one channel by garc,gafe
  void setSetting(UInt_t garc, UInt_t gafe, UChar_t veto_dac, UChar_t veto_vernier);
  
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

  /// The veto_dac register settings by garc,gafe
  UChar_t m_veto_dac[12][18];
  /// The veto_vernier register settings by garc,gafe
  UChar_t m_veto_vernier[12][18];

  /// the name of the file associated with this calibration
  mutable std::string m_fileName;  

};

#endif
