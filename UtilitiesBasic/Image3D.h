/*
* LightFieldDisplayModel - UtilitiesBasic - Image3D
*
* Basic 3D image class.
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#ifndef UTILITIESBASIC_IMAGE3D_H
#define UTILITIESBASIC_IMAGE3D_H

#include "geometry.h"
#include "Image2D.h"


class Image3D
{
public:
	Image3D();
	Image3D( const int width, const int height, const int depth );

	Vec3f& at( const int x, const int y, const int z );
	const Vec3f& at( const int x, const int y, const int z ) const;

	Image2D& Layer( const int z );
	const Image2D& Layer( const int z ) const;

	void Resize( const int width, const int height, const int depth );

	size_t Width() const { return width; }
	size_t Height() const { return height; }
	size_t Depth() const { return depth; }

	bool Load( const std::string& directory, const int numImages );
	bool Save( const std::string& directory ) const;

	void Clear();

private:
	std::vector<Image2D> data;
	size_t width;
	size_t height;
	size_t depth;
};

#endif // UTILITIESBASIC_IMAGE3D_H