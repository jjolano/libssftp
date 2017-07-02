TYPE ?= posix
LIBNAME = ssftp_$(TYPE)

distclean:
	$(MAKE) TYPE=posix clean
	$(MAKE) TYPE=cellos clean
	$(MAKE) TYPE=cellos_prx clean
	$(MAKE) TYPE=psl1ght clean

include mk/$(TYPE).mk
