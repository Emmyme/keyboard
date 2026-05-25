#pragma once

#include "tracking/HandTypes.h"

enum class Pose { None, Point, PinchIndex, OpenPalm };

Pose classifyPose(const HandLandmarks &hand);
