#include "editor_main_viewport_25d.h"

#include "editor_input_surface_25d.h"
#include "editor_transform_gizmo_25d.h"
#include "editor_viewport_rotation_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/world2d.hpp>
#elif GODOT_MODULE
#include "scene/resources/world_2d.h"
#endif

Vector2 EditorMainViewport25D::_get_warped_mouse_motion(const Ref<InputEventMouseMotion> &p_ev_mouse_motion) const {
#if GODOT_MODULE
	if (bool(EDITOR_GET("editors/3d/navigation/warped_mouse_panning"))) {
		return Input::get_singleton()->warp_mouse_motion(p_ev_mouse_motion, _input_surface_2pt5d->get_global_rect());
	}
#endif // GODOT_MODULE
	return p_ev_mouse_motion->get_relative();
}

void EditorMainViewport25D::_update_theme() {
	// Set the axis colors.
	_axis_colors.clear();
	_axis_colors.push_back(get_theme_color(StringName("axis_x_color"), StringName("Editor")));
	_axis_colors.push_back(get_theme_color(StringName("axis_y_color"), StringName("Editor")));
	_axis_colors.push_back(get_theme_color(StringName("axis_z_color"), StringName("Editor")));
	_axis_colors.push_back(get_theme_color(StringName("accent_color"), StringName("Editor")));
	ERR_FAIL_NULL(_transform_gizmo_2pt5d);
	_transform_gizmo_2pt5d->theme_changed(_axis_colors);
}

void EditorMainViewport25D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_theme();
		} break;
		case NOTIFICATION_READY: {
			ERR_FAIL_NULL(_camera_2pt5d);
			_camera_2pt5d->make_current();
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			_update_theme();
		} break;
	}
}

PackedColorArray EditorMainViewport25D::get_axis_colors() const {
	return _axis_colors;
}

Basis25D EditorMainViewport25D::get_view_basis_25d() const {
	if (_edited_scene_viewport != nullptr && _edited_scene_viewport->has_meta(StringName("world_25d"))) {
		Ref<World25D> world = _edited_scene_viewport->get_meta(StringName("world_25d"));
		if (world.is_valid()) {
			return world->get_basis();
		}
	}
	return Basis25D();
}

Ref<World25D> EditorMainViewport25D::get_world_25d() const {
	if (_edited_scene_viewport != nullptr && _edited_scene_viewport->has_meta(StringName("world_25d"))) {
		Ref<World25D> world = _edited_scene_viewport->get_meta(StringName("world_25d"));
		if (world.is_valid()) {
			return world;
		}
	}
	return Ref<World25D>();
}

void EditorMainViewport25D::selected_nodes_changed(const TypedArray<Node> &p_top_nodes) {
	_transform_gizmo_2pt5d->selected_nodes_changed(p_top_nodes);
}

void EditorMainViewport25D::navigation_focus_selected_nodes() {
	_camera_2pt5d->set_global_position_2d(_transform_gizmo_2pt5d->get_global_position_2d());
}

void EditorMainViewport25D::navigation_orbit(const Ref<InputEventMouseMotion> &p_input_event) {
	Vector2 relative = _get_warped_mouse_motion(p_input_event);
	const real_t degrees_per_pixel = EDITOR_GET("editors/3d/navigation_feel/orbit_sensitivity");
	const real_t radians_per_pixel = Math::deg_to_rad(degrees_per_pixel);
	const bool invert_y_axis = EDITOR_GET("editors/3d/navigation/invert_y_axis");
	const bool invert_x_axis = EDITOR_GET("editors/3d/navigation/invert_x_axis");
	Vector2 rotation_radians = relative * radians_per_pixel;
	if (invert_x_axis) {
		rotation_radians.x = -rotation_radians.x;
	}
	if (invert_y_axis) {
		rotation_radians.y = -rotation_radians.y;
	}
	Ref<World25D> world = get_world_25d();
	if (world.is_valid()) {
		World25D::Basis25DPreset basis_preset = world->get_basis_preset();
		switch (basis_preset) {
			case World25D::PRESET_FROM_ANGLE:
			case World25D::PRESET_DIMETRIC: {
				world->set_basis_angle(world->get_basis_angle() + rotation_radians.y);
			} break;
			case World25D::PRESET_TRIMETRIC: {
				world->set_basis_angle(world->get_basis_angle() + rotation_radians.y);
				world->set_basis_angle_z(world->get_basis_angle_z() + rotation_radians.x);
			} break;
			default:
				break;
		}
	}
	_viewport_rotation_2pt5d->queue_redraw();
}

