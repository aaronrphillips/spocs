DIST_DIR         =   /srv/lcs/warehouse/spocs
ACLOCAL_AMFLAGS  = -I config/m4
EXTRA_DIST       =    COPYING                          \
                      README                           \
                      conf                             \
                      config/m4                        \
                      data                             \
                      support/boost_install_script.sh  \
                      test-spocs.sh                    \
                      VERSION
if MAINTAINER
SUBDIRS          =    src test
EXTRA_DIST      +=    test
else
SUBDIRS          =    src
endif
DIST_SUBDIRS     =    src

TESTS            =    test-spocs.sh
 
SPOCS_CXX=`which $(CXX)`
SPOCS_CXX_DIR=`dirname $(SPOCS_CXX)`
SPOCS_CXX_ROOT=`dirname $(SPOCS_CXX_DIR)`
SPOCS_CXX_INCLUDE=`-I$(SPOCS_CXX_ROOT)/include`
SPOCS_CXX_LIB=`-L$(SPOCS_CXX_ROOT)/lib -L$(top_srcdir)/lib`

MAJOR_VERSION=`$(AWK) -F'_' '{print $$1}' $(top_srcdir)/VERSION`
MINOR_VERSION=`$(AWK) -F'_' '{print $$2}' $(top_srcdir)/VERSION`
MICRO_VERSION=`$(AWK) -F'_' '{print $$3}' $(top_srcdir)/VERSION`
NEW_MAJOR_VERSION=`$(AWK) -F'_' '{print $$1+1}' $(top_srcdir)/VERSION | bc`
NEW_MINOR_VERSION=`$(AWK) -F'_' '{print $$2+1}' $(top_srcdir)/VERSION | bc`
NEW_MICRO_VERSION=`$(AWK) -F'_' '{print $$3+1}' $(top_srcdir)/VERSION | bc`
BOOST_ROOT=`echo $(BOOST_CPPFLAGS) | sed -e "s/^\s*\-I//" -e "s/\/include//"`

dist-hook: clean
	-rm -rf       $(distdir)/data/output/gold4_cliques/*
	-rm -f        $(distdir)/data/*/*/*.sh
	-rm -f        $(distdir)/data/*/*/*.paralogs
	-rm -f        $(distdir)/data/*/*/*.hits
	-rm -f        $(distdir)/data/*/*/lookup.bin
	-rm -f  `find $(distdir) -type f -name \*.o`
	-rm -rf `find $(distdir) -type d -name .svn`

.PHONY: macro micro minor release
 

major: clean
	@echo "$(NEW_MAJOR_VERSION)_0_0" > $(top_srcdir)/VERSION
	cd $(top_srcdir) && autoreconf --install && ./configure `grep with-boost config.log | sed -e "s/.*configure //"`


minor: clean
	@echo "$(MAJOR_VERSION)_$(NEW_MINOR_VERSION)_0" > $(top_srcdir)/VERSION
	cd $(top_srcdir) && autoreconf --install && ./configure `grep with-boost config.log | sed -e "s/.*configure //"`


micro: clean
	@echo "$(MAJOR_VERSION)_$(MINOR_VERSION)_$(NEW_MICRO_VERSION)" > $(top_srcdir)/VERSION
	cd $(top_srcdir) && autoreconf --install && ./configure `grep with-boost config.log | sed -e "s/.*configure //"`


release: dist
	mv $(distdir).tar.gz $(DIST_DIR) && rm -f $(DIST_DIR)/spocs-stable.tar.gz && ln -s $(DIST_DIR)/$(distdir).tar.gz $(DIST_DIR)/spocs-stable.tar.gz


clean-local:
	-rm -rf $(top_srcdir)/data/output/gold4_cliques/*
	-rm  -f $(top_srcdir)/data/output/gold4_fasta/*.{pin,phr,psq,log}
	-rm  -f $(top_srcdir)/data/output/gold4_out_dir/*.{hits,paralogs,sh}
	-rm  -f $(top_srcdir)/data/output/gold4_out_dir/lookup.bin
	-rm  -f $(top_srcdir)/data/output/gold4_out_dir/Y_pestis_KIM10-OUTGROUP_M_thermoautotrophicus_DeltaH.blastout


install-data-local:
	-cp -R conf ${prefix}/
