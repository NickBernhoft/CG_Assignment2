# this Script was made by Nicholas Bernhoft on Aug 26, 2026

# this variable is the name of the .c file you intend to compile
programName="assignment02"

echo "building $programName for MacOS"

rm $programName
clang -I./include $programName.c include/glad/glad.c libglfw3_macos.a -o "$programName" -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

wait

# run inf successfully generated an executable
if [ -f ./$programName ]; then
	echo "compiled successfully!" & 
	./$programName
fi
