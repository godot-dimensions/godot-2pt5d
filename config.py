# This file is for building as a Godot engine module.
def can_build(env, platform):
	return True


def configure(env):
	pass


def get_doc_classes():
	return [
		"Camera25D",
		"Node25D",
		"World25D",
	]


def get_doc_path():
	return "addons/2pt5d/doc_classes"


def get_icons_path():
	return "addons/2pt5d/icons"
