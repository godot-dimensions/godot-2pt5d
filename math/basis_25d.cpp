#include "basis_25d.h"

// Trivial getters and setters.

Vector2 Basis25D::get_axis(const int p_axis) const {
	ERR_FAIL_INDEX_V(p_axis, 3, Vector2());
	return coord[p_axis];
}

void Basis25D::set_axis(const int p_axis, const Vector2 &p_axis_vec) {
	ERR_FAIL_INDEX(p_axis, 3);
	coord[p_axis] = p_axis_vec;
}

Vector3 Basis25D::get_row(const int p_row) const {
	ERR_FAIL_INDEX_V(p_row, 3, Vector3());
	switch (p_row) {
		case 0:
			return Vector3(axes.x.x, axes.y.x, axes.z.x);
		case 1:
			return Vector3(axes.x.y, axes.y.y, axes.z.y);
	}
	return draw_order;
}

void Basis25D::set_row(const int p_row, const Vector3 &p_row_vec) {
	ERR_FAIL_INDEX(p_row, 3);
	switch (p_row) {
		case 0:
			axes.x.x = p_row_vec.x;
			axes.y.x = p_row_vec.y;
			axes.z.x = p_row_vec.z;
			break;
		case 1:
			axes.x.y = p_row_vec.x;
			axes.y.y = p_row_vec.y;
			axes.z.y = p_row_vec.z;
			break;
		case 2:
			draw_order = p_row_vec;
			break;
	}
}

// Math.

real_t Basis25D::calculate_2d_rotation(const Vector3 &p_y_axis_up) const {
	const Vector2 local_y = xform_3d_to_2d(p_y_axis_up);
	if (local_y.is_zero_approx()) {
		return 0.0;
	}
	// Note: This is intentionally passing `y = y.x` and `x = -y.y` to atan2.
	// The default rotation is +Y up in 3D, but 2D rotation has 0.0 on the right.
	return Math::atan2(local_y.x, -local_y.y);
}

real_t Basis25D::calculate_draw_order(const Vector3 &p_vector3) const {
	return draw_order.dot(p_vector3);
}

Vector2 Basis25D::xform_3d_to_2d(const Vector3 &p_vector3) const {
	return axes.x * p_vector3.x + axes.y * p_vector3.y + axes.z * p_vector3.z;
}

Vector3 Basis25D::xform_inv_2d_to_3d(const Vector2 &p_vector2) const {
	Vector3 xform_transpose = get_row(0) * p_vector2.x + get_row(1) * p_vector2.y;
	return xform_transpose.slide(draw_order);
}

bool Basis25D::is_equal_approx(const Basis25D &p_b) const {
	return axes.x.is_equal_approx(p_b.axes.x) && axes.y.is_equal_approx(p_b.axes.y) && axes.z.is_equal_approx(p_b.axes.z) && draw_order.is_equal_approx(p_b.draw_order);
}

Basis25D::operator String() const {
	return String("[X: ") + axes.x.operator String() + ", Y: " + axes.y.operator String() + ", Z: " + axes.z.operator String() + ", DO: " + draw_order.operator String() + "]";
}

Basis25D::operator Basis() const {
	return Basis(Vector3(axes.x.x, axes.x.y, draw_order.x), Vector3(axes.y.x, axes.y.y, draw_order.y), Vector3(axes.z.x, axes.z.y, draw_order.z));
}

Basis25D::operator Transform2D() const {
	return Transform2D(axes.x, axes.y, axes.z);
}

// Constructors.

// This is both sqrt(3)/2 and also cos(tau/12) or cos(30 deg).
#define Math_SQRT3_OVER_2 0.86602540378443864676372317
// This is sqrt(1/3) following the convention of Math_SQRT12.
#define Math_SQRT13 0.57735026918962576450914878
#define AXONOMETRIC_DRAW_ORDER Vector3(Math_SQRT13, Math_SQRT13, Math_SQRT13)

