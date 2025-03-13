#pragma once

#include "editor_viewport_2pt5d_defines.h"

#if GDEXTENSION
#include <godot_cpp/classes/button.hpp>
#include <godot_cpp/classes/h_box_container.hpp>
#elif GODOT_MODULE
#include "scene/gui/box_container.h"
#include "scene/gui/button.h"
#endif

// Main class for the 2.5D editor main screen.
// Has a toolbar at the top and a display of the 2.5D scene below it.
// Has an EditorMainViewport25D for displaying the 2.5D scene.
class EditorMainScreen25D : public Control {
	GDCLASS(EditorMainScreen25D, Control);

public:
	// Ensure the MODE items are kept in sync with EditorTransformGizmo25D::GizmoMode.
	enum ToolbarButton {
		TOOLBAR_BUTTON_MODE_SELECT = 0, // 0
		TOOLBAR_BUTTON_MODE_MOVE = 1, // 1
		TOOLBAR_BUTTON_MODE_ROTATE = 2, // 2
		TOOLBAR_BUTTON_MODE_MAX, // 3
		TOOLBAR_BUTTON_USE_LOCAL_TRANSFORM = TOOLBAR_BUTTON_MODE_MAX, // Still 3
		TOOLBAR_BUTTON_FOCUS_SELECTED_NODES, // 4
		TOOLBAR_BUTTON_MAX,
	};

private:
	Button *_toolbar_buttons[TOOLBAR_BUTTON_MAX] = { nullptr };
	HBoxContainer *_toolbar_hbox = nullptr;
	EditorMainViewport25D *_editor_main_viewport_2pt5d = nullptr;
	Viewport *_edited_scene_viewport = nullptr;

	Button *_focus_selected_nodes = nullptr;
	Button *_zoom_minus = nullptr;
	Button *_zoom_reset = nullptr;
	Button *_zoom_plus = nullptr;

	void _on_button_toggled(const bool p_toggled_on, const int p_option);
	void _on_selection_changed();
	void _on_zoom_amount_changed(const double p_zoom_amount);
	void _on_zoom_reset_pressed();
	void _update_theme();

protected:
	static void _bind_methods() {}
	void _notification(int p_what);

public:
	void press_menu_item(const int p_option);
	void press_zoom_minus();
	void press_zoom_plus();

	void setup(EditorUndoRedoManager *p_undo_redo_manager);
};
