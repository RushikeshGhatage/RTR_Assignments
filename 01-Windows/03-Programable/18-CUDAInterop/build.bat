nvcc -c -o sinewave.cu.obj sinewave.cu
cl.exe /c /EHsc /I "C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.1\include" CUDAInterop.cpp
link.exe CUDAInterop.obj sinewave.cu.obj OGL.res user32.lib gdi32.lib /LIBPATH:"C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v11.1\lib\x64" /MACHINE:x64 /SUBSYSTEM:WINDOWS
