#include "node_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/viewport.hpp>
#elif GODOT_MODULE
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
			_world = node_25d_parent->get_world();
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
		} break;
	}
}

void Node25D::_get_property_list(List<PropertyInfo> *p_list) const {
	for (List<PropertyInfo>::Element *E = p_list->front(); E; E = E->next()) {
		PropertyInfo &prop = E->get();
		if (prop.name == StringName("world")) {
			Node25D *parent_25d = Object::cast_to<Node25D>(get_parent());
			if (parent_25d && parent_25d->get_world() == _world) {
				// If the parent has the same world, don't save this node's world into the tscn file.
				prop.usage = PROPERTY_USAGE_EDITOR;
			}
		}
	}
	CanvasItem::_get_property_list(p_list);
}

#ifdef TOOLS_ENABLED
Dictionary Node25D::_edit_get_state() const {
	Dictionary state;
	return state;
}

void Node25D::_edit_set_state(const Dictionary &p_state) {}

void Node25D::_edit_set_position(const Point2 &p_position) {}

Point2 Node25D::_edit_get_position() const {
	return Point2();
}

void Node25D::_edit_set_scale(const Size2 &p_scale) {}

Size2 Node25D::_edit_get_scale() const {
	return Size2();
}

void Node25D::_edit_set_rotation(real_t p_rotation) {}

real_t Node25D::_edit_get_rotation() const {
	return 0.0;
}

bool Node25D::_edit_use_rotation() const {
	return false;
}

void Node25D::_edit_set_rect(const Rect2 &p_edit_rect) {}
#endif // TOOLS_ENABLED

Transform2D Node25D::get_transform() const {
	return Transform2D();
}

void Node25D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_world"), &Node25D::get_world);
	ClassDB::bind_method(D_METHOD("set_world", "world"), &Node25D::set_world);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "world", PROPERTY_HINT_RESOURCE_TYPE, "World25D"), "set_world", "get_world");
}
