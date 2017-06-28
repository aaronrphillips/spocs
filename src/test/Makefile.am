bin_PROGRAMS                    = t_AlignmentsCollection             \
                                  t_Alignments                       \
                                  t_BlastRunner                      \
                                  t_Clique                           \
                                  t_CliqueRunner                     \
                                  t_Cluster                          \
                                  t_Clusters                         \
                                  t_FastaReader                      \
                                  t_FastaSequence                    \
                                  t_HtmlReport                       \
                                  t_InparaParser                     \
                                  t_Lookup                           \
                                  t_Metadata                         \
                                  t_Ortholog                         \
                                  t_osUtil                           \
                                  t_OutputHelper                     \
                                  t_ProgramOptions                   \
                                  t_ReportRipper                     \
                                  t_Solver                           \
                                  t_Subgraph


TESTS                           = $(bin_PROGRAMS)

common_CFLAGS                   = -Wall
common_LDADD                    = ../global.o                        \
                                  ../osUtil.o                        \
                                  ../FastaReader.o                   \
                                  ../Lookup.o                        \
                                  ../OutputHelper.o                  \
                                  $(BOOST_TEST_EXEC_MONITOR_LIB)     \
                                  $(BOOST_UNIT_TEST_FRAMEWORK_LIB)   \
                                  $(BOOST_PROGRAM_OPTIONS_LIB)       \
                                  $(BOOST_DATE_TIME_LIB)             \
                                  $(BOOST_FILESYSTEM_LIB)            \
                                  $(BOOST_REGEX_LIB)                 \
                                  $(BOOST_SERIALIZATION_LIB)         \
                                  $(BOOST_SYSTEM_LIB)

t_Alignments_SOURCES            = t_Alignments.cpp
t_Alignments_CFLAGS             = $(common_CFLAGS)
t_Alignments_LDADD              = ../Alignments.o                    \
                                  $(common_LDADD)
t_Alignments_LDFLAGS            = $(SPOCS_CXX_STATIC)                \
                                  $(BOOST_LDFLAGS)


t_AlignmentsCollection_SOURCES  = t_AlignmentsCollection.cpp
t_AlignmentsCollection_CFLAGS   = $(common_CFLAGS)
t_AlignmentsCollection_LDADD    = ../AlignmentsCollection.o          \
                                  ../Alignments.o                    \
                                  ../Ortholog.o                      \
                                  ../ReportRipper.o                  \
                                  $(common_LDADD)
t_AlignmentsCollection_LDFLAGS  = $(t_Alignments_LDFLAGS)


t_BlastRunner_SOURCES           = t_BlastRunner.cpp
t_BlastRunner_CFLAGS            = $(common_CFLAGS)
t_BlastRunner_LDADD             = ../BlastRunner.o                   \
                                  $(common_LDADD)
t_BlastRunner_LDFLAGS           = $(t_Alignments_LDFLAGS)


t_Clique_SOURCES                = t_Clique.cpp
t_Clique_CFLAGS                 = $(common_CFLAGS)
t_Clique_LDADD                  = ../Clique.o                        \
                                  $(common_LDADD)
t_Clique_LDFLAGS                = $(t_Alignments_LDFLAGS)


t_CliqueRunner_SOURCES          = t_CliqueRunner.cpp
t_CliqueRunner_CFLAGS           = $(common_CFLAGS)
t_CliqueRunner_LDADD            = ../CliqueRunner.o                  \
                                  ../Clique.o                        \
                                  ../HtmlReport.o                    \
                                  ../InparaParser.o                  \
                                  ../Solver.o                        \
                                  ../Subgraph.o                      \
                                  $(common_LDADD)
t_CliqueRunner_LDFLAGS          = $(t_Alignments_LDFLAGS)


t_Cluster_SOURCES               = t_Cluster.cpp
t_Cluster_CFLAGS                = $(common_CFLAGS)
t_Cluster_LDADD                 = ../Alignments.o                    \
                                  ../AlignmentsCollection.o          \
                                  ../Cluster.o                       \
                                  ../Clusters.o                      \
                                  ../Ortholog.o                      \
                                  ../ReportRipper.o                  \
                                  $(common_LDADD)
t_Cluster_LDFLAGS               = $(t_Alignments_LDFLAGS)


t_Clusters_SOURCES              = t_Clusters.cpp
t_Clusters_CFLAGS               = $(common_CFLAGS)
t_Clusters_LDADD                = ../Alignments.o                    \
                                  ../AlignmentsCollection.o          \
                                  ../Cluster.o                       \
                                  ../Clusters.o                      \
                                  ../Ortholog.o                      \
                                  ../ReportRipper.o                  \
                                  $(common_LDADD)
t_Clusters_LDFLAGS              = $(t_Alignments_LDFLAGS)


t_FastaReader_SOURCES           = t_FastaReader.cpp
t_FastaReader_CFLAGS            = $(common_CFLAGS)
t_FastaReader_LDADD             = $(common_LDADD)
t_FastaReader_LDFLAGS           = $(t_Alignments_LDFLAGS)


