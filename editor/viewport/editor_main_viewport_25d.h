#pragma once

#include "editor_viewport_2pt5d_defines.h"

#if GDEXTENSION
#include <godot_cpp/classes/input_event_mouse_motion.hpp>
#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/classes/sub_viewport_container.hpp>
#elif GODOT_MODULE
#include "scene/gui/subviewport_container.h"
#include "scene/main/viewport.h"
#endif

// Class for the 2.5D editor viewport (does not include the toolbar).
// Uses EditorInputSurface25D, EditorViewportRotation25D,
// and lots of other classes to handle the 2.5D editor viewport.
class EditorMainViewport25D : public Control {
	GDCLASS(EditorMainViewport25D, Control);

public:
	// Keep the first items in sync with EditorTransformGizmo25D.
	enum ToolbarButton {
		TOOLBAR_BUTTON_SELECT, // 0
		TOOLBAR_BUTTON_MOVE, // 1
		TOOLBAR_BUTTON_ROTATE, // 2
		TOOLBAR_BUTTON_SCALE, // 3
		TOOLBAR_BUTTON_MODE_MAX, // 4
		TOOLBAR_BUTTON_USE_LOCAL_TRANSFORM = TOOLBAR_BUTTON_MODE_MAX, // Still 4
		TOOLBAR_BUTTON_MAX
	};

private:
	SubViewportContainer *_sub_viewport_container = nullptr;
	SubViewport *_sub_viewport = nullptr;
	EditorInputSurface25D *_input_surface_2pt5d = nullptr;
	EditorMainScreen25D *_editor_main_screen = nullptr;
	EditorViewportRotation25D *_viewport_rotation_2pt5d = nullptr;
	Viewport *_edited_scene_viewport = nullptr;

	PackedColorArray _axis_colors;
	double _information_label_auto_hide_time = 0.0;

	Vector2 _get_warped_mouse_motion(const Ref<InputEventMouseMotion> &p_ev_mouse_motion) const;
	void _on_button_toggled(const bool p_toggled_on, const int p_option);
	void _update_theme();

protected:
	static void _bind_methods() {}
	void _notification(int p_what);

public:
	PackedColorArray get_axis_colors() const;
	Basis25D get_view_basis_25d() const;
	Ref<World25D> get_world_25d() const;
	void navigation_orbit(const Ref<InputEventMouseMotion> &p_input_event);
	void navigation_pan(const Ref<InputEventMouseMotion> &p_input_event);
	void navigation_change_zoom(const double p_zoom_change);
	void viewport_mouse_input(const Ref<InputEventMouse> &p_mouse_event);

	void set_edited_scene_viewport(Viewport *p_edited_scene_viewport);

	// TODO: Pass the transform gizmo to this function.
	void setup(EditorMainScreen25D *p_editor_main_screen);

	EditorMainViewport25D();
};
