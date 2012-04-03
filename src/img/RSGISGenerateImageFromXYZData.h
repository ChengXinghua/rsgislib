/*
 *  RSGISGenerateImageFromXYZData.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 10/02/2011.
 *  Copyright 2011 RSGISLib. All rights reserved.
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

#ifndef RSGISGenerateImageFromXYZData_H
#define RSGISGenerateImageFromXYZData_H

#include <iostream>
#include <fstream>

#include "gdal_priv.h"

#include "common/RSGISImageException.h"
#include "common/RSGISFileException.h"

#include "utils/RSGISTextUtils.h"

#include "img/RSGISImageUtils.h"

using namespace rsgis;
using namespace rsgis::utils;

namespace rsgis { namespace img {	
		
	class RSGISGenerateImageFromXYZData
	{
		struct XYZData
		{
			double x;
			double y;
			double z;
		};
		
	public:
		RSGISGenerateImageFromXYZData(string inputDataFile, string outputFile, char delimiter, string proj4, bool xyOrder, float resolution);
		void createImageRepresentingXYZData() throw(RSGISFileException, RSGISImageException);
		void readInputData() throw(RSGISFileException);
		~RSGISGenerateImageFromXYZData();
	protected:
		string inputFile;
		string outputFile;
		char delimiter;
		bool xyOrder;
		string proj4;
		vector<XYZData*> *data;
		double *bbox;
		float resolution;
	};
	
}}

#endif

