#pragma once
#include <cstdint>
#include <chrono>
#include <bitset>
#include "renderer.h"
	


namespace end
{
	// Simple app class for development and testing purposes
	struct dev_app_t
	{
		void update(view_t & viewM, std::bitset<256> bitTab, int inputPoint[2]);

		dev_app_t();

		void AABBSetup();

		double get_delta_time()const;
	};
}