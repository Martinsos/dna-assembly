export GXX := g++
export NVCC := nvcc
export FLAGS := -I$(CURDIR)/src/ -I$(CURDIR)/external/

src_util := src/util
external_swSharp := external/swSharp

lib := $(src_util) $(external_swSharp)

.PHONY: all $(lib)

all: $(lib)

$(lib):
	$(MAKE) --directory=$@