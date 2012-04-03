/*
 *  RSGISGlobalOptimiser2D.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/08/2009.
 *  Copyright 2009 RSGISLib.
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

#ifndef RSGISGlobalOptimiser2D_H
#define RSGISGlobalOptimiser2D_H

#include <iostream>
#include <string>
#include <math.h>

#include "math/RSGISGlobalOptimisationFunction.h"
#include "math/RSGISOptimisationException.h"

#include "geos/geom/Coordinate.h"
#include "geos/geom/Envelope.h"

using namespace std;
using namespace geos::geom;

namespace rsgis{namespace math{
	
	class RSGISGlobalOptimiser2D
		{
		public:
			RSGISGlobalOptimiser2D(RSGISGlobalOptimisationFunction *func, bool maximise)
			{
				this->func = func;
				this->maximise = maximise;
			}
			virtual vector<Coordinate*>* optimise4Neighbor(vector<Coordinate*> *coords, double step, Envelope *boundary) throw(RSGISOptimisationException) = 0;
			virtual vector<Coordinate*>* optimise8Neighbor(vector<Coordinate*> *coords, double step, Envelope *boundary) throw(RSGISOptimisationException) = 0;
			virtual ~RSGISGlobalOptimiser2D(){};
		protected:
			RSGISGlobalOptimisationFunction *func;
			bool maximise;
		};
}}

#endif




