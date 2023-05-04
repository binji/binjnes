.SUFFIXES:

all: debug

define BUILD
out/$1:
	mkdir -p out/$1
out/$1/Makefile: | out/$1
	cd out/$1 && cmake ../.. -DCMAKE_BUILD_TYPE=$1 -DWERROR=ON
.PHONY: $2
$2: out/$1/Makefile
	+$(MAKE) --no-print-directory -C out/$1
endef

$(eval $(call BUILD,Debug,debug))
$(eval $(call BUILD,Release,release))

# Emscripten stuff

EMSCRIPTEN_DIR := emscripten
EMSCRIPTEN_CMAKE := ${EMSCRIPTEN_DIR}/cmake/Modules/Platform/Emscripten.cmake

define EMSCRIPTEN_BUILD
out/$1:
	mkdir -p out/$1
out/$1/Makefile: | out/$1
	cd out/$1 && \
		cmake ../.. -DCMAKE_TOOLCHAIN_FILE=$(EMSCRIPTEN_CMAKE) \
								-DCMAKE_BUILD_TYPE=Release \
								-DWASM_SIMD=$3
								-DWERROR=ON
.PHONY: $2
$2: out/$1/Makefile
	$(MAKE) --no-print-directory -C out/$1
endef

$(eval $(call EMSCRIPTEN_BUILD,Wasm,wasm,OFF))
$(eval $(call EMSCRIPTEN_BUILD,WasmSimd,wasm-simd,ON))

.PHONY: demo
demo: wasm wasm-simd
	cp out/Wasm/binjnes.js docs/
	cp out/Wasm/binjnes.wasm docs/
	cp out/WasmSimd/binjnes-simd.js docs/binjnes-simd.js
	cp out/WasmSimd/binjnes-simd.wasm docs/binjnes-simd.wasm
