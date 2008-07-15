#!/usr/bin/env python
#
#                               Copyright 2007
#                                     by
#                        The Board of Trustees of the
#                     Leland Stanford Junior University.
#                            All rights reserved.
#

__facility__  = "calibGenACD"
__abstract__  = "XML utilities for the ACD calibrations system"
__author__    = "E. Charles"
__date__      = "$Date$"
__version__   = "$Revision$, $Author$"
__release__   = "$Name$"

import sys, os

from Ft.Xml.Domlette                 import NonvalidatingReader, implementation, PrettyPrint
from Ft.Xml                          import EMPTY_NAMESPACE, InputSource
from Ft.Lib                          import Uri
from Ft.Xml.Xvif                     import RelaxNgValidator

# qualify tags and attributes with a namespace.
ACD_NAMESPACE = EMPTY_NAMESPACE

DTD_VERISION = 'acdCalib_v3.dtd'


def insertDtd(stream):
    """
    """
    import os
    dtdFileName = os.path.join(os.getenv('CALIBUTILROOT'),'xml',DTD_VERISION)
    dtdFile = open(dtdFileName)

    stream.write("<!DOCTYPE acdCalib [\n")
    aLine = dtdFile.readline()
    while aLine != "":
        stream.write(aLine)
        aLine = dtdFile.readline()
    stream.write("] >\n")

def xmlToStream(doc, output=None):
    """ Write out a doc somewhere

    \param doc  An XML doc
    \param output  Either a string, a subclass of the python 'file' type, or a FileObj

    If output is a string, a file object will be opened for output.
    If output is a file object (such as sys.stdout), the xml will be
    sent to it.  Default is sys.stdout

    """
    rootList = doc.xpath('child::acdCalib')
    if rootList:
        rootNode = rootList[0]
    else:
        raise RuntimeError("no acdCalib node in ouput")
    if isinstance(output, basestring): # basestring to allow for unicode
        fileOut = open(output, 'w')
        insertDtd(fileOut)    
        PrettyPrint(rootNode, fileOut)
        fileOut.close()
    elif isinstance(output, file):
        insertDtd(output)
        PrettyPrint(rootNode, output)
    elif output is None:
        insertDtd(sys.stdout)
        PrettyPrint(rootNode, sys.stdout)
    else:
        raise RuntimeError("Don't recognise output type %s"%output)
        

def openXmlFile(input):
    """ open an xml file for reading

    \param fileObj
    \return (file,doc) the opened file object and the associated xml doc object

    after the user is done with the file object the should remember to clean up by
    calling file.close()
    """
    if isinstance(input, basestring): # basestring to allow for unicode
        fileName = input
    return openXmlFileByName(fileName)

def openXmlFileByName(fileName):    
    """ open an xml file for reading

    \param fileName
    \return (file,doc) the opened file object and the associated xml doc object

    after the user is done with the file object the should remember to clean up by
    calling file.close()
    """
    theFile = open(fileName,'r')
    input = Uri.OsPathToUri(fileName, attemptAbsolute=1)
    try:
        doc = NonvalidatingReader.parseUri(input)
    except:
        theFile.close()
        theFile = None
        doc = None
    return (theFile,doc)    

def makeDoc():
    """ creates an XML doc and returns it
    """
    outDoc = implementation.createDocument(ACD_NAMESPACE, None, None)
    return outDoc

def makeChildNode(parentNode, childName):
    """ makes a dom node and attaches it as a child to another node

    \param parentNode attachment point
    \param childName name of the child node
    """
    childNode = parentNode.rootNode.createElementNS(ACD_NAMESPACE, childName)
    parentNode.appendChild(childNode)
    return childNode

def makeNodeNotChild(parentNode, childName):
    """ makes a dom node

    \param parentNode some other node in the same document.
    \param childName name of the new node    
    """
    newNode = parentNode.rootNode.createElementNS(ACD_NAMESPACE, childName)
    return newNode

def makeTextChildNode(parentNode, value):
    """ makes a dom text node

    \param parentNode attachment point
    \param value text to put in child node 
    """
    childNode = parentNode.rootNode.createTextNode(value)
    parentNode.appendChild(childNode)
    return childNode

def makeChildNodeAndTextNode(parentNode, childName, text):
    """ makes a dom node and attaches it as a child to another node, then adds some text

    \param parentNode attachment point
    \param childName name of the child node
    \param text is the text to add to the child node
    """
    childNode = makeChildNode(parentNode,childName)
    textNode = makeTextChildNode(childNode,text)
    return childNode

