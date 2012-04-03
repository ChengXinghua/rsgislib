/*
 *  RSGISExportForPlottingIncremental.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 21/10/2009.
 *  Copyright 2009 RSGISLib. All rights reserved.
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

#include "RSGISExportForPlottingIncremental.h"


namespace rsgis{namespace utils{

	RSGISExportForPlottingIncremental::RSGISExportForPlottingIncremental()
	{
		outputFileStream = NULL;
		open = false;
		type = unknown;
	}
	
	bool RSGISExportForPlottingIncremental::openFile(string file, PlotTypes inType) throw(RSGISOutputStreamException)
	{
		outputFileStream = new ofstream();
		outputFileStream->open(file.c_str(), ios::out | ios::trunc);
		
		if(!outputFileStream->is_open())
		{
			open = false;
			
			string message = string("Could not open file ") + file;
			throw RSGISOutputStreamException(message);
		}
		open = true;
		
		if(inType == freq2d)
		{
			(*outputFileStream) << "#freq2D" << endl;
			(*outputFileStream) << "#value" << endl;
		}
		else if(inType == freq3d)
		{
			(*outputFileStream) << "#freq3D" << endl;
			(*outputFileStream) << "#value1,value2" << endl;
		}
		else if(inType == scatternd)
		{
			(*outputFileStream) << "#NDScatter" << endl;
			(*outputFileStream) << "#v1,v2,v3,...vn" << endl;
		}
		else if(inType == scatter2d)
		{
			(*outputFileStream) << "#2DScatter" << endl;
			(*outputFileStream) << "#x,y" << endl;
		}
		else if(inType == scatter3d)
		{
			(*outputFileStream) << "#3DScatter" << endl;
			(*outputFileStream) << "#x,y,z" << endl;
		}
		else if(inType == cscatter2d)
		{
			(*outputFileStream) << "#c2DScatter" << endl;
			(*outputFileStream) << "#x,y,c" << endl;
		}
		else if(inType == cscatter3d)
		{
			(*outputFileStream) << "#c3DScatter" << endl;
			(*outputFileStream) << "#x,y,z,c" << endl;
		}
		else if(inType == density)
		{
			(*outputFileStream) << "#Density" << endl;
			(*outputFileStream) << "#x,y" << endl;
		}
		else if(inType == surface)
		{
			(*outputFileStream) << "#Surface" << endl;
			(*outputFileStream) << "#x,y,z" << endl;
		}
		else if(inType == csurface)
		{
			(*outputFileStream) << "#cSurface" << endl;
			(*outputFileStream) << "#x,y,z,c" << endl;
		}
		else if(inType == lines2d)
		{
			(*outputFileStream) << "#lines2D" << endl;
			(*outputFileStream) << "#x1,y1,x2,y2" << endl;
		}
		else if(inType == lines3d)
		{
			(*outputFileStream) << "#lines3D" << endl;
			(*outputFileStream) << "#x1,y1,z1,x2,y2,z2" << endl;
		}
		else if(inType == triangle2d)
		{
			(*outputFileStream) << "#triangle2d" << endl;
			(*outputFileStream) << "#x1,y1" << endl;
			(*outputFileStream) << "#x2,y2" << endl;
			(*outputFileStream) << "#x3,y3" << endl;
		}
		else if(inType == triangle3d)
		{
			(*outputFileStream) << "#triangle3d" << endl;
			(*outputFileStream) << "#x1,y1,z1" << endl;
			(*outputFileStream) << "#x2,y2,z2" << endl;
			(*outputFileStream) << "#x3,y3,z3" << endl;
		}			
		else 
		{
			throw RSGISOutputStreamException("Type is unknown.");
		}
		
		this->type = inType;
		
		return open;
		
	}
	
	void RSGISExportForPlottingIncremental::writeFrequency2DLine(double value) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != freq2d)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << value << endl;
	}
	
	void RSGISExportForPlottingIncremental::writeFrequency3DLine(double value1, double value2) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != freq3d)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << value1 << "," << value2 << endl;	
	}

	void RSGISExportForPlottingIncremental::writeScatterNDLine(vector<double> *vals) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != scatter2d)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		bool first = true;
		vector<double>::iterator iterVals;
		for(iterVals = vals->begin(); iterVals != vals->end(); ++iterVals)
		{
			if(first)
			{
				(*outputFileStream) << *iterVals;
			}
			else 
			{
				(*outputFileStream) << "," << *iterVals;
			}

		}
		 (*outputFileStream) << endl;	
	}
	
	void RSGISExportForPlottingIncremental::writeScatter2DLine(double x, double y) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != scatter2d)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << endl;	
	}
	
	void RSGISExportForPlottingIncremental::writeScatter3DLine(double x, double y, double z) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != scatter3d)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << "," << z << endl;
	}
	
	void RSGISExportForPlottingIncremental::writeCScatter2DLine(double x, double y, double c) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != cscatter2d)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << "," << c << endl;
	}
	
	void RSGISExportForPlottingIncremental::writeCScatter3DLine(double x, double y, double z, double c) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != cscatter3d)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << "," << z << "," << c << endl;
	}
	
	void RSGISExportForPlottingIncremental::writeDensityLine(double x, double y) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != density)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << endl;
	}
	
	void RSGISExportForPlottingIncremental::writeSurfaceLine(double x, double y, double z) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != surface)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << "," << z << endl;
	}
	
	void RSGISExportForPlottingIncremental::writeCSurfaceLine(double x, double y, double z, double c) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != csurface)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x << "," << y << "," << z << "," << c << endl;
	}
	
	void RSGISExportForPlottingIncremental::writeLines2DLine(double x1, double y1, double x2, double y2) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != lines2d)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x1 << "," << y1 << "," << x2 << "," << y2 << endl;
	}
	
	void RSGISExportForPlottingIncremental::writeLines3DLine(double x1, double y1, double z1, double x2, double y2, double z2) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != lines3d)
		{
			throw RSGISOutputStreamException("Open file is not of type freq2d");
		}
		
		(*outputFileStream) << x1 << "," << y1 << "," << z1 << "," << x2 << "," << y2 << "," << z2 << endl;
	}
	
	void RSGISExportForPlottingIncremental::writeTriangle2D(double x1, double y1, double x2, double y2, double x3, double y3) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != triangle2d)
		{
			throw RSGISOutputStreamException("Open file is not of type triangle2d");
		}
		
		(*outputFileStream) << x1 << "," << y1 << endl;
		(*outputFileStream) << x2 << "," << y2 << endl;
		(*outputFileStream) << x3 << "," << y3 << endl;
	}
	
	void RSGISExportForPlottingIncremental::writeTriangle3D(double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3) throw(RSGISOutputStreamException)
	{
		if(!open)
		{
			throw RSGISOutputStreamException("File is not open!");
		}
		else if(this->type != triangle3d)
		{
			throw RSGISOutputStreamException("Open file is not of type triangle3d");
		}
		
		(*outputFileStream) << x1 << "," << y1 << "," << z1 << endl;
		(*outputFileStream) << x2 << "," << y2 << "," << z2 << endl;
		(*outputFileStream) << x3 << "," << y3 << "," << z3 << endl;
	}
	
	PlotTypes RSGISExportForPlottingIncremental::getPlotType()
	{
		return type;
	}
	
	void RSGISExportForPlottingIncremental::close()
	{
		if(open)
		{
			outputFileStream->flush();
			outputFileStream->close();
			delete outputFileStream;
			open = false;
			type = unknown;
		}
		
	}
	
	RSGISExportForPlottingIncremental::~RSGISExportForPlottingIncremental()
	{
		this->close();
	}
}}


