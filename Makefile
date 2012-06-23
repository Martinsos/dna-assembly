export GXX := g++
% export NVCC := nvcc

src_mappers := src/mappers
src_util := src/util
external_swSharp := external/swSharp
fm_index := src/FMindex

lib := $(src_util) $(src_mappers)
export INCLUDES := -I$(CURDIR)/src/ -I$(CURDIR)/external/
export PREFIX := $(CURDIR)
export CUDA_PATH := /usr/local/cuda/lib64


.PHONY: all $(lib) $(external_swSharp) $(fm_index)

all: $(lib) $(external_swSharp) $(fm_index)

$(lib): $(external_swSharp) $(fm_index)
	$(MAKE) --directory=$@

$(external_swSharp):
	$(MAKE) install --directory=$@

$(fm_index):
	$(MAKE) --directory=$@

$(src_mappers): $(src_util)