Basis25D Basis25D::from_preset(const Preset p_preset, const real_t p_angle, const real_t p_angle_z) {
	// Note: All of these include a flip to match Godot's +Y-down 2D coordinate system.
	switch (p_preset) {
		case PRESET_CUSTOM:
		case PRESET_FROM_ANGLE: {
			const real_t sine = Math::sin(p_angle);
			const real_t cosine = Math::cos(p_angle);
			return Basis25D(Vector2(1, 0), Vector2(0, -cosine), Vector2(0, sine), Vector3(CMP_EPSILON, sine, cosine));
		}
		case PRESET_ISOMETRIC:
			return Basis25D(Vector2(Math_SQRT3_OVER_2, 0.5), Vector2(0, -1), Vector2(-Math_SQRT3_OVER_2, 0.5), AXONOMETRIC_DRAW_ORDER);
		case PRESET_DIMETRIC: {
			const real_t sine = Math::sin(p_angle);
			const real_t cosine = Math::cos(p_angle);
			return Basis25D(Vector2(cosine, sine), Vector2(0, -1), Vector2(-cosine, sine), AXONOMETRIC_DRAW_ORDER);
		}
		case PRESET_TRIMETRIC: {
			const real_t sine = Math::sin(p_angle);
			const real_t cosine = Math::cos(p_angle);
			const real_t sine_z = Math::sin(p_angle_z);
			const real_t cosine_z = Math::cos(p_angle_z);
			return Basis25D(Vector2(cosine, sine), Vector2(0, -1), Vector2(-cosine_z, sine_z), AXONOMETRIC_DRAW_ORDER);
		}
		case PRESET_OBLIQUE_X:
			return Basis25D(Vector2(Math_SQRT12, Math_SQRT12), Vector2(0, -1), Vector2(-1, 0), Vector3(1, 0, 0));
		case PRESET_OBLIQUE_Y:
			return Basis25D(Vector2(1, 0), Vector2(-Math_SQRT12, -Math_SQRT12), Vector2(0, 1), Vector3(0, 1, 0));
		case PRESET_OBLIQUE_Z:
			return Basis25D(Vector2(1, 0), Vector2(0, -1), Vector2(-Math_SQRT12, Math_SQRT12), Vector3(0, 0, 1));
		case PRESET_FROM_POS_X:
			return Basis25D(Vector2(0, 0), Vector2(0, -1), Vector2(-1, 0), Vector3(1, 0, 0));
		case PRESET_FROM_NEG_X:
			return Basis25D(Vector2(0, 0), Vector2(0, -1), Vector2(1, 0), Vector3(-1, 0, 0));
		case PRESET_FROM_POS_Y:
			return Basis25D(Vector2(1, 0), Vector2(0, 0), Vector2(0, 1), Vector3(0, 1, 0));
		case PRESET_FROM_NEG_Y:
			return Basis25D(Vector2(1, 0), Vector2(0, 0), Vector2(0, -1), Vector3(0, -1, 0));
		case PRESET_FROM_POS_Z:
			return Basis25D(Vector2(1, 0), Vector2(0, -1), Vector2(0, 0), Vector3(0, 0, 1));
		case PRESET_FROM_NEG_Z:
			return Basis25D(Vector2(-1, 0), Vector2(0, -1), Vector2(0, 0), Vector3(0, 0, -1));
		case PRESET_MAX:
			break;
	}
	return Basis25D();
}

Basis25D::Basis25D() {
	// Default to 45 degree including a flip to match Godot's +Y-down 2D coordinate system.
	axes.x = Vector2(1, 0);
	axes.y = Vector2(0, -Math_SQRT12);
	axes.z = Vector2(0, Math_SQRT12);
	draw_order = Vector3(CMP_EPSILON, Math_SQRT12, Math_SQRT12);
}

Basis25D::Basis25D(const Vector2 &p_x, const Vector2 &p_y, const Vector2 &p_z, const Vector3 &p_draw_order) {
	axes.x = p_x;
	axes.y = p_y;
	axes.z = p_z;
	draw_order = p_draw_order;
}

Basis25D::Basis25D(const Basis &p_b) {
	axes.x = Vector2(p_b.get_column(0).x, p_b.get_column(0).y);
	axes.y = Vector2(p_b.get_column(1).x, p_b.get_column(1).y);
	axes.z = Vector2(p_b.get_column(2).x, p_b.get_column(2).y);
	draw_order = p_b.rows[2];
}

Basis25D::Basis25D(const Transform2D &p_t) {
	axes.x = p_t[0];
	axes.y = p_t[1];
	axes.z = p_t[2];
}
