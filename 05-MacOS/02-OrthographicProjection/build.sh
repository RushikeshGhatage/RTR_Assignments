mkdir -p OrthographicProjection.app/Contents/MacOS
clang++ -Wno-deprecated-declarations -o OrthographicProjection.app/Contents/MacOS/OrthographicProjection OrthographicProjection.mm -framework Cocoa -framework QuartzCore -framework OpenGL
