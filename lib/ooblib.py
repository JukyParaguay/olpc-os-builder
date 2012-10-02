# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

import os
import sys
import shutil
import hashlib
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

METADATA_NS = "http://linux.duke.edu/metadata/common"

cacheonly = 'OOB__cacheonly' in os.environ

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

def read_laptop_model_number():
    path = os.path.join(intermediatesdir, 'laptop_model_number')
    if not os.path.isfile(path):
        return "0"
    return open(path, "r").readline().strip()

def image_name():
    major_ver = read_config('global', 'olpc_version_major')
    minor_ver = read_config('global', 'olpc_version_minor')
    cust_tag = read_config('global', 'customization_tag')
    buildnr = int(read_buildnr())
    modelnr = read_laptop_model_number()

    return "%s%s%03d%s%s" % (major_ver, minor_ver, buildnr, cust_tag, modelnr)

def arch_matches(myarch, arch):
    # figure out if a package under 'arch' is suitable for 'myarch'
    # myarch is either 'i386', 'arm' or 'armhfp'
    # but 'arch' can be i386, i586, i686, armv5tel, armv7hl, and so on

    # noarch is always suitable
    if arch == 'noarch':
        return True

    if myarch.startswith('arm'):
        return arch.startswith('arm')
    elif myarch == 'i386':
        return arch in ['i386', 'i486', 'i586', 'i686']
    else:
        return False

def add_packages_from_xml(fd, pkglist, myarch):
    et = ElementTree(file=fd)
    root = et.getroot()
    for i in root.getchildren():
        if not i.tag.endswith("}package"):
            continue
        arch = i.find("{%s}arch" % METADATA_NS)
        name = i.find("{%s}name" % METADATA_NS)

        # Only add packages that are suitable for myarch.
        if myarch and arch is not None:
            if not arch_matches(myarch, arch.text):
                continue

        if name is not None:
            pkglist.add(name.text)

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
        fd = cachedurlopen(url)
        et = ElementTree(file=fd)
        root = et.getroot()
        # iterate over data tags
        for data in root.findall('{http://linux.duke.edu/metadata/repo}data'):
            type = data.attrib['type']
            location = data.find('{http://linux.duke.edu/metadata/repo}location')
            md[type] = location.attrib['href']
    except urllib2.HTTPError:
        pass
    return md

def ln_or_cp(src, dest):
    src_dev = os.stat(src).st_dev
    dest_dev = os.stat(dest).st_dev

    if src_dev == dest_dev:
        if os.path.isdir(dest):
            dest = os.path.join(dest, os.path.basename(src))
        os.link(src, dest)
    else:
        shutil.copy(src, dest)

def install_sugar_bundle(path):
    bundlesdir = os.path.join(intermediatesdir, "shared", "sugar-bundles")
    if not os.path.exists(bundlesdir):
        os.makedirs(bundlesdir)
    ln_or_cp(path, bundlesdir)

""" A wrapper around urllib2.urlopen() that stores responses in
    cache. When cacheonly=True, it works offline, never hitting
    the network.
"""
def cachedurlopen(url):
    class CachedURLException(Exception):
        def __init__(self, value):
            self.value=value

    cachedfpath = os.path.join(cachedir, 'simplecache', hashlib.sha1(url).hexdigest())
    if cacheonly:
        if os.path.exists(cachedfpath):
            return open(cachedfpath)
        else:
            print >>sys.stderr, "ERROR: No cached file for %s" % url
            raise CachedURLException("No cached file for %s" % url)

    ourcachedir=os.path.join(cachedir, 'simplecache')
    if not os.path.exists(ourcachedir):
        os.makedirs(ourcachedir)

    urlfd = urllib2.urlopen(url)
    fd = open(cachedfpath, 'w')
    fd.write(urlfd.read())
    urlfd.close()
    fd.close()

    return open(cachedfpath, 'r')
