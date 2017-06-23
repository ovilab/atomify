FROM ubuntu:16.04

WORKDIR /app

RUN apt-get -qq update
RUN apt-get install -qq sudo apt-utils

RUN sudo apt-get update -qq
RUN sudo apt-get install -qq curl p7zip-full git build-essential python snapcraft qt5-default locales
ADD qt5-ubuntu.sh /qt5-ubuntu.sh
RUN bash /qt5-ubuntu.sh /opt/qt5 extra-datavis3d extra-charts gamepad 3d base declarative graphicaleffects imageformats multimedia quickcontrols quickcontrols2 sensors svg tools translations xmlpatterns
