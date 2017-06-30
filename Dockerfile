FROM gcc:4.9.4

# get prereqs
RUN apt-get clean
RUN apt-get update
RUN apt-get install --fix-missing -y autotools-dev bc libboost-all-dev tzdata wget xz-utils 

# set time zone
ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN dpkg-reconfigure -f noninteractive tzdata

# get BLAST
WORKDIR /home/ubuntu
RUN wget http://ftp.ncbi.nlm.nih.gov/blast/executables/legacy/2.2.22/blast-2.2.22-x64-linux.tar.gz
RUN tar xzf blast-2.2.22-x64-linux.tar.gz
ENV PATH="/home/ubuntu/blast-2.2.22/bin:${PATH}"

# build SPOCS
RUN mkdir /home/ubuntu/spocs
RUN mkdir /home/ubuntu/spocs/lib /home/ubuntu/spocs/bin
WORKDIR /home/ubuntu/spocs
COPY conf/ conf/
COPY config/ config/
COPY data/ data/
COPY support/ support/
COPY test/ test/
#COPY test/ test/ #TODO
COPY AUTHORS COPYING ChangeLog INSTALL Makefile.am NEWS README VERSION config.h.in configure.ac rebuild_from_scratch.sh test-spocs.sh /home/ubuntu/spocs/
COPY src/ src/

RUN autoreconf --install --force
ENV LIBS=" -lboost_date_time -lboost_system -lboost_regex -lboost_serialization -lboost_filesystem -lboost_program_options -lpthread"
RUN ./configure --prefix=/home/ubuntu --with-boost=/usr/lib/x86_64-linux-gnu
RUN make -j 4

#RUN ./test-spocs.sh

CMD /home/ubuntu/spocs/src/spocs -h
