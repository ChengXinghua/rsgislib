/*
 *  RSGISRegistrationException.h
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 31/08/2010.
 *  Copyright 2010 RSGISLib.
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

#ifndef RSGISRegistrationException_H
#define RSGISRegistrationException_H

#include "RSGISException.h"

namespace rsgis 
{    
	class RSGISRegistrationException : public RSGISException
	{
	public:
		RSGISRegistrationException();
		RSGISRegistrationException(const char* message);
		RSGISRegistrationException(std::string message);
	};
}

#endif

