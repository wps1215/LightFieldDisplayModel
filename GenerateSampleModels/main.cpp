#include <iostream>
#include "HoloVizioModel.h"
#include "MultiViewModel.h"
#include "geometry.h"


void GenerateDefaultHoloVizioModel()
{
	HoloVizioModel holovizioModel;
	holovizioModel.name = "MyHoloVizio";
	holovizioModel.num_projectors = 21;
	holovizioModel.image_size_x = 1000;
	holovizioModel.image_size_y = 600;
	//holovizioModel.angular_scattering = 0.1f;
	holovizioModel.observer_distance = 2000.0f;
	holovizioModel.screen_size_x = 1000.0f;
	holovizioModel.screen_size_y = 600.0f;
	holovizioModel.projectors_pos_x.resize( holovizioModel.num_projectors );
	holovizioModel.projectors_pos_y.resize( holovizioModel.num_projectors );
	holovizioModel.projectors_pos_z.resize( holovizioModel.num_projectors );
	for ( int projId = 0; projId < holovizioModel.num_projectors; ++projId )
	{
		holovizioModel.projectors_pos_x.at( projId ) = 100.0f * static_cast<float>(projId - 10);
		holovizioModel.projectors_pos_y.at( projId ) = 0.0f;
		holovizioModel.projectors_pos_z.at( projId ) = -500.0f;
	}
	{
		// Evaluate angular scattering by rule of thumb.
		// Namely, angle of half-decay equals to 1.5 angle between central-most projectors.
		const int centralProjId = holovizioModel.num_projectors / 2;
		const Vec3f dirToLeftProj = Vec3f( holovizioModel.projectors_pos_x.at( centralProjId + 0 ), 0.0f, holovizioModel.projectors_pos_z.at( centralProjId + 0 ) ).normalize();
		const Vec3f dirToRightProj = Vec3f( holovizioModel.projectors_pos_x.at( centralProjId + 1 ), 0.0f, holovizioModel.projectors_pos_z.at( centralProjId + 1 ) ).normalize();
		const float cosine = dot( dirToLeftProj, dirToRightProj );
		const float angle = acos( cosine );
		holovizioModel.angular_scattering = angle * 1.5f;
	}
	holovizioModel.Serialize( "../../data/sample_holoviziomodel.json" );
}


void GenerateDefaultMultiViewModel()
{
	MultiViewModel multiviewModel;
	multiviewModel.name = "MyMultiView";
	multiviewModel.num_cameras = 21;
	multiviewModel.image_size_x = 1000;
	multiviewModel.image_size_y = 600;
	multiviewModel.screen_size_x = 1000.0f;
	multiviewModel.screen_size_y = 600.0f;
	multiviewModel.cameras_pos_x.resize( multiviewModel.num_cameras );
	multiviewModel.cameras_pos_y.resize( multiviewModel.num_cameras );
	multiviewModel.cameras_pos_z.resize( multiviewModel.num_cameras );
	for ( int cameraId = 0; cameraId < multiviewModel.num_cameras; ++cameraId )
	{
		multiviewModel.cameras_pos_x.at( cameraId ) = 100.0f * static_cast<float>(cameraId - 10);
		multiviewModel.cameras_pos_y.at( cameraId ) = 0.0f;
		multiviewModel.cameras_pos_z.at( cameraId ) = 2000.0f;
	}
	multiviewModel.Serialize( "../../data/sample_multiviewmodel.json" );
}



int main( int argc, char** argv )
{
	std::cout << "Program started..." << std::endl;

	GenerateDefaultHoloVizioModel();

	GenerateDefaultMultiViewModel();

	std::cout << "Program ended..." << std::endl;
	return 0;
}