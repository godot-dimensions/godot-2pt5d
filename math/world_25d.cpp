#include "world_25d.h"

// Basis properties.

void World25D::set_basis_preset(const Basis25DPreset p_basis_preset) {
	_basis_preset = p_basis_preset;
	if (_basis_preset != PRESET_CUSTOM) {
		set_basis(Basis25D::from_preset((Basis25D::Preset)_basis_preset, _basis_angle, _basis_angle_z));
	}
	notify_property_list_changed();
}

void World25D::set_basis_angle(const real_t p_basis_angle) {
	_basis_angle = p_basis_angle;
	if (_basis_preset != PRESET_CUSTOM) {
		set_basis(Basis25D::from_preset((Basis25D::Preset)_basis_preset, _basis_angle, _basis_angle_z));
	}
}

void World25D::set_basis_angle_z(const real_t p_basis_angle_z) {
	_basis_angle_z = p_basis_angle_z;
	if (_basis_preset != PRESET_CUSTOM) {
		set_basis(Basis25D::from_preset((Basis25D::Preset)_basis_preset, _basis_angle, _basis_angle_z));
	}
}

void World25D::set_basis(const Basis25D &p_basis) {
	_basis = p_basis;
	emit_signal(StringName("basis_changed"));
}

void World25D::set_basis_bind(const Basis &p_basis) {
	set_basis(Basis25D(p_basis));
}

void World25D::set_basis_x(const Vector2 &p_basis_x) {
	_basis[0] = p_basis_x;
	emit_signal(StringName("basis_changed"));
}

void World25D::set_basis_y(const Vector2 &p_basis_y) {
	_basis[1] = p_basis_y;
	emit_signal(StringName("basis_changed"));
}

void World25D::set_basis_z(const Vector2 &p_basis_z) {
	_basis[2] = p_basis_z;
	emit_signal(StringName("basis_changed"));
}

void World25D::set_basis_draw_order(const Vector3 &p_basis_draw_order) {
	_basis.draw_order = p_basis_draw_order;
	emit_signal(StringName("basis_changed"));
}

// Math functions.

real_t World25D::calculate_2d_rotation(const Vector3 &p_y_axis_up) const {
	return _basis.calculate_2d_rotation(p_y_axis_up);
}

real_t World25D::calculate_draw_order(const Vector3 &p_vector3) const {
	return _basis.calculate_draw_order(p_vector3);
}

Vector2 World25D::xform_3d_to_2d(const Vector3 &p_vector3) const {
	return _basis.xform_3d_to_2d(p_vector3) * _pixels_per_meter;
}

Vector3 World25D::xform_inv_2d_to_3d(const Vector2 &p_vector2) const {
	return _basis.xform_inv_2d_to_3d(p_vector2 / _pixels_per_meter);
}

void World25D::set_pixels_per_meter(const real_t p_pixels_per_meter) {
	_pixels_per_meter = p_pixels_per_meter;
	emit_signal(StringName("basis_changed"));
}

// Constructors.

Ref<World25D> World25D::from_custom(const Basis &p_basis) {
	Ref<World25D> world;
	world.instantiate();
	world->_basis = Basis25D(p_basis);
	world->_basis_preset = World25D::PRESET_CUSTOM;
	return world;
}

Ref<World25D> World25D::from_preset(const Basis25DPreset p_preset, const real_t p_angle, const real_t p_angle_z) {
	Ref<World25D> world;
	world.instantiate();
	world->_basis_angle = p_angle;
	world->_basis_angle_z = p_angle_z;
	world->set_basis_preset(p_preset);
	return world;
}

void World25D::_get_property_list(List<PropertyInfo> *p_list) const {
	for (List<PropertyInfo>::Element *E = p_list->front(); E; E = E->next()) {
		PropertyInfo &prop = E->get();
		if (prop.name == StringName("basis_angle")) {
			switch (_basis_preset) {
				case PRESET_FROM_ANGLE:
				case PRESET_DIMETRIC:
				case PRESET_TRIMETRIC: {
					prop.usage = PROPERTY_USAGE_DEFAULT;
				} break;
				default: {
					prop.usage = PROPERTY_USAGE_NONE;
				} break;
			}
		} else if (prop.name == StringName("basis_angle_z")) {
			prop.usage = (_basis_preset == PRESET_TRIMETRIC) ? PROPERTY_USAGE_DEFAULT : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis")) {
			prop.usage = (_basis_preset == PRESET_CUSTOM) ? PROPERTY_USAGE_STORAGE : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis_x")) {
			prop.usage = (_basis_preset == PRESET_CUSTOM) ? PROPERTY_USAGE_EDITOR : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis_y")) {
			prop.usage = (_basis_preset == PRESET_CUSTOM) ? PROPERTY_USAGE_EDITOR : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis_z")) {
			prop.usage = (_basis_preset == PRESET_CUSTOM) ? PROPERTY_USAGE_EDITOR : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis_draw_order")) {
			prop.usage = (_basis_preset == PRESET_CUSTOM) ? PROPERTY_USAGE_EDITOR : PROPERTY_USAGE_NONE;
		}
	}
	Resource::_get_property_list(p_list);
}

