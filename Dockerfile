FROM gcc:4.9.4

# get prereqs
RUN apt-get clean
RUN apt-get update
RUN apt-get install --fix-missing -y wget xz-utils tzdata libboost-all-dev

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
WORKDIR /home/ubuntu/spocs
COPY AUTHORS COPYING ChangeLog INSTALL Makefile.am NEWS README VERSION config.h.in configure.ac rebuild_from_scratch.sh /home/ubuntu/spocs/
COPY conf/ conf/
COPY config/ config/
COPY data/ data/
COPY lib/ lib/
COPY src/ src/
COPY support/ support/
COPY test/ test/

RUN ./rebuild_from_scratch.sh

CMD echo "hello, world!"
