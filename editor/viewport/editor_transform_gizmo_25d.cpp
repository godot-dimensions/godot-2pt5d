#include "editor_transform_gizmo_25d.h"

#include "editor_main_viewport_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/editor_inspector.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#elif GODOT_MODULE
#include "editor/editor_inspector.h"
#include "editor/editor_interface.h"
#endif

constexpr real_t GIZMO_25D_LINE_LENGTH_3D = 1.5;
constexpr real_t GIZMO_25D_LINE_THICKNESS_2D = 6.0;
constexpr int GIZMO_25D_RING_SEGMENTS = 64;

Line25D *EditorTransformGizmo25D::_make_line(const StringName &p_name, const Color &p_color, const Vector3 &p_point_3d) {
	Line25D *line = memnew(Line25D);
	line->set_name(p_name);
	PackedVector3Array points_3d = { Vector3(0.0, 0.0, 0.0), p_point_3d };
	line->set_points_3d(points_3d);
	line->set_width(GIZMO_25D_LINE_THICKNESS_2D);
	line->set_default_color(p_color);
	line->set_meta(StringName("original_color"), p_color);
	_line_holder->add_child(line);
	return line;
}

// This function is only designed to be called once.
Line25D *EditorTransformGizmo25D::_make_ring_2d(const StringName &p_name, const Color &p_color) {
	Line25D *rotation_line_2d = memnew(Line25D);
	rotation_line_2d->set_name(p_name);
	rotation_line_2d->set_width(GIZMO_25D_LINE_THICKNESS_2D);
	rotation_line_2d->set_default_color(p_color);
	rotation_line_2d->set_meta(StringName("original_color"), p_color);
	// Also create a screen-space 2D ring for the 2D view.
	_rotation_ring_2d = memnew(Line2D);
	PackedVector2Array rotate_points_2d;
	rotate_points_2d.resize(GIZMO_25D_RING_SEGMENTS + 3);
	for (int i = 1; i < GIZMO_25D_RING_SEGMENTS + 3; i++) {
		const real_t angle = Math_TAU * real_t(i - 1) / GIZMO_25D_RING_SEGMENTS;
		rotate_points_2d.set(i, Vector2(Math::sin(angle) * 64, Math::cos(angle) * -64));
	}
	_rotation_ring_2d->set_points(rotate_points_2d);
	_rotation_ring_2d->set_width(GIZMO_25D_LINE_THICKNESS_2D);
	_rotation_ring_2d->set_meta(StringName("original_color"), p_color);
	_rotation_ring_2d->set_default_color(p_color);
	rotation_line_2d->add_child(_rotation_ring_2d);
	_line_holder->add_child(rotation_line_2d);
	return rotation_line_2d;
}

Line25D *EditorTransformGizmo25D::_make_ring_3d(const StringName &p_name, const Color &p_color, const Vector3::Axis p_cos, const Vector3::Axis p_sin) {
	Line25D *ring_3d = memnew(Line25D);
	ring_3d->set_name(p_name);
	PackedVector3Array rotate_points_3d;
	rotate_points_3d.resize(GIZMO_25D_RING_SEGMENTS + 2);
	for (int i = 0; i < GIZMO_25D_RING_SEGMENTS + 2; i++) {
		const real_t angle = Math_TAU * real_t(i) / GIZMO_25D_RING_SEGMENTS;
		Vector3 point;
		point[p_cos] = Math::cos(angle) * GIZMO_25D_LINE_LENGTH_3D;
		point[p_sin] = Math::sin(angle) * GIZMO_25D_LINE_LENGTH_3D;
		rotate_points_3d.set(i, point);
	}
	Line2D *line_2d = memnew(Line2D);
	ring_3d->add_child(line_2d);
	ring_3d->set_points_3d(rotate_points_3d);
	ring_3d->set_width(GIZMO_25D_LINE_THICKNESS_2D);
	ring_3d->set_default_color(p_color);
	ring_3d->set_meta(StringName("original_color"), p_color);
	_line_holder->add_child(ring_3d);
	return ring_3d;
}

