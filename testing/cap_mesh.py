# Copyright 2013 DEVSIM LLC
#
# SPDX-License-Identifier: Apache-2.0

from devsim import (
    add_1d_contact,
    add_1d_mesh_line,
    add_1d_region,
    create_1d_mesh,
    finalize_mesh,
)


def cap_mesh(region_name, material_name):
    """
    Creates a simple cap mesh named cap1 for test purposes with a given region name
    Contacts, top, bot
    length 1.0
    material "Ox"
    mesh can be instantiated as a device
    """
    create_1d_mesh(mesh="cap1")
    for p, s, t in ((0.0, 0.1, "top"), (1.0, 0.1, "bot")):
        add_1d_mesh_line(mesh="cap1", pos=p, ps=s, tag=t)

    for t in ("top", "bot"):
        add_1d_contact(mesh="cap1", name=t, tag=t, material="metal")

    add_1d_region(
        mesh="cap1", material=material_name, region=region_name, tag1="top", tag2="bot"
    )
    finalize_mesh(mesh="cap1")
    return "cap1"
