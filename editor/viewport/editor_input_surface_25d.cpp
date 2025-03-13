#include "editor_input_surface_25d.h"

#include "editor_main_screen_25d.h"
#include "editor_main_viewport_25d.h"

void EditorInputSurface25D::GDEXTMOD_GUI_INPUT(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND(p_event.is_null());
	ERR_FAIL_NULL(_editor_main_screen);
	Ref<InputEventMouseButton> mouse_button = p_event;
	if (mouse_button.is_valid()) {
		MouseButton mouse_button_index = mouse_button->get_button_index();
		if (mouse_button_index == MOUSE_BUTTON_WHEEL_UP) {
			if (mouse_button->is_pressed()) {
				const Vector2 mouse_offset = get_local_mouse_position() - get_size() / 2.0f;
				_editor_main_viewport_2pt5d->navigation_change_zoom_level(1, mouse_offset);
			}
		} else if (mouse_button_index == MOUSE_BUTTON_WHEEL_DOWN) {
			if (mouse_button->is_pressed()) {
				const Vector2 mouse_offset = get_local_mouse_position() - get_size() / 2.0f;
				_editor_main_viewport_2pt5d->navigation_change_zoom_level(-1, mouse_offset);
			}
		} else {
			_editor_main_viewport_2pt5d->viewport_mouse_input(mouse_button);
		}
		grab_focus();
		return;
	}
	Ref<InputEventMouseMotion> mouse_motion = p_event;
	if (mouse_motion.is_valid()) {
		BitField<MouseButtonMask> mouse_button_mask = mouse_motion->get_button_mask();
		if (mouse_button_mask.has_flag(MOUSE_BUTTON_MASK_MIDDLE) || mouse_button_mask.has_flag(MOUSE_BUTTON_MASK_RIGHT)) {
			_editor_main_viewport_2pt5d->navigation_pan(mouse_motion);
		} else {
			_editor_main_viewport_2pt5d->viewport_mouse_input(mouse_motion);
			return;
		}
		grab_focus();
		return;
	}
	Ref<InputEventKey> key = p_event;
	if (key.is_valid()) {
		if (key->is_pressed()) {
			if (!Input::get_singleton()->is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)) {
				if (key->get_keycode() == KEY_Q) {
					_editor_main_screen->press_menu_item(EditorMainScreen25D::TOOLBAR_BUTTON_MODE_SELECT);
				} else if (key->get_keycode() == KEY_W) {
					_editor_main_screen->press_menu_item(EditorMainScreen25D::TOOLBAR_BUTTON_MODE_MOVE);
				} else if (key->get_keycode() == KEY_E) {
					_editor_main_screen->press_menu_item(EditorMainScreen25D::TOOLBAR_BUTTON_MODE_ROTATE);
				} else if (key->get_keycode() == KEY_T) {
					_editor_main_screen->press_menu_item(EditorMainScreen25D::TOOLBAR_BUTTON_USE_LOCAL_TRANSFORM);
				} else if (key->get_keycode() == KEY_F) {
					_editor_main_screen->press_menu_item(EditorMainScreen25D::TOOLBAR_BUTTON_FOCUS_SELECTED_NODES);
				}
			}
		}
		return;
	}
}

void EditorInputSurface25D::setup(EditorMainScreen25D *p_editor_main_screen, EditorMainViewport25D *p_editor_main_viewport) {
	// Things that we should do in the constructor but can't in GDExtension
	// due to how GDExtension runs the constructor for each registered class.
	set_name(StringName("EditorInputSurface25D"));
	set_anchors_and_offsets_preset(Control::PRESET_FULL_RECT);
	set_focus_mode(FOCUS_ALL);

	// Set up things with the arguments (not constructor things).
	_editor_main_screen = p_editor_main_screen;
	_editor_main_viewport_2pt5d = p_editor_main_viewport;
}
