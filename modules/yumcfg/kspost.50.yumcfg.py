# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

import os
import sys
import urllib2
from gzip import GzipFile
from StringIO import StringIO
from xml.etree.ElementTree import ElementTree

import ooblib

addrepos = []
excludes = set()

def add_excludes_from_xml(fd):
    et = ElementTree(file=fd)
    root = et.getroot()
    for i in root.getchildren():
        if not i.tag.endswith("}package"):
            continue
        for child in i.getchildren():
            if not child.tag.endswith("}name"):
                continue
            excludes.add(child.text)

# read in repos
for var in os.environ:
    if not var.startswith("CFG_yumcfg__addrepo"):
        continue
    value = os.environ[var]
    for_excludes, name, url = value.split(',', 2)
    for_excludes = bool(int(for_excludes))
    addrepos.append((for_excludes, name, url))

# generate excludes info
for for_excludes, name, url in addrepos:
    if not for_excludes:
        continue
    fd = urllib2.urlopen(url + "/repodata/primary.xml.gz")
    data = fd.read()
    fd.close()
    fd = GzipFile(fileobj=StringIO(data))
    add_excludes_from_xml(fd)

# write shell code to generate yum repo files
for for_excludes, name, url in addrepos:
    print "cat > /etc/yum.repos.d/%s.repo <<EOF" % name
    print "[%s]" % name
    print "name=%s" % name
    print "failovermethod=priority"
    print "baseurl=%s" % url
    print "enabled=1"
    print "metadata_expire=7d"
    print "gpgcheck=0"
    print "EOF\n\n"

# write shell code to force enable selected repos
force_enable = ooblib.read_config('yumcfg', 'force_enable')
if force_enable is not None:
    repos = force_enable.split(',')
    for repo in repos:
        repo = repo.strip()
        print "#enable first disabled repo in %s" % repo
        print "sed -i -e '0,/enabled=0/s//enabled=1/' /etc/yum.repos.d/%s.repo\n" % repo

# write shell code to force disable selected repos
force_disable = ooblib.read_config('yumcfg', 'force_disable')
if force_disable is not None:
    repos = force_disable.split(',')
    for repo in repos:
        repo = repo.strip()
        print "#disable first enabled repo in %s" % repo
        print "sed -i -e '0,/enabled=1/s//enabled=0/' /etc/yum.repos.d/%s.repo\n" % repo

# write shell code to generate excludes file
excludes = list(excludes)
excludes.sort()
print "cat > /etc/yum/olpc-exclude <<EOF"
sys.stdout.write("exclude=")
for pkg in excludes:
    sys.stdout.write(pkg + " ")
print "\nEOF\n"

# write shell code to add exclude info
add_excludes = ooblib.read_config('yumcfg', 'add_excludes_to')
if add_excludes is not None:
    repos = add_excludes.split(',')
    for repo in repos:
        repo = repo.strip()
        print "sed -i -e '/^enabled=/a include=file:///etc/yum/olpc-exclude' /etc/yum.repos.d/%s.repo\n" % repo

