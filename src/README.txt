
/share/apps/mvapich2/1.7/gcc/4.6.2/bin/mpic++ -O2   -g -L/pic/apps/boost/1.51/gcc/4.6.2/openmpi/1.5.4/lib -L/pic/apps/gcc/4.6.2/lib64 -o ../bin/spocs obj/*.o -lboost_program_options -lboost_date_time -lboost_filesystem -lboost_regex -lboost_serialization -lboost_system -lpthread

export LD_LIBRARY_PATH=/pic/apps/boost/1.51/gcc/4.6.2/openmpi/1.5.4/lib:/pic/apps/gcc/4.6.2/lib64


on bedstone:

/share/apps/mvapich2/1.8/gcc/4.6.2/bin/mpic++ -O2 -g -L/usr/local/boost_1_49_0_static64_release_multi/lib -L/usr/local/gcc_4_7_1/lib64 -o ../bin/spocs obj/*.o -lboost_program_options-mt-s -lboost_date_time-mt-s -lboost_filesystem-mt-s -lboost_regex-mt-s -lboost_serialization-mt-s -lboost_system-mt-s -lpthread

bash
export LD_LIBRARY_PATH=/usr/local/gcc_4_7_1/lib64
