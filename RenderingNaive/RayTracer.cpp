/*
* LightFieldDisplayModel - RenderingNaive - RayTracer
*
* Naive ray tracer, capable to render different light field representations (e.g., HoloVizio and MultiView).
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#include "raytracer.h"

#include <cmath>
#include <limits>
#include <vector>
#include <algorithm>


const float scene_epsilon = 0.05f;
const int default_maxraydepth = 4;


RayTracer::RayTracer()
	:maxRayDepth( default_maxraydepth )
{

}


RayTracer::~RayTracer()
{

}


void RayTracer::RemoveAllGeometry()
{
	spheres.clear();
}


void RayTracer::RemoveAllLights()
{
	lights.clear();
}


void RayTracer::AddSphere( const Sphere& sphere )
{
	spheres.push_back( sphere );
}


void RayTracer::AddLight( const Light& light )
{
	lights.push_back( light );
}


bool RayTracer::SetMaxRayDepth( const int raydepth )
{
	if ( raydepth < 0 )
		return false;
	this->maxRayDepth = raydepth;
	return true;
}


void RayTracer::RenderPinhole( Image2D& image, const Vec3f& position, const Vec2f& screenHalfSize )
{
	const int width = image.Width();
	const int height = image.Height();
	const Vec2f screenStart = -screenHalfSize;
	const Vec2f screenSize = screenHalfSize*2.0f;
#pragma omp parallel for
	for ( size_t j = 0; j < height; j++ )
	{
		for ( size_t i = 0; i < width; i++ )
		{
			const Vec3f screenPos = Vec3f(
				screenStart.x + screenSize.x*(static_cast<float>(i) + 0.5f) / width,
				//screenStart.y + screenSize.y*(static_cast<float>(j) + 0.5f) / height,
				screenStart.y + screenSize.y*(static_cast<float>(height-j-1) + 0.5f) / height,
				0.0f );
			const Vec3f rayOrigin = position;
			const Vec3f rayDirection = (screenPos-rayOrigin).normalize();
			image.at(i,j) = CastRay( rayOrigin, rayDirection );
		}
	}
}


void RayTracer::RenderProjector( Image2D& image, const Vec3f& position, const float observerDistance, const Vec2f& screenHalfSize )
{
	const int width = image.Width();
	const int height = image.Height();
	const Vec2f screenStart = -screenHalfSize;
	const Vec2f screenSize = screenHalfSize * 2.0f;
#pragma omp parallel for
	for ( size_t j = 0; j < height; j++ )
	{
		for ( size_t i = 0; i < width; i++ )
		{
			const Vec3f screenPos = Vec3f(
				screenStart.x + screenSize.x*(static_cast<float>(i) + 0.5f) / width,
				//screenStart.y + screenSize.y*(static_cast<float>(j) + 0.5f) / height,
				screenStart.y + screenSize.y*(static_cast<float>(height-j-1) + 0.5f) / height,
				0.0f );
			const float observerX = screenPos.x - (screenPos.x-position.x)/position.z*observerDistance;
			const Vec3f rayOrigin = Vec3f( observerX, 0.0f, observerDistance );
			const Vec3f rayDirection = (screenPos - rayOrigin).normalize();
			image.at(i,j) = CastRay( rayOrigin, rayDirection );
		}
	}
}


Vec3f RayTracer::CastRay( const Vec3f& origin, const Vec3f& direction, int depth )
{
	Vec3f point, N;
	Material material;

	if ( depth > maxRayDepth || !SceneIntersect( origin, direction, point, N, material ) ) {
		return Vec3f( 0.2f, 0.7f, 0.8f ); // background color
	}

	Vec3f reflect_dir = Reflect( direction, N ).normalize();
	Vec3f refract_dir = Refract( direction, N, material.refractive_index ).normalize();
	Vec3f reflect_orig = reflect_dir * N < 0.0f ? point - N * scene_epsilon : point + N * scene_epsilon; // offset the original point to avoid occlusion by the object itself
	Vec3f refract_orig = refract_dir * N < 0.0f ? point - N * scene_epsilon : point + N * scene_epsilon;
	Vec3f reflect_color = CastRay( reflect_orig, reflect_dir, depth + 1 );
	Vec3f refract_color = CastRay( refract_orig, refract_dir, depth + 1 );

	float diffuse_light_intensity = 0, specular_light_intensity = 0;
	for ( size_t i = 0; i < lights.size(); i++ ) {
		Vec3f light_dir = (lights[i].position - point).normalize();
		float light_distance = (lights[i].position - point).norm();

		Vec3f shadow_orig = light_dir * N < 0.0f ? point - N * scene_epsilon : point + N * scene_epsilon; // checking if the point lies in the shadow of the lights[i]
		Vec3f shadow_pt, shadow_N;
		Material tmpmaterial;
		if ( SceneIntersect( shadow_orig, light_dir, shadow_pt, shadow_N, tmpmaterial ) && (shadow_pt - shadow_orig).norm() < light_distance )
			continue;

		diffuse_light_intensity += lights[i].intensity * std::max( 0.f, light_dir*N );
		specular_light_intensity += powf( std::max( 0.f, -Reflect( -light_dir, N )*direction ), material.specular_exponent )*lights[i].intensity;
	}
	return material.diffuse_color * diffuse_light_intensity * material.albedo[0] + Vec3f( 1., 1., 1. )*specular_light_intensity * material.albedo[1] + reflect_color * material.albedo[2] + refract_color * material.albedo[3];
}


Vec3f RayTracer::Reflect( const Vec3f &I, const Vec3f &N )
{
	return I - N * 2.f*(I*N);
}


Vec3f RayTracer::Refract( const Vec3f &I, const Vec3f &N, const float eta_t, const float eta_i /*= 1.f */ )
{
	// Snell's law
	float cosi = -std::max( -1.f, std::min( 1.f, I*N ) );
	if ( cosi < 0 ) return Refract( I, -N, eta_i, eta_t ); // if the ray comes from the inside the object, swap the air and the media
	float eta = eta_i / eta_t;
	float k = 1 - eta * eta*(1 - cosi * cosi);
	return k < 0 ? Vec3f( 1, 0, 0 ) : I * eta + N * (eta*cosi - sqrtf( k )); // k<0 = total reflection, no ray to refract. I refract it anyways, this has no physical meaning
}


