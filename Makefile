TYPE ?= posix
LIBNAME = ssftp_$(TYPE)

include mk/$(TYPE).mk

libs:
	$(MAKE) TYPE=posix
	$(MAKE) TYPE=cellos
	$(MAKE) TYPE=cellos_prx
	$(MAKE) TYPE=psl1ght
	$(MAKE) TYPE=vita

distclean:
	$(MAKE) TYPE=posix clean
	$(MAKE) TYPE=cellos clean
	$(MAKE) TYPE=cellos_prx clean
	$(MAKE) TYPE=psl1ght clean
	$(MAKE) TYPE=vita clean
