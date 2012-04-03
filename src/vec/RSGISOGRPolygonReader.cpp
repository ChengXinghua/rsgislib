/*
 *  RSGISOGRPolygonReader.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 01/03/2010.
 *  Copyright 2010 RSGISLib. All rights reserved.
 *  This file is part of RSGISLib.
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

#include "RSGISOGRPolygonReader.h"

namespace rsgis{namespace vec{
	
	RSGISOGRPolygonReader::RSGISOGRPolygonReader(vector<OGRPolygon*> *polygons)
	{
		this->polygons = polygons;
	}
	
	void RSGISOGRPolygonReader::processFeature(OGRFeature *inFeature, OGRFeature *outFeature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		throw RSGISVectorException("Not implemented..");
	}
	
	void RSGISOGRPolygonReader::processFeature(OGRFeature *feature, Envelope *env, long fid) throw(RSGISVectorException)
	{
		OGRwkbGeometryType geometryType = feature->GetGeometryRef()->getGeometryType();
		
		// Calculate the centre coordinates
		if( geometryType == wkbPolygon )
		{
			OGRPolygon *polygon = (OGRPolygon *) feature->GetGeometryRef()->clone();
			polygons->push_back(polygon);
		} 
		else
		{
			string message = string("Unsupport data type: ") + string(feature->GetGeometryRef()->getGeometryName());
			throw RSGISVectorException(message);
		}
	}
	
	void RSGISOGRPolygonReader::createOutputLayerDefinition(OGRLayer *outputLayer, OGRFeatureDefn *inFeatureDefn) throw(RSGISVectorOutputException)
	{
		// Nothing to do!
	}
	
	RSGISOGRPolygonReader::~RSGISOGRPolygonReader()
	{

	}
}}







