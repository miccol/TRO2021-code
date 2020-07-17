#include <resource_handler.h>
#include <mutex>
#include <iostream>
#include <behaviortree_cpp_v3/tree_node.h>

using namespace std;
using namespace BT;
ResourceHandler::ResourceHandler(incFuncType iFunction)
{
incrementFunction = iFunction;
}

bool ResourceHandler::allocateResource(TreeNode* tree_node_ptr, const vector<string> resources)
{



    const std::lock_guard<std::mutex> lock(resources_mutex);

    for (auto itr = resources_allocated_.begin(); itr != resources_allocated_.end(); itr++)
    {
        if(nodes_allocating_[itr->first]==tree_node_ptr)
        itr->second = false;
    }

    for (string resource : resources)
    {


        if(nodes_allocating_[resource]!=tree_node_ptr &&
                resources_allocated_.count(resource) == 1 &&
                resources_allocated_.at(resource))
        {
            return false;
        }

    }

    for (string resource : resources)
    {
        nodes_allocating_[resource]=tree_node_ptr;
        resources_allocated_[resource] = true;

    }
    return true;
}

bool ResourceHandler::releaseResource(const string resource)
{
    const std::lock_guard<std::mutex> lock(resources_mutex);
    resources_allocated_[resource] = false;
    // cannot fail for now
    return true;
}

bool ResourceHandler::releaseResources(const vector<string> resources)
{
    const std::lock_guard<std::mutex> lock(resources_mutex);
    for (auto resource : resources)
    {
        resources_allocated_[resource] = false;
    }
    return true;
}




bool ResourceHandler::areResourcesAvailable(TreeNode* tree_node_ptr, const vector<string> resources)
{
    const std::lock_guard<std::mutex> lock(resources_mutex);
    for (string resource : resources)
    {
        if (resources_allocated_[resource] && (nodes_allocating_[resource]!= tree_node_ptr))
        {
            return false;
        }
    }
    return true;
}

double ResourceHandler::max_priority()
{
        double currentMax = 0;

        auto x = std::max_element(nodes_priorities_.begin(), nodes_priorities_.end(),
            [](const pair<BT::TreeNode*, double>& p1, const pair<BT::TreeNode*, double>& p2) {
                return p1.second < p2.second; });


        return x->second;
}

void ResourceHandler::increasePritority(TreeNode *node)
{
    nodes_priorities_[node] = incrementFunction(nodes_priorities_[node]);
}

void ResourceHandler::addNode(TreeNode *node)
{
    nodes_priorities_[node] = 0.0;
}

double ResourceHandler::priorityOf(TreeNode *child)
{
    return nodes_priorities_[child];
}
