#include "editor_viewport_rotation_25d.h"

#include "editor_main_viewport_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/font.hpp>
#include <godot_cpp/classes/input_event_screen_drag.hpp>
#include <godot_cpp/classes/input_event_screen_touch.hpp>
#elif GODOT_MODULE
#include "scene/resources/font.h"
#endif

// How far apart the axis circles are from the center of the gizmo.
constexpr float GIZMO_BASE_SIZE = 80.0f;

String _get_axis_letter_2pt5d(int p_axis) {
	switch (p_axis) {
		case 0:
			return "X";
		case 1:
			return "Y";
		case 2:
			return "Z";
		default:
			return "?";
	}
}

void EditorViewportRotation25D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (!is_connected(StringName("mouse_exited"), callable_mp(this, &EditorViewportRotation25D::_on_mouse_exited))) {
				connect(StringName("mouse_exited"), callable_mp(this, &EditorViewportRotation25D::_on_mouse_exited));
			}
			_update_theme();
		} break;
		case NOTIFICATION_DRAW: {
			if (_editor_main_viewport != nullptr) {
				_draw();
			}
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			_update_theme();
		} break;
	}
}

void EditorViewportRotation25D::_draw() {
	const Vector2 center = get_size() / 2.0f;
	if (_focused_axis.axis_number > -2 || _orbiting_mouse_button_index != -1) {
		draw_circle(center, center.x, Color(0.5f, 0.5f, 0.5f, 0.25f), true, -1.0f, true);
	}
	Vector<Axis2D> axis_to_draw;
	_get_sorted_axis(center, axis_to_draw);
	for (int i = 0; i < axis_to_draw.size(); ++i) {
		Axis2D axis = axis_to_draw[i];
		if (axis.axis_type == AXIS_TYPE_LINE) {
			_draw_axis_line(axis, center);
		} else if (axis.axis_type == AXIS_TYPE_SPECIAL) {
			_draw_axis_special(axis);
		} else {
			_draw_axis_circle(axis);
		}
	}
}

void EditorViewportRotation25D::_draw_axis_circle(const Axis2D &p_axis) {
	const bool is_focused = _focused_axis.axis_number == p_axis.axis_number && _focused_axis.axis_type == p_axis.axis_type;
	const Color axis_color = _axis_colors[p_axis.axis_number];
	const float alpha = MIN(2.0f, p_axis.z_index + 2.0f);
	const Color color = is_focused ? Color(axis_color.lightened(0.75f), 1.0f) : Color(axis_color, alpha);
	const real_t axis_circle_radius = (8.0f + p_axis.z_index) * _editor_scale;
	if (p_axis.axis_type == AXIS_TYPE_CIRCLE_POSITIVE) {
		draw_circle(p_axis.screen_point, axis_circle_radius, color, true, -1.0f, true);
		// Draw the axis letter for the positive axes.
		const String axis_letter = _get_axis_letter_2pt5d(p_axis.axis_number);
		const Ref<Font> &font = get_theme_font(StringName("rotation_control"), StringName("EditorFonts"));
		const int font_size = get_theme_font_size(StringName("rotation_control_size"), StringName("EditorFonts"));
		const Size2 char_size = font->get_char_size(axis_letter[0], font_size);
		const Vector2 char_offset = Vector2(-char_size.width / 2.0f, char_size.height * 0.25f);
		draw_char(font, p_axis.screen_point + char_offset, axis_letter, font_size, Color(0.0f, 0.0f, 0.0f, alpha * 0.6f));
	} else {
		// Draw an outline around the negative axes.
		draw_circle(p_axis.screen_point, axis_circle_radius, color, true, -1.0f, true);
		draw_circle(p_axis.screen_point, axis_circle_radius * 0.8f, color.darkened(0.4f), true, -1.0f, true);
	}
}

void EditorViewportRotation25D::_draw_axis_line(const Axis2D &p_axis, const Vector2 &p_center) {
	const bool is_focused = _focused_axis.axis_number == p_axis.axis_number && _focused_axis.axis_type == AXIS_TYPE_CIRCLE_POSITIVE;
	const Color axis_color = _axis_colors[p_axis.axis_number];
	const float alpha = MIN(2.0f, p_axis.z_index + 2.0f);
	const Color color = is_focused ? Color(axis_color.lightened(0.75f), 1.0f) : Color(axis_color, alpha);
	draw_line(p_center, p_axis.screen_point, color, 1.5f * _editor_scale, true);
}

