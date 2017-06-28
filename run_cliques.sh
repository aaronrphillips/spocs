#!/bin/bash

umask 022 

##SBATCH -A CBB_PORTAL
##SBATCH -t 6-23:59:59
#SBATCH -n 176
#SBATCH -N 22
#SBATCH -e $HOME/dev/cliques
#SBATCH -o $HOME/dev/cliques
##SBATCH -J MstrWrkr 
##SBATCH --partition=slurm

export tasks=`cat $0 | egrep -e "^#SBATCH [-]n" | awk '{print $NF}'`
export nodes=`cat $0 | egrep -e "^#SBATCH [-]N" | awk '{print $NF}'`
export work_dir="$HOME/dev/cliques"

message ()
{
  echo "Step $step: $1"
  ((step += 1))
}


usage ()
{
  echo ""
  echo "  Usage: $script tag [options to pass to spocs]"
  echo ""
}


fail ()
{
  echo ""
  echo "ERROR: $1"
  echo ""
  exit $failure
}


check_file ()
{
  if [ ! -f "$1" ]; then
    fail "File not found: $1"
  fi

  return 1
}


check_dir ()
{
  if [ ! -d "$1" ]; then
    fail "Directory not found: $1"
  fi

  return 1
}


check_dirs ()
{
  local dir 

  for dir in $*
  do
    check_dir $dir
  done

  return 1
}


#cd $work_dir
#rm slurm-*.out slurm-*.err

# Make sure all the binaries are built
cd $work_dir/src
make
if [ "$?" -ne "0" ]; then
  fail "Failed to build cliques software" 
fi

cd $work_dir


script=`basename $0`
#blast_path="/usr/local/blast-2.2.22/bin"
#blast_path="$HOME/dev/sblast/support/vendorsrc/NCBI/ncbi/bin"
blast_path="/lustre/cliques/blast-2.2.22/bin"


failure=1
success=0
step=1
tag=$1
shift
extra_args="$*"


#export LD_LIBRARY_PATH="$work_dir/lib"
export CLIQUES_HOME=$HOME/dev/cliques
export out_dir="/lustre/cliques"
export out_dir_tag="$out_dir/$tag"

#  TODO: FIX: CRAP: DORK: JUNK: REMOVE: DELETE:  # This is just for testing
#rm -rf ${out_dir_tag}_out_dir/* > /dev/null 2>&1


if [ -z "$tag" ]; then
  usage
  fail "Missing tag" 
fi


check_dirs $CLIQUES_HOME $out_dir


outgroup_file=`echo ${out_dir_tag}_fasta/OUTGROUP*.fasta 2> /dev/null`


message "START"
message "setting up environment"
export PATH="${blast_path}:${PATH}"
mkdir -p ${out_dir_tag}_{fasta,out_dir,cliques} > /dev/null 2>&1
message "cleaning up any old output in ${out_dir_tag}_cliques"

#  TODO: FIX: CRAP: DORK: JUNK: REMOVE: DELETE:  # This is just for testing
#rm -rf ${out_dir_tag}_cliques/* > /dev/null 2>&1

message "checking environment"
check_dirs ${out_dir_tag}_{fasta,out_dir,cliques}


message "checking outgroup"
outgroup_option=""
if [ ! -f "$outgroup_file" ]; then
  message "INFO: running without an outgroup file"
else
  outgroup_option="-o $outgroup_file"
fi


hash -r


message "running spocs with extra args = $extra_args"
message "$CLIQUES_HOME/src/spocs -s ${out_dir_tag}_fasta/*.fasta $outgroup_option -p ${out_dir_tag}_out_dir -R ${out_dir_tag}_cliques -H $extra_args"


$CLIQUES_HOME/src/spocs -s ${out_dir_tag}_fasta/*.fasta $outgroup_option -p ${out_dir_tag}_out_dir -R ${out_dir_tag}_cliques -H $extra_args
rc=$?
message "STOP"

exit $rc
