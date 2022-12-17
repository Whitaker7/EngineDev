#include "bvh.h"

namespace end
{

	//std::vector<bvh_node_t> nodes;
	bvh_node_t::bvh_node_t(bvh_node_t* root, uint32_t left_index, uint32_t right_index)
	{
		//TODO The root pointer is the array of the entire bvh
		root->left() == left_index;
		root->right() == right_index;
		//Build the aabb that encapsulates the left and right's aabb's
	}

	float bounding_volume_hierarchy_t::cost(const bvh_node_t& a, const bvh_node_t& b)
	{
		//TODO: calculate manhattan distance from the center points of the two aabb's
		float x1 = std::abs(a.aabb().center.x);
		float y1 = std::abs(a.aabb().center.y);
		float z1 = std::abs(a.aabb().center.z);
		float x2 = std::abs(b.aabb().center.x);
		float y2 = std::abs(b.aabb().center.y);
		float z2 = std::abs(b.aabb().center.z);

		float finalx = x1 + x2;
		float finaly = y1 + y2;
		float finalz = z1 + z2;

		float output = finalx + finaly + finalz;

		return output;
	}

	void IncreaseBounds(AABB aabb, bvh_node_t& node)
	{
		//first see if resizing is needed
		if (std::abs(aabb.center.x - node.aabb().center.x) <= ((aabb.xyz[0] * 0.5f) + (node.aabb().xyz[0] * 0.5f)) &&
			std::abs(aabb.center.y - node.aabb().center.y) <= ((aabb.xyz[1] * 0.5f) + (node.aabb().xyz[1] * 0.5f)) &&
			std::abs(aabb.center.z - node.aabb().center.z) <= ((aabb.xyz[2] * 0.5f) + (node.aabb().xyz[2] * 0.5f)))
		{
			//aabb.color = float4(1.0f, 0.0f, 0, 1.0f);
			return;
		}
		//birds eye view
		//need to move from center to bottom wall (x)
		//need to check if the aabb is to the left or to the right of the nodeaabb center
		float xResult;
		float yResult;
		float zResult;
		float x;
		float y;
		float z;
		bool right = true;
		bool infront = true;
		bool above = true;
		//this determins the x bounds of the aabb
		if (node.aabb().center.x >= aabb.center.x) // if node is to the right of aabb
		{
			x = node.aabb().center.x + (node.aabb().xyz[0] * 0.5f); // right wall of aabb of node
			float x2 = aabb.center.x - (aabb.xyz[0] * 0.5); // left wall of aabb of node
			xResult = std::abs(x2 - x);
			right = true;
		}
		else // if node is to the left of aabb
		{
			x = node.aabb().center.x - (node.aabb().xyz[0] * 0.5f); //left wall
			float x2 = aabb.center.x + (aabb.xyz[0] * 0.5); // right wall
			xResult = std::abs(x2 - x);
			right = false;
		}



		//this determins the y bounds
		if (node.aabb().center.y <= aabb.center.y) // if node below the aabb
		{
			y = node.aabb().center.y - (node.aabb().xyz[1] * 0.5f); // bottom wall of aabb of node
			float y2 = aabb.center.y + (aabb.xyz[1] * 0.5);// top wall of aabb of node
			yResult = std::abs(y2 - y);
			above = false;
		}
		else // if node is above the aabb
		{
			y = node.aabb().center.y + (node.aabb().xyz[1] * 0.5f); // top wall of aabb of node
			float y2 = aabb.center.y - (aabb.xyz[1] * 0.5); // bottom wall
			yResult = std::abs(y2 - y);
			above = true;
		}



		//this determins the z bounds
		if (node.aabb().center.z <= aabb.center.z) // if node behind the aabb
		{
			z = node.aabb().center.z - (node.aabb().xyz[2] * 0.5f); // bottom wall of aabb of node
			float z2 = aabb.center.z + (aabb.xyz[2] * 0.5);// top wall of aabb of node
			zResult = std::abs(z2 - z);
			infront = false;
		}
		else // if node is infront the aabb
		{
			z = node.aabb().center.z + (node.aabb().xyz[2] * 0.5f); // top wall of aabb of node
			float z2 = aabb.center.z - (aabb.xyz[2] * 0.5); // bottom wall
			zResult = std::abs(z2 - z);
			infront = true;
		}

		//if aabb is further away than the width of the nodesaabb update nodes width
		if (xResult > node.aabb().xyz[0])
		{
			//if the aabb is further away from the node than the nodes aabb width. we need to expand the width to inclue the aabb
			node.aabb().xyz[0] = xResult;
		}

		//if aabb is further away than the height of the nodes aabb then update the nodes height
		if (yResult > node.aabb().xyz[1])
		{
			node.aabb().xyz[1] = 6;
		}

		//if aabb is further away then the debpth of the nodes aabb then update the nodes depth
		if (zResult > node.aabb().xyz[2])
		{
			node.aabb().xyz[2] = zResult;
		}



		//now from the outside edge. in by half the width is the new center.
		if (infront)
		{
			node.aabb().center.z = z - (node.aabb().xyz[2] * 0.5f);
		}
		else
		{
			node.aabb().center.z = z + (node.aabb().xyz[2] * 0.5f);
		}
		if (!above)
		{
			node.aabb().center.y = y + (node.aabb().xyz[1] * 0.5f);
		}
		else
		{
			node.aabb().center.y = y - (node.aabb().xyz[1] * 0.5f);
		}
		if (right)
		{
			node.aabb().center.x = x - (node.aabb().xyz[0] * 0.5f);
		}
		else
		{
			node.aabb().center.x = x + (node.aabb().xyz[0] * 0.5f);
		}

	}

