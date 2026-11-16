README
this program was completetd on 11/16/2026 by Nicholas Bernhoft

USAGE:
assignment03.exe [width] [height]


BUILDING:
The only requirement to build this program is the minGW (or other) C++ compiler.
to build this program on windows, simply run the build03.bat script.


I decided to use a Frame Buffer (readFBO) system because it would be simpler than the suggested buffer output / pass through a do-nothing fragment and vertex shader.

note the rotation is with a fixed position light source where the camera is rotating about the stationary molegule. This shows the full lighting around each atom.