# -*- Mode:Python; indent-tabs-mode:nil; tab-width:4 -*-
#
# Copyright (C) 2015 Canonical Ltd
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import os
import snapcraft
import snapcraft.common


class QmlPlugin(snapcraft.BasePlugin):

    @classmethod
    def schema(cls):
        return {}

    def __init__(self, name, options, project):
        super().__init__(name, options, project)
        self.stage_packages.extend([
            'qtubuntu-desktop',
            'ttf-ubuntu-font-family',
        ])

    def snap_fileset(self):
        return ['*',
                'etc/xdg/qtchooser/snappy-qt5.conf',
                ]

    def _build_qt_config(self):
        arch = 'x86_64-linux-gnu'
        configdir = os.path.join(self.installdir, 'etc', 'xdg', 'qtchooser')
        os.makedirs(configdir, exist_ok=True)
        with open(os.path.join(configdir, 'snappy-qt5.conf'), 'w') as config:
            config.write('./usr/lib/{}/qt5/bin\n'.format(arch))
            config.write('./usr/lib/{}\n'.format(arch))

    def build(self):
        self._build_qt_config()

    def env(self, root):
        arch = 'x86_64-linux-gnu'
        return [
            'LD_LIBRARY_PATH={}/usr/lib/{}:$LD_LIBRARY_PATH'.format(
                root, arch),
            'LD_LIBRARY_PATH={}/lib64:$LD_LIBRARY_PATH'.format(
                root),
            # XKB config
            'QT_XKB_CONFIG_ROOT={}/usr/share/X11/xkb'.format(root),
            'XKB_CONFIG_ROOT=$SNAP/usr/share/X11/xkb',
            'QT_SELECT=snappy-qt5',
            # Qt Libs
            'LD_LIBRARY_PATH={}/lib:$LD_LIBRARY_PATH'
            .format(root, arch),
            'LD_LIBRARY_PATH={}/usr/lib/{}/pulseaudio:$LD_LIBRARY_PATH'
            .format(root, arch),
            # Qt Modules
            'QT_PLUGIN_PATH={}/plugins'.format(root),
            'QML2_IMPORT_PATH={}/qml'.format(root),
            # Mesa Libs
            'LD_LIBRARY_PATH={}/usr/lib/{}/mesa:$LD_LIBRARY_PATH'
            .format(root, arch),
            'LD_LIBRARY_PATH={}/usr/lib/{}/mesa-egl:$LD_LIBRARY_PATH'
            .format(root, arch),
            'LIBGL_DRIVERS_PATH={}/usr/lib/{}/dri'.format(root, arch),
            # XDG Config
            'XDG_CONFIG_DIRS={}/etc/xdg:$XDG_CONFIG_DIRS'.format(root),
            'XDG_CONFIG_DIRS={}/usr/xdg:$XDG_CONFIG_DIRS'.format(root),
            'XDG_DATA_DIRS={}/usr/share:$XDG_DATA_DIRS'.format(root),
            'XDG_DATA_HOME={}/usr/share'.format(root),
            # Font Config
            'FONTCONFIG_PATH={}/etc/fonts/config.d'.format(root),
            'FONTCONFIG_FILE={}/etc/fonts/fonts.conf'.format(root),
]
