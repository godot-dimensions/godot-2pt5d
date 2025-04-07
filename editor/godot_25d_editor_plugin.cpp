#include "godot_25d_editor_plugin.h"

#include "../nodes/node_25d.h"
#include "icons/editor_2pt5d_icons.h"

#if GDEXTENSION
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_selection.hpp>
#include <godot_cpp/classes/v_box_container.hpp>
#include <godot_cpp/classes/window.hpp>
#elif GODOT_MODULE
#include "editor/editor_data.h"
#include "editor/editor_interface.h"
#include "scene/main/window.h"

#if VERSION_HEX < 0x040400
#define set_button_icon set_icon
#endif
#endif

Button *Godot25DEditorPlugin::_find_button_by_text_2pt5d(Node *p_start, const String &p_text) {
	Button *start_button = Object::cast_to<Button>(p_start);
	if (start_button != nullptr) {
		if (start_button->get_text() == p_text) {
			return start_button;
		}
	}
	for (int i = 0; i < p_start->get_child_count(); i++) {
		Button *button = _find_button_by_text_2pt5d(p_start->get_child(i), p_text);
		if (button != nullptr) {
			return button;
		}
	}
	return nullptr;
}

void EditorCreate25DSceneButton::_notification(int p_what) {
	if (p_what == NOTIFICATION_THEME_CHANGED) {
		set_button_icon(_generate_editor_2pt5d_icon("Node25D"));
	}
}

void Godot25DEditorPlugin::_move_2pt5d_main_screen_tab_button() const {
	Control *editor = EditorInterface::get_singleton()->get_base_control();
	ERR_FAIL_NULL(editor);
	// Move 2.5D button to the left of the "3D" button, to the right of the "2D" button.
	Node *button_asset_lib_tab = editor->find_child("AssetLib", true, false);
	ERR_FAIL_NULL(button_asset_lib_tab);
	Node *main_editor_button_hbox = button_asset_lib_tab->get_parent();
	Button *button_2pt5d_tab = GET_NODE_TYPE(main_editor_button_hbox, Button, "2_5D");
	Button *button_3d_tab = GET_NODE_TYPE(main_editor_button_hbox, Button, "3D");
	ERR_FAIL_NULL(button_2pt5d_tab);
	ERR_FAIL_NULL(button_3d_tab);
	main_editor_button_hbox->move_child(button_2pt5d_tab, button_3d_tab->get_index());
}

void Godot25DEditorPlugin::_inject_2pt5d_scene_button() {
	Control *editor = EditorInterface::get_singleton()->get_base_control();
	ERR_FAIL_NULL(editor);
	// Add a "2.5D Scene" button above the "3D Scene" button, below the "2D Scene" button.
	Button *button_other_node_scene = _find_button_by_text_2pt5d(editor, "Other Node");
	ERR_FAIL_NULL(button_other_node_scene);
	Control *beginner_node_shortcuts = Object::cast_to<Control>(button_other_node_scene->get_parent()->get_child(0));
	Button *button_3d_scene = _find_button_by_text_2pt5d(beginner_node_shortcuts, "3D Scene");
	ERR_FAIL_NULL(button_3d_scene);
	// Now that we know where to insert the button, create it.
	EditorCreate25DSceneButton *button_2pt5d = memnew(EditorCreate25DSceneButton);
	button_2pt5d->set_name("Create25DSceneButton");
	button_2pt5d->set_text(TTR("2.5D Scene"));
	button_2pt5d->set_button_icon(_generate_editor_2pt5d_icon("Node25D"));
	button_2pt5d->connect(StringName("pressed"), callable_mp(this, &Godot25DEditorPlugin::_create_2pt5d_scene));
	beginner_node_shortcuts->add_child(button_2pt5d);
	beginner_node_shortcuts->move_child(button_2pt5d, button_3d_scene->get_index());
}

void Godot25DEditorPlugin::_create_2pt5d_scene() {
	EditorInterface *editor_interface = EditorInterface::get_singleton();
	Node25D *new_node = memnew(Node25D);
	Node *editor_node = editor_interface->get_base_control()->get_parent();
	ERR_FAIL_NULL(editor_node);
	editor_node->call(StringName("set_edited_scene"), new_node);
	editor_interface->edit_node(new_node);
	EditorSelection *editor_selection = editor_interface->get_selection();
	editor_selection->clear();
	editor_selection->add_node(new_node);
}

void Godot25DEditorPlugin::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			_move_2pt5d_main_screen_tab_button();
			call_deferred(StringName("_inject_2pt5d_scene_button"));
		} break;
	}
}

#if GDEXTENSION
Ref<Texture2D> Godot25DEditorPlugin::GDEXTMOD_GET_PLUGIN_ICON() const
#elif GODOT_MODULE
const Ref<Texture2D> Godot25DEditorPlugin::GDEXTMOD_GET_PLUGIN_ICON() const
#endif
{
	return _generate_editor_2pt5d_icon("25D");
}

bool Godot25DEditorPlugin::GDEXTMOD_HANDLES(Object *p_object) const {
	return Object::cast_to<Node25D>(p_object) != nullptr;
}

void Godot25DEditorPlugin::GDEXTMOD_MAKE_VISIBLE(bool p_visible) {
	ERR_FAIL_NULL(_main_screen);
	_main_screen->set_visible(p_visible);
}

void Godot25DEditorPlugin::_bind_methods() {
	ClassDB::bind_method(D_METHOD("_inject_2pt5d_scene_button"), &Godot25DEditorPlugin::_inject_2pt5d_scene_button);
}

Godot25DEditorPlugin::Godot25DEditorPlugin() {
	set_name(StringName("Godot25DEditorPlugin"));
	Control *godot_editor_main_screen = EditorInterface::get_singleton()->get_editor_main_screen();
	if (godot_editor_main_screen->has_node(NodePath("EditorMainScreen25D"))) {
		_main_screen = GET_NODE_TYPE(godot_editor_main_screen, EditorMainScreen25D, "EditorMainScreen25D");
	} else {
		_main_screen = memnew(EditorMainScreen25D);
		_main_screen->setup(get_undo_redo());
		godot_editor_main_screen->add_child(_main_screen);
	}
}
