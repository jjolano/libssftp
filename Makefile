TYPE ?= posix
LIBNAME = ssftp_$(TYPE)

libs:
	$(MAKE) TYPE=posix
	$(MAKE) TYPE=cellos
	$(MAKE) TYPE=cellos_prx
	$(MAKE) TYPE=psl1ght

distclean:
	$(MAKE) TYPE=posix clean
	$(MAKE) TYPE=cellos clean
	$(MAKE) TYPE=cellos_prx clean
	$(MAKE) TYPE=psl1ght clean

include mk/$(TYPE).mk
