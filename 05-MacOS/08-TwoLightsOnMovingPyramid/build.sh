mkdir -p TwoLightsOnMovingPyramid.app/Contents/MacOS
clang++ -Wno-deprecated-declarations -o TwoLightsOnMovingPyramid.app/Contents/MacOS/TwoLightsOnMovingPyramid TwoLightsOnMovingPyramid.mm -framework Cocoa -framework QuartzCore -framework OpenGL
