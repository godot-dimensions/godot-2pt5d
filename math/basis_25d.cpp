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

// Math.

real_t Basis25D::calculate_draw_order(const Vector3 &p_vector3) const {
	return draw_order.dot(p_vector3);
}

Vector2 Basis25D::xform(const Vector3 &p_vector3) const {
	return x * p_vector3.x + y * p_vector3.y + z * p_vector3.z;
}

bool Basis25D::is_equal_approx(const Basis25D &p_b) const {
	return x.is_equal_approx(p_b.x) && y.is_equal_approx(p_b.y) && z.is_equal_approx(p_b.z) && draw_order.is_equal_approx(p_b.draw_order);
}

Basis25D::operator String() const {
	String s = "[X: " + x + ", Y: " + y + ", Z: " + z;
	if (draw_order != Basis25D_DEFAULT_DRAW_ORDER) {
		s += ", DO: " + draw_order;
	}
	return s + "]";
}

Basis25D::operator Basis() const {
	return Basis(Vector3(x.x, x.y, draw_order.x), Vector3(y.x, y.y, draw_order.y), Vector3(z.x, z.y, draw_order.z));
}

Basis25D::operator Transform2D() const {
	return Transform2D(x, y, z);
}

// Constructors.

// This is both sqrt(3)/2 and also cos(tau/12) or cos(30 deg).
#define Math_SQRT3_OVER_2 0.86602540378443864676372317

Basis25D Basis25D::from_preset(const Preset p_preset, const real_t p_angle, const real_t p_angle_z) {
	// Note: All of these include a flip to match Godot's +Y-down 2D coordinate system.
	switch (p_preset) {
		case PRESET_CUSTOM:
		case PRESET_FROM_ANGLE: {
			const real_t sine = Math::sin(p_angle);
			const real_t cosine = Math::cos(p_angle);
			return Basis25D(Vector2(0, 1), Vector2(0, -cosine), Vector2(0, sine), Vector3(CMP_EPSILON, sine, cosine));
		}
		case PRESET_ISOMETRIC:
			return Basis25D(Vector2(Math_SQRT3_OVER_2, 0.5), Vector2(0, -1), Vector2(-Math_SQRT3_OVER_2, 0.5));
		case PRESET_DIMETRIC: {
			const real_t sine = Math::sin(p_angle);
			const real_t cosine = Math::cos(p_angle);
			return Basis25D(Vector2(cosine, sine), Vector2(0, -1), Vector2(-cosine, sine));
		}
		case PRESET_TRIMETRIC: {
			const real_t sine = Math::sin(p_angle);
			const real_t cosine = Math::cos(p_angle);
			const real_t sine_z = Math::sin(p_angle_z);
			const real_t cosine_z = Math::cos(p_angle_z);
			return Basis25D(Vector2(cosine, sine), Vector2(0, -1), Vector2(-cosine_z, sine_z));
		}
		case PRESET_OBLIQUE_X:
			return Basis25D(Vector2(Math_SQRT12, Math_SQRT12), Vector2(0, -1), Vector2(-1, 0), Vector3(1, 0, 0));
		case PRESET_OBLIQUE_Y:
			return Basis25D(Vector2(1, 0), Vector2(-Math_SQRT12, -Math_SQRT12), Vector2(0, 1), Vector3(0, 1, 0));
		case PRESET_OBLIQUE_Z:
			return Basis25D(Vector2(1, 0), Vector2(0, -1), Vector2(-Math_SQRT12, Math_SQRT12), Vector3(0, 0, 1));
		case PRESET_FROM_POS_X:
			return Basis25D(Vector2(0, 0), Vector2(0, -1), Vector2(1, 0), Vector3(1, 0, 0));
		case PRESET_FROM_NEG_X:
			return Basis25D(Vector2(0, 0), Vector2(0, -1), Vector2(-1, 0), Vector3(-1, 0, 0));
		case PRESET_FROM_POS_Y:
			return Basis25D(Vector2(1, 0), Vector2(0, 0), Vector2(0, 1), Vector3(0, 1, 0));
		case PRESET_FROM_NEG_Y:
			return Basis25D(Vector2(1, 0), Vector2(0, 0), Vector2(0, -1), Vector3(0, -1, 0));
		case PRESET_FROM_POS_Z:
			return Basis25D(Vector2(1, 0), Vector2(0, -1), Vector2(0, 0), Vector3(0, 0, 1));
		case PRESET_FROM_NEG_Z:
			return Basis25D(Vector2(-1, 0), Vector2(0, -1), Vector2(0, 0), Vector3(0, 0, -1));
	}
	return Basis25D();
}

Basis25D::Basis25D() {
	// Default to 45 degree including a flip to match Godot's +Y-down 2D coordinate system.
	x = Vector2(1, 0);
	y = Vector2(0, -Math_SQRT12);
	z = Vector2(0, Math_SQRT12);
}

Basis25D::Basis25D(const Vector2 &p_x, const Vector2 &p_y, const Vector2 &p_z, const Vector3 &p_draw_order) {
	x = p_x;
	y = p_y;
	z = p_z;
	draw_order = p_draw_order;
}

Basis25D::Basis25D(const Basis25D &p_b) {
	x = p_b.x;
	y = p_b.y;
	z = p_b.z;
}

Basis25D::Basis25D(const Basis &p_b) {
	x = Vector2(p_b.get_column(0).x, p_b.get_column(0).y);
	y = Vector2(p_b.get_column(1).x, p_b.get_column(1).y);
	z = Vector2(p_b.get_column(2).x, p_b.get_column(2).y);
	draw_order = p_b.rows[2];
}

Basis25D::Basis25D(const Transform2D &p_t) {
	x = p_t[0];
	y = p_t[1];
	z = p_t[2];
}
