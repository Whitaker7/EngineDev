#include "dev_app.h"
#include "math_types.h"
#include "debug_renderer.h"
#include <iostream>
#include "pools.h"
#include <DirectXMath.h>
using namespace DirectX;
//TODO include debug_renderer.h and pools.h and anything else you might need here

namespace end
{
	double delta_time = 0.0;
	float4 color(1.0f, 1.0f, 1.0f, 1.0f);
	float4 white(1, 1, 1, 1);
	bool colorChange = true; //allows for continous color changing
	float3 gravity(0, -9.8f, 0);
	int index = -1;
	int freeIndex = -1;
	int freeSortIndex = -1;
	float creationTimerHolder = 0.05f;//set to 0.05 for best emission
	float creationTimer = creationTimerHolder;




	XMMATRIX player = XMMatrixIdentity();
	XMFLOAT4X4 player44;
	XMVECTOR transVec = XMVectorSet(0, 0.05f, 0, 1.0f);
	XMVECTOR forVec = XMVectorSet(0, 0, 1, 1);
	XMVECTOR playerFor;
	XMMATRIX playerRot = XMMatrixIdentity();
	XMFLOAT3 transVec3;
	XMVECTOR scalOrigin;
	XMVECTOR rotatOrigin;
	XMVECTOR rotateQuat;
	float transY = 0;
	//player44 = *(float4x4*)(& player);
	XMFLOAT4X4 player44Copy;
	XMMATRIX lookAt = XMMatrixIdentity();
	XMFLOAT4X4 lookAt44;
	XMMATRIX turnTo = XMMatrixIdentity();
	XMFLOAT4X4 turnTo44;

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
	

