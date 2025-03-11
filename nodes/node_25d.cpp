#include "node_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/classes/viewport.hpp>
#elif GODOT_MODULE
#include "scene/3d/node_3d.h"
#include "scene/main/viewport.h"
#endif

void Node25D::_find_or_make_world() {
	Node *parent = get_parent();
	while (parent) {
		Node25D *node_25d_parent = Object::cast_to<Node25D>(parent);
		if (node_25d_parent) {
			// If the parent is a Node25D, use its world, even if it's null.
			// The parent should automatically get one when added to the tree,
			// but if it's not, then this may be intentional by the user.
			_world = node_25d_parent->get_world_25d();
			break;
		}
		Viewport *viewport = Object::cast_to<Viewport>(parent);
		if (viewport) {
			// If we find a Viewport, use its world, or provide one if it doesn't have one.
			if (viewport->has_meta(StringName("world_25d"))) {
				_world = viewport->get_meta(StringName("world_25d"));
			} else {
				if (_world.is_null()) {
					_world.instantiate();
				}
				viewport->set_meta(StringName("world_25d"), _world);
			}
			_world->connect("basis_changed", callable_mp(this, &Node25D::_update_transform_2d));
			break;
		}
		parent = parent->get_parent();
	}
}

void Node25D::_give_main_world_to_viewport() {
	Viewport *viewport = get_viewport();
	if (viewport) {
		if (viewport->has_meta(StringName("world_25d"))) {
			// By this point, we know the Viewport has a World25D, but is it marked as main?
			// If ours is marked as main, but the Viewport's is not, then ours takes precedence.
			Ref<World25D> viewport_world = viewport->get_meta(StringName("world_25d"));
			if (viewport_world->get_is_main_world()) {
				return; // Viewport already has a World25D marked as main.
			}
		}
		// Give the main world to the viewport if it needs one.
		viewport->set_meta(StringName("world_25d"), _world);
	}
}

void Node25D::_update_transform_2d() {
	if (_world.is_null()) {
		return;
	}
	const Transform2D local_canvas_transform = get_local_transform_2d();
#if GDEXTENSION
	set_transform(local_canvas_transform);
#elif GODOT_MODULE
	RenderingServer::get_singleton()->canvas_item_set_transform(get_canvas_item(), local_canvas_transform);
	_notify_transform();
#endif
}

void Node25D::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_ENTER_TREE: {
			if (_world.is_valid() && _world->get_is_main_world()) {
				// This is expected to be the case for the main scene root node.
				_give_main_world_to_viewport();
			} else {
				// If our World25D is not main, try to find another one first.
				// This is expected to happen for prefabs and other non-main scenes.
				// Such sub-scenes still need to do math and stuff when edited.
				_find_or_make_world();
			}
			_update_transform_2d();
		} break;
		case NOTIFICATION_EXIT_TREE: {
			if (_world.is_valid()) {
				const Callable update_transform = callable_mp(this, &Node25D::_update_transform_2d);
				if (_world->is_connected("basis_changed", update_transform)) {
					_world->disconnect("basis_changed", update_transform);
				}
			}
		} break;
		case NOTIFICATION_PROCESS: {
			_update_transform_2d();
		} break;
	}
}

void Node25D::_get_property_list(List<PropertyInfo> *p_list) const {
	for (List<PropertyInfo>::Element *E = p_list->front(); E; E = E->next()) {
		PropertyInfo &prop = E->get();
		if (prop.name == StringName("world")) {
			const Node25D *parent_25d = Object::cast_to<Node25D>(get_parent());
			if (parent_25d && parent_25d->get_world_25d() == _world) {
				// If the parent has the same world, don't save this node's world into the tscn file.
				prop.usage = PROPERTY_USAGE_EDITOR;
			}
		} else if (prop.name == StringName("local_rotation_2d")) {
			prop.usage = _rotation_edit_mode == ROTATION_EDIT_MODE_2D_ANGLE ? PROPERTY_USAGE_EDITOR : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("basis_3d")) {
			if (_rotation_edit_mode == ROTATION_EDIT_MODE_3D_BASIS) {
				prop.usage = PROPERTY_USAGE_DEFAULT;
			} else if (_rotation_edit_mode == ROTATION_EDIT_MODE_2D_ANGLE) {
				prop.usage = PROPERTY_USAGE_STORAGE;
			} else {
				prop.usage = PROPERTY_USAGE_NONE;
			}
		} else if (prop.name == StringName("euler_3d")) {
			prop.usage = _rotation_edit_mode == ROTATION_EDIT_MODE_3D_EULER ? PROPERTY_USAGE_DEFAULT : PROPERTY_USAGE_NONE;
		} else if (prop.name == StringName("quaternion_3d")) {
			prop.usage = _rotation_edit_mode == ROTATION_EDIT_MODE_3D_QUATERNION ? PROPERTY_USAGE_DEFAULT : PROPERTY_USAGE_NONE;
		}
	}
	CanvasItem::_get_property_list(p_list);
}

