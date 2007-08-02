
__facility__ = "GSFC/ACD"
__abstract__ = "ACD Miscellaneous Functions"
__author__   = "Karen Calvert <kcalvert@hammers.com>"
__date__     = ("$Date$").split(' ')[1]
__version__  = "$Revision$"
__credits__  = "NASA/GSFC - the Hammers Company"

def garcSerialMap(serial):
    __serialDict = {202:'11010',\
                    194:'1102',\
                     68:'0104',\
                    200:'1108',\
                     67:'0103',\
                    199:'1107',\
                    196:'1104',\
                    197:'1105',\
                     66:'0102',\
                    195:'1103',\
                     70:'0106',\
                    201:'1109',\
                     65:'0101',\
                    193:'1101',\
                    192:'1100',\
                    198:'1106'}
    if __serialDict.has_key(serial):
        return __serialDict[serial]
    else:
        print "Serial Number", serial, "Is Not In Serial List"
        return -1

def serial2GarcMap(encodedSerial):
    __serial2garcDict = { \
                    '11010':0,\
                     '1102':1,\
                     '0104':2,\
                     '1108':3,\
                     '0103':4,\
                     '1107':5,\
                     '1104':6,\
                     '1105':7,\
                     '0102':8,\
                     '1103':9,\
                     '0106':10,\
                     '1109':11,\
                     '0101':10,\
                     '1101':11,\
                     '1100':11,\
                     '1106':11}
    if __serial2garcDict.has_key(encodedSerial):
        return __serial2garcDict[encodedSerial]
    else:
        print "Serial Number", encodedSerial, "Is Not In Serial List"
        return -1    
