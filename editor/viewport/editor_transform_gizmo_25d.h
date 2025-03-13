#pragma once

#include "editor_viewport_2pt5d_defines.h"

#include "../../nodes/camera_25d.h"
#include "../../nodes/line_25d.h"

class EditorTransformGizmo25D : public Node25D {
	GDCLASS(EditorTransformGizmo25D, Node25D);

public:
	// Keep this in sync with the toolbar buttons in EditorMainScreen25D.
	enum class GizmoMode {
		SELECT = 0,
		MOVE = 1,
		ROTATE = 2,
	};

private:
	enum TransformPart {
		TRANSFORM_NONE,
		TRANSFORM_MOVE_X,
		TRANSFORM_MOVE_Y,
		TRANSFORM_MOVE_Z,
		TRANSFORM_ROTATE_2D,
		TRANSFORM_ROTATE_X,
		TRANSFORM_ROTATE_Y,
		TRANSFORM_ROTATE_Z,
		TRANSFORM_MAX,
	};

	Camera25D *_camera_2pt5d = nullptr;
	Node25D *_line_holder = nullptr;
	Line25D *_lines[TRANSFORM_MAX] = { nullptr };
	Line2D *_rotation_ring_2d = nullptr;
	EditorUndoRedoManager *_undo_redo = nullptr;

	GizmoMode _gizmo_mode = GizmoMode::SELECT;
	TransformPart _current_transformation = TRANSFORM_NONE;
	TransformPart _highlighted_transformation = TRANSFORM_NONE;

	Transform3D _old_transform;
	real_t _transform_reference_value;
	TypedArray<Node> _selected_top_nodes;
	Vector<Transform3D> _selected_top_node_old_transforms;

	bool _is_move_linear_enabled = true;
	bool _is_move_planar_enabled = false;
	bool _is_rotation_2d_enabled = false;
	bool _is_rotation_3d_enabled = false;
	bool _is_use_local_transform = false;

	// Setup functions.
	Line25D *_make_line(const StringName &p_name, const Color &p_color, const Vector3 &p_point_3d);
	Line25D *_make_ring_2d(const StringName &p_name, const Color &p_color);
	Line25D *_make_ring_3d(const StringName &p_name, const Color &p_color, const Vector3::Axis p_cos, const Vector3::Axis p_sin);
	void _generate_gizmo_lines(const PackedColorArray &p_axis_colors);

	// Misc internal functions.
	void _on_editor_inspector_property_edited(const String &p_prop);
	void _on_undo_redo_version_changed();
	void _update_gizmo_transform();
	void _update_gizmo_line_transform();
	bool _should_use_3d_rotation() const;

	// Highlighting functions, used when not transforming.
	TransformPart _check_for_best_hit(const Vector2 &p_local_position_2d) const;
	void _unhiglight_line(TransformPart p_transformation);
	void _highlight_line(TransformPart p_transformation);

	// Transformation functions.
	Vector2 _get_local_mouse_position(const Vector2 &p_viewport_mouse_position) const;
	real_t _get_scale_number() const;
	real_t _get_transform_value(const Vector2 &p_local_position) const;
	void _begin_transformation(const Vector2 &p_local_position);
	void _end_transformation();
	void _process_transform(const Vector2 &p_local_position);

protected:
	static void _bind_methods() {}
	void _notification(int p_what);

public:
	void selected_nodes_changed(const TypedArray<Node> &p_top_nodes);
	void theme_changed(const PackedColorArray &p_axis_colors);
	void set_gizmo_mode(const GizmoMode p_mode);
	bool gizmo_mouse_input(const Ref<InputEventMouse> &p_mouse_event);

	bool get_use_local_transform() const;
	void set_use_local_transform(const bool p_use_local_transform);

	void setup(Camera25D *p_editor_camera, EditorUndoRedoManager *p_undo_redo_manager);
};
