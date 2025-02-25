#pragma once

#include "node_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/camera2d.hpp>
#elif GODOT_MODULE
#include "scene/2d/camera_2d.h"
#endif

class Camera25D : public Node25D {
	GDCLASS(Camera25D, Node25D);

	Camera2D *_camera_2d = nullptr;

protected:
	static void _bind_methods();

public:
	Camera2D *get_camera_2d() const { return _camera_2d; }
	void make_current();

	bool get_is_enabled() const;
	void set_is_enabled(bool value);

	Vector2 get_zoom() const;
	void set_zoom(const Vector2 &p_zoom);

	Camera25D();
};
