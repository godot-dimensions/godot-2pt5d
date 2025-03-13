#pragma once

#include "editor_viewport_2pt5d_defines.h"

// Grabs input events and sends them to the main screen.
// This sits as a layer on top of most of the viewport, except the rotation gizmo.
class EditorInputSurface25D : public Control {
	GDCLASS(EditorInputSurface25D, Control);

	EditorMainScreen25D *_editor_main_screen = nullptr;
	EditorMainViewport25D *_editor_main_viewport_2pt5d = nullptr;

public:
	static void _bind_methods() {}
	virtual void GDEXTMOD_GUI_INPUT(const Ref<InputEvent> &p_event) override;

	void setup(EditorMainScreen25D *p_editor_main_screen, EditorMainViewport25D *p_editor_main_viewport);
};
