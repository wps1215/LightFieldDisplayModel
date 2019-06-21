/*
* LightFieldDisplayModel - UtilitiesBasic - Image2D
*
* Basic 2D image class.
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#ifndef UTILITIESBASIC_IMAGE2D_H
#define UTILITIESBASIC_IMAGE2D_H

#include "geometry.h"


class Image2D
{
public:
	Image2D();
	Image2D( const int width, const int height );

	Vec3f& at( const int x, const int y );
	const Vec3f& at( const int x, const int y ) const;

	void Resize( const int width, const int height );

	size_t Width() const { return width; }
	size_t Height() const { return height; }

	bool Load( const std::string& filepath );
	bool Save( const std::string& filepath ) const;

	void Clear();

private:
	std::vector<Vec3f> data;
	size_t width;
	size_t height;
};

#endif // UTILITIESBASIC_IMAGE2D_H