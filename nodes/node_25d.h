#pragma once

#include "../godot_2pt5d_defines.h"

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

protected:
	static void _bind_methods();

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
};