#ifdef TOOLS_ENABLED
Dictionary Node25D::_edit_get_state() const {
	Dictionary state;
	state["position"] = get_local_position_2d();
	state["position_3d"] = get_position_3d();
	state["basis_3d"] = get_basis_3d();
	state["rotation"] = get_local_rotation_2d();
	return state;
}

void Node25D::_edit_set_state(const Dictionary &p_state) {
	if (p_state.has("position_3d")) {
		set_position_3d(Vector3(p_state["position_3d"]));
	} else if (p_state.has("position")) {
		set_local_position_2d(Vector2(p_state["position"]));
	}
	if (p_state.has("basis_3d")) {
		set_basis_3d(Basis(p_state["basis_3d"]));
	} else if (p_state.has("rotation")) {
		set_local_rotation_2d(real_t(p_state["rotation"]));
	}
}

void Node25D::_edit_set_position(const Point2 &p_position) {
	set_local_position_2d(p_position);
}

Point2 Node25D::_edit_get_position() const {
	return get_local_position_2d();
}

void Node25D::_edit_set_scale(const Size2 &p_scale) {}

Size2 Node25D::_edit_get_scale() const {
	return Size2(1, 1);
}

void Node25D::_edit_set_rotation(real_t p_rotation) {
	set_local_rotation_2d(p_rotation);
}

real_t Node25D::_edit_get_rotation() const {
	return get_local_rotation_2d();
}

bool Node25D::_edit_use_rotation() const {
	return _rotation_edit_mode != ROTATION_EDIT_MODE_NONE;
}

void Node25D::_edit_set_rect(const Rect2 &p_edit_rect) {}
#endif // TOOLS_ENABLED

Transform2D Node25D::get_transform() const {
	ERR_FAIL_COND_V_MSG(_world.is_null(), Transform2D(), "Node25D must have a World25D to calculate 2D transform.");
	return get_local_transform_2d();
}

void Node25D::set_rotation_edit_mode(const RotationEditMode p_mode) {
	_rotation_edit_mode = p_mode;
	notify_property_list_changed();
}

// 3D local transform.

void Node25D::set_transform_3d(const Transform3D &p_transform) {
	_transform = p_transform;
	_update_transform_2d();
}

void Node25D::set_position_3d(const Vector3 &p_position) {
	_transform.origin = p_position;
	_update_transform_2d();
}

void Node25D::set_basis_3d(const Basis &p_basis) {
	_transform.basis = p_basis;
	_update_transform_2d();
}

void Node25D::set_euler_3d(const Vector3 &p_euler) {
	set_basis_3d(Basis::from_euler(p_euler));
}

void Node25D::set_quaternion_3d(const Quaternion &p_quaternion) {
	set_basis_3d(Basis(p_quaternion));
}

// 3D global transform.

Transform3D Node25D::get_global_transform_3d() const {
	const Node *parent = get_parent();
	if (parent) {
		const Node25D *node_25d_parent = Object::cast_to<Node25D>(parent);
		if (node_25d_parent) {
			return node_25d_parent->get_global_transform_3d() * _transform;
		} else {
			const Node3D *node_3d_parent = Object::cast_to<Node3D>(parent);
			if (node_3d_parent) {
				return node_3d_parent->get_global_transform() * _transform;
			}
		}
	}
	return _transform;
}

void Node25D::set_global_transform_3d(const Transform3D &p_transform) {
	const Node *parent = get_parent();
	if (parent) {
		const Node25D *node_25d_parent = Object::cast_to<Node25D>(parent);
		if (node_25d_parent) {
			const Transform3D parent_transform = node_25d_parent->get_global_transform_3d();
			set_transform_3d(parent_transform.affine_inverse() * p_transform);
			return;
		} else {
			const Node3D *node_3d_parent = Object::cast_to<Node3D>(parent);
			if (node_3d_parent) {
				const Transform3D parent_transform = node_3d_parent->get_global_transform();
				set_transform_3d(parent_transform.affine_inverse() * p_transform);
				return;
			}
		}
	}
	set_transform_3d(p_transform);
}

