SPOCS
=====

Quickstart
----------

To run spocs via the web interface, launch the container as follows:

`docker run -p 3000:3000 aaronrphillips/spocs`

Then open your browser to `localhost:3000` to view the web interface. After selecting input data and configuring parameters, click Submit to run the SPOCS job in the container. When finished, you will either be redirected to the HTML output, or the text report file will be downloaded (depending on the option selected)

To stop the container, run the following:

``docker stop `docker ps | grep aaronrphillips | awk '{print $1}'` ``

**NOTE**: You must restart the container before starting a new SPOCS run

Running SPOCS Manually
----------------------

You can also skip the web interface and run the `spocs` binary directly. To do this, simply add `spocs` to the end of your command:

 `docker run -p 3000:3000 aaronrphillips/spocs spocs <spocs args>`

### Persisting Data

By default, the web interface allows for downloading of results data, so no volumes need to be mounted. However, in order to run `spocs` manually you will have to mount at least one volume:

``docker run -p 3000:3000 -v `pwd`/host_dir:/data/spocs aaronrphillips/spocs spocs -s /data/spocs/fasta/a.fasta /data/spocs/fasta/b.fasta -p /data/spocs/paralogs -R /data/spocs/report``

A few notes:
  - In the example above, the directory `` `pwd`/host_dir`` on the host would have to have three subdirectories. `paralogs/` and `report/` should be empty, and `fasta/` should contain input data
  - When running the web interface, data can be persisted by mounting an empty host directory (`` `pwd`/host_dir`` above) to `/data/spocs`. 
  - When running `spocs` manually the directory can be mounted anywhere, as long as the arguments passed to `spocs` correspond to the correct location(s).

### Using wildcards

Unfortunately, `*` wildcards are not passed correctly to the `spocs` binary. This makes specifying input files tedious, since it requires you to list each file out individually (as in the example above). However, to use wildcards, you can wrap your spocs command in a call to `/bin/bash`:

``docker run -p 3000:3000 -v `pwd`/host_dir:/data/spocs aaronrphillips/spocs /bin/bash -c 'spocs -s /data/spocs/fasta/*.fasta ...'``

Citations, Disclaimers, and Acknowledgements
-------------------------------------------

To cite this software please use the following citation:

  Curtis DS, Phillips AR, Callister SJ, Conlan S, McCue LA. SPOCS: software for predicting and visualizing orthology/paralogy relationships among genomes. Bioinformatics. 2013;29(20):2641-2642. doi:10.1093/bioinformatics/btt454. 


DISCLAIMER:

The software and materials was prepared as an account of work by AUTHORS
at the Pacific Northwest National Laboratories and sponsored by an agency
of the United States Government. Neither the United States Government nor
the United States Department of Energy, nor Battelle, nor any of their
employees, MAKES ANY WARRANTY, EXPRESS OR IMPLIED, OR ASSUMES ANY LEGAL
LIABILITY OR RESPONSIBILITY FOR THE ACCURACY, COMPLETENESS, OR USEFULNESS
OF ANY INFORMATION, APPARATUS, PRODUCT, SOFTWARE, OR PROCESS DISCLOSED,
OR REPRESENTS THAT ITS USE WOULD NOT INFRINGE PRIVATELY OWNED RIGHTS.

ACKNOWLEDGMENT:

This software and its documentation were produced with United States
Government support under Contract Number DE-AC05-76RL01830 awarded by
the United States Department of Energy. The United States Government
retains a paid-up non-exclusive, irrevocable worldwide license to
reproduce, prepare derivative works, perform publicly and display
publicly by or for the US Government, including the right to distribute
to other US Government contractors.

The newest version of this software and documentation is at:
     http://cbb.pnnl.gov/portal/tools/spocs.html

Read the INSTALL file for installing the software on your system.
