#include "dev_app.h"
#include "math_types.h"
#include "debug_renderer.h"
#include <iostream>
#include "../../pools.h"
//TODO include debug_renderer.h and pools.h and anything else you might need here

namespace end
{
	double delta_time = 0.0;
	float4 color(0, 1.0f, 0.1f, 1.0f);
	float4 white(1, 1, 1, 1);
	bool colorChange = true; //allows for continuos color changing
	float3 gravity(0, -9.8f, 0);
	int index = -1;
	int freeIndex = -1;
	float creationTimerHolder = 0.05f;
	float creationTimer = creationTimerHolder;

	struct Particle
	{
		float3 pos;
		float3 prev_pos;
		float4 color;
		float timer = 0.5f;
		float3 velocity;
	};

	Particle particle;

	end::sorted_pool_t<Particle, 256> sorted_pool;
	//make free pool
	end::pool_t<Particle, 1024> free_pool;

	//make emitters
	struct Emitter
	{
		float3 spawn_pos;
		float4 spawn_color;
		// indices into the shared_pool 
		sorted_pool_t<int16_t, 256> indices;
	};

	Emitter emitters[4];

	
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

	//creats a randon float between min and max
	float Randf(float min, float max)
	{
		float ran = rand() / (float)RAND_MAX;
		return (max - min) * ran + min;
	}
	/*void SwapPartPos(Particle& part)
	{
		part.
	}*/
	

