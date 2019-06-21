/*
* LightFieldDisplayModel - LightFieldProcessing - LightFieldInterpolation
*
* Interpolation of light field between different representations (e.g., HoloVizio and MultiView).
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#ifndef LIGHTFIELDPROCESSING_LIGHTFIELDINTERPOLATION_H
#define LIGHTFIELDPROCESSING_LIGHTFIELDINTERPOLATION_H

#include "HoloVizioModel.h"
#include "MultiViewModel.h"

#include "geometry.h"

class Image2D;
class Image3D;


class LightFieldInterpolation
{
public:
	LightFieldInterpolation();
	LightFieldInterpolation( const HoloVizioModel& holoVizioModel, const MultiViewModel& multiViewModel );
	~LightFieldInterpolation();

	void SetHoloVizioModel( const HoloVizioModel& holoVizioModel );
	void SetMultiViewModel( const MultiViewModel& multiViewModel );

	bool Convert_MultiView_to_HoloVizio( const Image3D& multiViewImage, Image3D& holoVizioImage );
	bool Convert_HoloVizio_to_MultiView( const Image3D& holoVizioImage, Image3D& multiViewImage );

	bool Interpolate_MultiView_to_Projector( const Image3D& multiViewImage, Image2D& projectorImage, const Vec3f& projectorPos );
	bool Interpolate_HoloVizio_to_Camera( const Image3D& holoVizioImage, Image2D& cameraImage, const Vec3f& cameraPos );

	//bool Visualize_HoloVizio_to_MultiView( const std::vector<Image2D>& holoVizioImage, std::vector<Image2D>& multiViewImage );
	bool Visualize_HoloVizio_to_Camera( const Image3D& holoVizioImage, Image2D& cameraImage, const Vec3f& cameraPos, const bool normalize = true );
	float ProjectorWeight( const Vec3f& projectorPos, const Vec3f& screenPos, const Vec3f& cameraPos );

private:
	float InterpolatedCameraIndex( const Vec3f& screenPos, const Vec3f& projectorPos );
	float InterpolatedProjectorIndex( const Vec3f& screenPos, const Vec3f& cameraPos );

private:
	HoloVizioModel holoVizioModel;
	MultiViewModel multiViewModel;
};

#endif // LIGHTFIELDPROCESSING_LIGHTFIELDINTERPOLATION_H