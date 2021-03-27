# C1 quadratic spline

This is a desktop app that demonstrates how C1 quadratic splines work. You can add new control points by double-clicking and you can move points by dragging them.

The app is created using Qt5 and C++

![Demo](demo.gif?raw=true "Demo")

# Download

Builds for Windows are posted in the [releases](https://github.com/pgeorgiev98/c1_quadratic_spline/releases) section of the project.

Linux and Mac users have to build it from source.

# Building

	git clone https://github.com/pgeorgiev98/c1_quadratic_spline
	cd c1_quadratic_spline
	mkdir build
	cd build
	qmake ..
	make

	# To run:
	./c1_quadratic_spline
