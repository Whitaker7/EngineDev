#pragma once
#include <cstdint>
#include <chrono>
#include "renderer.h"
	


namespace end
{
	// Simple app class for development and testing purposes
	struct dev_app_t
	{
		void update(view_t & viewM);

		dev_app_t();

		double get_delta_time()const;
	};
}