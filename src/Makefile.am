bin_PROGRAMS       = spocs

spocs_SOURCES      = AlignmentsCollection.cpp     \
                     Alignments.cpp               \
                     BlastRunner.cpp              \
                     Clique.cpp                   \
                     CliqueRunner.cpp             \
                     Cluster.cpp                  \
                     Clusters.cpp                 \
                     FastaReader.cpp              \
                     FastaSequence.cpp            \
                     global.cpp                   \
                     HtmlReport.cpp               \
                     InparaParser.cpp             \
                     Lookup.cpp                   \
                     Metadata.cpp                 \
                     Ortholog.cpp                 \
                     osUtil.cpp                   \
                     OutputHelper.cpp             \
                     ProgramOptions.cpp           \
                     ReportRipper.cpp             \
                     Solver.cpp                   \
                     Subgraph.cpp                 \
                     spocs.cpp

spocs_LDADD        = $(BOOST_PROGRAM_OPTIONS_LIB) \
                     $(BOOST_DATE_TIME_LIB)       \
                     $(BOOST_FILESYSTEM_LIB)      \
                     $(BOOST_REGEX_LIB)           \
                     $(BOOST_SERIALIZATION_LIB)   \
                     $(BOOST_SYSTEM_LIB)          \
                     -lpthread

spocs_LDFLAGS      = $(SPOCS_CXX_STATIC)  $(BOOST_LDFLAGS)

AM_CXXFLAGS        = $(SPOCS_CXX_INCLUDE) $(BOOST_CPPFLAGS) $(SPOCS_VERSION) -Wall

AM_LDFLAGS         = $(SPOCS_CXX_LIB)

EXTRA_DIST         = AlignmentsCollection.hpp     \
                     Alignments.hpp               \
                     BlastRunner.hpp              \
                     Clique.hpp                   \
                     CliqueRunner.hpp             \
                     Cluster.hpp                  \
                     Clusters.hpp                 \
                     Exception.hpp                \
                     FastaReader.hpp              \
                     FastaSequence.hpp            \
                     global.hpp                   \
                     HtmlReport.hpp               \
                     InparaParser.hpp             \
                     Lookup.hpp                   \
                     Metadata.hpp                 \
                     Ortholog.hpp                 \
                     osUtil.hpp                   \
                     OutputHelper.hpp             \
                     ProgramOptions.hpp           \
                     ReportRipper.hpp             \
                     Solver.hpp                   \
                     Subgraph.hpp                 \
                     spocs.hpp                               

dist-hook: clean
	-rm -rf `find $(distdir) -type d -name .svn`