bool RayTracer::SceneIntersect( const Vec3f &orig, const Vec3f &dir, Vec3f &hit, Vec3f &N, Material &material )
{
	float spheres_dist = std::numeric_limits<float>::max();
	for ( size_t i = 0; i < spheres.size(); i++ ) {
		float dist_i;
		if ( spheres[i].ray_intersect( orig, dir, dist_i ) && dist_i < spheres_dist ) {
			spheres_dist = dist_i;
			hit = orig + dir * dist_i;
			N = (hit - spheres[i].center).normalize();
			material = spheres[i].material;
		}
	}

	float checkerboard_dist = std::numeric_limits<float>::max();
	if ( fabs( dir.y ) > 1e-3 ) {
		float d = -(orig.y + 200) / dir.y; // the checkerboard plane has equation y = -200
		Vec3f pt = orig + dir * d;
		if ( d > 0 && fabs(pt.x) < 1000 && fabs(pt.z) < 1000 && d < spheres_dist ) {
			checkerboard_dist = d;
			hit = pt;
			N = Vec3f( 0, 1, 0 );
			material.diffuse_color = (int( .5f*hit.x/100.0f + 1000.0f ) + int( .5f*hit.z/100.0f )) & 1 ? Vec3f( .3f, .3f, .3f ) : Vec3f( .3f, .2f, .1f );
		}
	}
	return std::min( spheres_dist, checkerboard_dist ) < 100000.0f;
}