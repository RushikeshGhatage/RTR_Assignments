mkdir -p 3MovingLightsOnSphere.app/Contents/MacOS
clang++ -Wno-deprecated-declarations -o 3MovingLightsOnSphere.app/Contents/MacOS/3MovingLightsOnSphere 3MovingLightsOnSphere.mm -framework Cocoa -framework QuartzCore -framework OpenGL
