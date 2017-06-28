#!/bin/csh

##SBATCH -A CBB_PORTAL
##SBATCH -t 6-23:59:59
#SBATCH -n 22
#SBATCH -N 22
##SBATCH -J MstrWrkr 
##SBATCH --partition=slurm

set tasks=`cat $0 | egrep -e "^#SBATCH [-]n" | awk '{print $NF}'`
set nodes=`cat $0 | egrep -e "^#SBATCH [-]N" | awk '{print $NF}'`
set work_dir="$HOME/dev/cliques/support/mpi"

cd $work_dir
#rm slurm-*.out
#make doith

echo "SCRIPT START "`date`
srun -n $tasks -N $nodes $work_dir/doith -t $work_dir/gold4.tasks -v -r n
echo "SCRIPT STOP  "`date`
exit
