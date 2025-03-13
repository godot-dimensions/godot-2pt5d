#include "camera_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/viewport.hpp>
#elif GODOT_MODULE
#include "scene/main/viewport.h"
#endif

void Camera25D::make_current() {
	_camera_2d->set_enabled(true);
	_camera_2d->make_current();
}

bool Camera25D::get_is_enabled() const {
	return _camera_2d->is_enabled();
}

void Camera25D::set_is_enabled(bool value) {
	_camera_2d->set_enabled(value);
}

Vector2 Camera25D::get_zoom() const {
	return _camera_2d->get_zoom();
}

void Camera25D::set_zoom(const Vector2 &p_zoom) {
	_camera_2d->set_zoom(p_zoom);
}

Vector2 Camera25D::viewport_to_world_2d(const Vector2 &p_viewport_point) const {
	const Vector2 viewport_size = get_viewport()->call(StringName("get_size"));
	const Vector2 zoom = get_zoom();
	const Vector2 local_vp = (p_viewport_point - viewport_size * 0.5f) / zoom;
	const Transform2D global_transform_2d = get_global_transform_2d();
	if (_camera_2d->is_ignoring_rotation()) {
		return local_vp + global_transform_2d.get_origin();
	}
	return global_transform_2d.xform(local_vp);
}

Vector2 Camera25D::world_to_viewport_2d(const Vector2 &p_global_world_point) const {
	const Transform2D global_transform_2d = get_global_transform_2d();
	Vector2 local_vp;
	if (_camera_2d->is_ignoring_rotation()) {
		local_vp = p_global_world_point - global_transform_2d.get_origin();
	} else {
		local_vp = global_transform_2d.affine_inverse().xform(p_global_world_point);
	}
	const Vector2 viewport_size = get_viewport()->call(StringName("get_size"));
	const Vector2 zoom = get_zoom();
	return local_vp * zoom + viewport_size * 0.5f;
}

Vector3 Camera25D::viewport_to_world_3d(const Vector2 &p_viewport_point) const {
	const Ref<World25D> &world = get_world_25d();
	if (world.is_null()) {
		return Vector3();
	}
	const Vector2 world_point = viewport_to_world_2d(p_viewport_point);
	return world->xform_inv_2d_to_3d(world_point);
}

Vector2 Camera25D::world_to_viewport_3d(const Vector3 &p_global_point) const {
	const Ref<World25D> &world = get_world_25d();
	if (world.is_null()) {
		return Vector2();
	}
	const Vector2 world_point_2d = world->xform_3d_to_2d(p_global_point);
	return world_to_viewport_2d(world_point_2d);
}

Camera25D::Camera25D() {
	_camera_2d = memnew(Camera2D);
	add_child(_camera_2d);
}

void Camera25D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_camera_2d"), &Camera25D::get_camera_2d);
	ClassDB::bind_method(D_METHOD("make_current"), &Camera25D::make_current);

	ClassDB::bind_method(D_METHOD("get_is_enabled"), &Camera25D::get_is_enabled);
	ClassDB::bind_method(D_METHOD("set_is_enabled", "value"), &Camera25D::set_is_enabled);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_enabled"), "set_is_enabled", "get_is_enabled");

	ClassDB::bind_method(D_METHOD("get_zoom"), &Camera25D::get_zoom);
	ClassDB::bind_method(D_METHOD("set_zoom", "zoom"), &Camera25D::set_zoom);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "zoom", PROPERTY_HINT_LINK), "set_zoom", "get_zoom");

	ClassDB::bind_method(D_METHOD("viewport_to_world_2d", "point"), &Camera25D::viewport_to_world_2d);
	ClassDB::bind_method(D_METHOD("world_to_viewport_2d", "point"), &Camera25D::world_to_viewport_2d);

	ClassDB::bind_method(D_METHOD("viewport_to_world_3d", "point"), &Camera25D::viewport_to_world_3d);
	ClassDB::bind_method(D_METHOD("world_to_viewport_3d", "point"), &Camera25D::world_to_viewport_3d);
}