void EditorMainViewport25D::navigation_pan(const Ref<InputEventMouseMotion> &p_input_event) {
	const Vector2 relative = _get_warped_mouse_motion(p_input_event);
	_camera_2pt5d->global_translate_2d(-relative / _camera_2pt5d->get_zoom());
}

void EditorMainViewport25D::navigation_change_zoom_level(const int p_zoom_level_change, const Vector2 &p_mouse_offset) {
	_zoom_level += p_zoom_level_change;
	constexpr double TWELFTH_ROOT_OF_TWO = 1.05946309435929526456;
	const double zoom = Math::pow(TWELFTH_ROOT_OF_TWO, _zoom_level);
	const Vector2 prev_zoom = _camera_2pt5d->get_zoom();
	const Vector2 new_zoom = Vector2(zoom, zoom);
	_camera_2pt5d->set_zoom(new_zoom);
	// Pan the camera to keep the mouse position in the same world position.
	if (!p_mouse_offset.is_zero_approx()) {
		const Vector2 view_offset = p_mouse_offset / prev_zoom - p_mouse_offset / new_zoom;
		_camera_2pt5d->global_translate_2d(view_offset);
	}
	emit_signal("zoom_amount_changed", zoom);
}

void EditorMainViewport25D::navigation_reset_zoom_level() {
	EditorMainViewport25D::navigation_change_zoom_level(-_zoom_level);
}

void EditorMainViewport25D::viewport_mouse_input(const Ref<InputEventMouse> &p_mouse_event) {
	const bool used_by_gizmo = _transform_gizmo_2pt5d->gizmo_mouse_input(p_mouse_event);
	if (used_by_gizmo) {
		return;
	}
	// TODO: Try to make a selection if the transform gizmo didn't use the mouse.
}

void EditorMainViewport25D::set_edited_scene_viewport(Viewport *p_edited_scene_viewport) {
	_edited_scene_viewport = p_edited_scene_viewport;
	_sub_viewport->set_world_2d(_edited_scene_viewport->find_world_2d());
	_viewport_rotation_2pt5d->queue_redraw();
}

void EditorMainViewport25D::set_gizmo_mode(const int p_mode) {
	_transform_gizmo_2pt5d->set_gizmo_mode((EditorTransformGizmo25D::GizmoMode)p_mode);
}

void EditorMainViewport25D::set_use_local_transform(const bool p_use_local_transform) {
	_transform_gizmo_2pt5d->set_use_local_transform(p_use_local_transform);
}

void EditorMainViewport25D::setup(EditorMainScreen25D *p_editor_main_screen, EditorUndoRedoManager *p_undo_redo_manager) {
	// Things that we should do in the constructor but can't in GDExtension
	// due to how GDExtension runs the constructor for each registered class.
	set_name(StringName("EditorMainViewport25D"));
	set_clip_children_mode(Control::CLIP_CHILDREN_AND_DRAW);
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	// Set up the viewport container.
	_sub_viewport_container = memnew(SubViewportContainer);
	_sub_viewport_container->set_clip_children_mode(Control::CLIP_CHILDREN_AND_DRAW);
	_sub_viewport_container->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	_sub_viewport_container->set_stretch(true);
	add_child(_sub_viewport_container);

	_sub_viewport = memnew(SubViewport);
	_sub_viewport_container->add_child(_sub_viewport);

	// Set up the 2.5D camera.
	_camera_2pt5d = memnew(Camera25D);
	_sub_viewport->add_child(_camera_2pt5d);

	// Set up the input surface and gizmos.
	_input_surface_2pt5d = memnew(EditorInputSurface25D);
	_sub_viewport_container->add_child(_input_surface_2pt5d);

	_transform_gizmo_2pt5d = memnew(EditorTransformGizmo25D);
	_transform_gizmo_2pt5d->setup(_camera_2pt5d, p_undo_redo_manager);
	_sub_viewport->add_child(_transform_gizmo_2pt5d);

	_viewport_rotation_2pt5d = memnew(EditorViewportRotation25D);
	_viewport_rotation_2pt5d->setup(this);
	_input_surface_2pt5d->add_child(_viewport_rotation_2pt5d);

	// Set up things with the arguments (not constructor things).
	_editor_main_screen_2pt5d = p_editor_main_screen;
	_input_surface_2pt5d->setup(p_editor_main_screen, this);
}

void EditorMainViewport25D::_bind_methods() {
	ADD_SIGNAL(MethodInfo("zoom_amount_changed", PropertyInfo(Variant::FLOAT, "zoom_amount")));
}
