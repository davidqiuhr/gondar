all:

format:
	clang-format -i src/*.{h,cc} test/*.{h,cc}

.PHONY: all format