void EditorViewportRotation25D::_draw_axis_special(const Axis2D &p_axis) {
	const bool is_focused = _focused_axis.axis_number == p_axis.axis_number && _focused_axis.axis_type == p_axis.axis_type;
	Color x_color = _axis_colors[0];
	Color y_color = _axis_colors[1];
	Color z_color = _axis_colors[2];
	if (is_focused) {
		x_color = x_color.lightened(0.25f);
		y_color = y_color.lightened(0.25f);
		z_color = z_color.lightened(0.25f);
	}
	const real_t special_radius = 5.0f * _editor_scale;
	const Ref<Font> &font = get_theme_font(StringName("rotation_control"), StringName("EditorFonts"));
	const int font_size = get_theme_font_size(StringName("rotation_control_size"), StringName("EditorFonts")) * 3 / 4;
	switch (p_axis.axis_number) {
		case 0: {
			// Isometric.
			_draw_filled_arc(p_axis.screen_point, special_radius, -Math_TAU / 12.0f, Math_TAU / 4.0f, x_color);
			_draw_filled_arc(p_axis.screen_point, special_radius, Math_TAU * 7.0f / 12.0f, Math_TAU * 11.0f / 12.0f, y_color);
			_draw_filled_arc(p_axis.screen_point, special_radius, Math_TAU / 4.0f, Math_TAU * 7.0f / 12.0f, z_color);
			draw_char(font, p_axis.screen_point + Vector2(-font_size * 0.15f, font_size * 0.4f), "I", font_size, Color(0.0f, 0.0f, 0.0f, 1.0f));
		} break;
		case 1: {
			// Dimetric.
			_draw_filled_arc(p_axis.screen_point, special_radius, 0.0f, Math_TAU / 4.0f, x_color);
			_draw_filled_arc(p_axis.screen_point, special_radius, Math_TAU / 2.0f, Math_TAU, y_color);
			_draw_filled_arc(p_axis.screen_point, special_radius, Math_TAU / 4.0f, Math_TAU / 2.0f, z_color);
			draw_char(font, p_axis.screen_point + Vector2(-font_size * 0.35f, font_size * 0.4f), "D", font_size, Color(0.0f, 0.0f, 0.0f, 1.0f));
		} break;
		case 2: {
			// Trimetric.
			_draw_filled_arc(p_axis.screen_point, special_radius, 0.0f, Math_TAU / 4.0f, x_color);
			_draw_filled_arc(p_axis.screen_point, special_radius, Math_TAU * 3.0f / 4.0f, Math_TAU, y_color);
			_draw_filled_arc(p_axis.screen_point, special_radius, Math_TAU / 4.0f, Math_TAU * 3.0f / 4.0f, z_color);
			draw_char(font, p_axis.screen_point + Vector2(-font_size * 0.28f, font_size * 0.4f), "T", font_size, Color(0.0f, 0.0f, 0.0f, 1.0f));
		} break;
		default: {
			// From Angle.
			_draw_filled_arc(p_axis.screen_point, special_radius, Math_TAU / 2.0f, Math_TAU, y_color);
			_draw_filled_arc(p_axis.screen_point, special_radius, 0.0f, Math_TAU / 2.0f, z_color);
			draw_char(font, p_axis.screen_point + Vector2(-font_size * 0.32f, font_size * 0.4f), "A", font_size, Color(0.0f, 0.0f, 0.0f, 1.0f));
		} break;
	}
}

void EditorViewportRotation25D::_draw_filled_arc(const Vector2 &p_center, real_t p_radius, real_t p_start_angle, real_t p_end_angle, const Color &p_color) {
	ERR_THREAD_GUARD;
	constexpr int ARC_POINTS = 16;
	const real_t angle_step = (p_end_angle - p_start_angle) / (ARC_POINTS - 1);
	PackedVector2Array points;
	points.resize(ARC_POINTS + 1);
	for (int i = 0; i < ARC_POINTS; i++) {
		const real_t angle = p_start_angle + i * angle_step;
		points.set(i, p_center + Vector2(Math::cos(angle), Math::sin(angle)) * p_radius);
	}
	points.set(ARC_POINTS, p_center);
	PackedColorArray colors = { p_color };
	draw_polygon(points, colors);
}

