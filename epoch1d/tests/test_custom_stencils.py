#!/usr/bin/env python

# Copyright (C) 2017 Alexander Blinne <A.Blinne@gsi.de>,
# Stephan Kuschel <Stephan.Kuschel@gmail.com>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

import numpy as np
import sdf
import matplotlib; matplotlib.use('Agg')
import matplotlib.pyplot as plt
import os
import os.path as osp
import unittest
import platform
from . import SimTest


def showdatafields(sdffile):
    data = sdf.read(sdffile, dict=True)
    print('Data fields present in "' + sdffile + '":')
    print(data.keys())

def plotdump(sdffile, key, ax):
    data_lehe_custom = sdf.read(sdffile[0], dict=True)
    data_lehe_x = sdf.read(sdffile[1], dict=True)
    data_optimized = sdf.read(sdffile[2], dict=True)

    axis = data_optimized[key].grid_mid.data[0]*1e6
    array_optimized = data_optimized[key].data
    array_lehe_x = data_lehe_x[key].data
    array_lehe_custom = data_lehe_custom[key].data
    array_optimized = data_optimized[key].data
    ax.plot(axis, array_lehe_x, label='Lehe (Builtin)', linewidth=1)
    ax.plot(axis, array_lehe_custom, label='Lehe (Custom)', linewidth=1, linestyle='dotted')
    ax.plot(axis, array_optimized, label='Optimized', linewidth=1)
    ax.set_title('{:2.1f} fs'.format(data_optimized['Header']['time']*1e15))
    ax.set_xlabel('x [µm]')
    ax.legend()

def plotevolution(key):
    sdffiles = zip([osp.join('lehe_custom','{:04d}.sdf'.format(i)) for i in range(8)],
                   [osp.join('lehe_x','{:04d}.sdf'.format(i)) for i in range(8)],
                   [osp.join('optimized','{:04d}.sdf'.format(i)) for i in range(8)])
    fig, axarr = plt.subplots(2,4, figsize=(16,9))
    for (sdffile, ax) in zip(sdffiles, np.ravel(axarr)):
        plotdump(sdffile, key, ax)
    fig.suptitle(key)
    fig.savefig(key.replace('/','_') + '.png', dpi=320)

def createplots():
    if platform.system() == 'Darwin':
        print('macosx backend')
        plt.switch_backend('macosx')
    #showdatafields('0000.sdf')
    plotevolution('Electric Field/Ey')


class test_custom_stencils(SimTest):

    def test_createplots(self):
        createplots()

if __name__=='__main__':
    unittest.main()