void EditorTransformGizmo25D::_generate_gizmo_lines(const PackedColorArray &p_axis_colors) {
	ERR_FAIL_NULL(_line_holder);
	// Create the linear move gizmo lines.
	_lines[TRANSFORM_MOVE_X] = _make_line(StringName("MoveX"), p_axis_colors[0], Vector3(GIZMO_25D_LINE_LENGTH_3D, 0.0, 0.0));
	_lines[TRANSFORM_MOVE_Y] = _make_line(StringName("MoveY"), p_axis_colors[1], Vector3(0.0, GIZMO_25D_LINE_LENGTH_3D, 0.0));
	_lines[TRANSFORM_MOVE_Z] = _make_line(StringName("MoveZ"), p_axis_colors[2], Vector3(0.0, 0.0, GIZMO_25D_LINE_LENGTH_3D));
	// Create the rotation gizmo lines and circles.
	_lines[TRANSFORM_ROTATE_2D] = _make_ring_2d(StringName("Rotate2D"), p_axis_colors[3]);
	_lines[TRANSFORM_ROTATE_X] = _make_ring_3d(StringName("RotateX"), p_axis_colors[0], Vector3::AXIS_Y, Vector3::AXIS_Z);
	_lines[TRANSFORM_ROTATE_Y] = _make_ring_3d(StringName("RotateY"), p_axis_colors[1], Vector3::AXIS_Z, Vector3::AXIS_X);
	_lines[TRANSFORM_ROTATE_Z] = _make_ring_3d(StringName("RotateZ"), p_axis_colors[2], Vector3::AXIS_X, Vector3::AXIS_Y);
	// Set the gizmo mode to set the visibility of these new meshes.
	set_gizmo_mode(GizmoMode::SELECT);
	_update_gizmo_transform();
}

void EditorTransformGizmo25D::_on_editor_inspector_property_edited(const String &p_prop) {
	_update_gizmo_transform();
}

void EditorTransformGizmo25D::_on_undo_redo_version_changed() {
	_update_gizmo_transform();
}

void EditorTransformGizmo25D::_update_gizmo_transform() {
	ERR_FAIL_NULL(_line_holder);
	int transform_count = 0;
	Transform3D sum_transform = Transform3D(Basis(0, 0, 0, 0, 0, 0, 0, 0, 0));
	const int size = _selected_top_nodes.size();
	Ref<World25D> new_world;
	for (int i = 0; i < size; i++) {
		Node25D *node_2pt5d = Object::cast_to<Node25D>(_selected_top_nodes[i]);
		if (node_2pt5d != nullptr) {
			Transform3D global_xform = node_2pt5d->get_global_transform_3d();
			_selected_top_node_old_transforms.set(i, global_xform);
			sum_transform.basis += global_xform.basis;
			sum_transform.origin += global_xform.origin;
			transform_count++;
			new_world = node_2pt5d->get_world_25d();
		}
	}
	set_world_25d_recursive(new_world);
	if (transform_count == 0) {
		set_visible(false);
	} else {
		set_transform_3d(sum_transform * (1.0f / real_t(transform_count)));
		_update_gizmo_line_transform();
		set_visible(true);
	}
}

void EditorTransformGizmo25D::_update_gizmo_line_transform() {
	ERR_FAIL_NULL(_lines[TRANSFORM_MOVE_X]);
	const real_t scale_num = _get_scale_number();
	for (int i = TRANSFORM_MOVE_X; i < TRANSFORM_MAX; i++) {
		_lines[i]->set_width(GIZMO_25D_LINE_THICKNESS_2D * scale_num);
	}
	const Basis scale_basis = Basis::from_scale(Vector3(scale_num, scale_num, scale_num));
	if (_is_use_local_transform) {
		const Basis rot = get_global_basis_3d().orthonormalized();
		_line_holder->set_global_basis_3d(rot * scale_basis);
	} else {
		_line_holder->set_global_basis_3d(scale_basis);
	}
	for (int i = TRANSFORM_MOVE_X; i < TRANSFORM_MAX; i++) {
		_lines[i]->update_points_2d();
	}
	if (_is_rotation_2d_enabled) {
		_rotation_ring_2d->set_scale(Vector2(scale_num, scale_num));
	}
}

