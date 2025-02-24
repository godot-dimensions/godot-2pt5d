#include "editor_main_viewport_25d.h"

#include "editor_input_surface_25d.h"
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
	// TODO: Pass the axis colors to the transform gizmo.
}

void EditorMainViewport25D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_theme();
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
		Basis25D::Preset basis_preset = world->get_basis_preset();
		switch (basis_preset) {
			case Basis25D::PRESET_FROM_ANGLE:
			case Basis25D::PRESET_DIMETRIC: {
				world->set_basis_angle(world->get_basis_angle() + rotation_radians.y);
			} break;
			case Basis25D::PRESET_TRIMETRIC: {
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
	//Vector2 relative = _get_warped_mouse_motion(p_input_event);
	// TODO: Pan the camera.
}

void EditorMainViewport25D::navigation_change_zoom(const double p_zoom_change) {
	// TODO: Zoom the camera and call set_information_text.
}

void EditorMainViewport25D::viewport_mouse_input(const Ref<InputEventMouse> &p_mouse_event) {
	// TODO: Pass the mouse input to the transform gizmo.
	// TODO: Try to make a selection if the transform gizmo didn't use the mouse.
}

void EditorMainViewport25D::set_edited_scene_viewport(Viewport *p_edited_scene_viewport) {
	_edited_scene_viewport = p_edited_scene_viewport;
	_sub_viewport->set_world_2d(_edited_scene_viewport->find_world_2d());
	_viewport_rotation_2pt5d->queue_redraw();
}

void EditorMainViewport25D::setup(EditorMainScreen25D *p_editor_main_screen) {
	_editor_main_screen = p_editor_main_screen;
	_input_surface_2pt5d->set_editor_main_screen(_editor_main_screen);
}

EditorMainViewport25D::EditorMainViewport25D() {
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

	// Set up the input surface and viewport rotation gizmo.
	_input_surface_2pt5d = memnew(EditorInputSurface25D);
	_input_surface_2pt5d->set_editor_main_viewport(this);
	_sub_viewport_container->add_child(_input_surface_2pt5d);

	_viewport_rotation_2pt5d = memnew(EditorViewportRotation25D);
	_viewport_rotation_2pt5d->set_editor_main_viewport(this);
	_input_surface_2pt5d->add_child(_viewport_rotation_2pt5d);
}