Vector3 Node25D::get_global_position_3d() const {
	return get_global_transform_3d().origin;
}

void Node25D::set_global_position_3d(const Vector3 &p_position) {
	Transform3D gt = get_global_transform_3d();
	gt.origin = p_position;
	set_global_transform_3d(gt);
}

Basis Node25D::get_global_basis_3d() const {
	return get_global_transform_3d().basis;
}

void Node25D::set_global_basis_3d(const Basis &p_basis) {
	Transform3D t = get_global_transform_3d();
	t.basis = p_basis;
	set_global_transform_3d(t);
}

// 2.5D local transform.

Transform2D Node25D::get_local_transform_2d() const {
	ERR_FAIL_COND_V_MSG(_world.is_null(), Transform2D(), "Node25D must have a World25D to calculate 2D transform.");
	const Transform2D global_canvas_transform = get_global_transform_2d();
	const CanvasItem *parent = Object::cast_to<CanvasItem>(get_parent());
	if (parent) {
		return parent->get_global_transform().affine_inverse() * global_canvas_transform;
	}
	return global_canvas_transform;
}

void Node25D::set_local_transform_2d(const Transform2D &p_transform) {
	ERR_FAIL_COND_MSG(_world.is_null(), "Node25D must have a World25D to calculate 2D transform.");
	const CanvasItem *parent = Object::cast_to<CanvasItem>(get_parent());
	if (parent) {
		set_global_transform_2d(parent->get_global_transform() * p_transform);
	}
	set_global_transform_2d(p_transform);
}

Vector2 Node25D::get_local_position_2d() const {
	ERR_FAIL_COND_V_MSG(_world.is_null(), Vector2(), "Node25D must have a World25D to calculate 2D transform.");
	return get_local_transform_2d().get_origin();
}

void Node25D::set_local_position_2d(const Vector2 &p_position) {
	ERR_FAIL_COND_MSG(_world.is_null(), "Node25D must have a World25D to calculate 2D transform.");
	const CanvasItem *parent = Object::cast_to<CanvasItem>(get_parent());
	if (parent) {
		set_global_position_2d(parent->get_global_transform().xform(p_position));
	} else {
		set_global_position_2d(p_position);
	}
}

real_t Node25D::get_local_rotation_2d() const {
	ERR_FAIL_COND_V_MSG(_world.is_null(), 0.0, "Node25D must have a World25D to calculate 2D rotation.");
	return _world->calculate_2d_rotation(get_basis_3d().get_column(1));
}

void Node25D::local_rotate_2d(const real_t p_rotation) {
	ERR_FAIL_COND_MSG(_world.is_null(), "Node25D must have a World25D to calculate 2D rotation.");
	const Basis axis_angle = Basis(_world->get_basis_draw_order(), -p_rotation);
	set_basis_3d(axis_angle * get_basis_3d());
}

void Node25D::set_local_rotation_2d(const real_t p_rotation) {
	ERR_FAIL_COND_MSG(_world.is_null(), "Node25D must have a World25D to calculate 2D rotation.");
	const real_t existing_rotation = get_local_rotation_2d();
	if (!Math::is_equal_approx(p_rotation, existing_rotation)) {
		local_rotate_2d(p_rotation - existing_rotation);
	}
}

// 2.5D global transform.

Transform2D Node25D::get_global_transform_2d() const {
	ERR_FAIL_COND_V_MSG(_world.is_null(), Transform2D(), "Node25D must have a World25D to calculate 2D transform.");
	const Transform3D global_transform = get_global_transform_3d();
	return _world->xform_3d_transform_to_2d(global_transform);
}

void Node25D::set_global_transform_2d(const Transform2D &p_transform) {
	ERR_FAIL_COND_MSG(_world.is_null(), "Node25D must have a World25D to calculate 2D transform.");
	set_global_position_2d(p_transform.get_origin());
	set_global_rotation_2d(p_transform.get_rotation());
}

Vector2 Node25D::get_global_position_2d() const {
	ERR_FAIL_COND_V_MSG(_world.is_null(), Vector2(), "Node25D must have a World25D to calculate 2D position.");
	const Vector3 global_pos = get_global_position_3d();
	return _world->xform_3d_to_2d(global_pos);
}

void Node25D::global_translate_2d(const Vector2 &p_offset) {
	ERR_FAIL_COND_MSG(_world.is_null(), "Node25D must have a World25D to calculate 2D position.");
	const Vector3 movement = _world->xform_inv_2d_to_3d(p_offset);
	set_global_position_3d(get_global_position_3d() + movement);
}

