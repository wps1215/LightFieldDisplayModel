/*
* LightFieldDisplayModel - UtilitiesBasic - Image2D
*
* Basic 2D image class.
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "Image2D.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include "tinyexr.h"


Image2D::Image2D()
	:width(0)
	,height(0)
{

}


Image2D::Image2D( const int width, const int height )
{
	Resize( width, height );
}


Vec3f& Image2D::at( const int x, const int y )
{
	const int x_clamped = std::min<int>( std::max<int>(x, 0), width-1 );
	const int y_clamped = std::min<int>( std::max<int>(y, 0), height-1 );
	return data[ x_clamped + y_clamped * width ];
}


const Vec3f& Image2D::at( const int x, const int y ) const
{
	const int x_clamped = std::min<int>( std::max<int>(x, 0), width-1 );
	const int y_clamped = std::min<int>( std::max<int>(y, 0), height-1 );
	return data[ x_clamped + y_clamped * width ];
}


void Image2D::Resize( const int width, const int height )
{
	if ( width > 0 && height > 0 )
	{
		this->width = width;
		this->height = height;
		data.resize( width*height );
		memset( &data[0], 0, sizeof(Vec3f)*width*height );
	}
}


bool Image2D::Load( const std::string& filepath )
{
	const std::string extension = filepath.substr( filepath.length() - 3 );
	if ( extension == "ppm" )
	{
		std::cout << "Loading ppm is not supported yet." << std::endl;
		return false;
	}
	else if ( extension == "exr" )
	{
		int width = 0, height = 0;
		float* rgba = nullptr;
		const char* err;

		const int ret = LoadEXR( &rgba, &width, &height, filepath.c_str(), &err );
		if ( ret != 0 ) {
			printf( "err: %s\n", err );
			//return -1;
			return false;
		}

		this->Resize( width, height );
		for ( int y = 0; y < height; ++y )
		{
			for ( int x = 0; x < width; ++x )
			{
				const int dataId = x+y*width;
				const Vec3f color( rgba[4*dataId+0], rgba[4*dataId+1], rgba[4*dataId+2] );
				this->at(x,y) = color;
			}
		}
	}
	else
	{
		std::cout << "LoadFramebuffer: unknown file format." << std::endl;
		return false;
	}
	return true;
}


bool Image2D::Save( const std::string& filepath ) const
{
	const size_t width = this->width;
	const size_t height = this->height;
	const std::string extension = filepath.substr( filepath.length()-3 );
	bool success = true;
	if ( extension == "ppm" )
	{
		std::ofstream ofs; // save the framebuffer to file
		ofs.open( filepath, std::ios::binary );
		if ( !ofs.is_open() )
		{
			success = false;
		}
		else
		{
			ofs << "P6\n" << width << " " << height << "\n255\n";
			for ( int y = 0; y < height; ++y )
			{
				for ( int x = 0; x < width; ++x )
				{
					Vec3f color = this->at( x, y );
					for ( size_t channel = 0; channel < 3; ++channel )
					{
						ofs << (char)(255 * std::max( 0.f, std::min( 1.f, color[channel] ) ));
					}
				}
			}
			ofs.close();
		}
	}
	else if ( extension == "exr" )
	{
		EXRHeader header;
		InitEXRHeader( &header );

		EXRImage image;
		InitEXRImage( &image );
		image.num_channels = 3;

		std::vector<float> images[3];
		images[0].resize( width * height );
		images[1].resize( width * height );
		images[2].resize( width * height );

		for ( int y = 0; y < height; ++y )
		{
			for ( int x = 0; x < width; ++x )
			{
				const Vec3f color = this->at(x,y);
				images[0][x+y*width] = color[0];
				images[1][x+y*width] = color[1];
				images[2][x+y*width] = color[2];
			}
		}

		float* image_ptr[3];
		image_ptr[0] = &(images[2].at(0)); // B
		image_ptr[1] = &(images[1].at(0)); // G
		image_ptr[2] = &(images[0].at(0)); // R

		image.images = (unsigned char**)image_ptr;
		image.width = (int)width;
		image.height = (int)height;

		header.num_channels = 3;
		header.channels = (EXRChannelInfo*)malloc( sizeof( EXRChannelInfo ) * header.num_channels );
		// Must be BGR(A) order, since most of EXR viewers expect this channel order.
		header.channels[0].name[0] = 'B'; header.channels[0].name[1] = '\0';
		header.channels[1].name[0] = 'G'; header.channels[1].name[1] = '\0';
		header.channels[2].name[0] = 'R'; header.channels[2].name[1] = '\0';

		header.pixel_types = (int*)malloc( sizeof( int ) * header.num_channels );
		header.requested_pixel_types = (int*)malloc( sizeof( int ) * header.num_channels );
		for ( int i = 0; i < header.num_channels; i++ ) {
			header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
			header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
		}

		const char* err;
		int ret = SaveEXRImageToFile( &image, &header, filepath.c_str(), &err );
		if ( ret != TINYEXR_SUCCESS ) {
			fprintf( stderr, "Save EXR err: %s\n", err );
			success = false;
		}
		free( header.channels );
		free( header.pixel_types );
		free( header.requested_pixel_types );
	}
	else
	{
		std::cout << "SaveFramebuffer: unknown file format." << std::endl;
		success = false;
	}
	return success;
}


void Image2D::Clear()
{
	this->data.clear();
	this->width = 0;
	this->height = 0;
}