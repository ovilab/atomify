FROM ubuntu:16.04

RUN apt-get -qq update
RUN apt-get install -qq sudo

RUN sudo apt-get -qq update
RUN sudo apt-get -qq install software-properties-common
RUN sudo apt-get -qq install python-software-properties
RUN sudo apt-add-repository -y ppa:beineri/opt-qt57-xenial
RUN sudo apt-get -qq update
RUN sudo apt-get -qq install qt573d qt57charts-no-lgpl qt57datavis-no-lgpl qt57declarative qt57svg

# set up env vars for Qt
RUN source /opt/qt57/bin/qt57-env.sh

# install packages needed by configure
RUN sudo apt-get -qq install patch git build-essential curl
RUN sudo apt-get -qq install snapcraft