	void bounding_volume_hierarchy_t::insert(const AABB& aabb, uint32_t element_id)
	{
		//TODO
		//create a bvh node using the passed in parameters(do not call new)
		bvh_node_t node = bvh_node_t(aabb, element_id);

		//TODO
		//if its the first node, it becomes the root. So just push it into bvh vector, then return
		if (bvh.size() == 0)
		{
			node.is_root();
			bvh.push_back(node);
			bool test = node.is_branch();
			return;
		}


		//TODO
		//start at index 0 of the bvh (root)
		uint32_t index = 0;
		bvh_node_t nodeCopy;
		//while the current node is a branch, 
		while (bvh[index].is_branch())
		{
			//make a copy of current node to hold incase i need it later
			nodeCopy = bvh[index];
			//Modify this node's aabb that encapsulates the current node, and the aabb that was passed in
			// if index node is not already within index aabb incread the size of the index aabb

			//modifies bounds if incoming aabb is outside of nodes aabb
			IncreaseBounds(node.aabb(), bvh[index]);

			//will need to make sure the left and right indices are valid
			if (bvh.size() == 1)
			{
				break;
			}

			
			float leftDis = bounding_volume_hierarchy_t::cost(bvh[bvh[index].left()], node);
			float rightDis = bounding_volume_hierarchy_t::cost(bvh[bvh[index].right()], node);
			//change your index to be the one you have chosen.
			if (leftDis <= rightDis)// if it is closer to the left index go down that branch
			{
				index = bvh[index].left();
			}
			else
			{
				index = bvh[index].right();
			}
			//index++;
		}
		

		//TODO
		//Once you have left that while loop, you now hold onto a leaf node index where we will add the node to
		node.left() = 0;
		nodeCopy.left() = 0;
		bvh.push_back(node); bvh.push_back(nodeCopy);
		//the 2 new nodes' indices(left and right child) will be the newly created indices once you push back twice on the bvh vector
		bvh[index].left() = bvh.size() - 1;
		bvh[index].right() = bvh.size() - 2;
		//the current node (who is now a branch) needs to be sized to fit the two new nodes(left and right, remember don't need to call new).
		
		//set the parents of these two nodes
		bvh[bvh.size() - 1].parent() = index;
		bvh[bvh.size() - 2].parent() = index;

		

	}
}