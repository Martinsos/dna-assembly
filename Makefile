export GXX := g++
% export NVCC := nvcc

src_mappers := src/mappers
src_util := src/util
external_swSharp := external/swSharp

lib := $(src_util) $(external_swSharp) $(src_mappers)
export INCLUDES := -I$(CURDIR)/src/ -I$(CURDIR)/external/
export PREFIX := $(CURDIR)
export CUDA_PATH := /usr/local/cuda/lib64


.PHONY: all $(lib)

all: $(lib)

$(lib):
	$(MAKE) --directory=$@

$(src_mappers): $(src_util)