def returnTextNodeValueBool(domNode):
    """ Convert a dom text node to a bool

    \return (status,value) status is True if the conversion worked, False otherwise
    """
    found = False
    val = None
    for cNode in domNode.childNodes:
        if cNode.nodeType != cNode.TEXT_NODE:
            continue
        try:
            valSt = cNode.nodeValue
            if valSt == 'False':
                val = False
                found = True
            elif valSt == 'True':
                val = True
                found = True
            break
        except:
            return (False,val)
    return (found,val)

def returnTextNodeValueFloat(domNode):
    """ Convert a dom text node to a float

    \return (status,value) status is True if the conversion worked, False otherwise
    """
    found = False
    val = 0
    for cNode in domNode.childNodes:
        if cNode.nodeType != cNode.TEXT_NODE:
            continue
        try:
            val = float(cNode.nodeValue)
            found = True
            break
        except:
            return (False,0)
    return (found,val)

def returnTextNodeValueInt(domNode,base=0):
    """ Convert a dom text node to an int

    \return (status,value) status is True if the conversion worked, False otherwise
    
    By default this uses int(%s,0), which assumes decimal representation
    unless the string starts with 0x (hex) or 0 (octal)
    """
    found = False
    val = 0
    for cNode in domNode.childNodes:
        if cNode.nodeType != cNode.TEXT_NODE:
            continue
        try:
            val = int(cNode.nodeValue,base)
            found = True
            break
        except:
            return (False,0)
    return (found,val)

def returnTextNodeValueLong(domNode,base=0):
    """ Convert a dom text node to a long

    \return (status,value) status is True if the conversion worked, False otherwise
    """
    found = False
    val = 0
    for cNode in domNode.childNodes:
        if cNode.nodeType != cNode.TEXT_NODE:
            continue
        val = long(cNode.nodeValue,base)
        found = True
        break
    return (found,val)

def returnTextNodeValueStr(domNode):
    """ Convert a dom text node to a str

    \return (status,value) status is True if the conversion worked, False otherwise
    """
    found = False
    val = None
    for cNode in domNode.childNodes:
        if cNode.nodeType != cNode.TEXT_NODE:
            continue
        try:
            val = str(cNode.nodeValue)
            found = True
            break
        except:
            return (0,None)
    return (found,val)

def returnTextNodeValueIntFields(domNode,nField,splitStr=':',type='u'):
    """ Convert a dom text node to a tuple of ints

    \param domNode the dom text node
    \param nField the number of fields to expect
    \param splitStr the field seperator
    \return (status,value) status is True if the conversion worked, False otherwise

    This uses int(%s,0), which assumes decimal representation
    unless the string starts with 0x (hex) or 0 (octal)
    """
    (isOk,theString) = returnTextNodeValueStr(domNode)
    if not isOk:
        return (False,None)
    fields = theString.split(splitStr)
    if nField != len(fields):
        return (False,None)
    index = 0
    import Numeric
    theVals = Numeric.zeros(nField,type)
    for field in fields:
        aVal = int(fields[index],0)
        theVals[index] = aVal
        index += 1
    return (True,theVals)
    
def getAttribute(node, attrName):
    """ Return the value of attribute 'attrName' as a string

    return None if attrName is not an attribute of node
    """
    attrList = node.xpath('attribute::%s'%attrName)
    if attrList:
        return str(attrList[0].value)
    else:
        return None

def getAttributeBool(node, attrName):
    """ Return the value of attribute 'attrName' as a string

    return None if attrName is not an attribute of node
    """
    attr = getAttribue(node,attrName)
    if attr == 'False' or attr == 'True':
        return eval(attr)
    return None
    
def setAttribute(node, attr, value):
    """ Wrap the setAttribute of a node.  Different XML engines use different interfaces
    """
    return node.setAttributeNS(ACD_NAMESPACE, attr, str(value))

def setAttributeBool(node, attr, value):
    """
    """
    if value:
        setAttribute(node,attr,'True')
    else:
        setAttribute(node,attr,'False')

def findChild(node, childName):
    """ Return the child of node with name childName

    return None if no such child exists
    """ 
    childList = node.xpath('child::%s'%childName)
    if childList:
        return childList[0]
    else:
        return None


def addComment(parentNode, comment):
    """
    """
    commentNode = parentNode.rootNode.createComment(comment)
    parentNode.appendChild(commentNode)
    return commentNode
    
