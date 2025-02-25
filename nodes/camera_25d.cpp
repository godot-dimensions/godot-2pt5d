#include "camera_25d.h"

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
}