void EditorViewportRotation25D::_get_sorted_axis(const Vector2 &p_center, Vector<Axis2D> &r_axis) {
	const Vector2 center = get_size() / 2.0f;
	const real_t radius = center.x - 10.0f * _editor_scale;
	const Basis25D camera_basis = _editor_main_viewport->get_view_basis_25d();
	// Add axes in each direction.
	for (int i = 0; i < 3; i++) {
		const Vector2 axis_2pt5d = camera_basis[i];
		const Vector2 axis_screen_position = axis_2pt5d * radius;

		if (axis_screen_position.is_zero_approx()) {
			// Special case when the axis is aligned with the camera.
			Axis2D axis;
			axis.axis_type = AXIS_TYPE_CIRCLE_POSITIVE;
			axis.axis_number = i;
			axis.screen_point = center;
			r_axis.push_back(axis);
		} else {
			Axis2D pos_axis;
			pos_axis.axis_type = AXIS_TYPE_CIRCLE_POSITIVE;
			pos_axis.axis_number = i;
			pos_axis.screen_point = center + axis_screen_position;
			pos_axis.z_index = camera_basis.draw_order[i];
			r_axis.push_back(pos_axis);

			Axis2D line_axis;
			line_axis.axis_type = AXIS_TYPE_LINE;
			line_axis.axis_number = i;
			line_axis.screen_point = center + axis_screen_position;
			// Ensure the lines draw behind their connected circles.
			line_axis.z_index = MIN(camera_basis.draw_order[i], 0.0f) - (float)CMP_EPSILON;
			r_axis.push_back(line_axis);

			Axis2D neg_axis;
			neg_axis.axis_type = AXIS_TYPE_CIRCLE_NEGATIVE;
			neg_axis.axis_number = i;
			neg_axis.screen_point = center - axis_screen_position;
			neg_axis.z_index = -camera_basis.draw_order[i];
			r_axis.push_back(neg_axis);
		}
	}
	// Add special cases to the corners for Isometric, Dimetric, Trimetric, and From Angle.
	{
		const real_t special_pos = center.x - 6.0f * _editor_scale;
		Axis2D special_axis;
		special_axis.axis_type = AXIS_TYPE_SPECIAL;
		special_axis.screen_point = center + Vector2(special_pos, -special_pos);
		special_axis.axis_number = 0;
		r_axis.push_back(special_axis);
		special_axis.screen_point = center + Vector2(special_pos, special_pos);
		special_axis.axis_number = 1;
		r_axis.push_back(special_axis);
		special_axis.screen_point = center + Vector2(-special_pos, special_pos);
		special_axis.axis_number = 2;
		r_axis.push_back(special_axis);
		special_axis.screen_point = center + Vector2(-special_pos, -special_pos);
		special_axis.axis_number = 3;
		r_axis.push_back(special_axis);
	}
	// Sort the axes by z_index.
	r_axis.sort_custom<Axis2DCompare>();
}

void EditorViewportRotation25D::_on_mouse_exited() {
	_focused_axis.axis_number = -2;
	queue_redraw();
}

void EditorViewportRotation25D::_process_click(const int p_index, const Vector2 p_position, const bool p_pressed, const bool p_modifier) {
	if (_orbiting_mouse_button_index != -1 && _orbiting_mouse_button_index != p_index) {
		return;
	}
	const Vector2 center = get_size() / 2.0f;
	if (p_pressed) {
		if (p_position.distance_to(center) < center.x) {
			_orbiting_mouse_button_index = p_index;
		}
	} else {
		// Only process the click if the mouse is released over the gizmo.
		// This allows people to click-and-drag to orbit without changing the axis.
		if (_focused_axis.axis_number > -1) {
			Ref<World25D> world = _editor_main_viewport->get_world_25d();
			if (world.is_valid()) {
				if (_focused_axis.axis_type == AXIS_TYPE_SPECIAL) {
					switch (_focused_axis.axis_number) {
						case 0: {
							world->set_basis_preset(World25D::PRESET_ISOMETRIC);
						} break;
						case 1: {
							world->set_basis_preset(World25D::PRESET_DIMETRIC);
						} break;
						case 2: {
							world->set_basis_preset(World25D::PRESET_TRIMETRIC);
						} break;
						default: {
							world->set_basis_preset(World25D::PRESET_FROM_ANGLE);
						} break;
					}
				} else if (p_modifier) {
					switch (_focused_axis.axis_number) {
						case 0: {
							world->set_basis_preset(World25D::PRESET_OBLIQUE_X);
						} break;
						case 1: {
							world->set_basis_preset(World25D::PRESET_OBLIQUE_Y);
						} break;
						case 2: {
							world->set_basis_preset(World25D::PRESET_OBLIQUE_Z);
						} break;
					}
				} else {
					const bool neg = (_focused_axis.axis_type == AXIS_TYPE_CIRCLE_NEGATIVE) != (_focused_axis.screen_point == center);
					switch (_focused_axis.axis_number) {
						case 0: {
							world->set_basis_preset(neg ? World25D::PRESET_FROM_NEG_X : World25D::PRESET_FROM_POS_X);
						} break;
						case 1: {
							world->set_basis_preset(neg ? World25D::PRESET_FROM_NEG_Y : World25D::PRESET_FROM_POS_Y);
						} break;
						case 2: {
							world->set_basis_preset(neg ? World25D::PRESET_FROM_NEG_Z : World25D::PRESET_FROM_POS_Z);
						} break;
					}
				}
			}
			_update_focus();
		}
		_orbiting_mouse_button_index = -1;
		if (Input::get_singleton()->get_mouse_mode() == Input::MOUSE_MODE_CAPTURED) {
			Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_VISIBLE);
			Input::get_singleton()->warp_mouse(_orbiting_mouse_start);
		}
	}
	queue_redraw();
}

