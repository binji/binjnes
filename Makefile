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
