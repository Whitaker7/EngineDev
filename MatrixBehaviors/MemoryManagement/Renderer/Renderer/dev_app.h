#pragma once
#include <cstdint>
#include <chrono>

namespace end
{
	// Simple app class for development and testing purposes
	struct dev_app_t
	{
		void update();

		dev_app_t();

		double get_delta_time()const;
	};
}