t_FastaSequence_SOURCES         = t_FastaSequence.cpp
t_FastaSequence_CFLAGS          = $(common_CFLAGS)
t_FastaSequence_LDADD           = ../FastaSequence.o                 \
                                  $(common_LDADD)
t_FastaSequence_LDFLAGS         = $(t_Alignments_LDFLAGS)


t_HtmlReport_SOURCES            = t_HtmlReport.cpp
t_HtmlReport_CFLAGS             = $(common_CFLAGS)
t_HtmlReport_LDADD              = ../HtmlReport.o                    \
                                  $(common_LDADD)
t_HtmlReport_LDFLAGS            = $(t_Alignments_LDFLAGS)


t_InparaParser_SOURCES          = t_InparaParser.cpp
t_InparaParser_CFLAGS           = $(common_CFLAGS)
t_InparaParser_LDADD            = ../InparaParser.o                  \
                                  $(common_LDADD)
t_InparaParser_LDFLAGS          = $(t_Alignments_LDFLAGS)


t_Lookup_SOURCES                = t_Lookup.cpp
t_Lookup_CFLAGS                 = $(common_CFLAGS)
t_Lookup_LDADD                  = $(common_LDADD)
t_Lookup_LDFLAGS                = $(t_Alignments_LDFLAGS)


t_Metadata_SOURCES              = t_Metadata.cpp
t_Metadata_CFLAGS               = $(common_CFLAGS)
t_Metadata_LDADD                = $(common_LDADD)
t_Metadata_LDFLAGS              = $(t_Alignments_LDFLAGS)


t_Ortholog_SOURCES              = t_Ortholog.cpp
t_Ortholog_CFLAGS               = $(common_CFLAGS)
t_Ortholog_LDADD                = ../Ortholog.o                      \
                                  $(common_LDADD)
t_Ortholog_LDFLAGS              = $(t_Alignments_LDFLAGS)


t_osUtil_SOURCES                = t_osUtil.cpp
t_osUtil_CFLAGS                 = $(common_CFLAGS)
t_osUtil_LDADD                  = $(common_LDADD)
t_osUtil_LDFLAGS                = $(t_Alignments_LDFLAGS)


t_OutputHelper_SOURCES          = t_OutputHelper.cpp
t_OutputHelper_CFLAGS           = $(common_CFLAGS)
t_OutputHelper_LDADD            = $(common_LDADD)
t_OutputHelper_LDFLAGS          = $(t_Alignments_LDFLAGS)


t_ProgramOptions_SOURCES        = t_ProgramOptions.cpp
t_ProgramOptions_CFLAGS         = $(common_CFLAGS)
t_ProgramOptions_LDADD          = ../ProgramOptions.o                \
                                  $(common_LDADD)
t_ProgramOptions_LDFLAGS        = $(t_Alignments_LDFLAGS)


t_ReportRipper_SOURCES          = t_ReportRipper.cpp
t_ReportRipper_CFLAGS           = $(common_CFLAGS)
t_ReportRipper_LDADD            = ../ReportRipper.o                  \
                                  $(common_LDADD)
t_ReportRipper_LDFLAGS          = $(t_Alignments_LDFLAGS)


t_Solver_SOURCES                = t_Solver.cpp
t_Solver_CFLAGS                 = $(common_CFLAGS)
t_Solver_LDADD                  = ../Solver.o                        \
                                  $(common_LDADD)
t_Solver_LDFLAGS                = $(t_Alignments_LDFLAGS)


t_Subgraph_SOURCES              = t_Subgraph.cpp
t_Subgraph_CFLAGS               = $(common_CFLAGS)
t_Subgraph_LDADD                = ../Solver.o                        \
                                  ../Subgraph.o                      \
                                  $(common_LDADD)
t_Subgraph_LDFLAGS              = $(t_Alignments_LDFLAGS)



AM_CXXFLAGS                     = $(SPOCS_CXX_INCLUDE)               \
                                  $(BOOST_CPPFLAGS)                  \
                                  $(SPOCS_VERSION)
AM_LDFLAGS                      = $(SPOCS_CXX_LIB)


#default:
#	/usr/local/gcc_4_7_1/bin/g++ -I. -I.. -I/usr/local/gcc_4_7_1/include -I/usr/local/boost_1_49_0_static64_debug_multi/include -D'SPOCS_VERSION="1_0_7"' -D'CXX_VER="GNU /usr/local/gcc_4_7_1/bin/g++ -v 4.8.0"' -D'CXX_OPTIMIZE_LEVEL="O2"' -D'CXX_DEBUG_LEVEL="ON"' -g -O2 -static  -L/usr/local/boost_1_49_0_static64_debug_multi/lib utilities.cpp -o utilities.o ../global.o ../osUtil.o ../FastaReader.o ../Lookup.o ../OutputHelper.o -lboost_test_exec_monitor-mt-sd -lboost_unit_test_framework-mt-sd -lboost_program_options-mt-sd -lboost_date_time-mt-sd -lboost_filesystem-mt-sd -lboost_regex-mt-sd -lboost_serialization-mt-sd -lboost_system-mt-sd -lpthread

dist-hook:
	-rm -rf `find $(distdir) -type d -name .svn`
