export GXX := g++
% export NVCC := nvcc

src_mappers := src/mappers
src_util := src/util
external_swSharp := external/swSharp

lib := $(src_util) $(src_mappers)
export INCLUDES := -I$(CURDIR)/src/ -I$(CURDIR)/external/
export PREFIX := $(CURDIR)
export CUDA_PATH := /usr/local/cuda/lib64


.PHONY: all $(lib) $(external_swSharp)

all: $(lib) $(external_swSharp)

$(lib): $(external_swSharp)
	$(MAKE) --directory=$@

$(external_swSharp):
	$(MAKE) install --directory=$@

$(src_mappers): $(src_util)