void EditorViewportRotation25D::_process_drag(const Ref<InputEventWithModifiers> &p_event, const int p_index, const Vector2 p_position) {
	if (_orbiting_mouse_button_index == p_index) {
		if (Input::get_singleton()->get_mouse_mode() == Input::MOUSE_MODE_VISIBLE) {
			Input::get_singleton()->set_mouse_mode(Input::MOUSE_MODE_CAPTURED);
			_orbiting_mouse_start = p_position;
		}
		_editor_main_viewport->navigation_orbit(p_event);
		_focused_axis.axis_number = -1;
	} else {
		_update_focus();
	}
	queue_redraw();
}

void EditorViewportRotation25D::_update_focus() {
	const Vector2 center = get_size() / 2.0f;
	const Vector2 mouse_pos = get_local_mouse_position();
	const int original_focus = _focused_axis.axis_number;
	_focused_axis = Axis2D();
	_focused_axis.z_index = -10.0f;
	if (mouse_pos.distance_to(center) < center.x) {
		_focused_axis.axis_number = -1;
	}
	Vector<Axis2D> axes;
	_get_sorted_axis(center, axes);
	for (int i = 0; i < axes.size(); i++) {
		const Axis2D &axis = axes[i];
		if (axis.z_index > _focused_axis.z_index && mouse_pos.distance_to(axis.screen_point) < 8.0f * _editor_scale) {
			_focused_axis = axis;
		}
	}

	if (_focused_axis.axis_number != original_focus) {
		queue_redraw();
	}
}

void EditorViewportRotation25D::_update_theme() {
	ERR_FAIL_NULL(_editor_main_viewport);
	_editor_scale = EDSCALE;
	const real_t scaled_size = GIZMO_BASE_SIZE * _editor_scale;
	set_custom_minimum_size(Vector2(scaled_size, scaled_size));
	set_offset(SIDE_RIGHT, -0.1f * scaled_size);
	set_offset(SIDE_BOTTOM, 1.1f * scaled_size);
	set_offset(SIDE_LEFT, -1.1f * scaled_size);
	set_offset(SIDE_TOP, 0.1f * scaled_size);
	_axis_colors = _editor_main_viewport->get_axis_colors();
	queue_redraw();
}

void EditorViewportRotation25D::GDEXTMOD_GUI_INPUT(const Ref<InputEvent> &p_event) {
	ERR_FAIL_COND(p_event.is_null());

	// Mouse events
	const Ref<InputEventMouseButton> mb = p_event;
	if (mb.is_valid() && mb->get_button_index() == MOUSE_BUTTON_LEFT) {
		const bool modifier = mb->is_shift_pressed() || mb->is_ctrl_pressed() || mb->is_meta_pressed() || mb->is_alt_pressed();
		_process_click(100, mb->get_position(), mb->is_pressed(), modifier);
	}

	const Ref<InputEventMouseMotion> mm = p_event;
	if (mm.is_valid()) {
		_process_drag(mm, 100, mm->get_global_position());
	}

	// Touch events
	const Ref<InputEventScreenTouch> screen_touch = p_event;
	if (screen_touch.is_valid()) {
		_process_click(screen_touch->get_index(), screen_touch->get_position(), screen_touch->is_pressed(), false);
	}

	const Ref<InputEventScreenDrag> screen_drag = p_event;
	if (screen_drag.is_valid()) {
		_process_drag(screen_drag, screen_drag->get_index(), screen_drag->get_position());
	}
}

void EditorViewportRotation25D::setup(EditorMainViewport25D *p_editor_main_viewport) {
	// Things that we should do in the constructor but can't in GDExtension
	// due to how GDExtension runs the constructor for each registered class.
	set_name(StringName("EditorViewportRotation25D"));
	set_anchors_and_offsets_preset(Control::PRESET_TOP_RIGHT);

	// Set up things with the arguments (not constructor things).
	_editor_main_viewport = p_editor_main_viewport;
}
