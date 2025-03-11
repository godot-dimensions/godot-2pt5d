#pragma once

#include "node_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/line2d.hpp>
#elif GODOT_MODULE
#include "scene/2d/line_2d.h"
#endif

class Line25D : public Node25D {
	GDCLASS(Line25D, Node25D);

	Line2D *_line_2d = nullptr;
	PackedVector3Array _points_3d;
	bool _use_node_transform = true;

	void _update_points_2d();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	Line2D *get_line_2d() const { return _line_2d; }

	Vector3 get_point_position(const int p_index) const;
	void set_point_position(const int p_index, const Vector3 &p_position);
	int get_point_count() const;
	void add_point(const Vector3 &p_position, const int p_at_index = -1);
	void remove_point(const int p_index);
	void clear_points();

	// Main properties.
	PackedVector3Array get_points_3d() const;
	void set_points_3d(const PackedVector3Array &p_points_3d);
	bool get_is_closed() const;
	void set_is_closed(const bool p_closed);
	bool get_use_node_transform() const { return _use_node_transform; }
	void set_use_node_transform(const bool p_use_transform) { _use_node_transform = p_use_transform; }
	float get_width() const;
	void set_width(const float p_width);
	Color get_default_color() const;
	void set_default_color(const Color &p_color);

	// Capping.
	Line2D::LineJointMode get_joint_mode() const;
	void set_joint_mode(Line2D::LineJointMode mode);
	Line2D::LineCapMode get_begin_cap_mode() const;
	void set_begin_cap_mode(Line2D::LineCapMode mode);
	Line2D::LineCapMode get_end_cap_mode() const;
	void set_end_cap_mode(Line2D::LineCapMode mode);

	Line25D();
};
