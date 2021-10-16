mkdir -p PerspectiveProjection.app/Contents/MacOS
clang++ -Wno-deprecated-declarations -o PerspectiveProjection.app/Contents/MacOS/PerspectiveProjection PerspectiveProjection.mm -framework Cocoa -framework QuartzCore -framework OpenGL
