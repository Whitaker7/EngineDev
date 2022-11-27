#include "dev_app.h"
#include "math_types.h"
#include "debug_renderer.h"
#include <iostream>
//TODO include debug_renderer.h and pools.h and anything else you might need here

namespace end
{
	double delta_time = 0.0;

	double dev_app_t::get_delta_time()const
	{
		return delta_time;
	}

	dev_app_t::dev_app_t()
	{
		std::cout << "Log whatever you need here.\n"; // Don’t forget to include <iostream>
	}
	
	double calc_delta_time()
	{
		static std::chrono::time_point<std::chrono::high_resolution_clock> last_time = std::chrono::high_resolution_clock::now();

		std::chrono::time_point<std::chrono::high_resolution_clock> new_time = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsed_seconds = new_time - last_time;
		last_time = new_time;

		return std::min(1.0 / 15.0, elapsed_seconds.count());
	}

	void dev_app_t::update()
	{
		delta_time = calc_delta_time();

		//This drawn the green checkmark
		end::debug_renderer::add_line(float3(-2, 0, 0), float3(0, -3, 0), float4(0.1f, 1, 0.1f, 1));
		end::debug_renderer::add_line(float3(0, -3, 0), float3(3, 4, 0), float4(0.1f, 1, 0.1f, 1));

		//TODO do you Updates here
	}
}
