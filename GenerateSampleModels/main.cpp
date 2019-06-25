#include <iostream>
#include "HoloVizioModel.h"
#include "MultiViewModel.h"
#include "geometry.h"


const float observerDist = 2000.0f;


void GenerateDefaultHoloVizioModel()
{
	HoloVizioModel holovizioModel;
	holovizioModel.name = "MyHoloVizio";
	holovizioModel.num_projectors = 21;
	holovizioModel.image_size_x = 1000;
	holovizioModel.image_size_y = 600;
	//holovizioModel.angular_scattering = 0.1f;
	holovizioModel.observer_distance = observerDist;
	holovizioModel.screen_size_x = 1000.0f;
	holovizioModel.screen_size_y = 600.0f;
	holovizioModel.projectors_pos_x.resize( holovizioModel.num_projectors );
	holovizioModel.projectors_pos_y.resize( holovizioModel.num_projectors );
	holovizioModel.projectors_pos_z.resize( holovizioModel.num_projectors );
	{
		// Generate uniformly distributed set of projectors.
		const int numProjectors = holovizioModel.num_projectors;
		const float projectorsMinX = -1000.0f;
		const float projectorsMaxX = 1000.0f;
		const float projectorsY = 0.0f;
		const float projectorsZ = -700.0f;
		for ( int projId = 0; projId < numProjectors; ++projId )
		{
			const float ratio = static_cast<float>(projId) / static_cast<float>(numProjectors-1);
			holovizioModel.projectors_pos_x.at( projId ) = projectorsMinX + ratio*(projectorsMaxX-projectorsMinX);
			holovizioModel.projectors_pos_y.at( projId ) = projectorsY;
			holovizioModel.projectors_pos_z.at( projId ) = projectorsZ;
		}
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
	{
		// Generate uniformly distributed set of views.
		const int numViews = multiviewModel.num_cameras;
		const float viewMinX = -2000.0f;
		const float viewMaxX = 2000.0f;
		const float viewY = 0.0f;
		const float viewZ = observerDist;
		for ( int viewId = 0; viewId < numViews; ++viewId )
		{
			const float ratio = static_cast<float>(viewId) / static_cast<float>(numViews-1);
			multiviewModel.cameras_pos_x.at( viewId ) = viewMinX + ratio*(viewMaxX-viewMinX);
			multiviewModel.cameras_pos_y.at( viewId ) = viewY;
			multiviewModel.cameras_pos_z.at( viewId ) = viewZ;
		}
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