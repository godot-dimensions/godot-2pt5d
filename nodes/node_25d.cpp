#include "node_25d.h"

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
}
