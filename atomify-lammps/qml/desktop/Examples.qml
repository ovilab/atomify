import QtQuick 2.7
import QtQuick.Controls 2.0
import "editor"

Pane {
    id: root
    property CodeEditorWindow codeEditorWindow

    Flickable {
        anchors.fill: parent

        Column {
            id: col

            anchors {
                margins: 15
                fill: parent
            }
            spacing: 10

            Image {
                width: col.width
                source: "qrc:/images/examples/lammpsLogo.gif"
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        codeEditorWindow.newTab()
                        codeEditorWindow.currentEditor.text =
"
# simulation of LAMMPS logo

#/atom 1 2.0 #3333FF
#/atom 2 2.0 #FFFF00

units		lj
atom_style	atomic
dimension	2

# create 2d lattice, low density so diffuses

lattice		sq2 0.5 origin 0.25 0.25 0.0
region		box block 0 31 0 7 -0.5 0.5
create_box	2 box
create_atoms	1 box

# LAMMPS letters via regions, convert to type 2 atoms

region	        L1 block 2 3 1 6 -0.5 0.5
region	        L2 block 2 5 1 2 -0.5 0.5
region		L union 2 L1 L2

region	        A1 block 6 7 1 6 -0.5 0.5
region	        A2 block 8 9 1 6 -0.5 0.5
region	        A3 block 6 9 3 4 -0.5 0.5
region	        A4 block 6 9 5 6 -0.5 0.5
region		A union 4 A1 A2 A3 A4

region	        1M1 block 10 11 1 6 -0.5 0.5
region	        1M2 block 14 15 1 6 -0.5 0.5
region	        1M3 block 10 15 5 6 -0.5 0.5
region	        1M4 block 12 13 3 6 -0.5 0.5
region		1M union 4 1M1 1M2 1M3 1M4

region	        2M1 block 16 17 1 6 -0.5 0.5
region	        2M2 block 20 21 1 6 -0.5 0.5
region	        2M3 block 16 21 5 6 -0.5 0.5
region	        2M4 block 18 19 3 6 -0.5 0.5
region		2M union 4 2M1 2M2 2M3 2M4

region	        P1 block 22 23 1 6 -0.5 0.5
region	        P2 block 24 25 3 6 -0.5 0.5
region	        P3 block 22 25 3 4 -0.5 0.5
region	        P4 block 22 25 5 6 -0.5 0.5
region		P union 4 P1 P2 P3 P4

region	        S1 block 26 29 5 6 -0.5 0.5
region	        S2 block 26 27 3 6 -0.5 0.5
region	        S3 block 26 29 3 4 -0.5 0.5
region	        S4 block 28 29 1 4 -0.5 0.5
region	        S5 block 26 29 1 2 -0.5 0.5
region		S union 5 S1 S2 S3 S4 S5

region          LAMMPS union 6 L A 1M 2M P S
set		region LAMMPS type 2

# LJ and other system parameters

mass		* 1.0
pair_style	lj/cut 2.5
pair_coeff	* * 1.0 1.0 2.5

timestep        0.005
neighbor	0.3 bin
neigh_modify	delay 0 every 1 check yes

velocity	all create 2.0 87287 loop geom

# Have thermo_style print pe and press every timestep so it is available for plotting
thermo_style custom step pe press
thermo 1

# run without integrator to 'pause' the visualization

run		100

# dissolve the lattice

fix		1 all nve
fix		2 all enforce2d
run		1000

# run without integrator to pause' the visualization

unfix		1
unfix		2
run		200

# reverse the velocities and continue run
# logo and lattice should reassemble within round-off errors

variable        vxflip atom -vx
variable        vyflip atom -vy
velocity	all set v_vxflip v_vyflip NULL

fix		1 all nve
fix		2 all enforce2d
run		1000

# run without integrator to 'pause' the visualization

unfix		1
unfix		2
run		100
"
                    }
                }
            }

            Image {
                width: col.width
                source: "qrc:/images/examples/example1.png"
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        codeEditorWindow.newTab()
                        codeEditorWindow.currentEditor.text =
"
#/atom 1 1.8 #E74C3C
#/atom 2 1.8 #3498DB
variable L equal 32.0
variable thickness equal 6.0

units lj
atom_style atomic
atom_modify map hash
lattice fcc 0.8442

variable left equal 0.5*$L-0.5*${thickness}
variable right equal 0.5*$L+0.5*${thickness}
variable Lhalf equal 0.5*$L
region system block 0 $L 0 $L 0 $L
region boxinside block 0 $L ${left} ${right} 0 $L
region boxoutside block 0 $L ${left} ${right} 0 $L side out
region sphere sphere 0.0 ${Lhalf} ${Lhalf} 15.0

create_box 3 system
create_atoms 1 region boxinside
create_atoms 2 region boxoutside
mass * 1.0
group type1 type 1
group type2 type 2

velocity type1 create 3.44 87287 loop geom
velocity type2 create 1.44 87287 loop geom

pair_style lj/cut 2.5
pair_coeff * * 1.0 1.0 2.5

neighbor 0.3 bin
neigh_modify delay 0 every 20 check no

compute kinetic all ke/atom
compute cc1 all chunk/atom bin/2d x lower 0.2 y lower 0.2
fix avehunk all ave/chunk 5 20 100 cc1 c_kinetic ave running

fix nve all nve

# Have thermo_style print pe and press every timestep so it is available for plotting
thermo_style custom step pe press
thermo 1
"
                    }
                }
            }

            Image {
                width: col.width
                source: "qrc:/images/examples/example2.png"
                fillMode: Image.PreserveAspectFit
                MouseArea {
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    onClicked: {
                        codeEditorWindow.newTab()
                        codeEditorWindow.currentEditor.text =
"
# 3d Lennard-Jones melt
units		lj
atom_style	atomic

lattice fcc 0.8442
region box block 0 10 0 10 0 50
create_box 2 box

create_atoms 1 box
region cylinder cylinder z 5 5 3 0 50
delete_atoms region cylinder
create_atoms 2 region cylinder
mass * 1.0

pair_style lj/cut 2.5
pair_coeff * * 1.0 1.0 2.5

neighbor 0.3 bin
neigh_modify delay 0 every 20 check no

# Have thermo_style print pe and press every timestep so it is available for plotting
thermo_style custom step pe press
thermo 1

group other type 2
group mobile type 2
velocity mobile create 1.44 87287 loop geom
fix nvt mobile nvt temp 1.0 1.0 1.0

fix gravity mobile gravity 2.0 vector 0 0 1

variable force equal sqrt(fx*fx+fy*fy+fz*fz)
compute kinetic all ke/atom
compute cc1 all chunk/atom bin/2d x lower 0.2 y lower 0.2
fix avehunk all ave/chunk 1 100 100 cc1 vz ave running
"
                    }
                }
            }

        }
    }
}
