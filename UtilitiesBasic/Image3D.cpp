/*
* LightFieldDisplayModel - UtilitiesBasic - Image3D
*
* Basic 3D image class.
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "Image3D.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>


//const std::string images_extension = "ppm";
const std::string images_extension = "exr";



Image3D::Image3D()
	:width(0)
	,height(0)
	,depth(0)
{
}


Image3D::Image3D( const int width, const int height, const int depth )
{
	Resize( width, height, depth );
}


Vec3f& Image3D::at( const int x, const int y, const int z )
{
	return Layer(z).at(x,y);
}


const Vec3f& Image3D::at( const int x, const int y, const int z ) const
{
	return Layer(z).at(x,y);
}


Image2D& Image3D::Layer( const int z )
{
	const int z_clamped = std::min<int>( std::max<int>(z, 0), depth-1 );
	return data.at(z_clamped);
}


const Image2D& Image3D::Layer( const int z ) const
{
	const int z_clamped = std::min<int>( std::max<int>(z, 0), depth-1 );
	return data.at(z_clamped);
}


void Image3D::Resize( const int width, const int height, const int depth )
{
	if ( width > 0 && height > 0 && depth > 0 )
	{
		this->width = width;
		this->height = height;
		this->depth = depth;
		data.resize(depth);
		for ( int z = 0; z < depth; ++z )
			data.at(z).Resize( width, height );
	}
}


bool Image3D::Load( const std::string& directory, const int numImages )
{
	if ( numImages <= 0 )
		return false;
	bool success = true;
	std::stringstream stringstream;
	this->data.resize( numImages );
	int width = 0, height = 0;
	for ( int imageId = 0; imageId < numImages; ++imageId )
	{
		stringstream.str( "" );
		stringstream << directory << std::setfill('0') << std::setw(4) << imageId << "." << images_extension;
		success = success && this->data.at(imageId).Load( stringstream.str() );
		if ( imageId == 0 )
		{
			width = data.at(imageId).Width();
			height = data.at(imageId).Height();
		}
		else if ( data.at(imageId).Width() != width || data.at(imageId).Height() != height )
		{
			success = false;
		}
		if ( !success )
			break;
	}
	if ( success )
	{
		this->width = width;
		this->height = height;
		this->depth = numImages;
	}
	else
	{
		this->data.clear();
		this->width = 0;
		this->height = 0;
		this->depth = 0;
	}
	return success;
}


bool Image3D::Save( const std::string& directory ) const
{
	std::stringstream stringstream;
	const int numImages = static_cast<int>( this->depth );
	if ( depth == 0 )
	{
		return false;
	}
	bool success = true;
	for ( int imageId = 0; imageId < numImages; ++imageId )
	{
		stringstream.str( "" );
		stringstream << directory << std::setfill('0') << std::setw(4) << imageId << "." << images_extension;
		success = success && this->Layer(imageId).Save( stringstream.str() );
		if ( !success )
			break;
	}
	return success;
}


void Image3D::Clear()
{
	this->data.clear();
	this->width = 0;
	this->height = 0;
	this->depth = 0;
}