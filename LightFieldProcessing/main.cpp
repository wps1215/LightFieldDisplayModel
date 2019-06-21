/*
* LightFieldDisplayModel - LightFieldProcessing - main
*
* Demo script for LightFieldProcessing.
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include <iomanip>
#include <iostream>
#include <sstream>

#include "Image2D.h"
#include "Image3D.h"

#include "LightFieldInterpolation.h"


const bool normalizeDisplayColor = true;



int main( int argc, char** argv )
{
	std::cout << "Program started..." << std::endl << std::endl;

	bool success = true;

	// +++++ Load HoloVizio and MultiView models. +++++
	HoloVizioModel holoVizioModel;
	MultiViewModel multiViewModel;

	success = success && holoVizioModel.Deserialize( "../../data/sample_holoviziomodel.json" );
	success = success && multiViewModel.Deserialize( "../../data/sample_multiviewmodel.json" );

	if ( !success )
	{
		std::cout << "Could not load HoloVizio or MultiView model. I quit." << std::endl;
		return 1;
	}
	// ----- Load HoloVizio and MultiView models. -----

	// +++++ Get basic info and check if it makes sence. +++++
	const int width = multiViewModel.screen_size_x;
	const int height = multiViewModel.screen_size_y;
	const int num_cameras = multiViewModel.num_cameras;
	const int num_projectors = holoVizioModel.num_projectors;
	const float observerDistance = holoVizioModel.observer_distance;
	const Vec2f screenHalfSize = Vec2f( holoVizioModel.screen_size_x, holoVizioModel.screen_size_y ) * 0.5f;

	if ( holoVizioModel.screen_size_x != width || holoVizioModel.screen_size_y != height )
	{
		std::cout << "Image size for HoloVizio and MultiView models must be equal. I quit." << std::endl;
		return 1;
	}
	// ----- Get basic info and check if it makes sence. -----

	Image3D multiViewImage;
	Image3D holoVizioImage;
	LightFieldInterpolation lfInterpolation( holoVizioModel, multiViewModel );

	// +++++ Interpolate from HoloVizio image to MultiView image. +++++
	std::cout << "Interpolating from HoloVizio image to MultiView image..." << std::endl;
	multiViewImage.Clear();
	holoVizioImage.Clear();
	holoVizioImage.Load( "../../output/rt_holovizio/", num_projectors );
	lfInterpolation.Convert_HoloVizio_to_MultiView( holoVizioImage, multiViewImage );
	multiViewImage.Save( "../../output/interp_multiview/" );
	std::cout << "Interpolating from HoloVizio image to MultiView image done." << std::endl;
	// ----- Interpolate from HoloVizio image to MultiView image. -----

	// +++++ Interpolate from MultiView image to HoloVizio image. +++++
	std::cout << "Interpolating from MultiView image to HoloVizio image..." << std::endl;
	multiViewImage.Clear();
	holoVizioImage.Clear();
	multiViewImage.Load( "../../output/rt_multiview/", num_cameras );
	lfInterpolation.Convert_MultiView_to_HoloVizio( multiViewImage, holoVizioImage );
	holoVizioImage.Save( "../../output/interp_holovizio/" );
	std::cout << "Interpolating from MultiView image to HoloVizio image done." << std::endl;
	// ----- Interpolate from MultiView image to HoloVizio image. -----

	// +++++ Simulate HoloVizio display for MultiView camera positions. +++++
	std::cout << "Simulating HoloVizio display for MultiView camera positions..." << std::endl;
	multiViewImage.Clear();
	holoVizioImage.Clear();
	holoVizioImage.Load( "../../output/rt_holovizio/", num_projectors );
	multiViewImage.Resize( width, height, num_cameras );
	for ( int cameraId = 0; cameraId < num_cameras; ++cameraId )
	{
		const Vec3f cameraPos(
			multiViewModel.cameras_pos_x.at(cameraId),
			multiViewModel.cameras_pos_y.at(cameraId),
			multiViewModel.cameras_pos_z.at(cameraId) );
		success = success && lfInterpolation.Visualize_HoloVizio_to_Camera( holoVizioImage, multiViewImage.Layer(cameraId), cameraPos, normalizeDisplayColor );
	}
	multiViewImage.Save( "../../output/perceived/" );
	std::cout << "Simulating HoloVizio display for MultiView camera positions done." << std::endl;
	// ----- Simulate HoloVizio display for MultiView camera positions. -----

	std::cout << "Program ended..." << std::endl;
	return 0;
}