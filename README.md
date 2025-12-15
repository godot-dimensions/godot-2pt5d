# Godot 2.5D

Node25D and other 2.5-dimensional types for making 2.5D games in Godot.

## Installation

First, have all the Godot dependencies installed on your computer as per the
[official instructions](https://docs.godotengine.org/en/stable/engine_details/development/compiling/index.html).

If you want all of the Godot Dimensions modules at once, you can grab a
branch of [our Godot fork](https://github.com/godot-dimensions/godot/tree/dimensions-latest).

```shell
git clone https://github.com/godot-dimensions/godot godot-dimensions
cd godot-dimensions
# First, make sure you can compile regular Godot.
scons
# Then, switch to the dimensions branch and download the submodules.
git switch dimensions-latest
git submodule update --init --recursive
scons
# Re-run the `scons` command to re-compile if you make changes.
```

Alternatively, if you only want 2.5D, you can start with any clone
of Godot Engine and add this module to it:

```shell
cd modules
git clone https://github.com/godot-dimensions/godot-2pt5d 2pt5d
# Make sure the folder is named `2pt5d`, not `godot-2pt5d`!
cd ..
scons
# Re-run the `scons` command to re-compile if you make changes.
```

It should now be successfully compiled, and you can run the executable in the `bin` folder.

## License

This repo is free and open source software licensed under The Unlicense.
Credit and attribution is appreciated but not required.

Some parts of some files in this repository are derivative from Godot Engine
and therefore [its MIT license](https://godotengine.org/license) applies.
You must provide credit to Godot Engine by including its LICENSE.
Considering this repo is only usable in conjunction with Godot anyway,
this will not be a problem because you should already be crediting Godot.