bool EditorTransformGizmo25D::_should_use_3d_rotation() const {
	const Ref<World25D> world = get_world_25d();
	if (world.is_null()) {
		return false;
	}
	// If the Y axis (used for rotation) is perpendicular, we can't use 2D rotation.
	const Vector2 y_axis = world->get_basis().get_axis(Vector3::AXIS_Y);
	if (y_axis.is_zero_approx()) {
		return true;
	}
	// If some nodes use 3D rotation, the gizmo should use 3D rotation.
	for (int i = 0; i < _selected_top_nodes.size(); i++) {
		Node25D *node_25d = Object::cast_to<Node25D>(_selected_top_nodes[i]);
		if (node_25d != nullptr) {
			Node25D::RotationEditMode rot_edit_mode = node_25d->get_rotation_edit_mode();
			if (rot_edit_mode > Node25D::ROTATION_EDIT_MODE_2D_ANGLE) {
				return true;
			}
		}
	}
	return false;
}

Vector2 _get_closest_point_to_segment(const Vector2 &p_point, const Vector2 &p_segment_a, const Vector2 &p_segment_b) {
	const Vector2 relative_point = p_point - p_segment_a;
	const Vector2 segment_displacement = p_segment_b - p_segment_a;
	const real_t length_sq = segment_displacement.length_squared();
	const real_t distance = segment_displacement.dot(relative_point) / length_sq;
	if (distance >= 1.0f) {
		return p_segment_b; // After first point.
	} else if (distance > 0.0f) {
		return p_segment_a + segment_displacement * distance; // Inside.
	} else {
		return p_segment_a; // Before first point (or fallback in case of NaN).
	}
}

real_t _distance_2d_to_line_2d_segment(const Line2D *p_line_2d, const Vector2 &p_point) {
	const PackedVector2Array line_points = p_line_2d->get_points();
	const Vector2 point = p_point / EDSCALE;
	real_t closest_distance = INFINITY;
	for (int i = 0; i < line_points.size() - 1; i++) {
		const real_t distance = _get_closest_point_to_segment(point, line_points[i], line_points[i + 1]).distance_to(point);
		if (distance < closest_distance) {
			closest_distance = distance;
		}
	}
	return closest_distance;
}

real_t _distance_2d_to_line_25d_segment(const Line25D *p_line_25d, const Vector2 &p_point) {
	const PackedVector2Array line_points = p_line_25d->get_line_2d()->get_points();
	real_t closest_distance = INFINITY;
	for (int i = 0; i < line_points.size() - 1; i++) {
		const real_t distance = _get_closest_point_to_segment(p_point, line_points[i], line_points[i + 1]).distance_to(p_point);
		if (distance < closest_distance) {
			closest_distance = distance;
		}
	}
	return closest_distance;
}

EditorTransformGizmo25D::TransformPart EditorTransformGizmo25D::_check_for_best_hit(const Vector2 &p_local_position_2d) const {
	const real_t scale_num = _get_scale_number();
	const Vector2 scaled_pos_2d = p_local_position_2d / scale_num;
	real_t current_distance_2d;
	real_t closest_distance_2d = (GIZMO_25D_LINE_THICKNESS_2D * 3.0f) * scale_num;
	TransformPart closest_part = TRANSFORM_NONE;
	if (_is_move_linear_enabled) {
		current_distance_2d = _distance_2d_to_line_25d_segment(_lines[TRANSFORM_MOVE_X], scaled_pos_2d);
		if (current_distance_2d < closest_distance_2d) {
			closest_distance_2d = current_distance_2d;
			closest_part = TRANSFORM_MOVE_X;
		}
		current_distance_2d = _distance_2d_to_line_25d_segment(_lines[TRANSFORM_MOVE_Y], scaled_pos_2d);
		if (current_distance_2d < closest_distance_2d) {
			closest_distance_2d = current_distance_2d;
			closest_part = TRANSFORM_MOVE_Y;
		}
		current_distance_2d = _distance_2d_to_line_25d_segment(_lines[TRANSFORM_MOVE_Z], scaled_pos_2d);
		if (current_distance_2d < closest_distance_2d) {
			closest_distance_2d = current_distance_2d;
			closest_part = TRANSFORM_MOVE_Z;
		}
	}
	if (_is_rotation_2d_enabled) {
		current_distance_2d = _distance_2d_to_line_2d_segment(_rotation_ring_2d, scaled_pos_2d);
		if (current_distance_2d < closest_distance_2d) {
			closest_distance_2d = current_distance_2d;
			closest_part = TRANSFORM_ROTATE_2D;
		}
	}
	if (_is_rotation_3d_enabled) {
		current_distance_2d = _distance_2d_to_line_25d_segment(_lines[TRANSFORM_ROTATE_X], scaled_pos_2d);
		if (current_distance_2d < closest_distance_2d) {
			closest_distance_2d = current_distance_2d;
			closest_part = TRANSFORM_ROTATE_X;
		}
		current_distance_2d = _distance_2d_to_line_25d_segment(_lines[TRANSFORM_ROTATE_Y], scaled_pos_2d);
		if (current_distance_2d < closest_distance_2d) {
			closest_distance_2d = current_distance_2d;
			closest_part = TRANSFORM_ROTATE_Y;
		}
		current_distance_2d = _distance_2d_to_line_25d_segment(_lines[TRANSFORM_ROTATE_Z], scaled_pos_2d);
		if (current_distance_2d < closest_distance_2d) {
			closest_distance_2d = current_distance_2d;
			closest_part = TRANSFORM_ROTATE_Z;
		}
	}
	return closest_part;
}

