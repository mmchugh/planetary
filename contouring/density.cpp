#include "density.h"

#include "noise/noise.h"
#include <glm/ext.hpp>

using namespace glm;

#define SEED 1334

float Density_Func(const vec3& worldPosition)
{
    const float radius = 24.f;
    noise::module::RidgedMulti ridgeModule;

	ridgeModule.SetSeed(SEED);
	double noise = ridgeModule.GetValue(worldPosition.x / 15.0f, worldPosition.y / 15.0f, worldPosition.z / 15.0f);

	return length(worldPosition) - radius - noise;
}
