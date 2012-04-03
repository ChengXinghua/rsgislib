/*
 *  RSGISImageUtils.cpp
 *  RSGIS_LIB
 *
 *  Created by Pete Bunting on 23/04/2008.
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

#include "RSGISImageUtils.h"

namespace rsgis{namespace img{

	RSGISImageUtils::RSGISImageUtils()
	{
		
	}
	
	void RSGISImageUtils::getImageOverlap(GDALDataset **datasets, int numDS,  int **dsOffsets, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
			//cout << "TL [" << transformations[i][0] << "," << transformations[i][3] << "]\n";
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		bool first = true;
		const char *proj = NULL;
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if(transformations[i][1] != pixelXRes & transformations[i][5] != pixelYRes)
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(string(datasets[i]->GetProjectionRef()) != string(proj))
					{
						cout << "Not all image bands have the same projection" << endl;
					}
					
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
				}
			}
			
			if(maxX - minX <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
			gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
			
			*width = floor(((maxX - minX)/pixelXRes));
			*height = floor(((maxY - minY)/pixelYResPos));
			
			double diffX = 0;
			double diffY = 0;
			
			for(int i = 0; i < numDS; i++)
			{
				diffX = minX - transformations[i][0];
				diffY = transformations[i][3] - maxY;
				
				if(diffX != 0)
				{
					dsOffsets[i][0] = ceil(diffX/pixelXRes);
				}
				else
				{
					dsOffsets[i][0] = 0;
				}
				
				if(diffY != 0)
				{
					dsOffsets[i][1] = ceil(diffY/pixelYResPos);
				}
				else
				{
					dsOffsets[i][1] = 0;
				}
			}
			
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
	
	void RSGISImageUtils::getImageOverlap(GDALDataset **datasets, int numDS,  int **dsOffsets, int *width, int *height, double *gdalTransform, Envelope *env) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
			
			//cout << "TL [" << transformations[i][0] << "," << transformations[i][3] << "]\n";
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		const char *proj = NULL;
		bool first = true;
		
		
		try
		{
			// Calculate Image Overlap.
			for(int i = 0; i < numDS; ++i)
			{
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if(transformations[i][1] != pixelXRes & transformations[i][5] != pixelYRes)
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(string(datasets[i]->GetProjectionRef()) != string(proj))
					{
						//cout << "Band 1 Projection = " << proj << endl;
						//cout << "Band " << i <<  " Projection = " << datasets[i]->GetProjectionRef()<< endl;
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
				}
			}
			/*
			cout << "MinX = " << minX << endl;
			cout << "MaxX = " << maxX << endl;
			cout << "MinY = " << minY << endl;
			cout << "MaxY = " << maxY << endl;
			*/
			if(maxX - minX <= 0)
			{
				cout << "MinX = " << minX << endl;
				cout << "MaxX = " << maxX << endl;
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				cout << "MinY = " << minY << endl;
				cout << "MaxY = " << maxY << endl;
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
			// Check if OK to process.
			// Changed from throwing exception (left old code) - Dan
			bool process = true;
			
			if(minX > env->getMinX())
			{
				//cout.precision(10);
				//cout << "Envelope does not fit within the image overlap (MinX)" << endl;
				process = false;
				//cout << "Image Min X = " << minX << " Envelope Min X = " << env->getMinX() << endl;
				//throw RSGISImageBandException("Envelope does not fit within the image overlap (MinX)");
			}
			
			if(minY > env->getMinY())
			{
				//cout.precision(10);
				//cout << "Envelope does not fit within the image overlap (MinY)" << endl;
				process = false;
				//cout << "Image Min Y = " << minY << " Envelope Min Y = " << env->getMinY() << endl;
				//throw RSGISImageBandException("Envelope does not fit within the image overlap (MinY)");
			}
			
			if(maxX < env->getMaxX())
			{
				//cout.precision(10);
				//cout << "Envelope does not fit within the image overlap (MaxX)" << endl;
				process = false;
				//cout << "Image Max X = " << maxX << " Envelope Max X = " << env->getMaxX() << endl;
				//throw RSGISImageBandException("Envelope does not fit within the image overlap (MaxX)");
			}
			
			if(maxY < env->getMaxY())
			{
				//cout.precision(10);
				//cout << "Envelope does not fit within the image overlap (MaxY)" << endl;
				process = false;
				//cout << "Image Max Y = " << maxY << " Envelope Max Y = " << env->getMaxY() << endl;
				//throw RSGISImageBandException("Envelope does not fit within the image overlap (MaxY)");
			}
			
			if(process)
			{
				// Trim to the envelope
				minX = env->getMinX();
				maxX = env->getMaxX();
				minY = env->getMinY();
				maxY = env->getMaxY();
				
				// Define output values.
				gdalTransform[0] = minX;
				gdalTransform[1] = pixelXRes;
				gdalTransform[2] = rotateX;
				gdalTransform[3] = maxY;
				gdalTransform[4] = rotateY;
				gdalTransform[5] = pixelYRes;
				
				*width = floor(((maxX - minX)/pixelXRes));
				*height = floor(((maxY - minY)/pixelYResPos));
				
				double diffX = 0;
				double diffY = 0;
				
				for(int i = 0; i < numDS; i++)
				{
					diffX = minX - transformations[i][0];
					diffY = transformations[i][3] - maxY;
					
					if(diffX != 0)
					{
						dsOffsets[i][0] = ceil(diffX/pixelXRes);
					}
					else
					{
						dsOffsets[i][0] = 0;
					}
					
					if(diffY != 0)
					{
						dsOffsets[i][1] = ceil(diffY/pixelYResPos);
					}
					else
					{
						dsOffsets[i][1] = 0;
					}
				}
			}
			/* Commented out else statement, this was added to fix problem with cut2poly but caused problems with zonal stats commands
			 * It has therefore been commented out - Dan Clewley 24/01/11 
             * Update (16/03/11) - Dan Clewley
             * - The uncommented code fixed the problem of a polygon that was large than the image but caused problems when
             *  the polygon was outside the scene.
             * - If the polygon does not fit inside the scene the center is calculated, if this is within the scene it calculates the overlap.
             *   
             */
			/*else
			{				
                // Calculate centre of envelope
                double centreEnvelopeX = (env->getMaxX() + env->getMinX()) / 2.0;
                double centreEnvelopeY = (env->getMaxY() + env->getMinY()) / 2.0;
                
                if((centreEnvelopeX < maxX) && (centreEnvelopeY < maxY)) // If center of envolope within overlap proceed
                {
					// Define output values.
                    gdalTransform[0] = minX;
                    gdalTransform[1] = pixelXRes;
                    gdalTransform[2] = rotateX;
                    gdalTransform[3] = maxY;
                    gdalTransform[4] = rotateY;
                    gdalTransform[5] = pixelYRes;
                    
                    *width = floor(((maxX - minX)/pixelXRes));
                    *height = floor(((maxY - minY)/pixelYResPos));
                    
                    double diffX = 0;
                    double diffY = 0;
                    
                    for(int i = 0; i < numDS; i++)
                    {
                        diffX = minX - transformations[i][0];
                        diffY = transformations[i][3] - maxY;
                        
                        if(diffX != 0)
                        {
                            dsOffsets[i][0] = ceil(diffX/pixelXRes);
                        }
                        else
                        {
                            dsOffsets[i][0] = 0;
                        }
                        
                        if(diffY != 0)
                        {
                            dsOffsets[i][1] = ceil(diffY/pixelYResPos);
                        }
                        else
                        {
                            dsOffsets[i][1] = 0;
                        }
                    }
                }
			}*/
			
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
    
    void RSGISImageUtils::getImageOverlapCut2Env(GDALDataset **datasets, int numDS,  int **dsOffsets, int *width, int *height, double *gdalTransform, Envelope *env) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
			
			//cout << "TL [" << transformations[i][0] << "," << transformations[i][3] << "]\n";
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		const char *proj = NULL;
		bool first = true;
		
		
		try
		{
			// Calculate Image Overlap.
			for(int i = 0; i < numDS; ++i)
			{
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if(transformations[i][1] != pixelXRes & transformations[i][5] != pixelYRes)
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(string(datasets[i]->GetProjectionRef()) != string(proj))
					{
						//cout << "Band 1 Projection = " << proj << endl;
						//cout << "Band " << i <<  " Projection = " << datasets[i]->GetProjectionRef()<< endl;
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
				}
			}
            
			if(maxX - minX <= 0)
			{
				cout << "MinX = " << minX << endl;
				cout << "MaxX = " << maxX << endl;
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				cout << "MinY = " << minY << endl;
				cout << "MaxY = " << maxY << endl;
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
			// Cut to env extent
			if(env->getMinX() > minX)
			{
                minX = env->getMinX();
			}
			
			if(env->getMinY() > minY)
			{
				minY = env->getMinY();
			}
			
			if(env->getMaxX() < maxX)
			{
				maxX = env->getMaxX();
			}
			
			if(env->getMaxY() < maxY)
			{
				maxY = env->getMaxY();
			}
			
            if(maxX - minX <= 0)
			{
				cout << "MinX = " << minX << endl;
				cout << "MaxX = " << maxX << endl;
				throw RSGISImageBandException("Images and Envelope do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				cout << "MinY = " << minY << endl;
				cout << "MaxY = " << maxY << endl;
				throw RSGISImageBandException("Images and Envelope do not overlap in the Y axis");
			}
            
            gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
			
			*width = floor(((maxX - minX)/pixelXRes));
			*height = floor(((maxY - minY)/pixelYResPos));
			
			double diffX = 0;
			double diffY = 0;
			
			for(int i = 0; i < numDS; i++)
			{
				diffX = minX - transformations[i][0];
				diffY = transformations[i][3] - maxY;
				
				if(diffX != 0)
				{
					dsOffsets[i][0] = ceil(diffX/pixelXRes);
				}
				else
				{
					dsOffsets[i][0] = 0;
				}
				
				if(diffY != 0)
				{
					dsOffsets[i][1] = ceil(diffY/pixelYResPos);
				}
				else
				{
					dsOffsets[i][1] = 0;
				}
			}
			
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
	
	void RSGISImageUtils::getImageOverlap(GDALDataset **datasets, int numDS, int *width, int *height, Envelope *env) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
			//cout << "TL [" << transformations[i][0] << "," << transformations[i][3] << "]\n";
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		const char *proj = NULL;
		bool first = true;
		
		try
		{
			
			// Calculate Image Overlap.
			for(int i = 0; i < numDS; i++)
			{
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);

					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if(transformations[i][1] != pixelXRes & transformations[i][5] != pixelYRes)
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(string(datasets[i]->GetProjectionRef()) != string(proj))
					{
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] > minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] < maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX < maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY > minY)
					{
						minY = tmpMinY;
					}
				}
			}
			
			if(maxX - minX <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the X axis");
			}
			
			if(maxY - minY <= 0)
			{
				throw RSGISImageBandException("Images do not overlap in the Y axis");
			}
			
			// Trim to the envelope
			if(minX > env->getMinX())
			{
				throw RSGISImageBandException("Envelope does not fit within the image overlap (MinX)");
			}
			
			if(minY > env->getMinY())
			{
				throw RSGISImageBandException("Envelope does not fit within the image overlap (MinY)");
			}
			
			if(maxX < env->getMaxX())
			{
				throw RSGISImageBandException("Envelope does not fit within the image overlap (MaxX)");
			}
			
			if(maxY < env->getMaxY())
			{
				throw RSGISImageBandException("Envelope does not fit within the image overlap (MaxY)");
			}
			
			minX = env->getMinX();
			maxX = env->getMaxX();
			minY = env->getMinY();
			maxY = env->getMaxY();
			
			*width = floor(((maxX - minX)/pixelXRes));
			*height = floor(((maxY - minY)/pixelYResPos));
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
	
	void RSGISImageUtils::getImagesExtent(GDALDataset **datasets, int numDS, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException)
	{
		double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
		for(int i = 0; i < numDS; i++)
		{
			transformations[i] = new double[6];
			datasets[i]->GetGeoTransform(transformations[i]);
			xSize[i] = datasets[i]->GetRasterXSize();
			ySize[i] = datasets[i]->GetRasterYSize();
			//cout << "TL [" << transformations[i][0] << "," << transformations[i][3] << "]\n";
			//cout << "Size: [" << xSize[i] << "," << ySize[i] << "]\n";
		}
		double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		const char *proj = NULL;
		bool first = true;
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
				if(first)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = datasets[i]->GetProjectionRef(); // Get projection of first band in image
					
					first = false;
				}
				else
				{
					if(transformations[i][1] != pixelXRes & transformations[i][5] != pixelYRes)
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(string(datasets[i]->GetProjectionRef()) != string(proj))
					{
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] < minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] > maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX > maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY < minY)
					{
						minY = tmpMinY;
					}
				}
			}
			
			gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
			
			*width = ceil(((maxX - minX)/pixelXRes));
			*height = ceil(((maxY - minY)/pixelYResPos));
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
	}
	
    void RSGISImageUtils::getImagesExtent(string *inputImages, int numDS, int *width, int *height, double *gdalTransform) throw(RSGISImageBandException)
    {
        double **transformations = new double*[numDS];
		int *xSize = new int[numDS];
		int *ySize = new int[numDS];
        GDALDataset *dataset = NULL;
		for(int i = 0; i < numDS; i++)
		{
            dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
            if(dataset == NULL)
            {
                string message = string("Could not open image ") + inputImages[i];
                throw RSGISImageException(message.c_str());
            }
            
			transformations[i] = new double[6];
			dataset->GetGeoTransform(transformations[i]);
			xSize[i] = dataset->GetRasterXSize();
			ySize[i] = dataset->GetRasterYSize();
			
            GDALClose(dataset);
		}
        
        double rotateX = 0;
		double rotateY = 0;
		double pixelXRes = 0;
		double pixelYRes = 0;
		double pixelYResPos = 0;
		double minX = 0;
		double maxX = 0;
		double tmpMaxX = 0;
		double minY = 0;
		double tmpMinY = 0;
		double maxY = 0;
		string proj = "";
		
		try
		{
			for(int i = 0; i < numDS; i++)
			{
                dataset = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
                if(dataset == NULL)
                {
                    string message = string("Could not open image ") + inputImages[i];
                    throw RSGISImageException(message.c_str());
                }
                
				if(i == 0)
				{
					pixelXRes = transformations[i][1];
					pixelYRes = transformations[i][5];
					
					rotateX = transformations[i][2];
					rotateY = transformations[i][4];
					
					if(pixelYRes < 0)
					{
						pixelYResPos = pixelYRes * (-1);
					}
					else
					{
						pixelYResPos = pixelYRes;
					}
					
					minX = transformations[i][0];
					maxY = transformations[i][3];
					
					maxX = minX + (xSize[i] * pixelXRes);
					minY = maxY - (ySize[i] * pixelYResPos);
					
					proj = string(dataset->GetProjectionRef()); // Get projection of first band in image
				}
				else
				{
					if(transformations[i][1] != pixelXRes & transformations[i][5] != pixelYRes)
					{
						throw RSGISImageBandException("Not all image bands have the same resolution..");
					}
					
					if(string(dataset->GetProjectionRef()) != proj)
					{
                        cout << "First: (" << i << ")" << proj << endl;
                        cout << "Dataset: (" << i << ")" << string(dataset->GetProjectionRef()) << endl;
						throw RSGISImageBandException("Not all image bands have the same projection..");
					}
					
					
					if(transformations[i][2] != rotateX & transformations[i][4] != rotateY)
					{
						throw RSGISImageBandException("Not all image bands have the same rotation..");
					}
					
					if(transformations[i][0] < minX)
					{
						minX = transformations[i][0];
					}
					
					if(transformations[i][3] > maxY)
					{
						maxY = transformations[i][3];
					}
					
					tmpMaxX = transformations[i][0] + (xSize[i] * pixelXRes);
					tmpMinY = transformations[i][3] - (ySize[i] * pixelYResPos);
					
					if(tmpMaxX > maxX)
					{
						maxX = tmpMaxX;
					}
					
					if(tmpMinY < minY)
					{
						minY = tmpMinY;
					}
				}
                GDALClose(dataset);
			}
			
			gdalTransform[0] = minX;
			gdalTransform[1] = pixelXRes;
			gdalTransform[2] = rotateX;
			gdalTransform[3] = maxY;
			gdalTransform[4] = rotateY;
			gdalTransform[5] = pixelYRes;
			
			*width = ceil(((maxX - minX)/pixelXRes));
			*height = ceil(((maxY - minY)/pixelYResPos));
		}
		catch(RSGISImageBandException& e)
		{
			if(transformations != NULL)
			{
				for(int i = 0; i < numDS; i++)
				{
					delete[] transformations[i];
				}
				delete[] transformations;
			}
			if(xSize != NULL)
			{
				delete[] xSize;
			}
			if(ySize != NULL)
			{
				delete[] ySize;
			}
			throw e;
		}
		
		if(transformations != NULL)
		{
			for(int i = 0; i < numDS; i++)
			{
				delete[] transformations[i];
			}
			delete[] transformations;
		}
		if(xSize != NULL)
		{
			delete[] xSize;
		}
		if(ySize != NULL)
		{
			delete[] ySize;
		}
    }
    
	void RSGISImageUtils::exportImageToTextCol(GDALDataset *image, int band, string outputText)throw(RSGISImageBandException, RSGISOutputStreamException)
	{
		RSGISImageUtils imgUtils;
		double *gdalTranslation = new double[6];
		float *inputData = NULL;
		
		GDALRasterBand *inputRasterBand = NULL;
		
		int width = 0;
		int height = 0;
		int numberOfBands = 0;
		
		try
		{
			width = image->GetRasterXSize();
			height = image->GetRasterYSize();
			numberOfBands = image->GetRasterCount();
			
			if(band == 0)
			{
				band++;
			}
			else if(band > numberOfBands)
			{
				throw RSGISImageBandException("There are not enough bands within the image.");
			}
			
			// Create textfile
			ofstream outFile(outputText.c_str(), ios::out | ios::trunc);
			
			if(!outFile.is_open())
			{
				throw RSGISOutputStreamException("Could not open text file.");
			}
			
			// Allocate memory
			inputData = (float *) CPLMalloc(sizeof(float)*width);
			
			// Get Raster band
			inputRasterBand = image->GetRasterBand(band);
			
			int feedback = height/10;
			int feedbackCounter = 0;
			cout << "Started" << flush;
			// Loop images to process data
			for(int i = 0; i < height; i++)
			{
				if((i % feedback) == 0)
				{
					cout << ".." << feedbackCounter << ".." << flush;
					feedbackCounter = feedbackCounter + 10;
				}
				inputRasterBand->RasterIO(GF_Read, 0, i, width, 1, inputData, width, 1, GDT_Float32, 0, 0);
				
				for(int j = 0; j < width; j++)
				{
					// Output to text file...
					outFile << inputData[j] << endl;
				}
			}
			cout << "..100 Complete.\n";
		}
		catch(RSGISImageBandException e)
		{
			
			throw e;
		}
		catch(RSGISOutputStreamException e)
		{
			
			throw e;
		}
		
		if(gdalTranslation != NULL)
		{
			delete[] gdalTranslation;
		}
		if(inputData != NULL)
		{
			delete inputData;
		}
	}
	
	GDALDataset* RSGISImageUtils::createBlankImage(string imageFile, double *transformation, int xSize, int ySize, int numBands, string projection, float value) throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDriver *poDriver = NULL;
		GDALRasterBand *imgBand = NULL;
		GDALDataset *outputImage = NULL;
		
		float *imgData = NULL;
		
		if(transformation[1] <= 0)
		{
			throw RSGISImageException("The resolution of the image needs to be > 0.");
		}
		if(numBands <= 0)
		{
			throw RSGISImageException("The number of bands needs to be > 0.");
		}
		
		try
		{
			if(xSize <= 0)
			{
				throw RSGISImageException("The image needs to have a xSize > 0");
			}
			if(ySize <= 0)
			{
				throw RSGISImageException("The image needs to have a ySize > 0");
			}
			
			poDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(poDriver == NULL)
			{
				throw RSGISImageException("ENVI image driver is not available.");
			}
			
			// Create new file. 
			// Set unsupported options to NULL
			char **papszOptions = poDriver->GetMetadata();
			/*papszOptions = CSLSetNameValue (papszOptions, "DMD_LONGNAME", NULL);
			papszOptions = CSLSetNameValue (papszOptions, "DMD_HELPTOPIC", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_EXTENSION", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONDATATYPES", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONOPTIONLIST", NULL); 
			papszOptions = CSLSetNameValue (papszOptions, "DCAP_VIRTUALIO", NULL); 
			papszOptions = CSLSetNameValue (papszOptions, "DCAP_CREATE", NULL); */
			
			//outputImage = poDriver->Create(imageFile.c_str(), xSize, ySize, numBands, GDT_Float32, poDriver->GetMetadata());
			outputImage = poDriver->Create(imageFile.c_str(), xSize, ySize, numBands, GDT_Float32, papszOptions);
			
			if(outputImage == NULL)
			{
				throw RSGISImageException("Image could not be created.");
			}
			outputImage->SetGeoTransform(transformation);
			outputImage->SetProjection(projection.c_str());
			
			imgData = (float *) CPLMalloc(sizeof(float)*xSize);
			for(int i = 0; i < xSize; i++)
			{
				imgData[i] = value;
			}
			
			/*cout << "Created Image: \n";
			cout << "xSize = " << xSize << endl;
			cout << "ySize = " << ySize << endl;*/
			cout << "Started (total " << numBands << ")." << flush;
			for(int n = 1; n <= numBands; n++)
			{
				cout << "." << n << "." << flush;
				imgBand = outputImage->GetRasterBand(n);
				for(int i = 0; i < ySize; i++)
				{
					imgBand->RasterIO(GF_Write, 0, i, xSize, 1, imgData, xSize, 1, GDT_Float32, 0, 0);
				}
			}
			cout << ". Complete\n";
		}
		catch(RSGISImageBandException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
			if(imgData != NULL)
			{
				delete imgData;
			}
			throw e;
		}
		catch(RSGISImageException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
			if(imgData != NULL)
			{
				delete imgData;
			}
			throw e;
		}
		
		if(imgData != NULL)
		{
			delete imgData;
		}
		
		return outputImage;
	}
    
    GDALDataset* RSGISImageUtils::createBlankImage(string imageFile, double *transformation, int xSize, int ySize, int numBands, string projection, float value, vector<string> bandNames) throw(RSGISImageException, RSGISImageBandException)
    {
        GDALAllRegister();
		GDALDriver *poDriver = NULL;
		GDALRasterBand *imgBand = NULL;
		GDALDataset *outputImage = NULL;
		
		float *imgData = NULL;
		
		if(transformation[1] <= 0)
		{
			throw RSGISImageException("The resolution of the image needs to be > 0.");
		}
		if(numBands <= 0)
		{
			throw RSGISImageException("The number of bands needs to be > 0.");
		}
		
		try
		{
			if(xSize <= 0)
			{
				throw RSGISImageException("The image needs to have a xSize > 0");
			}
			if(ySize <= 0)
			{
				throw RSGISImageException("The image needs to have a ySize > 0");
			}
			
			poDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(poDriver == NULL)
			{
				throw RSGISImageException("ENVI image driver is not available.");
			}
			
			// Create new file. 
			char **papszOptions = poDriver->GetMetadata();
			/*papszOptions = CSLSetNameValue (papszOptions, "DMD_LONGNAME", NULL);
			papszOptions = CSLSetNameValue (papszOptions, "DMD_HELPTOPIC", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_EXTENSION", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONDATATYPES", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONOPTIONLIST", NULL); 
			papszOptions = CSLSetNameValue (papszOptions, "DCAP_VIRTUALIO", NULL); 
			papszOptions = CSLSetNameValue (papszOptions, "DCAP_CREATE", NULL); */
			
			//outputImage = poDriver->Create(imageFile.c_str(), xSize, ySize, numBands, GDT_Float32, poDriver->GetMetadata());
			outputImage = poDriver->Create(imageFile.c_str(), xSize, ySize, numBands, GDT_Float32, papszOptions);
			if(outputImage == NULL)
			{
				throw RSGISImageException("Image could not be created.");
			}
			outputImage->SetGeoTransform(transformation);
			outputImage->SetProjection(projection.c_str());
			
			imgData = (float *) CPLMalloc(sizeof(float)*xSize);
			for(int i = 0; i < xSize; i++)
			{
				imgData[i] = value;
			}
			
			/*cout << "Created Image: \n";
             cout << "xSize = " << xSize << endl;
             cout << "ySize = " << ySize << endl;*/
			cout << "Started (total " << numBands << ")." << flush;
			for(int n = 1; n <= numBands; n++)
			{
				cout << "." << n << "." << flush;
				imgBand = outputImage->GetRasterBand(n);
                imgBand->SetDescription(bandNames.at(n-1).c_str());
				for(int i = 0; i < ySize; i++)
				{
					imgBand->RasterIO(GF_Write, 0, i, xSize, 1, imgData, xSize, 1, GDT_Float32, 0, 0);
				}
			}
			cout << ". Complete\n";
		}
		catch(RSGISImageBandException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
			if(imgData != NULL)
			{
				delete imgData;
			}
			throw e;
		}
		catch(RSGISImageException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
			if(imgData != NULL)
			{
				delete imgData;
			}
			throw e;
		}
		
		if(imgData != NULL)
		{
			delete imgData;
		}
		
		return outputImage;
    }
	
	GDALDataset* RSGISImageUtils::createBlankImage(string imageFile, Envelope extent, double resolution, int numBands, string projection, float value) throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDriver *poDriver = NULL;
		GDALRasterBand *imgBand = NULL;
		GDALDataset *outputImage = NULL;
		
		float *imgData = NULL;
		double *transformation = NULL;
		int xSize = 0;
		int ySize = 0;
		
		if(resolution <= 0)
		{
			throw RSGISImageException("The resolution of the image needs to be > 0.");
		}
		if(numBands <= 0)
		{
			throw RSGISImageException("The number of bands needs to be > 0.");
		}
		
		try
		{
			xSize = ceil((extent.getMaxX() - extent.getMinX())/resolution);
			ySize = ceil((extent.getMaxY() - extent.getMinY())/resolution);
			
			if(xSize <= 0)
			{
				throw RSGISImageException("The image needs to have a xSize > 0");
			}
			if(ySize <= 0)
			{
				throw RSGISImageException("The image needs to have a ySize > 0");
			}
			
			transformation = new double[6];
			transformation[0] = extent.getMinX();
			transformation[1] = resolution;
			transformation[2] = 0;
			transformation[3] = extent.getMaxY();
			transformation[4] = 0;
			transformation[5] = resolution * (-1);
			
			poDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(poDriver == NULL)
			{
				throw RSGISImageException("ENVI image driver is not available.");
			}
			
			// Create new file.
			char **papszOptions = poDriver->GetMetadata();
			/*papszOptions = CSLSetNameValue (papszOptions, "DMD_LONGNAME", NULL);
			papszOptions = CSLSetNameValue (papszOptions, "DMD_HELPTOPIC", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_EXTENSION", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONDATATYPES", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONOPTIONLIST", NULL); 
			papszOptions = CSLSetNameValue (papszOptions, "DCAP_VIRTUALIO", NULL); 
			papszOptions = CSLSetNameValue (papszOptions, "DCAP_CREATE", NULL); */
			
			//outputImage = poDriver->Create(imageFile.c_str(), xSize, ySize, numBands, GDT_Float32, poDriver->GetMetadata());
			outputImage = poDriver->Create(imageFile.c_str(), xSize, ySize, numBands, GDT_Float32, papszOptions);
			
			if(outputImage == NULL)
			{
				throw RSGISImageException("Image could not be created.");
			}
			outputImage->SetGeoTransform(transformation);
			outputImage->SetProjection(projection.c_str());
			
			imgData = (float *) CPLMalloc(sizeof(float)*xSize);
			for(int i = 0; i < xSize; i++)
			{
				imgData[i] = value;
			}
			
			cout << "Created Image: \n";
			cout << "xSize = " << xSize << endl;
			cout << "ySize = " << ySize << endl;
			
			for(int n = 1; n <= numBands; n++)
			{
				cout << "Zeroing band " << n << " of " << numBands << endl;
				imgBand = outputImage->GetRasterBand(n);
				for(int i = 0; i < ySize; i++)
				{
					imgBand->RasterIO(GF_Write, 0, i, xSize, 1, imgData, xSize, 1, GDT_Float32, 0, 0);
				}
			}
		}
		catch(RSGISImageBandException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
			if(imgData != NULL)
			{
				delete imgData;
			}
			throw e;
		}
		catch(RSGISImageException e)
		{
			if(transformation != NULL)
			{
				delete transformation;
			}
			if(imgData != NULL)
			{
				delete imgData;
			}
			throw e;
		}
		
		if(transformation != NULL)
		{
			delete transformation;
		}
		if(imgData != NULL)
		{
			delete imgData;
		}
		
		return outputImage;
	}
	
	void RSGISImageUtils::exportImageBands(string imageFile, string outputFilebase, string format) throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDataset *dataset = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALRasterBand *inputImgBand = NULL;
		GDALRasterBand *outputImgBand = NULL;
		GDALDataset *outputImage = NULL;
		string outImageFile = "";
		stringstream *outStrStream;
		double *transformation = new double[6];
		float *imageData = NULL;
		char **gdalDriverMetaInfo;
		
		try
		{
			cout << imageFile << endl;
			dataset = (GDALDataset *) GDALOpenShared(imageFile.c_str(), GA_ReadOnly);
			if(dataset == NULL)
			{
				string message = string("Could not open image ") + imageFile;
				throw RSGISImageException(message.c_str());
			}
			
			gdalDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
			if(gdalDriver == NULL)
			{
				string message = format + string(" image driver is not available.");
				throw RSGISImageException(message.c_str());
			}
			
			gdalDriverMetaInfo = gdalDriver->GetMetadata();
			if(CSLFetchBoolean(gdalDriverMetaInfo, GDAL_DCAP_CREATE, FALSE ))
			{
				cout << "Driver for " << format << " supports CreateCopy\n";
			}
			else
			{
				throw RSGISImageException("Image driver does not support create. Therefore cannot create file of this type.");
			}
			
			
			int numBands = dataset->GetRasterCount();
			int xSize = dataset->GetRasterXSize();
			int ySize = dataset->GetRasterYSize();
			dataset->GetGeoTransform(transformation);
			imageData = (float *) CPLMalloc(sizeof(float)*xSize);
			
			for(int i = 1; i <= numBands; i++)
			{
				cout << "Outputting band " << i << " of " << numBands << endl;
				outStrStream = new stringstream();
				*outStrStream << outputFilebase << "_b" << i << ".tif";
				outImageFile = outStrStream->str();
				cout << "File: " << outImageFile << endl;
				
				char **papszOptions = gdalDriver->GetMetadata();
				/*papszOptions = CSLSetNameValue (papszOptions, "DMD_LONGNAME", NULL);
				papszOptions = CSLSetNameValue (papszOptions, "DMD_HELPTOPIC", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_EXTENSION", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONDATATYPES", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONOPTIONLIST", NULL); 
				papszOptions = CSLSetNameValue (papszOptions, "DCAP_VIRTUALIO", NULL); 
				papszOptions = CSLSetNameValue (papszOptions, "DCAP_CREATE", NULL); */
				
				//outputImage = gdalDriver->Create(outImageFile.c_str(), xSize, ySize, 1, GDT_Float32, gdalDriver->GetMetadata());
				outputImage = gdalDriver->Create(outImageFile.c_str(), xSize, ySize, 1, GDT_Float32, papszOptions);
				
				if(outputImage == NULL)
				{
					throw RSGISImageException("Image could not be created.");
				}
				outputImage->SetGeoTransform(transformation);
				outputImage->SetProjection(dataset->GetProjectionRef());
				
				inputImgBand = dataset->GetRasterBand(i);
				outputImgBand = outputImage->GetRasterBand(1);
				for(int j = 0; j < ySize; j++)
				{
					inputImgBand->RasterIO(GF_Read, 0, j, xSize, 1, imageData, xSize, 1, GDT_Float32, 0, 0);
					outputImgBand->RasterIO(GF_Write, 0, j, xSize, 1, imageData, xSize, 1, GDT_Float32, 0, 0);
				}
				GDALClose(outputImage);
				delete outStrStream;
			}
			GDALClose(dataset);
		}
		catch(RSGISImageException e)
		{
			
			throw e;
		}
	}
	
	void RSGISImageUtils::exportImageStack(string *inputImages, string *outputImages, string outputFormat, int numImages)  throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDataset **inDatasets = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand *inputRasterBand = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		
		float *data = NULL;
		double *gdalTranslation = new double[6];
		int **dsOffsets = new int*[numImages];
		for(int i = 0; i < numImages; i++)
		{
			dsOffsets[i] = new int[2];
		}
		int stackHeight = 0;
		int stackWidth = 0;
		int numOutBands = 0;
		
		try
		{
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
			if(gdalDriver == NULL)
			{
				string message = string("Driver for ") + outputFormat + string(" does not exist\n");
				throw RSGISImageException(message.c_str());
			}
			
			inDatasets = new GDALDataset*[numImages];
			for(int i = 0; i < numImages; i++)
			{
				cout << inputImages[i] << endl;
				inDatasets[i] = (GDALDataset *) GDALOpenShared(inputImages[i].c_str(), GA_ReadOnly);
				if(inDatasets[i] == NULL)
				{
					string message = string("Could not open image ") + inputImages[i];
					throw RSGISImageException(message.c_str());
				}
			}
			
			// Find image overlap
			this->getImageOverlap(inDatasets, numImages, dsOffsets, &stackWidth, &stackHeight, gdalTranslation);
			
			cout << "Stack Height = " << stackHeight << endl;
			cout << "Stack Width = " << stackWidth << endl;
			
			data = (float *) CPLMalloc(sizeof(float)*stackWidth);
			
			for(int i = 0; i < numImages; i++)
			{
				cout << "Converting image " << inputImages[i] << endl;
				numOutBands = inDatasets[i]->GetRasterCount();
				char **papszOptions = gdalDriver->GetMetadata();
				/*papszOptions = CSLSetNameValue (papszOptions, "DMD_LONGNAME", NULL);
				papszOptions = CSLSetNameValue (papszOptions, "DMD_HELPTOPIC", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_EXTENSION", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONDATATYPES", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONOPTIONLIST", NULL); 
				papszOptions = CSLSetNameValue (papszOptions, "DCAP_VIRTUALIO", NULL); 
				papszOptions = CSLSetNameValue (papszOptions, "DCAP_CREATE", NULL); */
				
				//outputImageDS = gdalDriver->Create(outputImages[i].c_str(), stackWidth, stackHeight, numOutBands, GDT_Float32, gdalDriver->GetMetadata());
				outputImageDS = gdalDriver->Create(outputImages[i].c_str(), stackWidth, stackHeight, numOutBands, GDT_Float32, papszOptions);
				
				outputImageDS->SetGeoTransform(gdalTranslation);
				outputImageDS->SetProjection(inDatasets[0]->GetProjectionRef());
				
				for(int n = 1; n <= numOutBands; n++)
				{
					cout << "Image Band " << n << " of " << numOutBands << endl;
					
					inputRasterBand = inDatasets[i]->GetRasterBand(n);
					outputRasterBand = outputImageDS->GetRasterBand(n);
					
					int feedback = stackHeight/10;
					int feedbackCounter = 0;
					cout << "Started" << flush;
					
					for(int m = 0; m < stackHeight; m++)
					{
						if((m % feedback) == 0)
						{
							cout << ".." << feedbackCounter << ".." << flush;
							feedbackCounter = feedbackCounter + 10;
						}
						inputRasterBand->RasterIO(GF_Read, dsOffsets[i][0], (dsOffsets[i][1]+m), stackWidth, 1, data, stackWidth, 1, GDT_Float32, 0, 0);						
						outputRasterBand->RasterIO(GF_Write, 0, m, stackWidth, 1, data, stackWidth, 1, GDT_Float32, 0, 0);
					}
					cout << " Complete.\n";
				}

				GDALClose(outputImageDS);
			}
			
			for(int i = 0; i < numImages; i++)
			{
				GDALClose(inDatasets[i]);
			}
			delete inDatasets;
			delete data;
			delete gdalTranslation;
			
		}
		catch(RSGISImageException e)
		{
			throw e;
		}
	}
	
	void RSGISImageUtils::exportImageStackWithMask(string *inputImages, string *outputImages, string imageMask, string outputFormat, int numImages, float maskValue)  throw(RSGISImageException, RSGISImageBandException)
	{
		GDALAllRegister();
		GDALDataset **inDatasets = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALDataset *outputImageDS = NULL;
		GDALRasterBand *imageMaskBand = NULL;
		GDALRasterBand *inputRasterBand = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		
		float *data = NULL;
		float *mask = NULL;
		double *gdalTranslation = new double[6];
		int **dsOffsets = NULL;
		int stackHeight = 0;
		int stackWidth = 0;
		int numOutBands = 0;
		numImages++; //include image mask
		
		try
		{
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
			if(gdalDriver == NULL)
			{
				string message = string("Driver for ") + outputFormat + string(" does not exist\n");
				throw RSGISImageException(message.c_str());
			}
			
			inDatasets = new GDALDataset*[numImages];
			cout << imageMask << endl;
			inDatasets[0] = (GDALDataset *) GDALOpenShared(imageMask.c_str(), GA_ReadOnly);
			if(inDatasets[0] == NULL)
			{
				string message = string("Could not open image ") + imageMask;
				throw RSGISImageException(message.c_str());
			}
			for(int i = 1; i < numImages; i++)
			{
				cout << inputImages[i-1] << endl;
				inDatasets[i] = (GDALDataset *) GDALOpenShared(inputImages[i-1].c_str(), GA_ReadOnly);
				if(inDatasets[i] == NULL)
				{
					string message = string("Could not open image ") + inputImages[i-1];
					throw RSGISImageException(message.c_str());
				}
			}
						
			dsOffsets = new int*[numImages];
			for(int i = 0; i < numImages; i++)
			{
				dsOffsets[i] = new int[2];
			}
			
			// Find image overlap
			this->getImageOverlap(inDatasets, numImages, dsOffsets, &stackWidth, &stackHeight, gdalTranslation);
			
			cout << "Stack Height = " << stackHeight << endl;
			cout << "Stack Width = " << stackWidth << endl;
			
			data = (float *) CPLMalloc(sizeof(float)*stackWidth);
			mask = (float *) CPLMalloc(sizeof(float)*stackWidth);
			imageMaskBand = inDatasets[0]->GetRasterBand(1);
			for(int i = 1; i < numImages; i++)
			{
				cout << "Converting image " << inputImages[i-1] << endl;
				numOutBands = inDatasets[i]->GetRasterCount();
				char **papszOptions = gdalDriver->GetMetadata();
				/*papszOptions = CSLSetNameValue (papszOptions, "DMD_LONGNAME", NULL);
				papszOptions = CSLSetNameValue (papszOptions, "DMD_HELPTOPIC", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_EXTENSION", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONDATATYPES", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONOPTIONLIST", NULL); 
				papszOptions = CSLSetNameValue (papszOptions, "DCAP_VIRTUALIO", NULL); 
				papszOptions = CSLSetNameValue (papszOptions, "DCAP_CREATE", NULL); */
				
				//outputImageDS = gdalDriver->Create(outputImages[i-1].c_str(), stackWidth, stackHeight, numOutBands, GDT_Float32, gdalDriver->GetMetadata());
				outputImageDS = gdalDriver->Create(outputImages[i-1].c_str(), stackWidth, stackHeight, numOutBands, GDT_Float32, papszOptions);
				outputImageDS->SetGeoTransform(gdalTranslation);
				outputImageDS->SetProjection(inDatasets[0]->GetProjectionRef());
				
				for(int n = 1; n <= numOutBands; n++)
				{
					cout << "Image Band " << n << " of " << numOutBands << endl;
					inputRasterBand = inDatasets[i]->GetRasterBand(n);
					outputRasterBand = outputImageDS->GetRasterBand(n);
					
					int feedback = stackHeight/10;
					int feedbackCounter = 0;
					cout << "Started" << flush;
					
					for(int m = 0; m < stackHeight; m++)
					{
						if((m % feedback) == 0)
						{
							cout << ".." << feedbackCounter << ".." << flush;
							feedbackCounter = feedbackCounter + 10;
						}
						
						inputRasterBand->RasterIO(GF_Read, dsOffsets[i][0], (dsOffsets[i][1]+m), stackWidth, 1, data, stackWidth, 1, GDT_Float32, 0, 0);
						imageMaskBand->RasterIO(GF_Read, dsOffsets[0][0], (dsOffsets[0][1]+m), stackWidth, 1, mask, stackWidth, 1, GDT_Float32, 0, 0);
						
						for(int k = 0; k < stackWidth; k++)
						{
							if(mask[k] == 0)
							{
								data[k] = maskValue;
							}
						}
						
						outputRasterBand->RasterIO(GF_Write, 0, m, stackWidth, 1, data, stackWidth, 1, GDT_Float32, 0, 0);
					}
					cout << " Complete.\n";
				}
				GDALClose(outputImageDS);
			}
			for(int i = 0; i < numImages; i++)
			{
				GDALClose(inDatasets[i]);
			}
			delete inDatasets;
			delete data;
			delete gdalTranslation;
		}
		catch(RSGISImageException e)
		{
			throw e;
		}
	}
	
	void RSGISImageUtils::convertImageFileFormat(string inputImage, string outputImage, string outputImageFormat, bool projFromImage, string wktProjStr)
	{
		GDALAllRegister();
		GDALDataset *inDataset = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALDataset *outDataset = NULL;
		
		GDALRasterBand *inputRasterBand = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		
		float *data = NULL;
		
		char **gdalDriverMetaInfo;
		int numOutBands = 0;
		int width = 0;
		int height = 0;
		
		try
		{
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName(outputImageFormat.c_str());
			if(gdalDriver == NULL)
			{
				string message = string("Driver for ") + outputImageFormat + string(" does not exist\n");
				throw RSGISImageException(message.c_str());
			}
			
			gdalDriverMetaInfo = gdalDriver->GetMetadata();
			if(CSLFetchBoolean(gdalDriverMetaInfo, GDAL_DCAP_CREATECOPY, FALSE ))
			{
				cout << "Driver for " << outputImageFormat << " supports CreateCopy\n";
			}
			else
			{
				throw RSGISImageException("Image driver does not support image copy. Therefore cannot create file of this type.");
			}
			
			cout << "Openning image " << inputImage << endl;
			inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
			if(inDataset == NULL)
			{
				string message = string("Could not open image ") + inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			cout << "Creating image " << outputImage << endl;
			outDataset = gdalDriver->CreateCopy(outputImage.c_str(), inDataset, FALSE, NULL, NULL, NULL);
			if(outDataset == NULL)
			{
				string message = string("Could not open image ") + outputImage;
				throw RSGISImageException(outputImage.c_str());
			}
            
            if(!projFromImage)
            {
                outDataset->SetProjection(wktProjStr.c_str());
            }
			
			width = inDataset->GetRasterXSize();
			height = inDataset->GetRasterYSize();
			data = (float *) CPLMalloc(sizeof(float)*width);
			cout << "Image size [" << width << "," << height << "]\n";
			
			numOutBands = inDataset->GetRasterCount();
			for(int n = 1; n <= numOutBands; n++)
			{
				cout << "Image Band " << n << " of " << numOutBands << endl;
				inputRasterBand = inDataset->GetRasterBand(n);
				outputRasterBand = outDataset->GetRasterBand(n);
				
				int feedback = height/10;
				int feedbackCounter = 0;
				cout << "Started" << flush;
				
				for(int m = 0; m < height; m++)
				{
					if((m % feedback) == 0)
					{
						cout << ".." << feedbackCounter << ".." << flush;
						feedbackCounter = feedbackCounter + 10;
					}
					
					inputRasterBand->RasterIO(GF_Read, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
					outputRasterBand->RasterIO(GF_Write, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
				}
				cout << " Complete.\n";
			}
			
			GDALClose(outDataset);
			GDALClose(inDataset);
			delete data;
		}
		catch(RSGISImageException e)
		{
			throw e;
		}
	}
	
	float** RSGISImageUtils::getImageDataBlock(GDALDataset *dataset, int *dsOffsets, unsigned int width, unsigned int height, unsigned int *numVals)
	{
		//cout << "Reading data block\n";
		
		//cout << "width = " << width << endl;
		//cout << "height = " << height << endl;
		//cout << "Offset = [" << dsOffsets[0] << "," << dsOffsets[1] << "]\n";
		
		unsigned int numImageBands = dataset->GetRasterCount();
		*numVals = width*height;
		
		GDALRasterBand **rasterBands = new GDALRasterBand*[numImageBands];
		float **imgData = new float*[numImageBands];
		float **outVals = new float*[numImageBands];
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			outVals[i] = new float[*numVals];
			rasterBands[i] = dataset->GetRasterBand(i+1);
			imgData[i] = (float *) CPLMalloc(sizeof(float)*width);
		}
		
		unsigned int outValCounter = 0;
		for(unsigned int i = 0; i < height; ++i)
		{
			for(unsigned k = 0; k < numImageBands; ++k)
			{
				rasterBands[k]->RasterIO(GF_Read, dsOffsets[0], dsOffsets[1]+i, width, 1, imgData[k], width, 1, GDT_Float32, 0, 0);
			}
			
			for(unsigned int j = 0; j < width; ++j)
			{
				for(unsigned k = 0; k < numImageBands; ++k)
				{
					outVals[k][outValCounter] = imgData[k][j];
				}
				++outValCounter;
			}
		}
		
		for(unsigned int i = 0; i < numImageBands; ++i)
		{
			CPLFree(imgData[i]);
		}
		delete[] imgData;
		delete[]rasterBands;
		
		//cout << "Read data block\n";
		
		return outVals;
	}
	
	void RSGISImageUtils::copyImageRemoveSpatialReference(string inputImage, string outputImage)throw(RSGISImageException)
	{
		GDALAllRegister();
		GDALDataset *inDataset = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALDataset *outDataset = NULL;
		
		GDALRasterBand *inputRasterBand = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		
		float *data = NULL;
		
		int numOutBands = 0;
		int width = 0;
		int height = 0;
		double *transformation = NULL;
		
		try
		{
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(gdalDriver == NULL)
			{
				string message = string("Driver for ENVI does not exist\n");
				throw RSGISImageException(message.c_str());
			}
			
			
			cout << "Openning image " << inputImage << endl;
			inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
			if(inDataset == NULL)
			{
				string message = string("Could not open image ") + inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			numOutBands = inDataset->GetRasterCount();
			
			width = inDataset->GetRasterXSize();
			height = inDataset->GetRasterYSize();
			
			transformation = new double[6];
			transformation[0] = 0;
			transformation[1] = 1;
			transformation[2] = 0;
			transformation[3] = 0;
			transformation[4] = 0;
			transformation[5] = -1;
			
			cout << "Creating image " << outputImage << endl;
			char **papszOptions = gdalDriver->GetMetadata();
			/*papszOptions = CSLSetNameValue (papszOptions, "DMD_LONGNAME", NULL);
			papszOptions = CSLSetNameValue (papszOptions, "DMD_HELPTOPIC", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_EXTENSION", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONDATATYPES", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONOPTIONLIST", NULL); 
			papszOptions = CSLSetNameValue (papszOptions, "DCAP_VIRTUALIO", NULL); 
			papszOptions = CSLSetNameValue (papszOptions, "DCAP_CREATE", NULL); */
			
			//outDataset = gdalDriver->Create(outputImage.c_str(), width, height, numOutBands, GDT_Float32, gdalDriver->GetMetadata());
			outDataset = gdalDriver->Create(outputImage.c_str(), width, height, numOutBands, GDT_Float32, papszOptions);
			
			if(outDataset == NULL)
			{
				string message = string("Could not open image ") + outputImage;
				throw RSGISImageException(outputImage.c_str());
			}
			
			outDataset->SetGeoTransform(transformation);
			outDataset->SetProjection("");
			
			
			data = (float *) CPLMalloc(sizeof(float)*width);
			cout << "Image size [" << width << "," << height << "]\n";
			
			
			for(int n = 1; n <= numOutBands; n++)
			{
				cout << "Image Band " << n << " of " << numOutBands << endl;
				inputRasterBand = inDataset->GetRasterBand(n);
				outputRasterBand = outDataset->GetRasterBand(n);
				
				int feedback = height/10;
				int feedbackCounter = 0;
				cout << "Started" << flush;
				
				for(int m = 0; m < height; m++)
				{
					if((m % feedback) == 0)
					{
						cout << ".." << feedbackCounter << ".." << flush;
						feedbackCounter = feedbackCounter + 10;
					}
					
					inputRasterBand->RasterIO(GF_Read, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
					outputRasterBand->RasterIO(GF_Write, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
				}
				cout << " Complete.\n";
			}
			
			GDALClose(outDataset);
			GDALClose(inDataset);
			delete data;
			delete transformation;
		}
		catch(RSGISImageException e)
		{
			throw e;
		}
	}

	void RSGISImageUtils::copyImageDefiningSpatialReference(string inputImage, string outputImage, string proj, double tlX, double tlY, float xRes, float yRes)throw(RSGISImageException)
	{
		GDALAllRegister();
		GDALDataset *inDataset = NULL;
		GDALDriver *gdalDriver = NULL;
		GDALDataset *outDataset = NULL;
		
		GDALRasterBand *inputRasterBand = NULL;
		GDALRasterBand *outputRasterBand = NULL;
		
		float *data = NULL;
		
		int numOutBands = 0;
		int width = 0;
		int height = 0;
		double *transformation = NULL;
		
		try
		{
			// Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(gdalDriver == NULL)
			{
				string message = string("Driver for ENVI does not exist\n");
				throw RSGISImageException(message.c_str());
			}
			
			
			cout << "Openning image " << inputImage << endl;
			inDataset = (GDALDataset *) GDALOpenShared(inputImage.c_str(), GA_ReadOnly);
			if(inDataset == NULL)
			{
				string message = string("Could not open image ") + inputImage;
				throw RSGISImageException(message.c_str());
			}
			
			numOutBands = inDataset->GetRasterCount();
			
			width = inDataset->GetRasterXSize();
			height = inDataset->GetRasterYSize();
			
			transformation = new double[6];
			transformation[0] = tlX;
			transformation[1] = xRes;
			transformation[2] = 0;
			transformation[3] = tlY;
			transformation[4] = 0;
			transformation[5] = yRes;
			
			cout << "Creating image " << outputImage << endl;
			
			char **papszOptions = gdalDriver->GetMetadata();
			/*papszOptions = CSLSetNameValue (papszOptions, "DMD_LONGNAME", NULL);
			papszOptions = CSLSetNameValue (papszOptions, "DMD_HELPTOPIC", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_EXTENSION", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONDATATYPES", NULL); 	
			papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONOPTIONLIST", NULL); 
			papszOptions = CSLSetNameValue (papszOptions, "DCAP_VIRTUALIO", NULL); 
			papszOptions = CSLSetNameValue (papszOptions, "DCAP_CREATE", NULL); */
			
			//outDataset = gdalDriver->Create(outputImage.c_str(), width, height, numOutBands, GDT_Float32, gdalDriver->GetMetadata());
			outDataset = gdalDriver->Create(outputImage.c_str(), width, height, numOutBands, GDT_Float32, papszOptions);
			if(outDataset == NULL)
			{
				string message = string("Could not open image ") + outputImage;
				throw RSGISImageException(outputImage.c_str());
			}
			
			outDataset->SetGeoTransform(transformation);
			cout << "Defining projections as :\'" << proj << "\'\n";
			char **wktInSpatialRef = new char*[1];
			wktInSpatialRef[0] = const_cast<char *>(proj.c_str());
			OGRSpatialReference ogrSpatial = OGRSpatialReference();
			ogrSpatial.importFromWkt(wktInSpatialRef);
			
			char **wktspatialref = new char*[1];
			wktspatialref[0] = new char[10000];
			ogrSpatial.exportToWkt(wktspatialref);			
			
			CPLErr errorCode = outDataset->SetProjection(wktspatialref[0]);
			if(errorCode == CE_Failure)
			{
				throw RSGISImageException("Projection could not be defined.");
			}
			
			char **proj4spatialref = new char*[1];
			proj4spatialref[0] = new char[1000];
			ogrSpatial.exportToProj4(proj4spatialref);
			cout << "As Proj4: \'" << proj4spatialref[0] << "\'" << endl;
			
			OGRFree(wktspatialref);
			OGRFree(wktInSpatialRef);
			OGRFree(proj4spatialref);
			
			data = (float *) CPLMalloc(sizeof(float)*width);
			cout << "Image size [" << width << "," << height << "]\n";
			
			
			for(int n = 1; n <= numOutBands; n++)
			{
				cout << "Image Band " << n << " of " << numOutBands << endl;
				inputRasterBand = inDataset->GetRasterBand(n);
				outputRasterBand = outDataset->GetRasterBand(n);
				
				int feedback = height/10;
				int feedbackCounter = 0;
				cout << "Started" << flush;
				
				for(int m = 0; m < height; m++)
				{
					if((m % feedback) == 0)
					{
						cout << ".." << feedbackCounter << ".." << flush;
						feedbackCounter = feedbackCounter + 10;
					}
					
					inputRasterBand->RasterIO(GF_Read, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
					outputRasterBand->RasterIO(GF_Write, 0, m, width, 1, data, width, 1, GDT_Float32, 0, 0);
				}
				cout << " Complete.\n";
			}
			
			GDALClose(outDataset);
			GDALClose(inDataset);
			delete data;
			delete transformation;
		}
		catch(RSGISImageException e)
		{
			throw e;
		}
	}
    
    void RSGISImageUtils::createImageSlices(GDALDataset *dataset, string outputImageBase) throw(RSGISImageException)
    {
        RSGISMathsUtils mathUtils;
        
        GDALDriver *gdalDriver = NULL;
        GDALDataset *outDataset = NULL;
        GDALRasterBand *outputRasterBand = NULL;
        GDALRasterBand **inputBands = NULL;
        float *data = NULL;
        
        try
        {
            // Create new Image
			gdalDriver = GetGDALDriverManager()->GetDriverByName("ENVI");
			if(gdalDriver == NULL)
			{
				string message = string("Driver for ENVI does not exist\n");
				throw RSGISImageException(message.c_str());
			}
            
            unsigned int width = dataset->GetRasterXSize();
            unsigned int height = dataset->GetRasterCount();
            
            data = (float *) CPLMalloc(sizeof(float)*width);
            
            inputBands = new GDALRasterBand*[dataset->GetRasterCount()];
            for(int n = 0; n < dataset->GetRasterCount(); ++n)
            {
                inputBands[n] = dataset->GetRasterBand(n+1);
            }
            
            for(int i = 0; i < dataset->GetRasterYSize(); ++i)
            {
                string outputImage = outputImageBase + mathUtils.inttostring(i) + string(".env");
				char **papszOptions = gdalDriver->GetMetadata();
				/*papszOptions = CSLSetNameValue (papszOptions, "DMD_LONGNAME", NULL);
				papszOptions = CSLSetNameValue (papszOptions, "DMD_HELPTOPIC", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_EXTENSION", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONDATATYPES", NULL); 	
				papszOptions = CSLSetNameValue (papszOptions, "DMD_CREATIONOPTIONLIST", NULL); 
				papszOptions = CSLSetNameValue (papszOptions, "DCAP_VIRTUALIO", NULL); 
				papszOptions = CSLSetNameValue (papszOptions, "DCAP_CREATE", NULL); */
				
                //outDataset = gdalDriver->Create(outputImage.c_str(), width, height, 1, GDT_Float32, gdalDriver->GetMetadata());
				outDataset = gdalDriver->Create(outputImage.c_str(), width, height, 1, GDT_Float32, papszOptions);
                if(outDataset == NULL)
                {
                    string message = string("Could not open image ") + outputImage;
                    throw RSGISImageException(outputImage.c_str());
                }
                outputRasterBand = outDataset->GetRasterBand(1);
                
                for(int j = 0; j < dataset->GetRasterCount(); ++j)
                {
                    inputBands[j]->RasterIO(GF_Read, 0, i, width, 1, data, width, 1, GDT_Float32, 0, 0);
                    outputRasterBand->RasterIO(GF_Write, 0, ((dataset->GetRasterCount()-j)-1), width, 1, data, width, 1, GDT_Float32, 0, 0);
                }
                
                GDALClose(outDataset);
            }
            
            delete[] inputBands;
            delete[] data;
            
        }
        catch(RSGISImageBandException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::copyFloatGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException)
    {
        try
        {
            // Change dimensions are the same.
            if(inData->GetRasterXSize() != outData->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(inData->GetRasterYSize() != outData->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            if(inData->GetRasterCount() != outData->GetRasterCount())
            {
                throw RSGISImageException("Number of bands are not the same");
            }
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            
            GDALRasterBand **inputRasterBands = new GDALRasterBand*[numBands];
            GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
            float *data = new float[width];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                inputRasterBands[n] = inData->GetRasterBand(n+1);
                outputRasterBands[n] = outData->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    inputRasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, data, width, 1, GDT_Float32, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, data, width, 1, GDT_Float32, 0, 0);
                }
            }
                
            delete[] inputRasterBands;
            delete[] outputRasterBands;
            delete[] data;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::copyIntGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException)
    {
        try
        {
            // Change dimensions are the same.
            if(inData->GetRasterXSize() != outData->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(inData->GetRasterYSize() != outData->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            if(inData->GetRasterCount() != outData->GetRasterCount())
            {
                throw RSGISImageException("Number of bands are not the same");
            }
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            
            GDALRasterBand **inputRasterBands = new GDALRasterBand*[numBands];
            GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
            int *data = new int[width];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                inputRasterBands[n] = inData->GetRasterBand(n+1);
                outputRasterBands[n] = outData->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    inputRasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, data, width, 1, GDT_Int32, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, data, width, 1, GDT_Int32, 0, 0);
                }
            }
            
            delete[] inputRasterBands;
            delete[] outputRasterBands;
            delete[] data;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::copyUIntGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException)
    {
        try
        {
            // Change dimensions are the same.
            if(inData->GetRasterXSize() != outData->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(inData->GetRasterYSize() != outData->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            if(inData->GetRasterCount() != outData->GetRasterCount())
            {
                throw RSGISImageException("Number of bands are not the same");
            }
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            
            GDALRasterBand **inputRasterBands = new GDALRasterBand*[numBands];
            GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
            unsigned int *data = new unsigned int[width];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                inputRasterBands[n] = inData->GetRasterBand(n+1);
                outputRasterBands[n] = outData->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    inputRasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, data, width, 1, GDT_UInt32, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, data, width, 1, GDT_UInt32, 0, 0);
                }
            }
            
            delete[] inputRasterBands;
            delete[] outputRasterBands;
            delete[] data;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::copyFloat32GDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException)
    {
        try
        {
            // Change dimensions are the same.
            if(inData->GetRasterXSize() != outData->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(inData->GetRasterYSize() != outData->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            if(inData->GetRasterCount() != outData->GetRasterCount())
            {
                throw RSGISImageException("Number of bands are not the same");
            }
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            
            GDALRasterBand **inputRasterBands = new GDALRasterBand*[numBands];
            GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
            float *data = new float[width];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                inputRasterBands[n] = inData->GetRasterBand(n+1);
                outputRasterBands[n] = outData->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    inputRasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, data, width, 1, GDT_Float32, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, data, width, 1, GDT_Float32, 0, 0);
                }
            }
            
            delete[] inputRasterBands;
            delete[] outputRasterBands;
            delete[] data;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::copyByteGDALDataset(GDALDataset *inData, GDALDataset *outData) throw(RSGISImageException)
    {
        try
        {
            // Change dimensions are the same.
            if(inData->GetRasterXSize() != outData->GetRasterXSize())
            {
                throw RSGISImageException("Widths are not the same");
            }
            if(inData->GetRasterYSize() != outData->GetRasterYSize())
            {
                throw RSGISImageException("Heights are not the same");
            }
            if(inData->GetRasterCount() != outData->GetRasterCount())
            {
                throw RSGISImageException("Number of bands are not the same");
            }
            
            unsigned long width = inData->GetRasterXSize();
            unsigned long height = inData->GetRasterYSize();
            unsigned int numBands = inData->GetRasterCount();
            
            GDALRasterBand **inputRasterBands = new GDALRasterBand*[numBands];
            GDALRasterBand **outputRasterBands = new GDALRasterBand*[numBands];
            int *data = new int[width];
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                inputRasterBands[n] = inData->GetRasterBand(n+1);
                outputRasterBands[n] = outData->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    inputRasterBands[n]->RasterIO(GF_Read, 0, y, width, 1, data, width, 1, GDT_Byte, 0, 0);
                    outputRasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, data, width, 1, GDT_Byte, 0, 0);
                }
            }
            
            delete[] inputRasterBands;
            delete[] outputRasterBands;
            delete[] data;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::zerosUIntGDALDataset(GDALDataset *data) throw(RSGISImageException)
    {
        try
        {
            unsigned long width = data->GetRasterXSize();
            unsigned long height = data->GetRasterYSize();
            unsigned int numBands = data->GetRasterCount();
            
            GDALRasterBand **rasterBands = new GDALRasterBand*[numBands];
            unsigned int *dataVals = new unsigned int[width];
            
            for(unsigned int i = 0; i < width; ++i)
            {
                dataVals[i] = 0;
            }
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                rasterBands[n] = data->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    rasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, dataVals, width, 1, GDT_UInt32, 0, 0);
                }
            }
            
            delete[] rasterBands;
            delete[] dataVals;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::zerosFloatGDALDataset(GDALDataset *data) throw(RSGISImageException)
    {
        try
        {
            unsigned long width = data->GetRasterXSize();
            unsigned long height = data->GetRasterYSize();
            unsigned int numBands = data->GetRasterCount();
            
            GDALRasterBand **rasterBands = new GDALRasterBand*[numBands];
            float *dataVals = new float[width];
            
            for(unsigned int i = 0; i < width; ++i)
            {
                dataVals[i] = 0;
            }
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                rasterBands[n] = data->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    rasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, dataVals, width, 1, GDT_Float32, 0, 0);
                }
            }
            
            delete[] rasterBands;
            delete[] dataVals;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    void RSGISImageUtils::zerosByteGDALDataset(GDALDataset *data) throw(RSGISImageException)
    {
        try
        {
            unsigned long width = data->GetRasterXSize();
            unsigned long height = data->GetRasterYSize();
            unsigned int numBands = data->GetRasterCount();
            
            GDALRasterBand **rasterBands = new GDALRasterBand*[numBands];
            int *dataVals = new int[width];
            
            for(unsigned int i = 0; i < width; ++i)
            {
                dataVals[i] = 0;
            }
            
            for(unsigned int n = 0; n < numBands; ++n)
            {
                rasterBands[n] = data->GetRasterBand(n+1);
            }
            
            for(unsigned long y = 0; y < height; ++y)
            {
                for(unsigned int n = 0; n < numBands; ++n)
                {
                    rasterBands[n]->RasterIO(GF_Write, 0, y, width, 1, dataVals, width, 1, GDT_Byte, 0, 0);
                }
            }
            
            delete[] rasterBands;
            delete[] dataVals;
        }
        catch(RSGISImageException &e)
        {
            throw e;
        }
    }
    
    GDALDataset* RSGISImageUtils::createCopy(GDALDataset *inData, string outputFilePath, string outputFormat, GDALDataType eType, bool useImgProj, string proj)throw(RSGISImageException)
    {
        unsigned long width = inData->GetRasterXSize();
        unsigned long height = inData->GetRasterYSize();
        unsigned int numBands = inData->GetRasterCount();
        
        double *gdalTranslation = new double[6];
        inData->GetGeoTransform(gdalTranslation);
        
        // Process dataset in memory
        GDALDriver *gdalDriver = NULL;
        gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
        if(gdalDriver == NULL)
        {
            delete[] gdalTranslation;
            string message = string("Driver for ") + outputFormat + string(" does not exist\n");
            throw RSGISImageException(message.c_str());
        }
        GDALDataset *dataset = gdalDriver->Create(outputFilePath.c_str(), width, height, numBands, eType, NULL);
        if(dataset == NULL)
        {
            delete[] gdalTranslation;
            string message = string("Could not create GDALDataset.");
            throw RSGISImageException(message);
        }
        
        dataset->SetGeoTransform(gdalTranslation);
        if(useImgProj)
        {
            dataset->SetProjection(inData->GetProjectionRef());
        }
        else
        {
            dataset->SetProjection(proj.c_str());
        }
        
        delete[] gdalTranslation;
        
        return dataset;

    }
    
    GDALDataset* RSGISImageUtils::createCopy(GDALDataset *inData, unsigned int numBands, string outputFilePath, string outputFormat, GDALDataType eType, bool useImgProj, string proj)throw(RSGISImageException)
    {
        unsigned long width = inData->GetRasterXSize();
        unsigned long height = inData->GetRasterYSize();
        
        double *gdalTranslation = new double[6];
        inData->GetGeoTransform(gdalTranslation);
        
        // Process dataset in memory
        GDALDriver *gdalDriver = NULL;
        gdalDriver = GetGDALDriverManager()->GetDriverByName(outputFormat.c_str());
        if(gdalDriver == NULL)
        {
            delete[] gdalTranslation;
            string message = string("Driver for ") + outputFormat + string(" does not exist\n");
            throw RSGISImageException(message.c_str());
        }
        GDALDataset *dataset = gdalDriver->Create(outputFilePath.c_str(), width, height, numBands, eType, NULL);
        if(dataset == NULL)
        {
            delete[] gdalTranslation;
            string message = string("Could not create GDALDataset.");
            throw RSGISImageException(message);
        }
        
        dataset->SetGeoTransform(gdalTranslation);
        if(useImgProj)
        {
            dataset->SetProjection(inData->GetProjectionRef());
        }
        else
        {
            dataset->SetProjection(proj.c_str());
        }
        
        delete[] gdalTranslation;
        
        return dataset;
        
    }
	
	RSGISImageUtils::~RSGISImageUtils()
	{
		
	}
	
}}
