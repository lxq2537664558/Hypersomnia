#pragma once
#include "game/detail/world_camera.h"
#include "augs/misc/variable_delta_timer.h"
#include "augs/misc/measurements.h"

class viewing_session {
public:
	world_camera camera;

	augs::variable_delta_timer frame_timer;
	augs::measurements fps_profiler = augs::measurements(L"Frame");
	augs::measurements triangles = augs::measurements(L"Triangles", false);
};