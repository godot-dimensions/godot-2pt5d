#include "world_25d.h"

void World25D::set_basis_preset(const Basis25D::Preset p_basis_preset) {
	_basis_preset = p_basis_preset;
	if (_basis_preset != Basis25D::PRESET_CUSTOM) {
		_basis = Basis25D::from_preset(_basis_preset, _basis_angle, _basis_angle_z);
	}
	notify_property_list_changed();
}

void World25D::set_basis_angle(const real_t p_basis_angle) {
	_basis_angle = p_basis_angle;
	if (_basis_preset != Basis25D::PRESET_CUSTOM) {
		_basis = Basis25D::from_preset(_basis_preset, _basis_angle, _basis_angle_z);
	}
}

void World25D::set_basis_angle_z(const real_t p_basis_angle_z) {
	_basis_angle_z = p_basis_angle_z;
	if (_basis_preset != Basis25D::PRESET_CUSTOM) {
		_basis = Basis25D::from_preset(_basis_preset, _basis_angle, _basis_angle_z);
	}
}

Ref<World25D> World25D::from_custom(const Basis &p_basis) {
	Ref<World25D> world;
	world.instantiate();
	world->_basis = Basis25D(p_basis);
	world->_basis_preset = Basis25D::PRESET_CUSTOM;
	return world;
}

Ref<World25D> World25D::from_preset(const Basis25D::Preset p_preset, const real_t p_angle, const real_t p_angle_z) {
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
				case Basis25D::PRESET_FROM_ANGLE:
				case Basis25D::PRESET_DIMETRIC:
				case Basis25D::PRESET_TRIMETRIC: {
					prop.usage = PROPERTY_USAGE_DEFAULT;
				} break;
				default: {
					prop.usage = PROPERTY_USAGE_NONE;
				} break;
			}
		} else if (prop.name == StringName("basis_angle_z")) {
			prop.usage = (_basis_preset == Basis25D::PRESET_TRIMETRIC) ? PROPERTY_USAGE_DEFAULT : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis")) {
			prop.usage = (_basis_preset == Basis25D::PRESET_CUSTOM) ? PROPERTY_USAGE_STORAGE : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis_x")) {
			prop.usage = (_basis_preset == Basis25D::PRESET_CUSTOM) ? PROPERTY_USAGE_EDITOR : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis_y")) {
			prop.usage = (_basis_preset == Basis25D::PRESET_CUSTOM) ? PROPERTY_USAGE_EDITOR : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis_z")) {
			prop.usage = (_basis_preset == Basis25D::PRESET_CUSTOM) ? PROPERTY_USAGE_EDITOR : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis_draw_order")) {
			prop.usage = (_basis_preset == Basis25D::PRESET_CUSTOM) ? PROPERTY_USAGE_EDITOR : PROPERTY_USAGE_NONE;
		}
	}
	Resource::_get_property_list(p_list);
}

void World25D::_bind_methods() {
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

	ClassDB::bind_method(D_METHOD("get_is_main_world"), &World25D::get_is_main_world);
	ClassDB::bind_method(D_METHOD("set_is_main_world", "is_main_world"), &World25D::set_is_main_world);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_main_world"), "set_is_main_world", "get_is_main_world");

	ClassDB::bind_static_method("World25D", D_METHOD("from_custom", "basis"), &World25D::from_custom);
	ClassDB::bind_static_method("World25D", D_METHOD("from_preset", "preset", "angle", "angle_z"), &World25D::from_preset);
}
