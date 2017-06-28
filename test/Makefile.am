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


#######echo /usr/local/gcc_4_7_1/bin/g++ -static -L/usr/local/boost_1_53_0_static64_release_multi/lib -L../lib  -o t_AlignmentsCollection t_AlignmentsCollection-t_AlignmentsCollection.o ../src/AlignmentsCollection.o ../src/Alignments.o ../src/Ortholog.o ../src/ReportRipper.o ../src/global.o ../src/osUtil.o ../src/FastaReader.o ../src/Lookup.o ../src/OutputHelper.o ../lib/gtest_main.a -lboost_program_options-mt-s -lboost_date_time-mt-s -lboost_filesystem-mt-s -lboost_regex-mt-s -lboost_serialization-mt-s -lboost_system-mt-s -lpthread

#######echo /usr/local/gcc_4_7_1/bin/g++ -I/usr/local/gcc_4_7_1/include -Wall -I../support/vendorsrc/gtest/include -I/usr/local/boost_1_53_0_static64_release_multi/include -D'SPOCS_VERSION="1_0_10"' -D'CXX_VER="GNU /usr/local/gcc_4_7_1/bin/g++ -v 4.8.0"' -D'CXX_OPTIMIZE_LEVEL="O2"' -D'CXX_DEBUG_LEVEL="ON"' -Wall -g -O2 -static  -L/usr/local/boost_1_53_0_static64_release_multi/lib -L../lib  -o t_Alignments t_Alignments-t_Alignments.o ../src/Alignments.o ../src/global.o ../src/osUtil.o ../src/FastaReader.o ../src/Lookup.o ../src/OutputHelper.o -lboost_program_options-mt-s -lboost_date_time-mt-s -lboost_filesystem-mt-s -lboost_regex-mt-s -lboost_serialization-mt-s -lboost_system-mt-s -lgtest_main -lpthread 

TESTS                           = $(bin_PROGRAMS)

common_CXXFLAGS                 = -I$(top_srcdir)/support/vendorsrc/gtest-1.6.0/include

#common_CPPFLAGS                 = -Wall                              \
#                                  -I$(top_srcdir)/src                \
#                                  -I$(top_srcdir)/support/vendorsrc/gtest/include

common_LDADD                    = ../src/global.o                    \
                                  ../src/osUtil.o                    \
                                  ../src/FastaReader.o               \
                                  ../src/Lookup.o                    \
                                  ../src/OutputHelper.o              \
                                  ../lib/libgtest.a                  \
                                  $(BOOST_PROGRAM_OPTIONS_LIB)       \
                                  $(BOOST_DATE_TIME_LIB)             \
                                  $(BOOST_FILESYSTEM_LIB)            \
                                  $(BOOST_REGEX_LIB)                 \
                                  $(BOOST_SERIALIZATION_LIB)         \
                                  $(BOOST_SYSTEM_LIB)

t_Alignments_SOURCES            = t_Alignments.cpp
t_Alignments_CPPFLAGS           = $(common_CPPFLAGS)
t_Alignments_LDADD              = ../src/Alignments.o                \
                                  $(common_LDADD)
t_Alignments_LDFLAGS            = $(SPOCS_CXX_STATIC)                \
                                  $(BOOST_LDFLAGS)


t_AlignmentsCollection_SOURCES  = t_AlignmentsCollection.cpp
t_AlignmentsCollection_CPPFLAGS = $(common_CPPFLAGS)
t_AlignmentsCollection_LDADD    = ../src/AlignmentsCollection.o      \
                                  ../src/Alignments.o                \
                                  ../src/Ortholog.o                  \
                                  ../src/ReportRipper.o              \
                                  $(common_LDADD)
t_AlignmentsCollection_LDFLAGS  = $(t_Alignments_LDFLAGS)


t_BlastRunner_SOURCES           = t_BlastRunner.cpp
t_BlastRunner_CPPFLAGS          = $(common_CPPFLAGS)
t_BlastRunner_LDADD             = ../src/BlastRunner.o               \
                                  $(common_LDADD)
t_BlastRunner_LDFLAGS           = $(t_Alignments_LDFLAGS)


t_Clique_SOURCES                = t_Clique.cpp
t_Clique_CPPFLAGS               = $(common_CPPFLAGS)
t_Clique_LDADD                  = ../src/Clique.o                    \
                                  $(common_LDADD)
t_Clique_LDFLAGS                = $(t_Alignments_LDFLAGS)


t_CliqueRunner_SOURCES          = t_CliqueRunner.cpp
t_CliqueRunner_CPPFLAGS         = $(common_CPPFLAGS)
t_CliqueRunner_LDADD            = ../src/CliqueRunner.o              \
                                  ../src/Clique.o                    \
                                  ../src/HtmlReport.o                \
                                  ../src/InparaParser.o              \
                                  ../src/Solver.o                    \
                                  ../src/Subgraph.o                  \
                                  $(common_LDADD)
t_CliqueRunner_LDFLAGS          = $(t_Alignments_LDFLAGS)


t_Cluster_SOURCES               = t_Cluster.cpp
t_Cluster_CPPFLAGS              = $(common_CPPFLAGS)
t_Cluster_LDADD                 = ../src/Alignments.o                \
                                  ../src/AlignmentsCollection.o      \
                                  ../src/Cluster.o                   \
                                  ../src/Clusters.o                  \
                                  ../src/Ortholog.o                  \
                                  ../src/ReportRipper.o              \
                                  $(common_LDADD)
