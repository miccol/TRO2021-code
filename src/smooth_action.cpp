/* Copyright (C) 2019-2020 Michele Colledanchise - All Rights Reserved
*
*   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
*   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
*   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
*   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
*   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <smooth_action.h>
#include <behaviortree_cpp_v3/behavior_tree.h>

#include <string>
#include <algorithm>
#include <iomanip>      // std::setprecision
#include <chrono>
using namespace BT;
SmoothAction::SmoothAction() : progress_(0.0), paused_(false)
{

}

SmoothAction::~SmoothAction() {}

double SmoothAction::progress() const
{
    return progress_;
}

void SmoothAction::set_progress(const double progress)
{
    std::setprecision(5);

    progress_ = progress;
}

bool SmoothAction::paused() const
{
    return paused_;
}

void SmoothAction::set_paused(bool paused)
{
    paused_ = paused;
}

//std::vector<string> SmoothAction::resources_locked_at_running() const
//{
//    return resources_allocated_;
//}

//void SmoothAction::setResources_locked_at_running(const std::vector<string> &value)
//{
//    resources_allocated_ = value;
//}

void SmoothAction::allocateResource(const string resource)
{
    resources_allocated_.push_back(resource);
}

void SmoothAction::releaseResource(const string resource)
{
    resources_allocated_.erase(std::remove(resources_allocated_.begin(), resources_allocated_.end(), resource), resources_allocated_.end());
}

std::vector<string> SmoothAction::resources_allocated() const
{
    return resources_allocated_;
}

void SmoothAction::allocateResources(const std::vector<string> resources)
{
    for(auto resource : resources)
    {
        resources_allocated_.push_back(resource);
    }
}

void SmoothAction::releaseResources(const std::vector<string> resources)
{
    for(auto resource : resources)
    {
        resources_allocated_.erase(std::remove(resources_allocated_.begin(), resources_allocated_.end(), resource), resources_allocated_.end());
    }
}

void SmoothAction::set_time(std::chrono::steady_clock::time_point time)
{
    time_ = time;
}

std::chrono::steady_clock::time_point SmoothAction::time() const
{
    return time_;
}