	void dev_app_t::update()
	{
		delta_time = calc_delta_time(); //delta time just equals the amount of time between each frames

		
		
		//i could make a "timer" but float timer += delta time each frame

		//sorted pool
#pragma region SortedPool
		//for every particle we will just need to apply position and color
		//free list this shoudl be ontop

		//initialize the sorted pool list
		creationTimer -= delta_time;
		if (creationTimer <= 0)
		{
			for (int i = 0; i < 7; i++)
			{
				index = sorted_pool.alloc();
				if (index <= -1)
				{
					break;
				}
				sorted_pool[index + i].color = white;
				sorted_pool[index + i].velocity = float3(Randf(-2, 2), 10, Randf(-2, 2));
				sorted_pool[index + i].pos = float3(0, 0, 0);
				sorted_pool[index + i].prev_pos = float3(0, 0, 0);
			}
			creationTimer = creationTimerHolder; //resets it for all pools
		}



		//end::debug_renderer::add_line(sorted_pool[0].pos, sorted_pool[0].prev_pos, sorted_pool[0].color);

		for (int i = 0; i < sorted_pool.size(); i++)
		{
			end::debug_renderer::add_line(sorted_pool[i].pos, sorted_pool[i].prev_pos, sorted_pool[i].color);
			float3 posHolder = sorted_pool[i].pos; //store current position of the head of the particle
			sorted_pool[i].pos += sorted_pool[i].velocity * delta_time; //add velocity to the particle head(pos)
			sorted_pool[i].velocity += gravity * delta_time; //slow velocity due to gravity
			sorted_pool[i].prev_pos = posHolder; //set particle tail to previous head position

			sorted_pool[i].timer -= delta_time;
			if (sorted_pool[i].pos.y <= 0)
			{
				sorted_pool.free(i);
			}
		}


#pragma endregion

		//freeList
#pragma region FreeList
		emitters[0].spawn_color = float4(1.0f, 0, 0, 1.0f);
		emitters[0].spawn_pos = float3(7.5f, 0, 7.5f);

		free_pool[0].color = emitters[0].spawn_color;
		free_pool[0].pos = emitters[0].spawn_pos;
		free_pool[0].prev_pos = emitters[0].spawn_pos;
		free_pool[0].prev_pos.y = emitters[0].spawn_pos.y + 10.0f;
		free_pool[0].velocity = float3(Randf(-2, 2), 5, Randf(-2, 2));


		end::debug_renderer::add_line(free_pool[0].pos, free_pool[0].prev_pos, free_pool[0].color);

#pragma endregion




		

		//This drawn the green checkmark
		/*end::debug_renderer::add_line(float3(-2, 0, 0), float3(0, -3, 0), color);
		end::debug_renderer::add_line(float3(0, -3, 0), float3(3, 4, 0), color);*/
#pragma region Grid

		//this draws the grid horizontal
		end::debug_renderer::add_line(float3(-10, 0, 10), float3(10, 0, 10), color);
		end::debug_renderer::add_line(float3(-10, 0, 9), float3(10, 0, 9), color);
		end::debug_renderer::add_line(float3(-10, 0, 8), float3(10, 0, 8), color);
		end::debug_renderer::add_line(float3(-10, 0, 7), float3(10, 0, 7), color);
		end::debug_renderer::add_line(float3(-10, 0, 6), float3(10, 0, 6), color);
		end::debug_renderer::add_line(float3(-10, 0, 5), float3(10, 0, 5), color);
		end::debug_renderer::add_line(float3(-10, 0, 4), float3(10, 0, 4), color);
		end::debug_renderer::add_line(float3(-10, 0, 3), float3(10, 0, 3), color);
		end::debug_renderer::add_line(float3(-10, 0, 2), float3(10, 0, 2), color);
		end::debug_renderer::add_line(float3(-10, 0, 1), float3(10, 0, 1), color);
		end::debug_renderer::add_line(float3(-10, 0, 0), float3(10, 0, 0), color);
		end::debug_renderer::add_line(float3(-10, 0, -10), float3(10, 0, -10), color);
		end::debug_renderer::add_line(float3(-10, 0, -9), float3(10, 0, -9), color);
		end::debug_renderer::add_line(float3(-10, 0, -8), float3(10, 0, -8), color);
		end::debug_renderer::add_line(float3(-10, 0, -7), float3(10, 0, -7), color);
		end::debug_renderer::add_line(float3(-10, 0, -6), float3(10, 0, -6), color);
		end::debug_renderer::add_line(float3(-10, 0, -5), float3(10, 0, -5), color);
		end::debug_renderer::add_line(float3(-10, 0, -4), float3(10, 0, -4), color);
		end::debug_renderer::add_line(float3(-10, 0, -3), float3(10, 0, -3), color);
		end::debug_renderer::add_line(float3(-10, 0, -2), float3(10, 0, -2), color);
		end::debug_renderer::add_line(float3(-10, 0, -1), float3(10, 0, -1), color);
		end::debug_renderer::add_line(float3(-10, 0, -0), float3(10, 0, -0), color);

		//this draws the grid vertical
		end::debug_renderer::add_line(float3(10, 0, 10), float3(10, 0, -10), color);
		end::debug_renderer::add_line(float3(9, 0, 10), float3(9, 0, -10), color);
		end::debug_renderer::add_line(float3(8, 0, 10), float3(8, 0, -10), color);
		end::debug_renderer::add_line(float3(7, 0, 10), float3(7, 0, -10), color);
		end::debug_renderer::add_line(float3(6, 0, 10), float3(6, 0, -10), color);
		end::debug_renderer::add_line(float3(5, 0, 10), float3(5, 0, -10), color);
		end::debug_renderer::add_line(float3(4, 0, 10), float3(4, 0, -10), color);
		end::debug_renderer::add_line(float3(3, 0, 10), float3(3, 0, -10), color);
		end::debug_renderer::add_line(float3(2, 0, 10), float3(2, 0, -10), color);
		end::debug_renderer::add_line(float3(1, 0, 10), float3(1, 0, -10), color);
		end::debug_renderer::add_line(float3(0, 0, 10), float3(0, 0, -10), color);
		end::debug_renderer::add_line(float3(-1, 0, 10), float3(-1, 0, -10), color);
		end::debug_renderer::add_line(float3(-2, 0, 10), float3(-2, 0, -10), color);
		end::debug_renderer::add_line(float3(-3, 0, 10), float3(-3, 0, -10), color);
		end::debug_renderer::add_line(float3(-4, 0, 10), float3(-4, 0, -10), color);
		end::debug_renderer::add_line(float3(-5, 0, 10), float3(-5, 0, -10), color);
		end::debug_renderer::add_line(float3(-6, 0, 10), float3(-6, 0, -10), color);
		end::debug_renderer::add_line(float3(-7, 0, 10), float3(-7, 0, -10), color);
		end::debug_renderer::add_line(float3(-8, 0, 10), float3(-8, 0, -10), color);
		end::debug_renderer::add_line(float3(-9, 0, 10), float3(-9, 0, -10), color);
		end::debug_renderer::add_line(float3(-10, 0, 10), float3(-10, 0, -10), color);

#pragma endregion


		//TODO do you Updates here

		


		//change a color over time
#pragma region change line color
		if (colorChange == true)
		{
			color.x -= 0.25f * delta_time;
			color.y += 0.25f * delta_time;
		}
		else
		{
			color.x += 0.25f * delta_time;
			color.y -= 0.25f * delta_time;
		}

		if (color.x > 1)
		{
			colorChange = true;
		}
		if (color.x < 0)
		{
			colorChange = false;
		}
#pragma endregion

	}
}