void Node25D::set_global_position_2d(const Vector2 &p_position) {
	ERR_FAIL_COND_MSG(_world.is_null(), "Node25D must have a World25D to calculate 2D position.");
	const Vector2 current_pos = get_global_position_2d();
	if (!p_position.is_equal_approx(current_pos)) {
		const Vector3 movement = _world->xform_inv_2d_to_3d(p_position - current_pos);
		set_global_position_3d(get_global_position_3d() + movement);
	}
}

real_t Node25D::get_global_rotation_2d() const {
	ERR_FAIL_COND_V_MSG(_world.is_null(), 0.0, "Node25D must have a World25D to calculate 2D rotation.");
	return _world->calculate_2d_rotation(get_global_basis_3d().get_column(1));
}

void Node25D::global_rotate_2d(const real_t p_rotation) {
	ERR_FAIL_COND_MSG(_world.is_null(), "Node25D must have a World25D to calculate 2D rotation.");
	const Basis axis_angle = Basis(_world->get_basis_draw_order(), -p_rotation);
	set_global_basis_3d(axis_angle * get_global_basis_3d());
}

void Node25D::set_global_rotation_2d(const real_t p_rotation) {
	ERR_FAIL_COND_MSG(_world.is_null(), "Node25D must have a World25D to calculate 2D rotation.");
	const real_t existing_rotation = get_global_rotation_2d();
	if (!Math::is_equal_approx(p_rotation, existing_rotation)) {
		global_rotate_2d(p_rotation - existing_rotation);
	}
}

void Node25D::set_world_25d_recursive(const Ref<World25D> &p_world) {
	_world = p_world;
	for (int i = 0; i < get_child_count(); i++) {
		Node25D *node_25d = Object::cast_to<Node25D>(get_child(i));
		if (node_25d) {
			node_25d->set_world_25d_recursive(p_world);
		}
	}
}