void EditorTransformGizmo25D::_unhiglight_line(TransformPart p_transformation) {
	if (p_transformation == TRANSFORM_NONE) {
		return;
	}
	if (p_transformation == TRANSFORM_ROTATE_2D) {
		const Color original_color = _rotation_ring_2d->get_meta(StringName("original_color"));
		_rotation_ring_2d->set_default_color(original_color);
		return;
	}
	Line25D *line = _lines[p_transformation];
	const Color original_color = line->get_meta(StringName("original_color"));
	line->set_default_color(original_color);
}

void EditorTransformGizmo25D::_highlight_line(TransformPart p_transformation) {
	if (p_transformation == TRANSFORM_NONE) {
		return;
	}
	if (p_transformation == TRANSFORM_ROTATE_2D) {
		const Color original_color = _rotation_ring_2d->get_meta(StringName("original_color"));
		_rotation_ring_2d->set_default_color(original_color.lightened(0.5));
		return;
	}
	Line25D *line = _lines[p_transformation];
	const Color original_color = line->get_meta(StringName("original_color"));
	line->set_default_color(original_color.lightened(0.5));
}

Vector2 EditorTransformGizmo25D::_get_local_mouse_position(const Vector2 &p_viewport_mouse_position) const {
	const Transform3D transform_3d = _current_transformation == TRANSFORM_NONE ? get_global_transform_3d() : _old_transform;
	const Ref<World25D> world = get_world_25d();
	ERR_FAIL_COND_V_MSG(world.is_null(), Vector2(), "World was null while getting the local mouse position. Transforms should only happen when a Node25D with a valid World25D is selected.");
	Transform2D local_to_world = world->xform_3d_transform_to_2d(transform_3d) * _line_holder->get_local_transform_2d();
	if (!_is_use_local_transform) {
		local_to_world.set_rotation(0.0);
	}
	const Vector2 world_position = _camera_2pt5d->viewport_to_world_2d(p_viewport_mouse_position);
	return local_to_world.affine_inverse().xform(world_position) * _get_scale_number();
}

real_t EditorTransformGizmo25D::_get_scale_number() const {
	return EDSCALE / _camera_2pt5d->get_zoom().x;
}