	void dev_app_t::update(view_t& viewM, std::bitset<4> bitTab)
	{
		delta_time = calc_delta_time(); //delta time just equals the amount of time between each frames



		//i could make a "timer" but float timer += delta time each frame

		//sorted pool
//#pragma region SortedPool
//		//for every particle we will just need to apply position and color
//		//free list this shoudl be ontop
//
//		//initialize the sorted pool list
//		creationTimer -= delta_time;
//		if (creationTimer <= 0)
//		{
//			for (int i = 0; i < 7; i++)
//			{
//				index = sorted_pool.alloc();
//				if (index <= -1)
//				{
//					sorted_pool.free(index);
//					break;
//				}
//				sorted_pool[index + i].color = white;
//				sorted_pool[index + i].velocity = float3(Randf(-2, 2), 10, Randf(-2, 2));
//				sorted_pool[index + i].pos = float3(0, 0, 0);
//				sorted_pool[index + i].prev_pos = float3(0, 0, 0);
//			}
//			//creationTimer = creationTimerHolder; //resets it for all pools
//		}
//
//
//
//		//end::debug_renderer::add_line(sorted_pool[0].pos, sorted_pool[0].prev_pos, sorted_pool[0].color);
//
//		for (int i = 0; i < sorted_pool.size(); i++)
//		{
//			end::debug_renderer::add_line(sorted_pool[i].pos, sorted_pool[i].prev_pos, sorted_pool[i].color);
//			sorted_pool[i].prev_pos = sorted_pool[i].pos; //store current position of the head of the particle
//			sorted_pool[i].pos += sorted_pool[i].velocity * delta_time; //add velocity to the particle head(pos)
//			sorted_pool[i].velocity += gravity * delta_time; //slow velocity due to gravity
//
//			//	sorted_pool[i].timer -= delta_time;
//			if (sorted_pool[i].pos.y <= 0)
//			{
//				sorted_pool.free(i);
//			}
//		}
//
//
//#pragma endregion
//
//		//freeList
//#pragma region FreeList
//
//		//set emitters spawn location and color
//		//top right
//		emitters[0].spawn_color = float4(1.0f, 0, 0, 1.0f);
//		emitters[0].spawn_pos = float3(7.5f, 0, 7.5f);
//		//top left
//		emitters[1].spawn_color = float4(0.0f, 1.0f, 0, 1.0f);
//		emitters[1].spawn_pos = float3(-7.5f, 0, 7.5f);
//		//bottom right
//		emitters[2].spawn_color = float4(1.0f, 0, 1.0f, 1.0f);
//		emitters[2].spawn_pos = float3(7.5f, 0, -7.5f);
//		//bottom left
//		emitters[3].spawn_color = float4(0, 0, 1.0f, 1.0f);
//		emitters[3].spawn_pos = float3(-7.5f, 0, -7.5f);
//
//		if (creationTimer <= 0)
//		{
//			for (int u = 0; u < 4; u++)
//			{
//				for (int i = 0; i < 7; i++)
//				{
//					freeIndex = free_pool.alloc();
//					freeSortIndex = emitters[u].indices.alloc();
//					//check that both freeindex and freesortindex are valid here
//					//if either is invalid the other needs to be freed
//					if (freeIndex == -1 && freeSortIndex > -1)
//					{
//						emitters[u].indices.free(freeSortIndex);
//						break;
//					}
//					if (freeSortIndex == -1 && freeIndex > -1)
//					{
//						free_pool.free(freeIndex);
//						break;
//					}
//					if (freeIndex < 0 && freeSortIndex < 0)
//					{
//						break;
//					}
//					emitters[u].indices[freeSortIndex] = freeIndex;
//
//					Particle& part = free_pool[freeIndex];
//
//					//free_pool[freeIndex].color = emitters[0].spawn_color;
//					//free_pool[freeIndex].pos = emitters[0].spawn_pos;
//					////free_pool[freeIndex].prev_pos = float3(Randf(-10, 10), 3, Randf(-10, 10));
//					//free_pool[freeIndex].prev_pos = emitters[0].spawn_pos;
//					//free_pool[freeIndex].velocity = float3(Randf(-2, 2), 10, Randf(-2, 2));
//
//					part.color = emitters[u].spawn_color;
//					part.pos = emitters[u].spawn_pos;
//					//part.prev_pos = float3(Randf(-10, 10), 3, Randf(-10, 10));
//					part.prev_pos = emitters[u].spawn_pos;
//					part.velocity = float3(Randf(-2, 2), 10, Randf(-2, 2));
//				}
//
//				creationTimer = creationTimerHolder;
//
//			}
//		}
//
//		for (int u = 0; u  < 4; u ++)
//		{
//			for (int i = 0; i < emitters[u].indices.size(); i++)
//			{
//				int16_t nFreeIndex = emitters[u].indices[i]; // n-prefix stands for "int" (for some reason)
//				Particle& particle = free_pool[nFreeIndex];  // this is a reference to a particle, so it works like a pointer, we are modifying the source and not a copy
//
//				//end::debug_renderer::add_line(free_pool[emitters[0].indices[i]].pos, free_pool[emitters[0].indices[i]].prev_pos, free_pool[emitters[0].indices[i]].color);
//				//	float3 posHolder = free_pool[emitters[0].indices[i]].pos; //store current position of the head of the particle
//				//	free_pool[emitters[0].indices[i]].pos += free_pool[emitters[0].indices[i]].velocity * delta_time; //add velocity to the particle head(pos)
//				//	free_pool[emitters[0].indices[i]].velocity += gravity * delta_time; //slow velocity due to gravity
//				//	free_pool[emitters[0].indices[i]].prev_pos = posHolder; //set particle tail to previous head position
//
//				end::debug_renderer::add_line(particle.pos, particle.prev_pos, particle.color);
//				particle.prev_pos = particle.pos; //store current position of the head of the particle
//				particle.pos += particle.velocity * delta_time; //add velocity to the particle head(pos)
//				particle.velocity += gravity * delta_time; //slow velocity due to gravity
//
//				//sorted_pool[i].timer -= delta_time;
//				if (particle.pos.y <= 0)
//				{
//					free_pool.free(nFreeIndex);
//					emitters[u].indices.free(i);
//				}
//			}
//		}
		


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


		//store player in a modifiable variable
		//player44Copy 
		if (bitTab[2] == 1)
		{
			transY -= bitTab[2] * delta_time;
			playerRot = XMMatrixRotationY(transY);
		}
		if (bitTab[3] == 1)
		{
			transY += bitTab[3] * delta_time;
			playerRot = XMMatrixRotationY(transY);
		}
		player = XMMatrixTranslationFromVector(transVec);
		player = XMMatrixMultiply(playerRot, player);
		XMStoreFloat4x4(&player44, player);
		
		//move player based on input
		//bitTab is [4] is up,down,left,right
		//player44._43 += bitTab[0] * delta_time; //move forward with UP_KEY
		//player44._43 -= bitTab[1] * delta_time; //back with DOWN_KEY
		//player44._41 -= bitTab[2] * delta_time;	//left with LEFT_KEY
		//player44._41 += bitTab[3] * delta_time;	//right with RIGHT_KEY
		//drawPlayer
		end::debug_renderer::add_line(float3(player44._41, player44._42, player44._43), float3(player44._11, player44._12, player44._43 + player44._13), float4(1,0,0,1));//x (right)
		end::debug_renderer::add_line(float3(player44._41, player44._42, player44._43), float3(player44._41, player44._42 + 1.0f, player44._43), float4(0,1,0,1)); // y (up)
		end::debug_renderer::add_line(float3(player44._41, player44._42, player44._43), float3(player44._31, player44._32, player44._43 + player44._33), float4(0,0,1,1));// z (forward)

		
		//transVec = XMVectorSet(player44._31, player44._32, player44._43 + player44._33, 1);
		playerFor = XMVector3Transform(forVec, playerRot);
		XMStoreFloat3(&transVec3, playerFor);
		transVec = XMVectorSetX(transVec, player44._41 + (transVec3.x * delta_time));
		transVec = XMVectorSetZ(transVec, player44._43 + (transVec3.z * delta_time));
		//transY += 1.0 * delta_time;
		//draws lookat
		lookAt = XMMatrixTranslation(-5.0f, 2.0f, -5.0f);
		XMStoreFloat4x4(&lookAt44, lookAt);
		end::debug_renderer::add_line(float3(lookAt44._41, lookAt44._42, lookAt44._43), float3(lookAt44._41 + 1.0f, lookAt44._42, lookAt44._43), float4(1, 0, 0, 1));
		end::debug_renderer::add_line(float3(lookAt44._41, lookAt44._42, lookAt44._43), float3(lookAt44._41, lookAt44._42 + 1.0f, lookAt44._43), float4(0, 1, 0, 1));
		end::debug_renderer::add_line(float3(lookAt44._41, lookAt44._42, lookAt44._43), float3(lookAt44._41, lookAt44._42, lookAt44._43 + 1.0f), float4(0, 0, 1, 1));
		//draws turnto
		turnTo = XMMatrixTranslation(5.0f, 2.0f, 5.0f);
		XMStoreFloat4x4(&turnTo44, turnTo);
		end::debug_renderer::add_line(float3(turnTo44._41, turnTo44._42, turnTo44._43), float3(turnTo44._41 + 1.0f, turnTo44._42, turnTo44._43), float4(1, 0, 0, 1));
		end::debug_renderer::add_line(float3(turnTo44._41, turnTo44._42, turnTo44._43), float3(turnTo44._41, turnTo44._42 + 1.0f, turnTo44._43), float4(0, 1, 0, 1));
		end::debug_renderer::add_line(float3(turnTo44._41, turnTo44._42, turnTo44._43), float3(turnTo44._41, turnTo44._42, turnTo44._43 + 1.0f), float4(0, 0, 1, 1));


		//Test to make sure it works and it doesss 
		//viewM.view_mat = *(float4x4_a*)(& turnTo44);


		//this is a pseudo function to cast between pointers of the same "memory"
		/*vec3& GetAxisY(float4x4& mIn)
		{
			return *(vec3*)& mIn._21;
		}*/



		//change a color over time
#pragma region change line color
		/*if (colorChange == true)
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
		}*/
#pragma endregion

	}
}
