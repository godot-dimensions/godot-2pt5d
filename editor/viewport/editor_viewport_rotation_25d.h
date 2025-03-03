#pragma once

#include "editor_viewport_2pt5d_defines.h"

// Editor viewport rotation navigation gizmo (the thing in the top right corner).
// Shows the current view rotation and allows the user to rotate the view.
// Users can drag to spin like a ball, or click on an axis to make that perpendicular to the view.
class EditorViewportRotation25D : public Control {
	GDCLASS(EditorViewportRotation25D, Control);

	enum AxisType2D {
		AXIS_TYPE_CIRCLE_POSITIVE,
		AXIS_TYPE_CIRCLE_NEGATIVE,
		AXIS_TYPE_LINE,
		AXIS_TYPE_SPECIAL,
	};

	struct Axis2D {
		Vector2 screen_point;
		float z_index = 0.0f;
		AxisType2D axis_type = AXIS_TYPE_CIRCLE_POSITIVE;
		int axis_number = -2; // 0 to 2 for XYZ, or -1 for the background circle, or -2 for none.
	};

	struct Axis2DCompare {
		_FORCE_INLINE_ bool operator()(const Axis2D &p_left, const Axis2D &p_right) const {
			return p_left.z_index < p_right.z_index;
		}
	};

	EditorMainViewport25D *_editor_main_viewport = nullptr;
	PackedColorArray _axis_colors;
	Axis2D _focused_axis;
	Vector2i _orbiting_mouse_start;
	int _orbiting_mouse_button_index = -1;
	real_t _editor_scale = 1.0f;

	void _draw_axis_circle(const Axis2D &p_axis);
	void _draw_axis_line(const Axis2D &p_axis, const Vector2 &p_center);
	void _draw_axis_special(const Axis2D &p_axis);
	void _draw_filled_arc(const Vector2 &p_center, const real_t p_radius, const real_t p_start_angle, const real_t p_end_angle, const Color &p_color);
	void _get_sorted_axis(const Vector2 &p_center, Vector<Axis2D> &r_axis);
	void _on_mouse_exited();
	void _process_click(const int p_index, const Vector2 p_position, const bool p_pressed, const bool p_modifier);
	void _process_drag(const Ref<InputEventWithModifiers> &p_event, const int p_index, const Vector2 p_position);
	void _update_focus();
	void _update_theme();

protected:
	static void _bind_methods() {}
	void _notification(int p_what);

public:
#if GDEXTENSION
	void _draw() override;
#elif GODOT_MODULE
	void _draw();
#endif

	virtual void GDEXTMOD_GUI_INPUT(const Ref<InputEvent> &p_event) override;

	void setup(EditorMainViewport25D *p_editor_main_viewport);
};
