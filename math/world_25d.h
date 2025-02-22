#pragma once

#include "../godot_2pt5d_defines.h"

#include "basis_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/resource.hpp>
#elif GODOT_MODULE
#include "core/io/resource.h"
#endif

class World25D : public Resource {
	GDCLASS(World25D, Resource);

private:
	Basis25D _basis;
	Basis25D::Preset _basis_preset = Basis25D::PRESET_FROM_ANGLE;
	real_t _basis_angle = Math_TAU / 8.0;
	real_t _basis_angle_z = Math_TAU / 16.0;
	bool _is_main_world = false;

protected:
	static void _bind_methods();
	void _get_property_list(List<PropertyInfo> *p_list) const;

public:
	Basis25D::Preset get_basis_preset() const { return _basis_preset; }
	void set_basis_preset(const Basis25D::Preset p_basis_preset);

	real_t get_basis_angle() const { return _basis_angle; }
	void set_basis_angle(const real_t p_basis_angle);

	real_t get_basis_angle_z() const { return _basis_angle; }
	void set_basis_angle_z(const real_t p_basis_angle);

	Basis25D get_basis() const { return _basis; }
	void set_basis(const Basis25D &p_basis) { _basis = p_basis; }

	Basis get_basis_bind() const { return _basis; }
	void set_basis_bind(const Basis &p_basis) { _basis = p_basis; }

	Vector2 get_basis_x() const { return _basis[0]; }
	void set_basis_x(const Vector2 &p_basis_x) { _basis[0] = p_basis_x; }

	Vector2 get_basis_y() const { return _basis[1]; }
	void set_basis_y(const Vector2 &p_basis_y) { _basis[1] = p_basis_y; }

	Vector2 get_basis_z() const { return _basis[2]; }
	void set_basis_z(const Vector2 &p_basis_z) { _basis[2] = p_basis_z; }

	Vector3 get_basis_draw_order() const { return _basis.draw_order; }
	void set_basis_draw_order(const Vector3 &p_basis_draw_order) { _basis.draw_order = p_basis_draw_order; }

	bool get_is_main_world() const { return _is_main_world; }
	void set_is_main_world(const bool p_is_main_world) { _is_main_world = p_is_main_world; }

	static Ref<World25D> from_custom(const Basis &p_basis);
	static Ref<World25D> from_preset(const Basis25D::Preset p_preset, const real_t p_angle, const real_t p_angle_z);
};

VARIANT_ENUM_CAST(Basis25D::Preset);
