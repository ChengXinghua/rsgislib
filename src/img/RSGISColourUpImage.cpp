/*
 *  RSGISColourUpImage.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 17/11/2008.
 *  Copyright 2008 RSGISLib.
 * 
 *  RSGISLib is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  RSGISLib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with RSGISLib.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "RSGISColourUpImage.h"

namespace rsgis{ namespace img{

	RSGISColourUpImage::RSGISColourUpImage(int numberOutBands, ClassColour **classColour, int numClasses) : RSGISCalcImageValue(numberOutBands)
	{
		this->classColour = classColour;
		this->numClasses = numClasses;
	}
	
	void RSGISColourUpImage::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
	{
		if(!(numClasses > 0))
		{
			throw RSGISImageCalcException("The number of classes needs to be greater than zero.");
		}
		if(classColour[0]->numInputBands > numBands)
		{
			throw RSGISImageCalcException("The number of bands for the classification is more than the input image(s)");
		}
		if(numOutBands != 3)
		{
			throw RSGISImageCalcException("The number of output bands needs to be 3.");
		}
		
		output[0] = 0;
		output[1] = 0;
		output[2] = 0;
		
		for(int i = 0; i < numClasses; i++)
		{
			if(classColour[i]->imgBand > numBands)
			{
				throw RSGISImageCalcException("The image band is not available within image\n");
			}
			
			if(bandValues[classColour[i]->imgBand] > classColour[i]->lower & bandValues[classColour[i]->imgBand] <= classColour[i]->upper)
			{
				output[0] = classColour[i]->red;
				output[1] = classColour[i]->green;
				output[2] = classColour[i]->blue;
				break;
			}
		}
	}
	
	void RSGISColourUpImage::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISColourUpImage::calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISColourUpImage::calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISColourUpImage::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}

	bool RSGISColourUpImage::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	RSGISColourUpImage::~RSGISColourUpImage()
	{
		
	}
    
    
    RSGISColourUpImageBand::RSGISColourUpImageBand(int numberOutBands, ClassColour **classColour, int numClasses) : RSGISCalcImageValue(numberOutBands)
	{
		this->classColour = classColour;
		this->numClasses = numClasses;
	}
	
	void RSGISColourUpImageBand::calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException)
	{
		if(!(numClasses > 0))
		{
			throw RSGISImageCalcException("The number of classes needs to be greater than zero.");
		}
		if(numOutBands != 3)
		{
			throw RSGISImageCalcException("The number of output bands needs to be 3.");
		}
		
		output[0] = 0;
		output[1] = 0;
		output[2] = 0;
		
		for(int i = 0; i < numClasses; i++)
		{
			if(bandValues[0] > classColour[i]->lower & bandValues[0] <= classColour[i]->upper)
			{
				output[0] = classColour[i]->red;
				output[1] = classColour[i]->green;
				output[2] = classColour[i]->blue;
				break;
			}
		}
	}
	
	void RSGISColourUpImageBand::calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISColourUpImageBand::calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISColourUpImageBand::calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	void RSGISColourUpImageBand::calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
    
	bool RSGISColourUpImageBand::calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException)
	{
		throw RSGISImageCalcException("Not Implemented");
	}
	
	RSGISColourUpImageBand::~RSGISColourUpImageBand()
	{
		
	}
    
    
    
	
	RSGISClassColourReader::RSGISClassColourReader()
	{
		
	}
	
	ClassColour** RSGISClassColourReader::readClassColourXML(int *numClasses, string xmlFile) throw(RSGISParseColourException)
	{
		RSGISMathsUtils mathsUtils;
		
		XMLCh tempStr[100];
		DOMImplementation *impl = NULL;
		DOMLSParser* parser = NULL;
		ErrorHandler* errHandler = NULL;
		DOMDocument *doc = NULL;
		DOMElement *rootClassificationElement = NULL;
		DOMNodeList *classesNode = NULL;
		DOMElement *classesElement = NULL;
		DOMNodeList *classNodesList = NULL;
		DOMElement *classElement = NULL;

		
		ClassColour **classData = NULL;
		int numBands = 0;
		
		try 
		{
			XMLPlatformUtils::Initialize();
			
			XMLString::transcode("LS", tempStr, 99);
			impl = DOMImplementationRegistry::getDOMImplementation(tempStr);
			if(impl == NULL)
			{
				throw RSGISParseColourException("DOMImplementation is NULL");
			}
			// Create Parser
			parser = ((DOMImplementationLS*)impl)->createLSParser(DOMImplementationLS::MODE_SYNCHRONOUS, 0);
			errHandler = (ErrorHandler*) new HandlerBase();
			parser->getDomConfig()->setParameter(XMLUni::fgDOMErrorHandler, errHandler);
			
			// Open Document
			doc = parser->parseURI(xmlFile.c_str());	
			
			// Get the Root element
			rootClassificationElement = doc->getDocumentElement();
			//cout << "Root Element: " << XMLString::transcode(rootClassificationElement->getTagName()) << endl;
			if(!XMLString::equals(rootClassificationElement->getTagName(), XMLString::transcode("slices")))
			{
				throw RSGISParseColourException("Incorrect root element; Root element should be \"slices\"");
			}
			//cout << ""
			numBands = mathsUtils.strtoint(XMLString::transcode(rootClassificationElement->getAttribute(XMLString::transcode("inputbands"))));
			
			classesNode = rootClassificationElement->getElementsByTagName(XMLString::transcode("classes"));
			if(classesNode->getLength() != 1)
			{
				throw RSGISParseColourException("There should be only 1 classes node");
			}
			classesElement = static_cast<DOMElement*>(classesNode->item(0));
			
			classNodesList = classesElement->getElementsByTagName(XMLString::transcode("class"));
			*numClasses = classNodesList->getLength();
			//cout << "There are " << *numClasses << " class nodes." << endl;
			
			classData = new ClassColour*[*numClasses];
			for(int i = 0; i < *numClasses; i++)
			{
				classData[i] = new ClassColour();
				classElement = static_cast<DOMElement*>(classNodesList->item(i));
				classData[i]->className = string(XMLString::transcode(classElement->getAttribute(XMLString::transcode("name"))));
				classData[i]->classID = mathsUtils.strtoint(XMLString::transcode(classElement->getAttribute(XMLString::transcode("id"))));
				classData[i]->imgBand = mathsUtils.strtoint(XMLString::transcode(classElement->getAttribute(XMLString::transcode("band"))));
				classData[i]->lower = mathsUtils.strtofloat(XMLString::transcode(classElement->getAttribute(XMLString::transcode("lower"))));
				classData[i]->upper = mathsUtils.strtofloat(XMLString::transcode(classElement->getAttribute(XMLString::transcode("upper"))));
				classData[i]->red = mathsUtils.strtoint(XMLString::transcode(classElement->getAttribute(XMLString::transcode("red"))));
				classData[i]->green = mathsUtils.strtoint(XMLString::transcode(classElement->getAttribute(XMLString::transcode("green"))));
				classData[i]->blue = mathsUtils.strtoint(XMLString::transcode(classElement->getAttribute(XMLString::transcode("blue"))));
				classData[i]->numInputBands = numBands;
			}
			
			parser->release();
			delete errHandler;
			
			XMLPlatformUtils::Terminate();
		}
		catch (const XMLException& e) 
		{
			char *message = XMLString::transcode(e.getMessage());
			string outMessage =  string("XMLException : ") + string(message);
			throw RSGISParseColourException(outMessage.c_str());
		}
		catch (const DOMException& e) 
		{
			char *message = XMLString::transcode(e.getMessage());
			string outMessage =  string("DOMException : ") + string(message);
			throw RSGISParseColourException(outMessage.c_str());
		}
		catch(RSGISInputStreamException e)
		{
			throw RSGISParseColourException(e.what());
		}
		catch(RSGISParseColourException e)
		{
			throw e;
		}
		
	/*	for(int i = 0; i < *numClasses; i++)
		{
			cout << "Class: " << classData[i]->className << " has ID: " << classData[i]->classID << endl;
		}
		*/
		return classData;
	}
	
	RSGISClassColourReader::~RSGISClassColourReader()
	{
		
	}
	
	
}}



