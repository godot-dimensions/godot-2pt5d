#pragma once

#include "../godot_2pt5d_defines.h"

struct _NO_DISCARD_ Basis25D {
	union {
		struct {
			Vector2 x;
			Vector2 y;
			Vector2 z;
		} axes;

		Vector2 coord[3];
	};
	Vector3 draw_order;

	// Keep this in sync with World25D.
	enum Preset {
		PRESET_CUSTOM,
		PRESET_FROM_ANGLE,
		PRESET_ISOMETRIC,
		PRESET_DIMETRIC,
		PRESET_TRIMETRIC,
		PRESET_OBLIQUE_X,
		PRESET_OBLIQUE_Y,
		PRESET_OBLIQUE_Z,
		PRESET_FROM_POS_X,
		PRESET_FROM_NEG_X,
		PRESET_FROM_POS_Y,
		PRESET_FROM_NEG_Y,
		PRESET_FROM_POS_Z,
		PRESET_FROM_NEG_Z,
		PRESET_MAX,
	};

	// Trivial getters and setters.
	const Vector2 &operator[](int p_axis) const {
		DEV_ASSERT((unsigned int)p_axis < 3);
		return coord[p_axis];
	}
	Vector2 &operator[](int p_axis) {
		DEV_ASSERT((unsigned int)p_axis < 3);
		return coord[p_axis];
	}
	Vector2 get_axis(const int p_axis) const;
	void set_axis(const int p_axis, const Vector2 &p_axis_vec);
	Vector3 get_row(const int p_row) const;
	void set_row(const int p_row, const Vector3 &p_row_vec);

	// Math.
	real_t calculate_2d_rotation(const Vector3 &p_y_axis_up) const;
	real_t calculate_draw_order(const Vector3 &p_vector3) const;
	Vector2 xform_3d_to_2d(const Vector3 &p_vector3) const;
	Vector3 xform_inv_2d_to_3d(const Vector2 &p_vector2) const;

	bool is_equal_approx(const Basis25D &p_b) const;
	operator String() const;
	// Basis25D can be packed into a Basis without loss, and it even preserves its meaning!
	// However note that if you tried to use this as a 3D basis you'd want to flip the Y row,
	// since the data in Basis25D assumes -Y is up in 2D, but in 3D +Y is up.
	operator Basis() const;
	// Basis25D can have its 3D-to-2D transformation packed into a Transform2D,
	// where the Z axis is stored as the origin of the Transform2D, and draw order is discarded.
	// This has no geometric meaning, it's just because Transform2D has three Vector2 columns.
	operator Transform2D() const;

	// Constructors.
	static Basis25D from_preset(const Preset p_preset = PRESET_FROM_ANGLE, const real_t p_angle = Math_TAU / 8.0, const real_t p_angle_z = Math_TAU / 16.0);
	Basis25D();
	Basis25D(const Vector2 &p_x, const Vector2 &p_y, const Vector2 &p_z, const Vector3 &p_draw_order);
	Basis25D(const Basis &p_b);
	Basis25D(const Transform2D &p_t);
};
