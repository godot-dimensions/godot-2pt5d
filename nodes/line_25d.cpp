#include "line_25d.h"

void Line25D::_notification(int p_what) {
	Node25D::_notification(p_what);
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE:
		case NOTIFICATION_PROCESS: {
			update_points_2d();
		} break;
	}
}

Vector3 Line25D::get_point_position(const int p_index) const {
	ERR_FAIL_INDEX_V(p_index, _points_3d.size(), Vector3());
	return _points_3d[p_index];
}

void Line25D::set_point_position(const int p_index, const Vector3 &p_position) {
	ERR_FAIL_INDEX(p_index, _points_3d.size());
	_points_3d.set(p_index, p_position);
	update_points_2d();
}

int Line25D::get_point_count() const {
	return _points_3d.size();
}

void Line25D::add_point(const Vector3 &p_position, const int p_at_index) {
	if (p_at_index == -1) {
		_points_3d.push_back(p_position);
	} else {
		_points_3d.insert(p_at_index, p_position);
	}
	update_points_2d();
}

void Line25D::remove_point(const int p_index) {
	ERR_FAIL_INDEX(p_index, _points_3d.size());
	_points_3d.remove_at(p_index);
	update_points_2d();
}

void Line25D::clear_points() {
	_points_3d.clear();
	_line_2d->clear_points();
}

void Line25D::update_points_2d() {
	const Ref<World25D> world = get_world_25d();
	if (world.is_null()) {
		return;
	}
	PackedVector2Array points_2d;
	points_2d.resize(_points_3d.size());
	if (_use_node_transform) {
		const Transform2D global_transform_2d_inv = get_global_transform_2d().affine_inverse();
		const Transform3D global_transform_3d = get_global_transform_3d();
		for (int i = 0; i < _points_3d.size(); i++) {
			points_2d.set(i, global_transform_2d_inv.xform(world->xform_3d_to_2d(global_transform_3d.xform(_points_3d[i]))));
		}
	} else {
		for (int i = 0; i < _points_3d.size(); i++) {
			points_2d.set(i, world->xform_3d_to_2d(_points_3d[i]));
		}
	}
	_line_2d->set_points(points_2d);
}

// Main properties.

PackedVector3Array Line25D::get_points_3d() const {
	return _points_3d;
}

void Line25D::set_points_3d(const PackedVector3Array &p_points) {
	_points_3d = p_points;
	update_points_2d();
}

bool Line25D::get_is_closed() const {
	return _line_2d->is_closed();
}

void Line25D::set_is_closed(const bool p_closed) {
	_line_2d->set_closed(p_closed);
}

float Line25D::get_width() const {
	return _line_2d->get_width();
}

void Line25D::set_width(const float p_width) {
	_line_2d->set_width(p_width);
}

Color Line25D::get_default_color() const {
	return _line_2d->get_default_color();
}

void Line25D::set_default_color(const Color &p_color) {
	_line_2d->set_default_color(p_color);
}

// Capping.

Line2D::LineJointMode Line25D::get_joint_mode() const {
	return _line_2d->get_joint_mode();
}

void Line25D::set_joint_mode(Line2D::LineJointMode mode) {
	_line_2d->set_joint_mode(mode);
}

Line2D::LineCapMode Line25D::get_begin_cap_mode() const {
	return _line_2d->get_begin_cap_mode();
}

void Line25D::set_begin_cap_mode(Line2D::LineCapMode mode) {
	_line_2d->set_begin_cap_mode(mode);
}

Line2D::LineCapMode Line25D::get_end_cap_mode() const {
	return _line_2d->get_end_cap_mode();
}

void Line25D::set_end_cap_mode(Line2D::LineCapMode mode) {
	_line_2d->set_end_cap_mode(mode);
}

Line25D::Line25D() {
	_line_2d = memnew(Line2D);
	add_child(_line_2d);
	set_process(true);
}

void Line25D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_line_2d"), &Line25D::get_line_2d);

	ClassDB::bind_method(D_METHOD("get_point_position", "index"), &Line25D::get_point_position);
	ClassDB::bind_method(D_METHOD("set_point_position", "index", "position"), &Line25D::set_point_position);
	ClassDB::bind_method(D_METHOD("get_point_count"), &Line25D::get_point_count);
	ClassDB::bind_method(D_METHOD("add_point", "position", "at_index"), &Line25D::add_point, DEFVAL(-1));
	ClassDB::bind_method(D_METHOD("remove_point", "index"), &Line25D::remove_point);
	ClassDB::bind_method(D_METHOD("clear_points"), &Line25D::clear_points);
	ClassDB::bind_method(D_METHOD("update_points_2d"), &Line25D::update_points_2d);

	// Main properties.
	ClassDB::bind_method(D_METHOD("get_points_3d"), &Line25D::get_points_3d);
	ClassDB::bind_method(D_METHOD("set_points_3d", "points"), &Line25D::set_points_3d);
	ADD_PROPERTY(PropertyInfo(Variant::PACKED_VECTOR3_ARRAY, "points_3d"), "set_points_3d", "get_points_3d");

	ClassDB::bind_method(D_METHOD("get_is_closed"), &Line25D::get_is_closed);
	ClassDB::bind_method(D_METHOD("set_is_closed", "closed"), &Line25D::set_is_closed);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_closed"), "set_is_closed", "get_is_closed");

	ClassDB::bind_method(D_METHOD("get_use_node_transform"), &Line25D::get_use_node_transform);
	ClassDB::bind_method(D_METHOD("set_use_node_transform", "use_transform"), &Line25D::set_use_node_transform);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "use_node_transform"), "set_use_node_transform", "get_use_node_transform");

	ClassDB::bind_method(D_METHOD("get_width"), &Line25D::get_width);
	ClassDB::bind_method(D_METHOD("set_width", "width"), &Line25D::set_width);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "width"), "set_width", "get_width");

	ClassDB::bind_method(D_METHOD("get_default_color"), &Line25D::get_default_color);
	ClassDB::bind_method(D_METHOD("set_default_color", "color"), &Line25D::set_default_color);
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "default_color"), "set_default_color", "get_default_color");

	// Capping.
	ClassDB::bind_method(D_METHOD("get_joint_mode"), &Line25D::get_joint_mode);
	ClassDB::bind_method(D_METHOD("set_joint_mode", "mode"), &Line25D::set_joint_mode);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "joint_mode", PROPERTY_HINT_ENUM, "Sharp,Bevel,Round"), "set_joint_mode", "get_joint_mode");

	ClassDB::bind_method(D_METHOD("get_begin_cap_mode"), &Line25D::get_begin_cap_mode);
	ClassDB::bind_method(D_METHOD("set_begin_cap_mode", "mode"), &Line25D::set_begin_cap_mode);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "begin_cap_mode", PROPERTY_HINT_ENUM, "None,Box,Round"), "set_begin_cap_mode", "get_begin_cap_mode");

	ClassDB::bind_method(D_METHOD("get_end_cap_mode"), &Line25D::get_end_cap_mode);
	ClassDB::bind_method(D_METHOD("set_end_cap_mode", "mode"), &Line25D::set_end_cap_mode);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "end_cap_mode", PROPERTY_HINT_ENUM, "None,Box,Round"), "set_end_cap_mode", "get_end_cap_mode");
}
