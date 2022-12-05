#include "dev_app.h"
#include "math_types.h"
#include "debug_renderer.h"
#include <iostream>
#include "pools.h"
#include <DirectXMath.h>
//#include <WinUser.h>
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
	float playerSpeed = 3;



	//mouse movement variables
	int xPos = 0;
	int yPos = 0;
	XMMATRIX xMouseMat = XMMatrixIdentity();
	XMMATRIX yMouseMat = XMMatrixIdentity();
	XMFLOAT4X4 xMouse44;
	XMFLOAT4X4 yMouse44;
	XMVECTOR cameraTrans = XMVectorSet(0, 0, 0, 1);
	XMVECTOR camFor;
	XMVECTOR camPos = XMVectorSet(0.0f, 15.0f, -15.0f, 1.0f);
	XMFLOAT4 camPos4;
	XMMATRIX totalCamRot;
	XMMATRIX viewHolder;

	XMMATRIX player = XMMatrixIdentity();
	XMFLOAT4X4 player44;
	XMVECTOR playerPos = {0, 0, 0, 0};
	XMVECTOR transVec = XMVectorSet(0, 0.05f, 0, 1.0f);
	XMVECTOR forVec = XMVectorSet(0, 0, 1, 0);
	XMVECTOR upVec = XMVectorSet(0, 1, 0, 0);
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
	XMMATRIX turnTo = XMMatrixTranslation(5.0f, 2.0f, 5.0f);;
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
	
	XMMATRIX LookAt(XMVECTOR pos, XMVECTOR target, XMVECTOR localUp)
	{
		XMVECTOR zaxis = XMVector3Normalize(target - pos);
		XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(zaxis, localUp));
		XMVECTOR yaxis = XMVector3Cross(xaxis, zaxis);

		XMFLOAT3 zaxis3;
		XMFLOAT3 xaxis3;
		XMFLOAT3 yaxis3;
		XMFLOAT3 eye;

		XMStoreFloat3(&eye, pos);
		XMStoreFloat3(&zaxis3, zaxis);
		XMStoreFloat3(&xaxis3, xaxis);
		XMStoreFloat3(&yaxis3, yaxis);

		//zaxis = XMVectorNegate(zaxis); // might only need to do this for right hand orientation

		XMFLOAT4X4 viewMatrix = {
			xaxis3.x, yaxis3.x, zaxis3.x, 0.0f,
			xaxis3.y, yaxis3.y, zaxis3.y, 0.0f,
			xaxis3.z, yaxis3.z, zaxis3.z, 0.0f,
			//-XMVector3Dot(xaxis, pos), -XMVector3Dot(yaxis, pos), -XMVector3Dot(zaxis, pos), 1.0f
			eye.x, eye.y, eye.z, 1.0f
		};

		return *(XMMATRIX*)(&viewMatrix);
	}

	XMMATRIX TurnTo(XMMATRIX viewer, XMVECTOR target, float speed)
	{
		XMFLOAT4X4 viewer44;
		XMStoreFloat4x4(&viewer44, viewer);
		XMVECTOR eye = XMVectorSet(viewer44._41, viewer44._42, viewer44._43, 0);
		viewer44._41 = 0;
		viewer44._42 = 0;
		viewer44._43 = 0;
		XMVECTOR xDefault = XMVectorSet(1, 0, 0, 0);
		XMVECTOR eyeXaxis = XMVector4Transform(xDefault, *(XMMATRIX*)(&viewer44));
		XMVECTOR eyeZaxis = XMVector4Transform(forVec, *(XMMATRIX*)(&viewer44));
		XMVECTOR eyeYaxis = XMVector4Transform(upVec, *(XMMATRIX*)(&viewer44));

		// Turn-To (Y-Axis) pseudocode
		//Get the vector that points from looker position to target positionand normalize it(vToTarget)
		XMVECTOR vToTarget;
		vToTarget = XMVector4Normalize(target - eye);
		//Dot vToTarget against the X - Axis of the looker matrix(fAngleDot)
		XMVECTOR vAngleDot = XMVector4Dot(vToTarget, eyeXaxis);
		float fAngleDotX;
		XMStoreFloat(&fAngleDotX, vAngleDot);
		vAngleDot = XMVector4Dot(vToTarget, eyeYaxis);
		float fAngleDotY;
		XMStoreFloat(&fAngleDotY, vAngleDot);
		// Rotate the looker matrix about the Y - axis by(fAngleDot * fDeltaTime)
		XMMATRIX xRotation = XMMatrixRotationX(-fAngleDotY * speed);
		XMMATRIX yRotation = XMMatrixRotationY(fAngleDotX * speed);
		XMMATRIX totalRotation = XMMatrixMultiply(xRotation, yRotation);
		viewer = XMMatrixMultiply(totalRotation, viewer);

		return viewer;
		
	}


	void dev_app_t::update(view_t& viewM, std::bitset<256> bitTab,int inputPoint[2])
	{
		delta_time = calc_delta_time(); //delta time just equals the amount of time between each frames

		//sprint
		if (bitTab[0x10] == 1)
		{
			playerSpeed = 6;
		}
		else
			playerSpeed = 3;

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
		if (bitTab[0x25] == 1)
		{
			transY -= bitTab[0x25] * delta_time * 3;
			playerRot = XMMatrixRotationY(transY);
		}
		if (bitTab[0x27] == 1)
		{
			transY += bitTab[0x27] * delta_time * 3;
			playerRot = XMMatrixRotationY(transY);
		}
		player = XMMatrixTranslationFromVector(transVec);
		player = XMMatrixMultiply(playerRot, player);
		XMStoreFloat4x4(&player44, player);
		
		//drawPlayer
		end::debug_renderer::add_line(float3(player44._41, player44._42, player44._43), float3(player44._41 + player44._11, player44._42, player44._43 + player44._13), float4(1,0,0,1));//x (right)
		end::debug_renderer::add_line(float3(player44._41, player44._42, player44._43), float3(player44._41, player44._42 + 1.0f, player44._43), float4(0,1,0,1)); // y (up)
		end::debug_renderer::add_line(float3(player44._41, player44._42, player44._43), float3(player44._41 + player44._31, player44._42, player44._43 + player44._33), float4(0,0,1,1));// z (forward)

		
		//move player based on input
		playerFor = XMVector3Transform(forVec, playerRot); //gets a vector for the "forward" direction 
		XMStoreFloat3(&transVec3, playerFor);
		if (bitTab[0x26] == 1)
		{
			transVec += playerFor * delta_time * playerSpeed;
		}
		if (bitTab[0x28] == 1)
		{
			transVec -= playerFor * delta_time * playerSpeed;
		}
		//transY += 1.0 * delta_time;
		//draws lookat
		//lookAt = XMMatrixTranslation(-5.0f, 2.0f, -5.0f);
		XMVECTOR lookAtPos = XMVectorSet(-5.0f, 2.0f, -5.0f, 1);
		playerPos = XMVectorSet(player44._41, player44._42, player44._43, 0);
	
		lookAt = LookAt(lookAtPos, playerPos, upVec);
		XMStoreFloat4x4(&lookAt44, lookAt);
		end::debug_renderer::add_line(float3(lookAt44._41, lookAt44._42, lookAt44._43), float3(lookAt44._41 + lookAt44._11, lookAt44._42 + lookAt44._21, lookAt44._43 + lookAt44._31), float4(1, 0, 0, 1));
		end::debug_renderer::add_line(float3(lookAt44._41, lookAt44._42, lookAt44._43), float3(lookAt44._41 + lookAt44._12, lookAt44._42 + lookAt44._22, lookAt44._43 + lookAt44._32), float4(0, 1, 0, 1));
		end::debug_renderer::add_line(float3(lookAt44._41, lookAt44._42, lookAt44._43), float3(lookAt44._41 + lookAt44._13, lookAt44._42 + lookAt44._23, lookAt44._43 + lookAt44._33), float4(0, 0, 1, 1));

		//draws turnto
		//turnTo = XMMatrixTranslation(5.0f, 2.0f, 5.0f);
		XMVECTOR turnToPos = XMVectorSet(5.0f, 2.0f, 5.0f, 1.0f);
		turnTo = TurnTo(turnTo, playerPos, delta_time);
		XMStoreFloat4x4(&turnTo44, turnTo);
		end::debug_renderer::add_line(float3(turnTo44._41, turnTo44._42, turnTo44._43), float3(turnTo44._41 + turnTo44._11, turnTo44._42 + turnTo44._12, turnTo44._43 + turnTo44._13), float4(1, 0, 0, 1));
		end::debug_renderer::add_line(float3(turnTo44._41, turnTo44._42, turnTo44._43), float3(turnTo44._41 + turnTo44._21, turnTo44._42 + turnTo44._22, turnTo44._43 + turnTo44._23), float4(0, 1, 0, 1));
		end::debug_renderer::add_line(float3(turnTo44._41, turnTo44._42, turnTo44._43), float3(turnTo44._41 + turnTo44._31, turnTo44._42 + turnTo44._32, turnTo44._43 + turnTo44._33), float4(0, 0, 1, 1));



#pragma region cameraMovement
		//mouse movement
		XMStoreFloat4(&camPos4, camPos);
		if (inputPoint[0] != xPos || inputPoint[1] != yPos)
		{
			xPos -= inputPoint[0];
			yPos -= inputPoint[1];
			yPos = -yPos;
			xPos = -xPos;
			float totalYaw = xPos * delta_time;
			//xMouseMat = XMMatrixRotationY(totalYaw);
			//viewHolder = XMMatrixMultiply(xMouseMat, *(XMMATRIX*)(&viewM.view_mat));

			float totalPitch = yPos * delta_time;
			//yMouseMat = XMMatrixRotationX(totalPitch);
			//xMouseMat = XMMatrixRotationRollPitchYaw(totalPitch, totalYaw, 0);
			XMMATRIX xRotation = XMMatrixRotationX(totalPitch);
			XMMATRIX yRotation = XMMatrixRotationY(totalYaw);
			XMFLOAT4X4 yRot44;
			XMStoreFloat4x4(&yRot44, yRotation);
			yRot44._41 = 0;
			yRot44._42 = 0;
			yRot44._43 = 0;

			totalCamRot = XMMatrixMultiply(xRotation, yRotation);
			
			viewHolder = XMMatrixMultiply(*(XMMATRIX*)(&viewM.view_mat), *(XMMATRIX*)(&yRot44));
			viewHolder = XMMatrixMultiply(xRotation, viewHolder);

			xPos = inputPoint[0];
			yPos = inputPoint[1];

			viewM.view_mat = *(float4x4_a*)(&viewHolder);
		}

		//wsad movement
		XMVECTOR forward;
		XMVECTOR right;
		right = *(XMVECTOR*)(&viewM.view_mat[0]);
		XMFLOAT3 cameraTrans3;
		XMFLOAT4X4 camHold = *(XMFLOAT4X4*)(&viewM.view_mat);
		camHold._41 = 0;
		camHold._42 = 0;
		camHold._43 = 0;
		XMMATRIX cam;
		//forward = XMVector3Transform({ 0,0,1 }, xMouseMat); //gets a vector for the "forward" direction 
		forward = *(XMVECTOR*)(&viewM.view_mat[2]);
		XMStoreFloat3(&cameraTrans3, forward);
		if (bitTab['W'] == 1)
		{
			camPos += forward * delta_time * playerSpeed;
			viewM.view_mat[3].xyz = *(float3*)(&camPos);
		}
		if (bitTab['S'] == 1)
		{
			camPos -= forward * delta_time * playerSpeed;
			viewM.view_mat[3].xyz = *(float3*)(&camPos);
		}
		if (bitTab['D'] == 1)
		{
			camPos += right * delta_time * playerSpeed;
			viewM.view_mat[3].xyz = *(float3*)(&camPos);
		}
		if (bitTab['A'] == 1)
		{
			camPos -= right * delta_time * playerSpeed;
			viewM.view_mat[3].xyz = *(float3*)(&camPos);
		}
#pragma endregion

		

		//Test to make sure it works and it doesss 
		//if (inputPoint[0] != xPos || inputPoint[1] != yPos)
		//{
		//	//get the difference between current and previous pos
		//	// - if moved to the left positive to the right
		//	xPos -= inputPoint[0];
		//	//get a matrix copy of the view
		//	//xMouseMat = *(XMMATRIX*)(&viewM.view_mat);
		//	//modifiable copy of the view 
		//	XMMATRIX viewHolder = *(XMMATRIX*)(&viewM.view_mat);
		//	//rotate by time (only yaw)
		//	xMouseMat = XMMatrixRotationRollPitchYaw(0, xPos * delta_time, 0);
		//	//combine the previous and the new rotates view matrix
		//	xMouseMat = XMMatrixMultiply(viewHolder, xMouseMat);
		//	//set the view reference to new rotated view
		//	viewM.view_mat = *(float4x4_a*)(&xMouseMat);

		//	//reset the xpos to current pos
		//	xPos = inputPoint[0];


		//}
		//if (inputPoint[1] != yPos)
		//{
		//	//get the difference between current and previous pos
		//	// - if moved to the up positive down
		//	yPos -= inputPoint[1];
		//	yPos = -yPos;
		//	//get a matrix copy of the view
		//	//yMouseMat = *(XMMATRIX*)(&viewM.view_mat);
		//	//modifiable copy of the view 
		//	XMMATRIX viewHolder = *(XMMATRIX*)(&viewM.view_mat);
		//	//rotate by time (only pitch)
		//	yMouseMat = XMMatrixRotationRollPitchYaw(yPos * delta_time, 0, 0);
		//	//combine the previous and the new rotates view matrix
		//	yMouseMat = XMMatrixMultiply(viewHolder, yMouseMat);
		//	//set the view reference to new rotated view
		//	viewM.view_mat = *(float4x4_a*)(&yMouseMat);

		//	//reset the xpos to current pos
		//	yPos = inputPoint[1];
		//}
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
