/*
* LightFieldDisplayModel - LightFieldProcessing - LightFieldInterpolation
*
* Interpolation of light field between different representations (e.g., HoloVizio and MultiView).
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "LightFieldInterpolation.h"

#define _USE_MATH_DEFINES
#include <algorithm>

#include "Image2D.h"
#include "Image3D.h"


const int num_projectors_contributes_halved = 4;
const float contribution_epsilon = 0.01f;

const float gaussian_half_decay = 1.11741f;
const float gaussian_half_decay_sqr = gaussian_half_decay*gaussian_half_decay;



LightFieldInterpolation::LightFieldInterpolation()
{
}



LightFieldInterpolation::LightFieldInterpolation( const HoloVizioModel& holoVizioModel, const MultiViewModel& multiViewModel )
	:holoVizioModel(holoVizioModel)
	,multiViewModel(multiViewModel)
{

}



LightFieldInterpolation::~LightFieldInterpolation()
{
}



void LightFieldInterpolation::SetHoloVizioModel( const HoloVizioModel& holoVizioModel )
{
	this->holoVizioModel = holoVizioModel;
}



void LightFieldInterpolation::SetMultiViewModel( const MultiViewModel& multiViewModel )
{
	this->multiViewModel = multiViewModel;
}



bool LightFieldInterpolation::Convert_MultiView_to_HoloVizio( const Image3D& multiViewImage, Image3D& holoVizioImage )
{
	const int num_cameras = multiViewModel.num_cameras;
	const int num_projectors = holoVizioModel.num_projectors;
	const int width = multiViewModel.image_size_x;
	const int height = multiViewModel.image_size_y;

	if ( num_cameras == 0 || num_projectors == 0 )
		return false;
	if ( width == 0 || height == 0 )
		return false;
	if ( holoVizioModel.image_size_x != width || holoVizioModel.image_size_y != height )
		return false;
	if ( holoVizioModel.screen_size_x != multiViewModel.screen_size_x || holoVizioModel.screen_size_y != multiViewModel.screen_size_y )
		return false;
	if ( multiViewImage.Width() != width || multiViewImage.Height() != height || multiViewImage.Depth() != num_cameras )
		return false;

	bool success = true;
	holoVizioImage.Resize( width, height, num_projectors );
	for ( int projId = 0; projId < num_projectors; ++projId )
	{
		const Vec3f projectorPos( holoVizioModel.projectors_pos_x[projId], holoVizioModel.projectors_pos_y[projId], holoVizioModel.projectors_pos_z[projId] );
		success = success && Interpolate_MultiView_to_Projector( multiViewImage, holoVizioImage.Layer(projId), projectorPos );
	}
	return success;
}



bool LightFieldInterpolation::Convert_HoloVizio_to_MultiView( const Image3D& holoVizioImage, Image3D& multiViewImage )
{
	const int num_cameras = multiViewModel.num_cameras;
	const int num_projectors = holoVizioModel.num_projectors;
	const int width = multiViewModel.image_size_x;
	const int height = multiViewModel.image_size_y;
	const Vec2f screenSize( multiViewModel.screen_size_x, multiViewModel.screen_size_y );
	const Vec2f screenStart = -screenSize * 0.5f;
	const float observerDistance = holoVizioModel.observer_distance;

	if ( num_cameras == 0 || num_projectors == 0 )
		return false;
	if ( width == 0 || height == 0 )
		return false;
	if ( holoVizioModel.image_size_x != width || holoVizioModel.image_size_y != height )
		return false;
	if ( holoVizioModel.screen_size_x != screenSize.x || holoVizioModel.screen_size_y != screenSize.y )
		return false;
	if ( holoVizioImage.Width() != width || holoVizioImage.Height() != height || holoVizioImage.Depth() != num_projectors )
		return false;

	bool success = true;
	multiViewImage.Resize( width, height, num_cameras );
	for ( int cameraId = 0; cameraId < num_cameras; ++cameraId )
	{
		const Vec3f cameraPos( multiViewModel.cameras_pos_x[cameraId], multiViewModel.cameras_pos_y[cameraId], multiViewModel.cameras_pos_z[cameraId] );
		success = success && Interpolate_HoloVizio_to_Camera( holoVizioImage, multiViewImage.Layer(cameraId), cameraPos );
	}
	return success;
}



bool LightFieldInterpolation::Interpolate_MultiView_to_Projector( const Image3D& multiViewImage, Image2D& projectorImage, const Vec3f& projectorPos )
{
	const int num_cameras = multiViewModel.num_cameras;
	const int width = multiViewModel.image_size_x;
	const int height = multiViewModel.image_size_y;
	const Vec2f screenSize( multiViewModel.screen_size_x, multiViewModel.screen_size_y );
	const Vec2f screenStart = -screenSize * 0.5f;

	if ( num_cameras == 0 )
		return false;
	if ( width == 0 || height == 0 )
		return false;
	if ( multiViewImage.Width() != width || multiViewImage.Height() != height || multiViewImage.Depth() != num_cameras )
		return false;

	projectorImage.Resize( width, height );

	for ( int x = 0; x < width; ++x )
	{
		for ( int y = 0; y < height; ++y )
		{
			const Vec3f screenPos = Vec3f(
				screenStart.x + screenSize.x*(static_cast<float>(x) + 0.5f) / width,
				screenStart.y + screenSize.y*(static_cast<float>(y) + 0.5f) / height,
				0.0f );
			const float interpolatedCameraIndex = InterpolatedCameraIndex( screenPos, projectorPos );
			const int leftCameraId = std::min<int>(std::max<int>( static_cast<int>(interpolatedCameraIndex), 0 ), num_cameras-2 );
			const float weight = std::min<float>(std::max<float>( interpolatedCameraIndex - static_cast<float>(leftCameraId), 0.0f), 1.0f );
			const Vec3f leftCameraValue = multiViewImage.at(x,y,leftCameraId+0);
			const Vec3f rightCameraValue = multiViewImage.at(x,y,leftCameraId+1);
			const Vec3f interpolatedValue = leftCameraValue*(1.0f-weight) + rightCameraValue*weight;
			projectorImage.at(x,y) = interpolatedValue;
		}
	}
	return true;
}



bool LightFieldInterpolation::Interpolate_HoloVizio_to_Camera( const Image3D& holoVizioImage, Image2D& cameraImage, const Vec3f& cameraPos )
{
	const int num_projectors = holoVizioModel.num_projectors;
	const int width = holoVizioModel.image_size_x;
	const int height = holoVizioModel.image_size_y;
	const Vec2f screenSize( holoVizioModel.screen_size_x, holoVizioModel.screen_size_y );
	const Vec2f screenStart = -screenSize * 0.5f;

	if ( num_projectors == 0 )
		return false;
	if ( width == 0 || height == 0 )
		return false;
	if ( holoVizioImage.Width() != width || holoVizioImage.Height() != height || holoVizioImage.Depth() != num_projectors )
		return false;

	cameraImage.Resize( width, height );

	for ( int x = 0; x < width; ++x )
	{
		for ( int y = 0; y < height; ++y )
		{
			const Vec3f screenPos = Vec3f(
				screenStart.x + screenSize.x*(static_cast<float>(x) + 0.5f) / width,
				screenStart.y + screenSize.y*(static_cast<float>(y) + 0.5f) / height,
				0.0f );
			const float interpolatedProjectorIndex = InterpolatedProjectorIndex( screenPos, cameraPos );
			const int leftProjId = std::min<int>(std::max<int>( static_cast<int>(interpolatedProjectorIndex), 0 ), num_projectors-2 );
			const float weight = std::min<float>(std::max<float>( interpolatedProjectorIndex - static_cast<float>(leftProjId), 0.0f), 1.0f );
			const Vec3f leftProjValue = holoVizioImage.at(x,y,leftProjId+0);
			const Vec3f rightProjValue = holoVizioImage.at(x,y,leftProjId+1);
			const Vec3f interpolatedValue = leftProjValue*(1.0f-weight) + rightProjValue*weight;
			cameraImage.at(x,y) = interpolatedValue;
		}
	}

	return true;
}



bool LightFieldInterpolation::Visualize_HoloVizio_to_Camera( const Image3D& holoVizioImage, Image2D& cameraImage, const Vec3f& cameraPos, const bool normalize )
{
	const int num_projectors = holoVizioModel.num_projectors;
	const int width = holoVizioModel.image_size_x;
	const int height = holoVizioModel.image_size_y;
	const Vec2f screenSize( holoVizioModel.screen_size_x, holoVizioModel.screen_size_y );
	const Vec2f screenStart = -screenSize * 0.5f;

	if ( num_projectors == 0 )
		return false;
	if ( width == 0 || height == 0 )
		return false;
	if ( holoVizioImage.Width() != width || holoVizioImage.Height() != height || holoVizioImage.Depth() != num_projectors )
		return false;

	cameraImage.Resize( width, height );

	for ( int x = 0; x < width; ++x )
	{
		for ( int y = 0; y < height; ++y )
		{
			const Vec3f screenPos = Vec3f(
				screenStart.x + screenSize.x*(static_cast<float>(x) + 0.5f) / width,
				screenStart.y + screenSize.y*(static_cast<float>(y) + 0.5f) / height,
				0.0f );
			const float interpolatedProjectorIndex = InterpolatedProjectorIndex( screenPos, cameraPos );
			const int leftProjId = std::min<int>(std::max<int>( static_cast<int>(interpolatedProjectorIndex), 0 ), num_projectors-2 );
			Vec3f interpolatedValue = Vec3f(0.0f,0.0f,0.0f);
			float sumOfVeights = 0.0f;
			const int projIdMin = std::max<int>( leftProjId - num_projectors_contributes_halved + 1, 0 );
			const int projIdMax = std::min<int>( leftProjId + num_projectors_contributes_halved, num_projectors - 1 );
			for ( int projId = projIdMin; projId <= projIdMax; ++projId )
			{
				const Vec3f projectorPos( holoVizioModel.projectors_pos_x[projId], holoVizioModel.projectors_pos_y[projId], holoVizioModel.projectors_pos_z[projId] );
				const float weight = ProjectorWeight( projectorPos, screenPos, cameraPos );
				if ( weight > contribution_epsilon )
				{
					const Vec3f color = holoVizioImage.at(x,y,projId);
					interpolatedValue = interpolatedValue + color*weight;
					sumOfVeights += weight;
				}
			}
			if ( normalize && sumOfVeights > contribution_epsilon )
				interpolatedValue = interpolatedValue * (1.0f/sumOfVeights);
			cameraImage.at(x,y) = interpolatedValue;
		}
	}

	return true;
}



float LightFieldInterpolation::ProjectorWeight( const Vec3f& projectorPos, const Vec3f& screenPos, const Vec3f& cameraPos )
{
	const float projectorTan = (projectorPos.x - screenPos.x) / (projectorPos.z - screenPos.z);
	const float cameraTan = (cameraPos.x - screenPos.x) / (cameraPos.z - screenPos.z);
	const float projectorAngle = atan( projectorTan );
	const float cameraAngle = atan( cameraTan );
	const float angleDiff = abs( projectorAngle - cameraAngle );
	const float angularScatteringSqr = holoVizioModel.angular_scattering*holoVizioModel.angular_scattering;
	const float gaussianArgSqr = angleDiff*angleDiff;
	const float gaussianSigmaSqr = angularScatteringSqr/gaussian_half_decay_sqr;
	//const float weight = std::exp(-gaussianArgSqr/(2.0f*gaussianSigmaSqr)) / sqrt(2.0f*gaussianSigmaSqr*3.14159f);
	const float weight = std::exp( -gaussianArgSqr/gaussianSigmaSqr );
	return weight;
}



float LightFieldInterpolation::InterpolatedCameraIndex( const Vec3f& screenPos, const Vec3f& projectorPos )
{
	const int num_cameras = multiViewModel.num_cameras;
	// Calculate the angle that we are looking for.
	const float targetTan = (screenPos.x - projectorPos.x) / (screenPos.z - projectorPos.z);
	// Use binary-like search to find two closest angles.
	int leftCameraId = 0;
	int rightCameraId = num_cameras - 1;
	float leftCameraTan = (screenPos.x - multiViewModel.cameras_pos_x[leftCameraId]) / (screenPos.z - multiViewModel.cameras_pos_z[leftCameraId]);
	float rightCameraTan = (screenPos.x - multiViewModel.cameras_pos_x[rightCameraId]) / (screenPos.z - multiViewModel.cameras_pos_z[rightCameraId]);
	// leftCameraTan must be lesser than rightCameraTan.
	if ( leftCameraTan > rightCameraTan )
		return 0.0f;
	for ( int iter = 0; iter < 50 && (rightCameraId - leftCameraId)>1; ++iter )
	{
		const int testCameraId = (leftCameraId + rightCameraId) / 2;
		const float testCameraTan = (screenPos.x - multiViewModel.cameras_pos_x[testCameraId]) / (screenPos.z - multiViewModel.cameras_pos_z[testCameraId]);
		// We must increase leftCameraTan, but keep it smaller than targetTan.
		// We must decrease rightCameraTan, but keep it bigger than targetTan.
		if ( testCameraTan > targetTan )
		{
			rightCameraId = testCameraId;
			rightCameraTan = testCameraTan;
		}
		else
		{
			leftCameraId = testCameraId;
			leftCameraTan = testCameraTan;
		}
	}
	// Now we have leftCameraId and rightCameraId that are closest to the ray of interest.
	const float targetAngle = std::atan( targetTan );
	const float leftCameraAngle = std::atan( leftCameraTan );
	const float rightCameraAngle = std::atan( rightCameraTan );
	//float weight = (targetTan - leftCameraTan) / (rightCameraTan - leftCameraTan);
	float weight = (targetAngle - leftCameraAngle) / (rightCameraAngle - leftCameraAngle);
	weight = std::min<float>( std::max<float>( weight, 0.0f ), 1.0f );
	return static_cast<float>(leftCameraId) + weight;
}



float LightFieldInterpolation::InterpolatedProjectorIndex( const Vec3f& screenPos, const Vec3f& cameraPos )
{
	const int num_projectors = holoVizioModel.num_projectors;
	// Calculate the angle that we are looking for.
	const float targetTan = (screenPos.x - cameraPos.x) / (screenPos.z - cameraPos.z);
	// Use binary-like search to find two closest angles.
	int leftProjId = 0;
	int rightProjId = num_projectors - 1;
	float leftProjTan = (screenPos.x - holoVizioModel.projectors_pos_x[leftProjId]) / (screenPos.z - holoVizioModel.projectors_pos_z[leftProjId]);
	float rightProjTan = (screenPos.x - holoVizioModel.projectors_pos_x[rightProjId]) / (screenPos.z - holoVizioModel.projectors_pos_z[rightProjId]);
	// leftProjTan must be bigger than rightCameraTan.
	if ( leftProjTan < rightProjTan )
		return 0.0f;
	for ( int iter = 0; iter < 50 && (rightProjId - leftProjId)>1; ++iter )
	{
		const int testProjId = (leftProjId + rightProjId) / 2;
		const float testProjTan = (screenPos.x - holoVizioModel.projectors_pos_x[testProjId]) / (screenPos.z - holoVizioModel.projectors_pos_z[testProjId]);
		// We must decrease leftProjTan, but keep it bigger than targetTan.
		// We must increase rightProjTan, but keep it smaller than targetTan.
		if ( testProjTan < targetTan )
		{
			rightProjId = testProjId;
			rightProjTan = testProjTan;
		}
		else
		{
			leftProjId = testProjId;
			leftProjTan = testProjTan;
		}
	}
	// Now we have leftProjId and rightProjId that are closest to the ray of interest.
	const float targetAngle = std::atan( targetTan );
	const float leftProjAngle = std::atan( leftProjTan );
	const float rightProjAngle = std::atan( rightProjTan );
	//float weight = (targetTan - leftCameraTan) / (rightCameraTan - leftCameraTan);
	float weight = (targetAngle - leftProjAngle) / (rightProjAngle - leftProjAngle);
	return static_cast<float>(leftProjId) + weight;
}
