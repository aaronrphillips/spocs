#!/bin/csh

##SBATCH -A CBB_PORTAL
##SBATCH -t 6-23:59:59
#SBATCH -n 4
#SBATCH -N 2
##SBATCH -J MstrWrkr 
##SBATCH --partition=slurm

set path=(/lustre/cliques/blast-2.2.22/bin; $path)

set tasks=`cat $0 | egrep -e "^#SBATCH [-]n" | awk '{print $NF}'`
set nodes=`cat $0 | egrep -e "^#SBATCH [-]N" | awk '{print $NF}'`

echo "SCRIPT START "`date`

mkdir -p  /lustre/cliques/gold15_cliques
rm    -rf /lustre/cliques/gold15_cliques/*
mkdir -p  /lustre/cliques/big8_cliques
rm    -rf /lustre/cliques/big8_cliques/*

#srun -n $tasks -N $nodes /home/d3x528/dev/cliques/bin/spocs -s /lustre/cliques/gold15_fasta/*.fasta -o /lustre/cliques/gold15_fasta/OUTGROUP_M_thermoautotrophicus_DeltaH.fasta -p /lustre/cliques/gold15_out_dir -R /lustre/cliques/gold15_cliques -H
srun -n 1 -N 1 /home/d3x528/dev/cliques/bin/spocs -s /lustre/cliques/gold15_fasta/*.fasta -o /lustre/cliques/gold15_fasta/OUTGROUP_M_thermoautotrophicus_DeltaH.fasta -p /lustre/cliques/gold15_out_dir -R /lustre/cliques/gold15_cliques -H

srun -n 1 -N 1 /home/d3x528/dev/cliques/bin/spocs -s /lustre/cliques/big8_fasta/*.fasta -o /lustre/cliques/big8_fasta/OUTGROUP_M_thermoautotrophicus_DeltaH.fasta -p /lustre/cliques/big8_out_dir -R /lustre/cliques/big8_cliques -H

echo "SCRIPT STOP  "`date`

exit