void Node25D::_bind_methods() {
	// 3D local transform.
	ClassDB::bind_method(D_METHOD("get_transform_3d"), &Node25D::get_transform_3d);
	ClassDB::bind_method(D_METHOD("set_transform_3d", "transform"), &Node25D::set_transform_3d);
	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM3D, "transform_3d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_transform_3d", "get_transform_3d");

	ClassDB::bind_method(D_METHOD("get_position_3d"), &Node25D::get_position_3d);
	ClassDB::bind_method(D_METHOD("set_position_3d", "position"), &Node25D::set_position_3d);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "position_3d", PROPERTY_HINT_NONE, "suffix:m"), "set_position_3d", "get_position_3d");

	ClassDB::bind_method(D_METHOD("get_rotation_edit_mode"), &Node25D::get_rotation_edit_mode);
	ClassDB::bind_method(D_METHOD("set_rotation_edit_mode", "mode"), &Node25D::set_rotation_edit_mode);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "rotation_edit_mode", PROPERTY_HINT_ENUM, "None,2D Angle,3D Euler,3D Quaternion,3D Basis"), "set_rotation_edit_mode", "get_rotation_edit_mode");

	ClassDB::bind_method(D_METHOD("get_basis_3d"), &Node25D::get_basis_3d);
	ClassDB::bind_method(D_METHOD("set_basis_3d", "basis"), &Node25D::set_basis_3d);
	ADD_PROPERTY(PropertyInfo(Variant::BASIS, "basis_3d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_basis_3d", "get_basis_3d");

	ClassDB::bind_method(D_METHOD("get_euler_3d"), &Node25D::get_euler_3d);
	ClassDB::bind_method(D_METHOD("set_euler_3d", "euler"), &Node25D::set_euler_3d);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "euler_3d", PROPERTY_HINT_NONE, "radians_as_degrees", PROPERTY_USAGE_NONE), "set_euler_3d", "get_euler_3d");

	ClassDB::bind_method(D_METHOD("get_quaternion_3d"), &Node25D::get_quaternion_3d);
	ClassDB::bind_method(D_METHOD("set_quaternion_3d", "quaternion"), &Node25D::set_quaternion_3d);
	ADD_PROPERTY(PropertyInfo(Variant::QUATERNION, "quaternion_3d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_quaternion_3d", "get_quaternion_3d");

	// 3D global transform.
	ClassDB::bind_method(D_METHOD("get_global_transform_3d"), &Node25D::get_global_transform_3d);
	ClassDB::bind_method(D_METHOD("set_global_transform_3d", "transform"), &Node25D::set_global_transform_3d);
	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM3D, "global_transform_3d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_global_transform_3d", "get_global_transform_3d");

	ClassDB::bind_method(D_METHOD("get_global_basis_3d"), &Node25D::get_global_basis_3d);
	ClassDB::bind_method(D_METHOD("set_global_basis_3d", "basis"), &Node25D::set_global_basis_3d);
	ADD_PROPERTY(PropertyInfo(Variant::BASIS, "global_basis_3d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_global_basis_3d", "get_global_basis_3d");

	ClassDB::bind_method(D_METHOD("get_global_position_3d"), &Node25D::get_global_position_3d);
	ClassDB::bind_method(D_METHOD("set_global_position_3d", "position"), &Node25D::set_global_position_3d);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "global_position_3d", PROPERTY_HINT_NONE, "suffix:m", PROPERTY_USAGE_NONE), "set_global_position_3d", "get_global_position_3d");

	// 2.5D local transform.
	ClassDB::bind_method(D_METHOD("get_local_transform_2d"), &Node25D::get_local_transform_2d);
	ClassDB::bind_method(D_METHOD("set_local_transform_2d", "transform"), &Node25D::set_local_transform_2d);
	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM2D, "local_transform_2d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_local_transform_2d", "get_local_transform_2d");

	ClassDB::bind_method(D_METHOD("get_local_position_2d"), &Node25D::get_local_position_2d);
	ClassDB::bind_method(D_METHOD("set_local_position_2d", "position"), &Node25D::set_local_position_2d);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "local_position_2d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_local_position_2d", "get_local_position_2d");

	ClassDB::bind_method(D_METHOD("get_local_rotation_2d"), &Node25D::get_local_rotation_2d);
	ClassDB::bind_method(D_METHOD("local_rotate_2d", "rotation"), &Node25D::local_rotate_2d);
	ClassDB::bind_method(D_METHOD("set_local_rotation_2d", "rotation"), &Node25D::set_local_rotation_2d);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "local_rotation_2d", PROPERTY_HINT_NONE, "radians_as_degrees", PROPERTY_USAGE_NONE), "set_local_rotation_2d", "get_local_rotation_2d");

	// 2.5D global transform.
	ClassDB::bind_method(D_METHOD("get_global_transform_2d"), &Node25D::get_global_transform_2d);
	ClassDB::bind_method(D_METHOD("set_global_transform_2d", "transform"), &Node25D::set_global_transform_2d);
	ADD_PROPERTY(PropertyInfo(Variant::TRANSFORM2D, "global_transform_2d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_global_transform_2d", "get_global_transform_2d");

	ClassDB::bind_method(D_METHOD("get_global_position_2d"), &Node25D::get_global_position_2d);
	ClassDB::bind_method(D_METHOD("global_translate_2d", "offset"), &Node25D::global_translate_2d);
	ClassDB::bind_method(D_METHOD("set_global_position_2d", "position"), &Node25D::set_global_position_2d);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "global_position_2d", PROPERTY_HINT_NONE, "", PROPERTY_USAGE_NONE), "set_global_position_2d", "get_global_position_2d");

	ClassDB::bind_method(D_METHOD("get_global_rotation_2d"), &Node25D::get_global_rotation_2d);
	ClassDB::bind_method(D_METHOD("global_rotate_2d", "rotation"), &Node25D::global_rotate_2d);
	ClassDB::bind_method(D_METHOD("set_global_rotation_2d", "rotation"), &Node25D::set_global_rotation_2d);
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "global_rotation_2d", PROPERTY_HINT_NONE, "radians_as_degrees", PROPERTY_USAGE_NONE), "set_global_rotation_2d", "get_global_rotation_2d");

	ClassDB::bind_method(D_METHOD("get_world_25d"), &Node25D::get_world_25d);
	ClassDB::bind_method(D_METHOD("set_world_25d", "world_25d"), &Node25D::set_world_25d);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "world_25d", PROPERTY_HINT_RESOURCE_TYPE, "World25D"), "set_world_25d", "get_world_25d");

	BIND_ENUM_CONSTANT(ROTATION_EDIT_MODE_NONE);
	BIND_ENUM_CONSTANT(ROTATION_EDIT_MODE_2D_ANGLE);
	BIND_ENUM_CONSTANT(ROTATION_EDIT_MODE_3D_EULER);
	BIND_ENUM_CONSTANT(ROTATION_EDIT_MODE_3D_QUATERNION);
	BIND_ENUM_CONSTANT(ROTATION_EDIT_MODE_3D_BASIS);
}

Node25D::Node25D() {
	set_process(true);
}