t_Cluster_LDFLAGS               = $(t_Alignments_LDFLAGS)


t_Clusters_SOURCES              = t_Clusters.cpp
t_Clusters_CPPFLAGS             = $(common_CPPFLAGS)
t_Clusters_LDADD                = ../src/Alignments.o                \
                                  ../src/AlignmentsCollection.o      \
                                  ../src/Cluster.o                   \
                                  ../src/Clusters.o                  \
                                  ../src/Ortholog.o                  \
                                  ../src/ReportRipper.o              \
                                  $(common_LDADD)
t_Clusters_LDFLAGS              = $(t_Alignments_LDFLAGS)


t_FastaReader_SOURCES           = t_FastaReader.cpp
t_FastaReader_CPPFLAGS          = $(common_CPPFLAGS)
t_FastaReader_LDADD             = $(common_LDADD)
t_FastaReader_LDFLAGS           = $(t_Alignments_LDFLAGS)


t_FastaSequence_SOURCES         = t_FastaSequence.cpp
t_FastaSequence_CPPFLAGS        = $(common_CPPFLAGS)
t_FastaSequence_LDADD           = ../src/FastaSequence.o             \
                                  $(common_LDADD)
t_FastaSequence_LDFLAGS         = $(t_Alignments_LDFLAGS)


t_HtmlReport_SOURCES            = t_HtmlReport.cpp
t_HtmlReport_CPPFLAGS           = $(common_CPPFLAGS)
t_HtmlReport_LDADD              = ../src/HtmlReport.o                \
                                  $(common_LDADD)
t_HtmlReport_LDFLAGS            = $(t_Alignments_LDFLAGS)


t_InparaParser_SOURCES          = t_InparaParser.cpp
t_InparaParser_CPPFLAGS         = $(common_CPPFLAGS)
t_InparaParser_LDADD            = ../src/InparaParser.o              \
                                  $(common_LDADD)
t_InparaParser_LDFLAGS          = $(t_Alignments_LDFLAGS)


t_Lookup_SOURCES                = t_Lookup.cpp
t_Lookup_CPPFLAGS               = $(common_CPPFLAGS)
t_Lookup_LDADD                  = $(common_LDADD)
t_Lookup_LDFLAGS                = $(t_Alignments_LDFLAGS)


t_Metadata_SOURCES              = t_Metadata.cpp
t_Metadata_CPPFLAGS             = $(common_CPPFLAGS)
t_Metadata_LDADD                = ../src/Metadata.o                  \
                                  $(common_LDADD)
t_Metadata_LDFLAGS              = $(t_Alignments_LDFLAGS)


t_Ortholog_SOURCES              = t_Ortholog.cpp
t_Ortholog_CPPFLAGS             = $(common_CPPFLAGS)
t_Ortholog_LDADD                = ../src/Ortholog.o                  \
                                  $(common_LDADD)
t_Ortholog_LDFLAGS              = $(t_Alignments_LDFLAGS)


t_osUtil_SOURCES                = t_osUtil.cpp
t_osUtil_CPPFLAGS               = $(common_CPPFLAGS)
t_osUtil_LDADD                  = $(common_LDADD)
t_osUtil_LDFLAGS                = $(t_Alignments_LDFLAGS)


t_OutputHelper_SOURCES          = t_OutputHelper.cpp
t_OutputHelper_CPPFLAGS         = $(common_CPPFLAGS)
t_OutputHelper_LDADD            = $(common_LDADD)
t_OutputHelper_LDFLAGS          = $(t_Alignments_LDFLAGS)


t_ProgramOptions_SOURCES        = t_ProgramOptions.cpp
t_ProgramOptions_CPPFLAGS       = $(common_CPPFLAGS)
t_ProgramOptions_LDADD          = ../src/ProgramOptions.o            \
                                  $(common_LDADD)
t_ProgramOptions_LDFLAGS        = $(t_Alignments_LDFLAGS)


t_ReportRipper_SOURCES          = t_ReportRipper.cpp
t_ReportRipper_CPPFLAGS         = $(common_CPPFLAGS)
t_ReportRipper_LDADD            = ../src/ReportRipper.o              \
                                  $(common_LDADD)
t_ReportRipper_LDFLAGS          = $(t_Alignments_LDFLAGS)


t_Solver_SOURCES                = t_Solver.cpp
t_Solver_CPPFLAGS               = $(common_CPPFLAGS)
t_Solver_LDADD                  = ../src/Solver.o                    \
                                  $(common_LDADD)
t_Solver_LDFLAGS                = $(t_Alignments_LDFLAGS)


t_Subgraph_SOURCES              = t_Subgraph.cpp
t_Subgraph_CPPFLAGS             = $(common_CPPFLAGS)
t_Subgraph_LDADD                = ../src/Solver.o                    \
                                  ../src/Subgraph.o                  \
                                  $(common_LDADD)
t_Subgraph_LDFLAGS              = $(t_Alignments_LDFLAGS)



AM_CXXFLAGS                     = $(SPOCS_CXX_INCLUDE)               \
                                  $(common_CXXFLAGS)                 \
                                  $(BOOST_CPPFLAGS)                  \
                                  $(SPOCS_VERSION) -Wall
AM_LDFLAGS                      = $(SPOCS_CXX_LIB)


clean-local :
	rm -f $(TESTS) *.a *.o
