FROM ovilab/atomify-lammps-build-essential

WORKDIR /app

ADD . /app/
RUN ./configure.py
RUN snapcraft
