#pragma once

#include "viewport/editor_main_screen_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/editor_plugin.hpp>

#define GDEXTMOD_GET_PLUGIN_ICON _get_plugin_icon
#define GDEXTMOD_GET_PLUGIN_NAME _get_plugin_name
#define GDEXTMOD_HANDLES _handles
#define GDEXTMOD_HAS_MAIN_SCREEN _has_main_screen
#define GDEXTMOD_MAKE_VISIBLE _make_visible
#elif GODOT_MODULE
#include "editor/plugins/editor_plugin.h"

#if VERSION_HEX < 0x040400
#define GDEXTMOD_GET_PLUGIN_ICON get_icon
#define GDEXTMOD_GET_PLUGIN_NAME get_name
#else
#define GDEXTMOD_GET_PLUGIN_ICON get_plugin_icon
#define GDEXTMOD_GET_PLUGIN_NAME get_plugin_name
#endif
#define GDEXTMOD_HANDLES handles
#define GDEXTMOD_HAS_MAIN_SCREEN has_main_screen
#define GDEXTMOD_MAKE_VISIBLE make_visible
#endif

class EditorCreate25DSceneButton : public Button {
	GDCLASS(EditorCreate25DSceneButton, Button);

protected:
	static void _bind_methods() {}
	void _notification(int p_what);
};

class Godot25DEditorPlugin : public EditorPlugin {
	GDCLASS(Godot25DEditorPlugin, EditorPlugin);
	EditorMainScreen25D *_main_screen = nullptr;

	Button *_find_button_by_text(Node *p_start, const String &p_text);
	void _move_2pt5d_main_screen_tab_button() const;
	void _inject_2pt5d_scene_button();
	void _create_2pt5d_scene();

protected:
	static void _bind_methods();
	void _notification(int p_what);

public:
#if GDEXTENSION
	virtual Ref<Texture2D> GDEXTMOD_GET_PLUGIN_ICON() const override;
#elif GODOT_MODULE
	virtual const Ref<Texture2D> GDEXTMOD_GET_PLUGIN_ICON() const override;
#endif
	virtual String GDEXTMOD_GET_PLUGIN_NAME() const override { return "2.5D"; }
	virtual bool GDEXTMOD_HANDLES(Object *p_object) const override;
	virtual bool GDEXTMOD_HAS_MAIN_SCREEN() const override { return true; }
	virtual void GDEXTMOD_MAKE_VISIBLE(bool p_visible) override;

	Godot25DEditorPlugin();
};
