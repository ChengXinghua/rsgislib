/*
 *  RSGISImageCalcValueBaysianNoPrior.h
 *  RSGIS_LIB
 *
 *  Created by Daniel Clewley on 12/12/2008.
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

#ifndef RSGISImageCalcValueBaysianNoPrior_H
#define RSGISImageCalcValueBaysianNoPrior_H

#include <iostream>
#include "img/RSGISCalcImage.h"
#include "img/RSGISCalcImageValue.h"
#include "img/RSGISImageBandException.h"
#include "img/RSGISImageCalcException.h"
#include "math/RSGISBaysianStatsNoPrior.h"
#include "math/RSGISBaysianStatsException.h"
#include "math/RSGISBaysianDeltaType.h"
#include "common/RSGISImageException.h"

#include "gdal_priv.h"

using namespace std;
using namespace rsgis::math;

namespace rsgis{namespace img{
	class RSGISImageCalcValueBaysianNoPrior	: public RSGISCalcImageValue
		{
		public:
			RSGISImageCalcValueBaysianNoPrior(int numberOutBands, RSGISMathFunction *function, double variance, double interval, double minVal, double maxVal, double lowerLimit, double upperLimit, deltatypedef deltatype);
			void calcImageValue(float *bandValues, int numBands, float *output) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands, Envelope extent) throw(RSGISImageCalcException);
			void calcImageValue(float *bandValues, int numBands, float *output, Envelope extent) throw(RSGISImageCalcException);
			void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
            void calcImageValue(float ***dataBlock, int numBands, int winSize, float *output, Envelope extent) throw(RSGISImageCalcException){throw RSGISImageCalcException("No implemented");};
			bool calcImageValueCondition(float ***dataBlock, int numBands, int winSize, float *output) throw(RSGISImageCalcException);
			~RSGISImageCalcValueBaysianNoPrior();
		protected:
			RSGISMathFunction *function;
			RSGISMathFunction *probDistro;
			double variance;
			double interval;
			double minVal;
			double maxVal;
			double upperLimit;
			double lowerLimit;
			double* outputVals;
			deltatypedef deltatype;
			RSGISBaysianStatsNoPrior *baysianStats;
		};	
}}
#endif
