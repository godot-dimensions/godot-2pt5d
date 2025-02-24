#include "editor_main_screen_25d.h"

#include "editor_main_viewport_25d.h"
#include "editor_viewport_rotation_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_selection.hpp>
#include <godot_cpp/classes/v_separator.hpp>

#if GODOT_VERSION < 0x040500
#define get_top_selected_nodes get_transformable_selected_nodes
#endif // GODOT_VERSION
#elif GODOT_MODULE
#include "editor/editor_data.h"
#include "editor/editor_interface.h"
#include "scene/gui/separator.h"

// See https://github.com/godotengine/godot/pull/99897
#if GODOT_VERSION < 0x040500
#define get_top_selected_nodes get_selected_node_list
#else
#define get_top_selected_nodes get_top_selected_node_list
#endif // GODOT_VERSION
#endif

void EditorMainScreen25D::_on_button_toggled(const bool p_toggled_on, const int p_option) {
	press_menu_item(p_option);
}

void EditorMainScreen25D::_on_selection_changed() {
	EditorSelection *selection = EditorInterface::get_singleton()->get_selection();
	TypedArray<Node> top_selected_nodes;
#if GDEXTENSION
	top_selected_nodes = selection->get_top_selected_nodes();
#elif GODOT_MODULE
	List<Node *> &top_selected_node_list = selection->get_top_selected_nodes();
	for (Node *node : top_selected_node_list) {
		top_selected_nodes.push_back(node);
	}
#endif
	// TODO: Pass the top selected nodes to the transform gizmo.
}

void EditorMainScreen25D::_update_theme() {
	// Set the toolbar offsets. Note that these numbers are designed to match Godot's 2D and 3D editor toolbars.
	_toolbar_hbox->set_offset(Side::SIDE_LEFT, 4.0f * EDSCALE);
	_toolbar_hbox->set_offset(Side::SIDE_RIGHT, -4.0f * EDSCALE);
	_toolbar_hbox->set_offset(Side::SIDE_TOP, 0.0f);
	_toolbar_hbox->set_offset(Side::SIDE_BOTTOM, 29.5f * EDSCALE);
	_editor_main_viewport->set_offset(Side::SIDE_TOP, 33.0f * EDSCALE);
	// Set icons.
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_SELECT]->set_button_icon(get_editor_theme_icon(StringName("ToolSelect")));
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_MOVE]->set_button_icon(get_editor_theme_icon(StringName("ToolMove")));
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_ROTATE]->set_button_icon(get_editor_theme_icon(StringName("ToolRotate")));
}

void EditorMainScreen25D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_update_theme();
		} break;
		case NOTIFICATION_PROCESS: {
			Node *edited_scene_root = EditorInterface::get_singleton()->get_edited_scene_root();
			if (edited_scene_root == nullptr) {
				_edited_scene_viewport = nullptr;
				return;
			}
			_edited_scene_viewport = edited_scene_root->get_viewport();
			_editor_main_viewport->set_edited_scene_viewport(_edited_scene_viewport);
		}
		case NOTIFICATION_THEME_CHANGED: {
			_update_theme();
		} break;
	}
}

void EditorMainScreen25D::press_menu_item(const int p_option) {
	switch (p_option) {
		case TOOLBAR_BUTTON_MODE_SELECT:
		case TOOLBAR_BUTTON_MODE_MOVE:
		case TOOLBAR_BUTTON_MODE_ROTATE: {
			for (int i = 0; i < TOOLBAR_BUTTON_MODE_MAX; i++) {
				_toolbar_buttons[i]->set_pressed(i == p_option);
			}
			// TODO: Pass the gizmo mode to the transform gizmo.
		} break;
	}
}

void EditorMainScreen25D::setup(EditorUndoRedoManager *p_undo_redo_manager) {
	// TODO: Pass the undo/redo manager to the transform gizmo.
}

EditorMainScreen25D::EditorMainScreen25D() {
	set_name(StringName("EditorMainScreen25D"));
	set_visible(false);
	set_v_size_flags(SIZE_EXPAND_FILL);
	set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	// Set up the toolbar and tool buttons.
	_toolbar_hbox = memnew(HBoxContainer);
	_toolbar_hbox->set_anchors_and_offsets_preset(Control::PRESET_TOP_WIDE);

	add_child(_toolbar_hbox);
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_SELECT] = memnew(Button);
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_SELECT]->set_toggle_mode(true);
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_SELECT]->set_theme_type_variation("FlatButton");
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_SELECT]->set_pressed(true);
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_SELECT]->set_tooltip_text(TTR("(Q) Select nodes and manipulate their position."));
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_SELECT]->connect(StringName("pressed"), callable_mp(this, &EditorMainScreen25D::press_menu_item).bind(TOOLBAR_BUTTON_MODE_SELECT));
	_toolbar_hbox->add_child(_toolbar_buttons[TOOLBAR_BUTTON_MODE_SELECT]);
	_toolbar_hbox->add_child(memnew(VSeparator));

	_toolbar_buttons[TOOLBAR_BUTTON_MODE_MOVE] = memnew(Button);
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_MOVE]->set_toggle_mode(true);
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_MOVE]->set_theme_type_variation("FlatButton");
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_MOVE]->set_tooltip_text(TTR("(W) Move selected node, changing its position."));
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_MOVE]->connect(StringName("pressed"), callable_mp(this, &EditorMainScreen25D::press_menu_item).bind(TOOLBAR_BUTTON_MODE_MOVE));
	_toolbar_hbox->add_child(_toolbar_buttons[TOOLBAR_BUTTON_MODE_MOVE]);

	_toolbar_buttons[TOOLBAR_BUTTON_MODE_ROTATE] = memnew(Button);
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_ROTATE]->set_toggle_mode(true);
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_ROTATE]->set_theme_type_variation("FlatButton");
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_ROTATE]->set_tooltip_text(TTR("(E) Rotate selected node."));
	_toolbar_buttons[TOOLBAR_BUTTON_MODE_ROTATE]->connect(StringName("pressed"), callable_mp(this, &EditorMainScreen25D::press_menu_item).bind(TOOLBAR_BUTTON_MODE_ROTATE));
	_toolbar_hbox->add_child(_toolbar_buttons[TOOLBAR_BUTTON_MODE_ROTATE]);
	_toolbar_hbox->add_child(memnew(VSeparator));

	// Set up the viewport.
	_editor_main_viewport = memnew(EditorMainViewport25D);
	_editor_main_viewport->set_name(StringName("EditorMainViewport25D"));
	_editor_main_viewport->set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	_editor_main_viewport->set_offset(Side::SIDE_TOP, 33.0f * EDSCALE);
	_editor_main_viewport->setup(this);
	add_child(_editor_main_viewport);

	EditorInterface::get_singleton()->get_selection()->connect(StringName("selection_changed"), callable_mp(this, &EditorMainScreen25D::_on_selection_changed));

	set_process(true);
}
