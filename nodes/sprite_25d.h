#pragma once

#include "node_25d.h"

#if GDEXTENSION
#include <godot_cpp/classes/sprite2d.hpp>
#elif GODOT_MODULE
#include "scene/2d/sprite_2d.h"
#endif

class Sprite25D : public Node25D {
	GDCLASS(Sprite25D, Node25D);

	Sprite2D *_sprite_2d = nullptr;

protected:
	static void _bind_methods();

public:
	Sprite2D *get_sprite_2d() const { return _sprite_2d; }

	Ref<Texture2D> get_texture() const;
	void set_texture(const Ref<Texture2D> &p_texture);

	// Offset.
	bool get_is_centered() const;
	void set_is_centered(const bool p_centered);
	Vector2 get_offset() const;
	void set_offset(const Vector2 &p_offset);
	bool get_flip_horizontal() const;
	void set_flip_horizontal(const bool p_flip);
	bool get_flip_vertical() const;
	void set_flip_vertical(const bool p_flip);

	// Animation
	int get_horizontal_frames() const;
	void set_horizontal_frames(const int p_frames);
	int get_vertical_frames() const;
	void set_vertical_frames(const int p_frames);
	int get_frame() const;
	void set_frame(const int p_frame);
	Vector2i get_frame_coords() const;
	void set_frame_coords(const Vector2i &p_coords);

	Sprite25D();
};
