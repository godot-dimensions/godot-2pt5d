#pragma once

#include "editor_viewport_2pt5d_defines.h"

#include "editor_transform_gizmo_25d.h"

#include "../../nodes/camera_25d.h"

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

private:
	SubViewportContainer *_sub_viewport_container = nullptr;
	SubViewport *_sub_viewport = nullptr;
	Camera25D *_camera_2pt5d = nullptr;
	EditorInputSurface25D *_input_surface_2pt5d = nullptr;
	EditorMainScreen25D *_editor_main_screen_2pt5d = nullptr;
	EditorTransformGizmo25D *_transform_gizmo_2pt5d = nullptr;
	EditorViewportRotation25D *_viewport_rotation_2pt5d = nullptr;
	Viewport *_edited_scene_viewport = nullptr;

	PackedColorArray _axis_colors;
	int _zoom_level = 0;

	Vector2 _get_warped_mouse_motion(const Ref<InputEventMouseMotion> &p_ev_mouse_motion) const;
	void _on_button_toggled(const bool p_toggled_on, const int p_option);
	void _update_theme();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
	PackedColorArray get_axis_colors() const;
	Basis25D get_view_basis_25d() const;
	Ref<World25D> get_world_25d() const;
	void selected_nodes_changed(const TypedArray<Node> &p_top_nodes);
	void navigation_focus_selected_nodes();
	void navigation_orbit(const Ref<InputEventMouseMotion> &p_input_event);
	void navigation_pan(const Ref<InputEventMouseMotion> &p_input_event);
	void navigation_change_zoom_level(const int p_zoom_level_change, const Vector2 &p_mouse_offset = Vector2());
	void navigation_reset_zoom_level();
	double navigation_get_zoom_amount() const;
	void viewport_mouse_input(const Ref<InputEventMouse> &p_mouse_event);

	void set_edited_scene_viewport(Viewport *p_edited_scene_viewport);
	void set_gizmo_mode(const int p_gizmo_mode);
	void set_use_local_transform(const bool p_use_local_transform);

	void setup(EditorMainScreen25D *p_editor_main_screen, EditorUndoRedoManager *p_undo_redo_manager);
};
