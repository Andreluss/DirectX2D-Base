#pragma once
#include "common.h"

class Transform
{
public:
    D2D1_POINT_2F position{ 0, 0 };
    // Scale factor for the object.
    // Needs to be applied to the object's size when drawing.
    float scale{ 1 };
};

