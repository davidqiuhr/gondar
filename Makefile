all:

format:
	clang-format -i src/*.{h,cc}

.PHONY: all format
