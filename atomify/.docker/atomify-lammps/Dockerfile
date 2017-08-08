FROM ubuntu:16.04

RUN apt-get -qq update
RUN apt-get install -qq sudo

RUN sudo apt-get -qq update
RUN sudo apt-get -qq install software-properties-common
RUN sudo apt-get -qq install python-software-properties
RUN sudo apt-add-repository -y ppa:beineri/opt-qt58-xenial
RUN sudo apt-get -qq update
RUN sudo apt-get -qq install qt583d qt58charts-no-lgpl qt58datavis3d-no-lgpl qt58declarative qt58svg

# install packages needed by configure
RUN sudo apt-get -qq install patch git build-essential curl

# needed by build
RUN sudo apt-get -qq install mesa-common-dev
RUN sudo apt-get -qq install libgl1-mesa-dev

# needed by deploy
RUN sudo apt-get -qq install snapcraft
RUN sudo snap install core

WORKDIR /app

ADD . /app/
RUN ./configure.py
