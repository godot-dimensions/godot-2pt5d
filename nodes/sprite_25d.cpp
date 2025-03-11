#include "sprite_25d.h"

void Sprite25D::set_texture(const Ref<Texture2D> &p_texture) {
	_sprite_2d->set_texture(p_texture);
}

Ref<Texture2D> Sprite25D::get_texture() const {
	return _sprite_2d->get_texture();
}

// Offset.

bool Sprite25D::get_is_centered() const {
	return _sprite_2d->is_centered();
}

void Sprite25D::set_is_centered(const bool p_centered) {
	_sprite_2d->set_centered(p_centered);
}

Vector2 Sprite25D::get_offset() const {
	return _sprite_2d->get_offset();
}

void Sprite25D::set_offset(const Vector2 &p_offset) {
	_sprite_2d->set_offset(p_offset);
}

bool Sprite25D::get_flip_horizontal() const {
	return _sprite_2d->is_flipped_h();
}

void Sprite25D::set_flip_horizontal(const bool p_flip) {
	_sprite_2d->set_flip_h(p_flip);
}

bool Sprite25D::get_flip_vertical() const {
	return _sprite_2d->is_flipped_v();
}

void Sprite25D::set_flip_vertical(const bool p_flip) {
	_sprite_2d->set_flip_v(p_flip);
}

// Animation

int Sprite25D::get_horizontal_frames() const {
	return _sprite_2d->get_hframes();
}

void Sprite25D::set_horizontal_frames(const int p_frames) {
	_sprite_2d->set_hframes(p_frames);
}

int Sprite25D::get_vertical_frames() const {
	return _sprite_2d->get_vframes();
}

void Sprite25D::set_vertical_frames(const int p_frames) {
	_sprite_2d->set_vframes(p_frames);
}

int Sprite25D::get_frame() const {
	return _sprite_2d->get_frame();
}

void Sprite25D::set_frame(const int p_frame) {
	_sprite_2d->set_frame(p_frame);
}

Vector2i Sprite25D::get_frame_coords() const {
	return _sprite_2d->get_frame_coords();
}

void Sprite25D::set_frame_coords(const Vector2i &p_coords) {
	_sprite_2d->set_frame_coords(p_coords);
}

Sprite25D::Sprite25D() {
	_sprite_2d = memnew(Sprite2D);
	add_child(_sprite_2d);
	set_process(true);
}

void Sprite25D::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_sprite_2d"), &Sprite25D::get_sprite_2d);

	ClassDB::bind_method(D_METHOD("get_texture"), &Sprite25D::get_texture);
	ClassDB::bind_method(D_METHOD("set_texture", "texture"), &Sprite25D::set_texture);
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture", "get_texture");

	// Offset.
	ADD_GROUP("Offset", "");
	ClassDB::bind_method(D_METHOD("get_is_centered"), &Sprite25D::get_is_centered);
	ClassDB::bind_method(D_METHOD("set_is_centered", "centered"), &Sprite25D::set_is_centered);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "is_centered"), "set_is_centered", "get_is_centered");

	ClassDB::bind_method(D_METHOD("get_offset"), &Sprite25D::get_offset);
	ClassDB::bind_method(D_METHOD("set_offset", "offset"), &Sprite25D::set_offset);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "offset"), "set_offset", "get_offset");

	ClassDB::bind_method(D_METHOD("get_flip_horizontal"), &Sprite25D::get_flip_horizontal);
	ClassDB::bind_method(D_METHOD("set_flip_horizontal", "flip"), &Sprite25D::set_flip_horizontal);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_horizontal"), "set_flip_horizontal", "get_flip_horizontal");

	ClassDB::bind_method(D_METHOD("get_flip_vertical"), &Sprite25D::get_flip_vertical);
	ClassDB::bind_method(D_METHOD("set_flip_vertical", "flip"), &Sprite25D::set_flip_vertical);
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "flip_vertical"), "set_flip_vertical", "get_flip_vertical");

	// Animation.
	ADD_GROUP("Animation", "");
	ClassDB::bind_method(D_METHOD("get_horizontal_frames"), &Sprite25D::get_horizontal_frames);
	ClassDB::bind_method(D_METHOD("set_horizontal_frames", "frames"), &Sprite25D::set_horizontal_frames);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "horizontal_frames"), "set_horizontal_frames", "get_horizontal_frames");

	ClassDB::bind_method(D_METHOD("get_vertical_frames"), &Sprite25D::get_vertical_frames);
	ClassDB::bind_method(D_METHOD("set_vertical_frames", "frames"), &Sprite25D::set_vertical_frames);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "vertical_frames"), "set_vertical_frames", "get_vertical_frames");

	ClassDB::bind_method(D_METHOD("get_frame"), &Sprite25D::get_frame);
	ClassDB::bind_method(D_METHOD("set_frame", "frame"), &Sprite25D::set_frame);
	ADD_PROPERTY(PropertyInfo(Variant::INT, "frame"), "set_frame", "get_frame");

	ClassDB::bind_method(D_METHOD("get_frame_coords"), &Sprite25D::get_frame_coords);
	ClassDB::bind_method(D_METHOD("set_frame_coords", "coords"), &Sprite25D::set_frame_coords);
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2I, "frame_coords"), "set_frame_coords", "get_frame_coords");
}
