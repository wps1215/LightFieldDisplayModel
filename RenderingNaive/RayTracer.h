/*
* LightFieldDisplayModel - RenderingNaive - RayTracer
*
* Naive ray tracer, capable to render different light field representations (e.g., HoloVizio and MultiView).
*
* Copyright (C) 2019 by Oleksii Doronin
*
* This code is licensed under the MIT license (MIT) (http://opensource.org/licenses/MIT)
*/

#ifndef RENDERINGNAIVE_RAYTRACER_H
#define RENDERINGNAIVE_RAYTRACER_H

#define _USE_MATH_DEFINES
#include "geometry.h"
#include "Image2D.h"


struct Light;
struct Material;
struct Sphere;



// Based on tinyraytracer project: https://github.com/ssloy/tinyraytracer

class RayTracer
{
public:
	RayTracer();
	~RayTracer();

	void RemoveAllGeometry();
	void RemoveAllLights();

	void AddSphere( const Sphere& sphere );
	void AddLight( const Light& light );

	bool SetMaxRayDepth( const int raydepth );

	void RenderPinhole( Image2D& image, const Vec3f& position, const Vec2f& screenHalfSize );
	void RenderProjector( Image2D& image, const Vec3f& position, const float observerDistance, const Vec2f& screenHalfSize );

private:
	Vec3f CastRay( const Vec3f& origin, const Vec3f& direction, int depth = 0 );
	Vec3f Reflect( const Vec3f &I, const Vec3f &N );
	Vec3f Refract( const Vec3f &I, const Vec3f &N, const float eta_t, const float eta_i = 1.0f );
	bool SceneIntersect( const Vec3f &orig, const Vec3f &dir, Vec3f &hit, Vec3f &N, Material &material );

private:
	std::vector<Sphere> spheres;
	std::vector<Light>  lights;

	int maxRayDepth;
};


struct Light {
	Light( const Vec3f &p, const float i ) noexcept : position( p ), intensity( i ) {}
	Vec3f position;
	float intensity;
};

struct Material {
	Material( const float r, const Vec4f &a, const Vec3f &color, const float spec ) noexcept : refractive_index( r ), albedo( a ), diffuse_color( color ), specular_exponent( spec ) {}
	Material() noexcept : refractive_index(1), albedo(1,0,0,0), diffuse_color(), specular_exponent() {}
	float refractive_index;
	Vec4f albedo;
	Vec3f diffuse_color;
	float specular_exponent;
};

struct Sphere {
	Vec3f center;
	float radius;
	Material material;

	Sphere( const Vec3f &c, const float r, const Material &m ) noexcept : center( c ), radius( r ), material( m ) {}

	bool ray_intersect( const Vec3f &orig, const Vec3f &dir, float &t0 ) const {
		const Vec3f L = center - orig;
		const float tca = L * dir;
		const float d2 = L * L - tca * tca;
		if ( d2 > radius*radius ) return false;
		float thc = sqrtf( radius*radius - d2 );
		t0 = tca - thc;
		float t1 = tca + thc;
		if ( t0 < 0 ) t0 = t1;
		if ( t0 < 0 ) return false;
		return true;
	}
};


#endif // RENDERINGNAIVE_RAYTRACER_H