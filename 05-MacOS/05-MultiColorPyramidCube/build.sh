mkdir -p MultiColorPyramidCube.app/Contents/MacOS
clang++ -Wno-deprecated-declarations -o MultiColorPyramidCube.app/Contents/MacOS/MultiColorPyramidCube MultiColorPyramidCube.mm -framework Cocoa -framework QuartzCore -framework OpenGL
