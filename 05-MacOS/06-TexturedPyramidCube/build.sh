mkdir -p TexturedPyramidCube.app/Contents/MacOS
clang++ -Wno-deprecated-declarations -o TexturedPyramidCube.app/Contents/MacOS/TexturedPyramidCube TexturedPyramidCube.mm -framework Cocoa -framework QuartzCore -framework OpenGL
