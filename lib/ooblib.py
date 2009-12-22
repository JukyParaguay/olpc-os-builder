# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

import os
from xml.etree.ElementTree import ElementTree

libdir = os.environ['OOB__libdir']
bindir = os.environ['OOB__bindir']
builddir = os.environ['OOB__builddir']
cachedir = os.environ['OOB__cachedir']
intermediatesdir = os.environ['OOB__intermediatesdir']
outputdir = os.environ['OOB__outputdir']
statedir = os.environ['OOB__statedir']
fsmount = os.environ['OOB__fsmount']

def read_config(module, option):
    vname = "CFG_%s__%s" % (module, option)
    if not vname in os.environ:
        return None
    return os.environ[vname]

def read_config_bool(module, option):
    vname = "CFG_%s__%s" % (module, option)
    if not vname in os.environ:
        return None
    return bool(int(os.environ[vname]))

def add_packages_from_xml(fd, pkglist):
    et = ElementTree(file=fd)
    root = et.getroot()
    for i in root.getchildren():
        if not i.tag.endswith("}package"):
            continue
        for child in i.getchildren():
            if not child.tag.endswith("}name"):
                continue
            pkglist.add(child.text)

