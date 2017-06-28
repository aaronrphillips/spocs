#!/bin/bash

blastall=`which blastall`

rm -rf data/output/gold4_cliques/*      > /dev/null 2>&1
rm data/output/gold4_out_dir/lookup.bin > /dev/null 2>&1
rm data/output/gold4_out_dir/*paralogs  > /dev/null 2>&1
rm data/output/gold4_out_dir/*hits      > /dev/null 2>&1
rm data/output/gold4_out_dir/*journal   > /dev/null 2>&1
rm data/output/gold4_out_dir/*orthologs > /dev/null 2>&1
rm data/output/gold4_out_dir/*rejected  > /dev/null 2>&1
src/spocs -s data/output/gold4_fasta/*.fasta -o data/output/gold4_fasta/OUTGROUP_M_thermoautotrophicus_DeltaH.fasta -R data/output/gold4_cliques -p data/output/gold4_out_dir -D data/output/gold4_metadata.csv -M 3 -H -d
count=`diff -w data/output/gold4_cliques/cliques.report data/ref/gold4_cliques.report | wc -l | sed -e "s/\s*//g"`
echo "DIFF COUNT: $count"
exit $count
