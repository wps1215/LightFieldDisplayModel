/*
* LightFieldDisplayModel - RenderingNaive - RayTracer
*
* Demo script for RenderingNaive.
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include <iostream>

#include "HoloVizioModel.h"
#include "MultiViewModel.h"

#include "RayTracer.h"
#include "Image3D.h"


void SetupScene( RayTracer& rayTracer )
{
	Material      ivory( 1.0f, Vec4f( 0.6f, 0.3f, 0.1f, 0.0f ), Vec3f( 0.4f, 0.4f, 0.3f ), 50.0f );
	Material      glass( 1.5f, Vec4f( 0.0f, 0.5f, 0.1f, 0.8f ), Vec3f( 0.6f, 0.7f, 0.8f ), 125.0f );
	Material red_rubber( 1.0f, Vec4f( 0.9f, 0.1f, 0.0f, 0.0f ), Vec3f( 0.3f, 0.1f, 0.1f ), 10.0f );
	Material     mirror( 1.0f, Vec4f( 0.0f, 10.0f, 0.8f, 0.0f ), Vec3f( 1.0f, 1.0f, 1.0f ), 1425.0f );

	rayTracer.AddSphere( Sphere( Vec3f( -180.0f, -80.0f, -40.0f ), 80.0f, ivory ) );
	rayTracer.AddSphere( Sphere( Vec3f( -100.0f, -100.0f, 120.0f ), 80.0f, glass ) );
	rayTracer.AddSphere( Sphere( Vec3f( 0.0f, -60.0f, -120.0f ), 120.0f, red_rubber ) );
	rayTracer.AddSphere( Sphere( Vec3f( 300.0f, 120.0f, -200.0f ), 160.0f, mirror ) );

	rayTracer.AddLight( Light( Vec3f( -1000.0f, 1000.0f, 1000.0f ), 1.5f ) );
	rayTracer.AddLight( Light( Vec3f( 1500.0f, 2500.0f, -1200.0f ), 1.8f ) );
	rayTracer.AddLight( Light( Vec3f( 1500.0f, 1000.0f, 1500.0f ), 1.7f ) );
}



int main( int argc, char** argv )
{
	std::cout << "Program started..." << std::endl << std::endl;

	bool success = true;

	// +++++  +++++
	// -----  -----

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

	// +++++ Setup ray tracing. +++++
	RayTracer rayTracer;
	SetupScene( rayTracer );
	// ----- Setup ray tracing. -----

	Image3D image3d;

	// +++++ Render MultiView images and save. +++++
	std::cout << "Rendering MultiView images (" << num_cameras << " in total)..." << std::endl;
	image3d.Resize( width, height, num_cameras );
	for ( int viewId = 0; viewId < num_cameras; ++viewId )
	{
		std::cout << "Rendering image " << viewId << " from " << num_cameras << "..." << std::endl;
		const Vec3f cameraPos(
			multiViewModel.cameras_pos_x[viewId],
			multiViewModel.cameras_pos_y[viewId],
			multiViewModel.cameras_pos_z[viewId] );
		rayTracer.RenderPinhole( image3d.Layer(viewId), cameraPos, screenHalfSize );
	}
	std::cout << "Rendering MultiView images done." << std::endl;
	std::cout << "Saving MultiView images..." << std::endl;
	image3d.Save( "../../output/rt_multiview/" );
	std::cout << "Saving MultiView images done." << std::endl;
	// ----- Render MultiView images and save. -----

	// +++++ Render HoloVizio images and save. +++++
	std::cout << "Rendering HoloVizio images (" << num_projectors << " in total)..." << std::endl;
	image3d.Resize( width, height, num_projectors );
	for ( int projId = 0; projId < num_projectors; ++projId )
	{
		std::cout << "Rendering image " << projId << " from " << num_projectors << "..." << std::endl;
		const Vec3f projectorPos(
			holoVizioModel.projectors_pos_x[projId],
			holoVizioModel.projectors_pos_y[projId],
			holoVizioModel.projectors_pos_z[projId] );
		rayTracer.RenderProjector( image3d.Layer(projId), projectorPos, observerDistance, screenHalfSize );
	}
	std::cout << "Rendering HoloVizio images done." << std::endl;
	std::cout << "Saving HoloVizio images..." << std::endl;
	image3d.Save( "../../output/rt_holovizio/" );
	std::cout << "Saving HoloVizio images done." << std::endl;
	// ----- Render HoloVizio images and save. -----

	std::cout << std::endl << "Program ended..." << std::endl;
	return 0;
}