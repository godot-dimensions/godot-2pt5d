#pragma once

#include "../../godot_2pt5d_defines.h"
#include "../../math/basis_25d.h"
#include "../../nodes/node_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/control.hpp>
#include <godot_cpp/classes/editor_interface.hpp>
#include <godot_cpp/classes/editor_settings.hpp>
#include <godot_cpp/classes/editor_undo_redo_manager.hpp>
#include <godot_cpp/classes/input.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_mouse_motion.hpp>

#define EDITOR_GET(m_name) Variant()
#define EDSCALE EditorInterface::get_singleton()->get_editor_scale()
#define ERR_THREAD_GUARD
#define TTR(m_text) m_text
#define get_editor_theme_icon(m_name) get_theme_icon(m_name, StringName("EditorIcons"))
#elif GODOT_MODULE
#include "editor/editor_settings.h"
#include "editor/editor_undo_redo_manager.h"
#include "editor/themes/editor_scale.h"
#include "scene/gui/control.h"

#if VERSION_HEX < 0x040400
#define set_button_icon set_icon
#endif

#define KEY_E Key::E
#define KEY_F Key::F
#define KEY_Q Key::Q
#define KEY_T Key::T
#define KEY_W Key::W

#define MOUSE_BUTTON_MASK_MIDDLE MouseButtonMask::MIDDLE
#define MOUSE_BUTTON_MASK_RIGHT MouseButtonMask::RIGHT
#define MOUSE_BUTTON_RIGHT MouseButton::RIGHT
#define MOUSE_BUTTON_WHEEL_UP MouseButton::WHEEL_UP
#define MOUSE_BUTTON_WHEEL_DOWN MouseButton::WHEEL_DOWN
#endif

class EditorInputSurface25D;
class EditorMainScreen25D;
class EditorMainViewport25D;
class EditorViewportRotation25D;