real_t EditorTransformGizmo25D::_get_transform_value(const Vector2 &p_local_position) const {
	switch (_current_transformation) {
		case TRANSFORM_NONE: {
			return 0.0;
		} break;
		case TRANSFORM_MOVE_X: {
			const Vector2 x = _lines[TRANSFORM_MOVE_X]->get_line_2d()->get_points()[1];
			return GIZMO_25D_LINE_LENGTH_3D * p_local_position.dot(x) / x.length_squared();
		} break;
		case TRANSFORM_MOVE_Y: {
			const Vector2 y = _lines[TRANSFORM_MOVE_Y]->get_line_2d()->get_points()[1];
			return GIZMO_25D_LINE_LENGTH_3D * p_local_position.dot(y) / y.length_squared();
		} break;
		case TRANSFORM_MOVE_Z: {
			const Vector2 z = _lines[TRANSFORM_MOVE_Z]->get_line_2d()->get_points()[1];
			return GIZMO_25D_LINE_LENGTH_3D * p_local_position.dot(z) / z.length_squared();
		} break;
		case TRANSFORM_ROTATE_2D: {
			return p_local_position.angle();
		} break;
		case TRANSFORM_ROTATE_X:
		case TRANSFORM_ROTATE_Y:
		case TRANSFORM_ROTATE_Z: {
			const Ref<World25D> world = get_world_25d();
			ERR_FAIL_COND_V(world.is_null(), 0.0);
			Vector3 draw_order = world->get_basis_draw_order();
			if (_is_use_local_transform) {
				draw_order = get_global_basis_3d().xform(draw_order);
			}
			const int axis = _current_transformation - TRANSFORM_ROTATE_X;
			return p_local_position.angle() * SIGN(draw_order[axis]);
		} break;
		case TRANSFORM_MAX: {
			return 0.0;
		} break;
	}
	return 0.0;
}

void EditorTransformGizmo25D::_begin_transformation(const Vector2 &p_local_position_2d) {
	_old_transform = get_global_transform_3d();
	_transform_reference_value = _get_transform_value(p_local_position_2d);
	_selected_top_node_old_transforms.resize(_selected_top_nodes.size());
	for (int i = 0; i < _selected_top_nodes.size(); i++) {
		Node25D *node_25d = Object::cast_to<Node25D>(_selected_top_nodes[i]);
		if (node_25d != nullptr) {
			_selected_top_node_old_transforms.set(i, node_25d->get_global_transform_3d());
		}
	}
}

void EditorTransformGizmo25D::_end_transformation() {
	if (_current_transformation == TRANSFORM_NONE) {
		return;
	}
	// Create an undo/redo action for the transformation.
	const bool is_move_only = _current_transformation >= TRANSFORM_MOVE_X && _current_transformation <= TRANSFORM_MOVE_Z;
	_undo_redo->create_action(is_move_only ? String("Move 2.5D nodes with gizmo") : String("Transform 2.5D nodes with gizmo"));
	const int size = _selected_top_nodes.size();
	for (int i = 0; i < size; i++) {
		Node25D *node_25d = Object::cast_to<Node25D>(_selected_top_nodes[i]);
		if (node_25d != nullptr) {
			_undo_redo->add_do_property(node_25d, StringName("global_position_3d"), node_25d->get_global_position_3d());
			_undo_redo->add_undo_property(node_25d, StringName("global_position_3d"), _selected_top_node_old_transforms[i].origin);
			if (!is_move_only) {
				_undo_redo->add_do_property(node_25d, StringName("global_basis_3d"), node_25d->get_global_basis_3d());
				_undo_redo->add_undo_property(node_25d, StringName("global_basis_3d"), _selected_top_node_old_transforms[i].basis);
				Node25D::RotationEditMode rot_edit_mode = node_25d->get_rotation_edit_mode();
				if (_is_rotation_2d_enabled) {
					if (rot_edit_mode < Node25D::ROTATION_EDIT_MODE_2D_ANGLE) {
						_undo_redo->add_do_property(node_25d, StringName("rotation_edit_mode"), Node25D::ROTATION_EDIT_MODE_2D_ANGLE);
						_undo_redo->add_undo_property(node_25d, StringName("rotation_edit_mode"), rot_edit_mode);
						node_25d->set_rotation_edit_mode(Node25D::ROTATION_EDIT_MODE_2D_ANGLE);
					}
				}
				if (_is_rotation_3d_enabled) {
					if (rot_edit_mode < Node25D::ROTATION_EDIT_MODE_3D_EULER) {
						_undo_redo->add_do_property(node_25d, StringName("rotation_edit_mode"), Node25D::ROTATION_EDIT_MODE_3D_EULER);
						_undo_redo->add_undo_property(node_25d, StringName("rotation_edit_mode"), rot_edit_mode);
						node_25d->set_rotation_edit_mode(Node25D::ROTATION_EDIT_MODE_3D_EULER);
					}
				}
			}
		}
	}
	_undo_redo->commit_action(false);
	// Clear out the transformation data and mark the scene as unsaved.
	_old_transform = Transform3D();
	_transform_reference_value = 0.0f;
	_current_transformation = TRANSFORM_NONE;
	EditorInterface::get_singleton()->mark_scene_as_unsaved();
}

