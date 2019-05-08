if not exist .\build64 (
	mkdir build64
)
cd build64
cmd /c "cmake -DCMAKE_GENERATOR_PLATFORM=x64 .."	