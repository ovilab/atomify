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

    def env(self, root):
        arch = 'x86_64-linux-gnu'
        return [
            'QT_PLUGIN_PATH={}/plugins'.format(root),
            'QML2_IMPORT_PATH={}/qml'.format(root),
            'XDG_DATA_HOME={}/usr/share'.format(root),
        ]
