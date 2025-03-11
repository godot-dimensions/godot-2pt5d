#pragma once

#include "../godot_2pt5d_defines.h"

#include "../math/world_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/node2d.hpp>

// Godot does not let us extend CanvasItem in GDExtension :(
// https://github.com/godotengine/godot/pull/67510
class Node25D : public Node2D {
	GDCLASS(Node25D, Node2D);
#elif GODOT_MODULE
#include "scene/main/canvas_item.h"

class Node25D : public CanvasItem {
	GDCLASS(Node25D, CanvasItem);
#else
#error "Must build as Godot GDExtension or Godot module."
#endif
public:
	enum RotationEditMode {
		ROTATION_EDIT_MODE_NONE,
		ROTATION_EDIT_MODE_2D_ANGLE,
		ROTATION_EDIT_MODE_3D_EULER,
		ROTATION_EDIT_MODE_3D_QUATERNION,
		ROTATION_EDIT_MODE_3D_BASIS,
	};

private:
	Transform3D _transform;
	Ref<World25D> _world;
	RotationEditMode _rotation_edit_mode = ROTATION_EDIT_MODE_NONE;

	void _find_or_make_world();
	void _give_main_world_to_viewport();
	void _update_transform_2d();

protected:
	static void _bind_methods();
	void _notification(int p_what);
	void _get_property_list(List<PropertyInfo> *p_list) const;

public:
#ifdef TOOLS_ENABLED
	virtual Dictionary _edit_get_state() const MODULE_OVERRIDE;
	virtual void _edit_set_state(const Dictionary &p_state) MODULE_OVERRIDE;

	virtual void _edit_set_position(const Point2 &p_position) MODULE_OVERRIDE;
	virtual Point2 _edit_get_position() const MODULE_OVERRIDE;

	virtual void _edit_set_scale(const Size2 &p_scale) MODULE_OVERRIDE;
	virtual Size2 _edit_get_scale() const MODULE_OVERRIDE;

	virtual void _edit_set_rotation(real_t p_rotation) MODULE_OVERRIDE;
	virtual real_t _edit_get_rotation() const MODULE_OVERRIDE;
	virtual bool _edit_use_rotation() const MODULE_OVERRIDE;

	virtual void _edit_set_rect(const Rect2 &p_edit_rect) MODULE_OVERRIDE;
#endif

	Transform2D get_transform() const MODULE_OVERRIDE;

	RotationEditMode get_rotation_edit_mode() const { return _rotation_edit_mode; }
	void set_rotation_edit_mode(const RotationEditMode p_mode);

	// 3D local transform.
	Transform3D get_transform_3d() const { return _transform; }
	void set_transform_3d(const Transform3D &p_transform);
	Vector3 get_position_3d() const { return _transform.origin; }
	void set_position_3d(const Vector3 &p_position);
	Basis get_basis_3d() const { return _transform.basis; }
	void set_basis_3d(const Basis &p_basis);
	Vector3 get_euler_3d() const { return _transform.basis.get_euler(); }
	void set_euler_3d(const Vector3 &p_euler);
	Quaternion get_quaternion_3d() const { return _transform.basis.get_rotation_quaternion(); }
	void set_quaternion_3d(const Quaternion &p_quaternion);

	// 3D global transform.
	Transform3D get_global_transform_3d() const;
	void set_global_transform_3d(const Transform3D &p_transform);
	Vector3 get_global_position_3d() const;
	void set_global_position_3d(const Vector3 &p_position);
	Basis get_global_basis_3d() const;
	void set_global_basis_3d(const Basis &p_basis);

	// 2.5D local transform.
	Transform2D get_local_transform_2d() const;
	void set_local_transform_2d(const Transform2D &p_transform);
	Vector2 get_local_position_2d() const;
	void set_local_position_2d(const Vector2 &p_position);
	real_t get_local_rotation_2d() const;
	void local_rotate_2d(const real_t p_rotation);
	void set_local_rotation_2d(const real_t p_rotation);

	// 2.5D global transform.
	Transform2D get_global_transform_2d() const;
	void set_global_transform_2d(const Transform2D &p_transform);
	Vector2 get_global_position_2d() const;
	void global_translate_2d(const Vector2 &p_offset);
	void set_global_position_2d(const Vector2 &p_position);
	real_t get_global_rotation_2d() const;
	void global_rotate_2d(const real_t p_rotation);
	void set_global_rotation_2d(const real_t p_rotation);

	Ref<World25D> get_world_25d() const { return _world; }
	void set_world_25d(const Ref<World25D> &p_world) { _world = p_world; }
	void set_world_25d_recursive(const Ref<World25D> &p_world);
	Node25D();
};

VARIANT_ENUM_CAST(Node25D::RotationEditMode);
