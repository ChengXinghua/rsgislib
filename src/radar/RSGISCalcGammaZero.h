/*
 *  RSGISCalcGammaZero.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 25/04/2008.
 *  Copyright 2008 RSGISLib. All rights reserved.
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

#ifndef RSGISCalcGammaZero_H
#define RSGISCalcGammaZero_H

#include <iostream>
#include <string>
#include <math.h>
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"

using namespace std;
using namespace rsgis::img;

namespace rsgis 
{
	namespace radar
	{
		class RSGISCalcGammaZero : public RSGISCalcImageValue
			{
			public: 
				RSGISCalcGammaZero(int numberOutBands, bool deg);
				void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException);
				void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
				void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
				bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output)throw(RSGISImageCalcException){throw RSGISImageCalcException("Not implemented");};
				~RSGISCalcGammaZero();
			protected:
				bool deg;
			};
	}
}
#endif

