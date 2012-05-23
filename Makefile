GXX := g++
NVCC := nvcc

src_util := src/util
external_swSharp := external/swSharp

lib := $(src_util) $(external_swSharp)

.PHONY: all $(lib)

all: $(lib)

$(lib):
	$(MAKE) --directory=$@