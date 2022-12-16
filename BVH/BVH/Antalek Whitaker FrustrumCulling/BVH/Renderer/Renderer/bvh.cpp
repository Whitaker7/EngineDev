#include "bvh.h"

namespace end
{
	bvh_node_t::bvh_node_t(bvh_node_t* root, uint32_t left_index, uint32_t right_index)
	{
		//TODO The root pointer is the array of the entire bvh
		//Build the aabb that encapsulates the left and right's aabb's
	}

	float bounding_volume_hierarchy_t::cost(const bvh_node_t& a, const bvh_node_t& b)
	{
		//TODO: calculate manhattan distance from the center points of the two aabb's

		return 0;
	}

	void bounding_volume_hierarchy_t::insert(const AABB& aabb, uint32_t element_id)
	{
		//TODO
		//create a bvh node using the passed in parameters(do not call new)

		//TODO
		//if its the first node, it becomes the root. So just push it into bvh vector, then return
		
		//TODO
		//start at index 0 of the bvh (root)
		//while the current node is a branch, 
		//Modify this node's aabb that encapsulates the current node, and the aabb that was passed in
		//figure out if you are going with the left or right child
		//change your index to be the one you have chosen.

		//TODO
		//Once you have left that while loop, you now hold onto a leaf node index where we will add the node to
		//the 2 new nodes' indices(left and right child) will be the newly created indices once you push back twice on the bvh vector
		//the current node (who is now a branch) needs to be sized to fit the two new nodes(left and right, remember don't need to call new).
		//set the parents of these two nodes
	}
}