#include "register_types.h"

#if GDEXTENSION
#include <godot_cpp/classes/editor_plugin_registration.hpp>
#include <godot_cpp/classes/engine.hpp>
#elif GODOT_MODULE
#include "core/config/engine.h"
#include "editor/plugins/editor_plugin.h"
#include "editor/themes/editor_color_map.h"
#endif

#include "math/world_25d.h"
#include "nodes/camera_25d.h"
#include "nodes/node_25d.h"

#ifdef TOOLS_ENABLED
#include "editor/godot_25d_editor_plugin.h"
#endif // TOOLS_ENABLED

#if GDEXTENSION
// GDExtension has a nervous breakdown whenever singleton or casted classes are not registered.
// We don't need to register these in principle, and we don't need it for a module, just for GDExtension.
#ifdef TOOLS_ENABLED
#include "editor/viewport/editor_input_surface_25d.h"
#include "editor/viewport/editor_main_screen_25d.h"
#include "editor/viewport/editor_main_viewport_25d.h"
#include "editor/viewport/editor_viewport_rotation_25d.h"
#endif // TOOLS_ENABLED
#endif // GDEXTENSION

inline void add_godot_singleton(const StringName &p_singleton_name, Object *p_object) {
#if GDEXTENSION
	Engine::get_singleton()->register_singleton(p_singleton_name, p_object);
#elif GODOT_MODULE
	Engine::get_singleton()->add_singleton(Engine::Singleton(p_singleton_name, p_object));
#endif
}

inline void remove_godot_singleton(const StringName &p_singleton_name) {
#if GDEXTENSION
	Engine::get_singleton()->unregister_singleton(p_singleton_name);
#elif GODOT_MODULE
	Engine::get_singleton()->remove_singleton(p_singleton_name);
#endif
}

void initialize_2pt5d_module(ModuleInitializationLevel p_level) {
	// Note: Classes MUST be registered in inheritance order.
	if (p_level == MODULE_INITIALIZATION_LEVEL_CORE) {
		GDREGISTER_CLASS(World25D);
	} else if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		GDREGISTER_CLASS(Node25D);
		GDREGISTER_CLASS(Camera25D);
#ifdef TOOLS_ENABLED
	} else if (p_level == MODULE_INITIALIZATION_LEVEL_EDITOR) {
#ifdef GDEXTENSION
		GDREGISTER_CLASS(EditorCreate25DSceneButton);
		GDREGISTER_CLASS(EditorInputSurface25D);
		GDREGISTER_CLASS(EditorMainScreen25D);
		GDREGISTER_CLASS(EditorMainViewport25D);
		GDREGISTER_CLASS(EditorViewportRotation25D);
		GDREGISTER_CLASS(Godot25DEditorPlugin);
#elif GODOT_MODULE
		EditorColorMap::add_conversion_color_pair("c9f", "74a");
#endif // GDEXTENSION or GODOT_MODULE
		EditorPlugins::add_by_type<Godot25DEditorPlugin>();
#endif // TOOLS_ENABLED
	}
}

void uninitialize_2pt5d_module(ModuleInitializationLevel p_level) {
	if (p_level == MODULE_INITIALIZATION_LEVEL_SCENE) {
		// Perform cleanup here.
		// You can remove singletons using remove_godot_singleton().
	}
}
