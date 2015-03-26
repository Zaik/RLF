########
# Compiles C++ source files, nothing complicated
# Supports simple SOURCE and BUILD directories
#
# Jonatan Waern
# 4/3 -15
########

source_dir = ./source/
build_dir = ./build/
cpp_flags = -g -std=c++11

%:  $(source_dir)%.cpp
	g++ -o $(build_dir)$@ $< $(cpp_flags)

clean:
	rm -f $(build_dir)*
	rm -f *~ $(build_dir)*~ $(source_dir)*~

.phony: clean