void EditorTransformGizmo25D::_process_transform(const Vector2 &p_local_position_2d) {
	const real_t value_change = _get_transform_value(p_local_position_2d) - _transform_reference_value;
	Transform3D transform_change;
	switch (_current_transformation) {
		case TRANSFORM_NONE: {
			// Do nothing.
		} break;
		case TRANSFORM_MOVE_X: {
			transform_change.origin = Vector3(value_change, 0.0, 0.0);
		} break;
		case TRANSFORM_MOVE_Y: {
			transform_change.origin = Vector3(0.0, value_change, 0.0);
		} break;
		case TRANSFORM_MOVE_Z: {
			transform_change.origin = Vector3(0.0, 0.0, value_change);
		} break;
		case TRANSFORM_ROTATE_2D: {
			const Ref<World25D> world = get_world_25d();
			transform_change.basis = Basis(world->get_basis_draw_order(), -value_change);
		} break;
		case TRANSFORM_ROTATE_X: {
			transform_change.basis = Basis(Vector3(1.0, 0.0, 0.0), -value_change);
		} break;
		case TRANSFORM_ROTATE_Y: {
			transform_change.basis = Basis(Vector3(0.0, 1.0, 0.0), -value_change);
		} break;
		case TRANSFORM_ROTATE_Z: {
			transform_change.basis = Basis(Vector3(0.0, 0.0, 1.0), -value_change);
		} break;
		case TRANSFORM_MAX: {
			ERR_FAIL_MSG("Invalid transformation.");
		} break;
	}
	Transform3D new_transform;
	if (_is_use_local_transform) {
		new_transform = _old_transform * transform_change;
	} else {
		new_transform.basis = transform_change.basis * _old_transform.basis;
		new_transform.origin = transform_change.origin + _old_transform.origin;
	}
	set_transform_3d(new_transform);
	// We want the global diff so we can apply it from the left on the global transform of all selected nodes.
	// Without this, the transforms would be relative to each node (ex: "moving on X" moves on each node's X axis).
	transform_change = new_transform * _old_transform.affine_inverse();
	for (int i = 0; i < _selected_top_nodes.size(); i++) {
		Node25D *node_25d = Object::cast_to<Node25D>(_selected_top_nodes[i]);
		if (node_25d != nullptr) {
			node_25d->set_global_transform_3d(transform_change * _selected_top_node_old_transforms[i]);
		}
	}
}

void EditorTransformGizmo25D::_notification(int p_what) {
	Node25D::_notification(p_what);
	switch (p_what) {
		case NOTIFICATION_PROCESS: {
			_update_gizmo_line_transform();
		} break;
	}
}

void EditorTransformGizmo25D::selected_nodes_changed(const TypedArray<Node> &p_top_nodes) {
	_end_transformation();
	_selected_top_nodes = p_top_nodes;
	const int size = _selected_top_nodes.size();
	if (_selected_top_node_old_transforms.size() < size) {
		_selected_top_node_old_transforms.resize(size);
	}
	set_gizmo_mode(_gizmo_mode);
	_update_gizmo_transform();
}

void EditorTransformGizmo25D::theme_changed(const PackedColorArray &p_axis_colors) {
	if (_lines[TRANSFORM_MOVE_X] == nullptr) {
		_generate_gizmo_lines(p_axis_colors);
	}
}

