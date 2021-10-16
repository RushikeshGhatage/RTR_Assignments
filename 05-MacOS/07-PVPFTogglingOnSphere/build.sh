mkdir -p PVPFTogglingOnSphere.app/Contents/MacOS
clang++ -Wno-deprecated-declarations -o PVPFTogglingOnSphere.app/Contents/MacOS/PVPFTogglingOnSphere PVPFTogglingOnSphere.mm -framework Cocoa -framework QuartzCore -framework OpenGL
