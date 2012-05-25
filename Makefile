export GXX := g++
% export NVCC := nvcc
export FLAGS := -I$(CURDIR)/src/ -I$(CURDIR)/external/

src_mappers := src/mappers
src_util := src/util
external_swSharp := external/swSharp

lib := $(src_util) $(external_swSharp) $(src_mappers)

.PHONY: all $(lib)

all: $(lib)

$(lib):
	$(MAKE) --directory=$@