Add the following to the "./snapdump" folder before you snap:

- atomify (binary file)
- Qt lib directory
- Qt plugins directory
- Qt qml directory

Run the following commands:

    snapcraft snap
    sudo snap install atomify*.snap
    atomify

If everything works as expected, neuronify should run now.