void World25D::_bind_methods() {
	// Basis properties.
	ClassDB::bind_method(D_METHOD("get_basis_preset"), &World25D::get_basis_preset);
	ClassDB::bind_method(D_METHOD("set_basis_preset", "basis_preset"), &World25D::set_basis_preset);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "basis_preset", PROPERTY_HINT_ENUM, "Custom,From Angle,Isometric,Dimetric,Trimetric,Oblique X,Oblique Y,Oblique Z,From Pos X,From Neg X,From Pos Y,From Neg Y,From Pos Z,From Neg Z"), "set_basis_preset", "get_basis_preset");

	ClassDB::bind_method(D_METHOD("get_basis_angle"), &World25D::get_basis_angle);
	ClassDB::bind_method(D_METHOD("set_basis_angle", "basis_angle"), &World25D::set_basis_angle);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "basis_angle", PROPERTY_HINT_RANGE, "0.1,89.9,0.1,or_less,or_greater,radians_as_degrees"), "set_basis_angle", "get_basis_angle");

	ClassDB::bind_method(D_METHOD("get_basis_angle_z"), &World25D::get_basis_angle_z);
	ClassDB::bind_method(D_METHOD("set_basis_angle_z", "basis_angle_z"), &World25D::set_basis_angle_z);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "basis_angle_z", PROPERTY_HINT_RANGE, "0.1,89.9,0.1,or_less,or_greater,radians_as_degrees"), "set_basis_angle_z", "get_basis_angle_z");

	ClassDB::bind_method(D_METHOD("get_basis"), &World25D::get_basis_bind);
	ClassDB::bind_method(D_METHOD("set_basis", "basis"), &World25D::set_basis_bind);
	ADD_PROPERTY(PropertyInfo(Variant::BASIS, "basis"), "set_basis", "get_basis");

	ClassDB::bind_method(D_METHOD("get_basis_x"), &World25D::get_basis_x);
	ClassDB::bind_method(D_METHOD("set_basis_x", "basis_x"), &World25D::set_basis_x);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "basis_x"), "set_basis_x", "get_basis_x");

	ClassDB::bind_method(D_METHOD("get_basis_y"), &World25D::get_basis_y);
	ClassDB::bind_method(D_METHOD("set_basis_y", "basis_y"), &World25D::set_basis_y);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "basis_y"), "set_basis_y", "get_basis_y");

	ClassDB::bind_method(D_METHOD("get_basis_z"), &World25D::get_basis_z);
	ClassDB::bind_method(D_METHOD("set_basis_z", "basis_z"), &World25D::set_basis_z);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "basis_z"), "set_basis_z", "get_basis_z");

	ClassDB::bind_method(D_METHOD("get_basis_draw_order"), &World25D::get_basis_draw_order);
	ClassDB::bind_method(D_METHOD("set_basis_draw_order", "basis_draw_order"), &World25D::set_basis_draw_order);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "basis_draw_order"), "set_basis_draw_order", "get_basis_draw_order");

	// Not basis properties.
	ClassDB::bind_method(D_METHOD("get_pixels_per_meter"), &World25D::get_pixels_per_meter);
	ClassDB::bind_method(D_METHOD("set_pixels_per_meter", "pixels_per_meter"), &World25D::set_pixels_per_meter);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "pixels_per_meter", PROPERTY_HINT_RANGE, "1,1024,1,or_less,or_greater"), "set_pixels_per_meter", "get_pixels_per_meter");

	ClassDB::bind_method(D_METHOD("get_is_main_world"), &World25D::get_is_main_world);
	ClassDB::bind_method(D_METHOD("set_is_main_world", "is_main_world"), &World25D::set_is_main_world);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_main_world"), "set_is_main_world", "get_is_main_world");

	// Math functions.
	ClassDB::bind_method(D_METHOD("calculate_2d_rotation", "y_axis_up"), &World25D::calculate_2d_rotation);
	ClassDB::bind_method(D_METHOD("calculate_draw_order", "vector3"), &World25D::calculate_draw_order);
	ClassDB::bind_method(D_METHOD("xform_3d_to_2d", "vector3"), &World25D::xform_3d_to_2d);
	ClassDB::bind_method(D_METHOD("xform_inv_2d_to_3d", "vector2"), &World25D::xform_inv_2d_to_3d);

	// Constructors.
	ClassDB::bind_static_method("World25D", D_METHOD("from_custom", "basis"), &World25D::from_custom);
	ClassDB::bind_static_method("World25D", D_METHOD("from_preset", "preset", "angle", "angle_z"), &World25D::from_preset);

	// Signals.
	ADD_SIGNAL(MethodInfo("basis_changed"));

	// Enums.
	BIND_ENUM_CONSTANT(PRESET_CUSTOM);
	BIND_ENUM_CONSTANT(PRESET_FROM_ANGLE);
	BIND_ENUM_CONSTANT(PRESET_ISOMETRIC);
	BIND_ENUM_CONSTANT(PRESET_DIMETRIC);
	BIND_ENUM_CONSTANT(PRESET_TRIMETRIC);
	BIND_ENUM_CONSTANT(PRESET_OBLIQUE_X);
	BIND_ENUM_CONSTANT(PRESET_OBLIQUE_Y);
	BIND_ENUM_CONSTANT(PRESET_OBLIQUE_Z);
	BIND_ENUM_CONSTANT(PRESET_FROM_POS_X);
	BIND_ENUM_CONSTANT(PRESET_FROM_NEG_X);
	BIND_ENUM_CONSTANT(PRESET_FROM_POS_Y);
	BIND_ENUM_CONSTANT(PRESET_FROM_NEG_Y);
	BIND_ENUM_CONSTANT(PRESET_FROM_POS_Z);
	BIND_ENUM_CONSTANT(PRESET_FROM_NEG_Z);
}
