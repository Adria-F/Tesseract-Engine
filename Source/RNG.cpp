#pragma once
#include "Globals.h"

#include "PCG/include/pcg_variants.h"
#include "PCG/extras/entropy.h"
#include <time.h>
#include <math.h>

pcg32_random_t rng;

void startRNGSeed()
{
	pcg32_srandom_r(&rng, time(NULL), (intptr_t)&rng);
}

//Float between 0 and 1
float getRandom()
{	
	return ldexp(pcg32_random_r(&rng), -32);
}

int getRandomBetween(int min, int max)
{
	return (min < max) ? pcg32_boundedrand_r(&rng, (max - min)) + min : (min == max) ? max : 0;
}

uint generateUID()
{
	return pcg32_random_r(&rng);
}