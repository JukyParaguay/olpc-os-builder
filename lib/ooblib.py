# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

import os
import urllib2
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

def read_buildnr():
    buildnr_path = os.path.join(intermediatesdir, 'buildnr')
    if not os.path.isfile(buildnr_path):
        return "0"
    return open(buildnr_path, "r").readline().strip()

def image_name():
    name_tmpl = read_config('global', 'image_name')
    return name_tmpl % int(read_buildnr())

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

def get_repomd(baseurl):

    # default
    md = {
        'primary'      : 'repodata/primary.xml.gz',
        'primary_db'   : 'repodata/primary.sqlite.bz2',
        'group'        : 'repodata/comps.xml',
        'group_gz'     : 'repodata/repodata/comps.xml.gz',
        'filelists'    : 'repodata/filelists.xml.gz',
        'filelists_db' : 'repodata/filelists.sqlite.bz2',
        'other'        : 'repodata/other.xml.gz',
        'other_db'     : 'repodata/other.sqlite.bz2'
        }

    url = "%s/repodata/repomd.xml" % baseurl
    try:
        fd = urllib2.urlopen(url)
        et = ElementTree(file=fd)
        root = et.getroot()
        # iterate over data tags
        for data in root.findall('{http://linux.duke.edu/metadata/repo}data'):
            type = data.attrib['type']
            location = data.find('{http://linux.duke.edu/metadata/repo}location')
            md[type] = location.attrib['href']
    except:
        pass
    return md
