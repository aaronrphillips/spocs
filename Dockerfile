FROM ubuntu:latest

RUN apt-get clean
RUN apt-get update
RUN apt-get install --fix-missing -y wget xz-utils tzdata

ENV TZ=America/Los_Angeles
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone
RUN dpkg-reconfigure -f noninteractive tzdata

CMD echo "hello, world!"