void EditorTransformGizmo25D::set_gizmo_mode(const GizmoMode p_mode) {
	_gizmo_mode = p_mode;
	switch (p_mode) {
		case GizmoMode::SELECT: {
			_is_move_linear_enabled = true;
			_is_move_planar_enabled = false;
			_is_rotation_2d_enabled = false;
			_is_rotation_3d_enabled = false;
		} break;
		case GizmoMode::MOVE: {
			_is_move_linear_enabled = true;
			_is_move_planar_enabled = true;
			_is_rotation_2d_enabled = false;
			_is_rotation_3d_enabled = false;
		} break;
		case GizmoMode::ROTATE: {
			_is_move_linear_enabled = false;
			_is_move_planar_enabled = false;
			if (_should_use_3d_rotation()) {
				_is_rotation_2d_enabled = false;
				_is_rotation_3d_enabled = true;
			} else {
				_is_rotation_2d_enabled = true;
				_is_rotation_3d_enabled = false;
			}
		} break;
	}
	// Update mesh instance visibility.
	_lines[TRANSFORM_MOVE_X]->set_visible(_is_move_linear_enabled);
	_lines[TRANSFORM_MOVE_Y]->set_visible(_is_move_linear_enabled);
	_lines[TRANSFORM_MOVE_Z]->set_visible(_is_move_linear_enabled);
	_lines[TRANSFORM_ROTATE_2D]->set_visible(_is_rotation_2d_enabled);
	_lines[TRANSFORM_ROTATE_X]->set_visible(_is_rotation_3d_enabled);
	_lines[TRANSFORM_ROTATE_Y]->set_visible(_is_rotation_3d_enabled);
	_lines[TRANSFORM_ROTATE_Z]->set_visible(_is_rotation_3d_enabled);
	set_visible(true);
}

bool EditorTransformGizmo25D::gizmo_mouse_input(const Ref<InputEventMouse> &p_mouse_event) {
	if (!is_visible()) {
		return false;
	}
	_update_gizmo_line_transform();
	const Vector2 local_position = _get_local_mouse_position(p_mouse_event->get_position());
	const Ref<InputEventMouseButton> mouse_button = p_mouse_event;
	if (mouse_button.is_valid()) {
		if (mouse_button->get_button_index() != MOUSE_BUTTON_LEFT) {
			return false;
		}
		if (mouse_button->is_pressed() && _current_transformation == TRANSFORM_NONE) {
			// If we are not transforming anything and the user clicks, try to start a transformation.
			_current_transformation = _check_for_best_hit(local_position);
			if (_current_transformation != TRANSFORM_NONE) {
				_begin_transformation(local_position);
			}
		} else if (!mouse_button->is_pressed() && _current_transformation != TRANSFORM_NONE) {
			// If we are transforming something and the user releases the click, end the transformation.
			_end_transformation();
		}
		return false;
	}
	const Ref<InputEventMouseMotion> mouse_motion = p_mouse_event;
	if (mouse_motion.is_valid()) {
		if (_current_transformation == TRANSFORM_NONE) {
			// If we receive motion but no transformation is happening, it means
			// we need to highlight the closest part of the gizmo (if any).
			TransformPart hit = _check_for_best_hit(local_position);
			if (hit == _highlighted_transformation) {
				return false;
			}
			_unhiglight_line(_highlighted_transformation);
			_highlight_line(hit);
			_highlighted_transformation = hit;
			return true;
		} else {
			// If we are transforming something, process the transformation.
			_process_transform(local_position);
			return true;
		}
	}
	return false;
}

bool EditorTransformGizmo25D::get_use_local_transform() const {
	return _is_use_local_transform;
}

void EditorTransformGizmo25D::set_use_local_transform(const bool p_use_local_transform) {
	_is_use_local_transform = p_use_local_transform;
	_update_gizmo_transform();
}

void EditorTransformGizmo25D::setup(Camera25D *p_editor_camera, EditorUndoRedoManager *p_undo_redo_manager) {
	// Things that we should do in the constructor but can't in GDExtension
	// due to how GDExtension runs the constructor for each registered class.
	_line_holder = memnew(Node25D);
	_line_holder->set_name(StringName("GizmoLineHolder25D"));
	add_child(_line_holder);
	set_z_index(4000);
	EditorInterface::get_singleton()->get_inspector()->connect(StringName("property_edited"), callable_mp(this, &EditorTransformGizmo25D::_on_editor_inspector_property_edited));

	// Set up things with the arguments (not constructor things).
	_camera_2pt5d = p_editor_camera;
	_undo_redo = p_undo_redo_manager;
	p_undo_redo_manager->connect(StringName("version_changed"), callable_mp(this, &EditorTransformGizmo25D::_on_undo_redo_version_changed));
}
