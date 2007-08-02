#This is the reverse tile mapping Class which contains a reverse
#look up table for the FULL ACD
            
class AcdTileMap:
    tileDict = {"000":{'A':(2,6), 'B':(3,11), 'Number':0},\
                "001":{'A':(2,12),'B':(3,5),  'Number':1},\
                "002":{'A':(2,17),'B':(3,0), 'Number':2},\
                "003":{'A':(4,6), 'B':(5,11), 'Number':3},\
                "004":{'A':(4,11), 'B':(5,6), 'Number':4},\
                "010":{'A':(2,7), 'B':(3,10), 'Number':5},\
                "011":{'A':(2,13), 'B':(3,4), 'Number':6},\
                "012":{'A':(4,4), 'B':(5,13), 'Number':7},\
                "013":{'A':(4,5), 'B':(5,12), 'Number':8},\
                "014":{'A':(4,10), 'B':(5,7), 'Number':9},\
                "020":{'A':(2,8), 'B':(3,9),  'Number':10},\
                "021":{'A':(2,14), 'B':(3,3), 'Number':11},\
                "022":{'A':(2,15), 'B':(3,2), 'Number':12},\
                "023":{'A':(8,15), 'B':(9,2), 'Number':13},\
                "024":{'A':(8,9), 'B':(9,8),  'Number':14},\
                "030":{'A':(10,10), 'B':(11,7), 'Number':15},\
                "031":{'A':(10,5), 'B':(11,12), 'Number':16},\
                "032":{'A':(10,4), 'B':(11,13), 'Number':17},\
                "033":{'A':(8,14), 'B':(9,3),  'Number':18},\
                "034":{'A':(8,8), 'B':(9,9),   'Number':19},\
                "040":{'A':(10,11), 'B':(11,6),'Number':20},\
                "041":{'A':(10,6), 'B':(11,11),'Number':21},\
                "042":{'A':(8,17),'B':(9,0), 'Number':22},\
                "043":{'A':(8,13),'B':(9,4), 'Number':23},\
                "044":{'A':(8,7), 'B':(9,10), 'Number':24},\
                "NA02":{'A':(10,13),'B':(11,16), 'Number':25},\
                "NA03":{'A':(10,16),'B':(3,16), 'Number':26},\
                "100":{'A':(2,1), 'B':(1,3), 'Number':27},\
                "101":{'A':(0,6), 'B':(1,7), 'Number':28},\
                "102":{'A':(0,9), 'B':(1,8), 'Number':29},\
                "103":{'A':(0,10),'B':(1,11), 'Number':30},\
                "104":{'A':(0,14),'B':(11,1), 'Number':31},\
                "110":{'A':(2,0), 'B':(1,2), 'Number':32},\
                "111":{'A':(0,5), 'B':(1,6), 'Number':33},\
                "112":{'A':(0,8), 'B':(1,9), 'Number':34},\
                "113":{'A':(0,11),'B':(1,12), 'Number':35},\
                "114":{'A':(0,15),'B':(11,0), 'Number':36},\
                "120":{'A':(0,0), 'B':(1,1), 'Number':37},\
                "121":{'A':(0,4), 'B':(1,5), 'Number':38},\
                "122":{'A':(0,7), 'B':(1,10), 'Number':39},\
                "123":{'A':(0,12),'B':(1,13), 'Number':40},\
                "124":{'A':(0,16),'B':(1,17), 'Number':41},\
                "130":{'A':(0,17),'B':(1,0), 'Number':42},\
                "NA04":{'A':(0,1), 'B':(1,16), 'Number':43},\
                "NA05":{'A':(0,3), 'B':(1,14), 'Number':44},\
                "200":{'A':(2,5), 'B':(3,12), 'Number':45},\
                "201":{'A':(2,11),'B':(3,6), 'Number':46},\
                "202":{'A':(4,3), 'B':(5,14), 'Number':47},\
                "203":{'A':(4,7), 'B':(5,10), 'Number':48},\
                "204":{'A':(4,12),'B':(5,5), 'Number':49},\
                "210":{'A':(2,3), 'B':(3,14), 'Number':50},\
                "211":{'A':(2,10),'B':(3,7), 'Number':51},\
                "212":{'A':(4,2), 'B':(5,15), 'Number':52},\
                "213":{'A':(4,8), 'B':(5,9), 'Number':53},\
                "214":{'A':(4,14),'B':(5,3), 'Number':54},\
                "220":{'A':(2,2), 'B':(3,15), 'Number':55},\
                "221":{'A':(2,9), 'B':(3,8), 'Number':56},\
                "222":{'A':(4,0), 'B':(5,17), 'Number':57},\
                "223":{'A':(4,9), 'B':(5,8), 'Number':58},\
                "224":{'A':(4,15),'B':(5,2), 'Number':59},\
                "230":{'A':(4,17),'B':(3,17), 'Number':60},\
                "NA06":{'A':(2,16),'B':(3,13), 'Number':61},\
                "NA07":{'A':(4,13),'B':(5,16), 'Number':62},\
                "300":{'A':(6,14),'B':(5,1), 'Number':63},\
                "301":{'A':(6,10),'B':(7,11), 'Number':64},\
                "302":{'A':(6,9), 'B':(7,8), 'Number':65},\
                "303":{'A':(6,6), 'B':(7,7), 'Number':66},\
                "304":{'A':(8,1), 'B':(7,3), 'Number':67},\
                "310":{'A':(6,15), 'B':(5,0), 'Number':68},\
                "311":{'A':(6,11), 'B':(7,12), 'Number':69},\
                "312":{'A':(6,8), 'B':(7,9), 'Number':70},\
                "313":{'A':(6,5), 'B':(7,6), 'Number':71},\
                "314":{'A':(8,0), 'B':(7,2), 'Number':72},\
                "320":{'A':(6,16), 'B':(7,17), 'Number':73},\
                "321":{'A':(6,12), 'B':(7,13), 'Number':74},\
                "322":{'A':(6,7), 'B':(7,10), 'Number':75},\
                "323":{'A':(6,4), 'B':(7,5), 'Number':76},\
                "324":{'A':(6,0), 'B':(7,1), 'Number':77},\
                "330":{'A':(6,17), 'B':(7,0), 'Number':78},\
                "NA08":{'A':(6,3), 'B':(7,14), 'Number':79},\
                "NA09":{'A':(6,1), 'B':(7,16), 'Number':80},\
                "400":{'A':(10,12), 'B':(11,5), 'Number':81},\
                "401":{'A':(10,7), 'B':(11,10), 'Number':82},\
                "402":{'A':(10,3), 'B':(11,14), 'Number':83},\
                "403":{'A':(8,12), 'B':(9,5), 'Number':84},\
                "404":{'A':(8,6), 'B':(9,11), 'Number':85},\
                "410":{'A':(10,14), 'B':(11,3), 'Number':86},\
                "411":{'A':(10,8), 'B':(11,9), 'Number':87},\
                "412":{'A':(10,2), 'B':(11,15), 'Number':88},\
                "413":{'A':(8,11), 'B':(9,6), 'Number':89},\
                "414":{'A':(8,5), 'B':(9,12), 'Number':90},\
                "420":{'A':(10,15), 'B':(11,2), 'Number':91},\
                "421":{'A':(10,9), 'B':(11,8), 'Number':92},\
                "422":{'A':(10,0), 'B':(11,17), 'Number':93},\
                "423":{'A':(8,10), 'B':(9,7), 'Number':94},\
                "424":{'A':(8,3), 'B':(9,14), 'Number':95},\
                "430":{'A':(10,17), 'B':(9,17), 'Number':96},\
                "NA00":{'A':(8,2), 'B':(9,15), 'Number':97},\
                "NA01":{'A':(8,16), 'B':(9,16), 'Number':98},\
                "500":{'A':(6,13), 'B':(1,4), 'Number':99},\
                "501":{'A':(6,2), 'B':(1,15), 'Number':100},\
                "502":{'A':(0,2), 'B':(7,15), 'Number':101},\
                "503":{'A':(0,13), 'B':(7,4), 'Number':102},\
                "600":{'A':(2,4), 'B':(11,4), 'Number':103},\
                "601":{'A':(10,1), 'B':(3,1), 'Number':104},\
                "602":{'A':(4,1), 'B':(9,1), 'Number':105},\
                "603":{'A':(8,4), 'B':(5,4), 'Number':106},\
                "NA10":{'A':(4,16), 'B':(9,13), 'Number':107}}
            
    def __init__(self):
        pass
        
    def getTilePair(self, tileName):
        tilePairList = []
        if(AcdTileMap.tileDict.has_key(tileName)):
            tilePair = AcdTileMap.tileDict[tileName]['A']
            tilePairList.append(tilePair)
            tilePair = AcdTileMap.tileDict[tileName]['B']
            tilePairList.append(tilePair)
        else:
            tilePairList = None
            log.critical("Tile Name %s is not in Tile List"%(tileName))
        return tilePairList
    
    def getA(self, tileName):
        tilePair = AcdTileMap.tileDict[tileName]['A']
        return tilePair
    
    def getB(self, tileName):
        tilePair = AcdTileMap.tileDict[tileName]['B']
        return tilePair
    
    def number(self, tileName):
        if AcdTileMap.tileDict.has_key(tileName):
            return AcdTileMap.tileDict[tileName]['Number']
        else:
            print "tile name ",tileName," not in Tile Map"
            return -1
    
    def getTileNameList(self):
        tileNameList = AcdTileMap.tileDict.keys()
        tileNameList.sort()
        return tileNameList
    
    def getPopTileNameList(self):
        tilePopList = AcdTileMap.getTileNameList(self)
        tileList = []
        for tileName in tilePopList:
            if(tileName[0] != 'N'):
                tileList.append(tileName)
        tileList.sort()
        return tileList

    def otherSide(self, garc, gafe):
        for tileName in AcdTileMap.tileDict:
            (garcA, gafeA) = AcdTileMap.tileDict[tileName]['A']
            if garcA == garc and gafeA == gafe:
                pair = AcdTileMap.tileDict[tileName]['B']
                break
            else:
                (garcB, gafeB) = AcdTileMap.tileDict[tileName]['B']
                if garcB == garc and gafeB == gafe:
                    pair = (garcA, gafeA)
                    break
        return pair

    def XZM(self):
        return(["200","201","202","203","204","210","211","212","213","214",\
               "220","221","222","223","224","230"])
    def YZP(self):
        return(["300","301","302","303","304","310","311","312","313","314",\
               "320","321","322","323","324","330"])
    def YZM(self):
        return(["100","101","102","103","104","110","111","112","113","114",\
               "120","121","122","123","124","130"])
    def XZP(self):
        return(["400","401","402","403","404","410","411","412","413","414",\
               "420","421","422","423","424","430"])
    def XY(self):
        return(["000","001","002","003","004","010","011","012","013","014",\
               "020","021","022","023","024","030","031","032","033","034",\
               "040","041","042","043","044"])
    def RBN(self):
        return(["500","501","502","503","600","601","602","603"])
    def NA(self):
        return(["NA00","NA01","NA02","NA03","NA04","NA05","NA06","NA07","NA08","NA09","NA10"])
    
        
               
