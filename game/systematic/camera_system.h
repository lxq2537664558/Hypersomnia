#pragma once

class cosmos;
class fixed_step;

class camera_system {
public:
	void react_to_input_intents(fixed_step& step);
	void resolve_cameras_transforms_and_smoothing(fixed_step& step);
};