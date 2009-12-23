# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

import os
import sys
import ooblib
import urllib2
from gzip import GzipFile
from StringIO import StringIO

def add_to_excludes(url, addexcludes):
    url = "%s/repodata/primary.xml.gz" % url
    print >>sys.stderr, "Reading package information from", url
    fd = urllib2.urlopen(url)
    data = fd.read()
    fd.close()
    fd = GzipFile(fileobj=StringIO(data))
    ooblib.add_packages_from_xml(fd, addexcludes)

excludepkgs = set()
addexcludes = ooblib.read_config('repos', 'add_excludes_to')
fedora = ooblib.read_config('repos', 'fedora')
fver = ooblib.read_config('global', 'fedora_release').strip()

# clean up addexcludes list
if addexcludes is not None:
    addexcludes = addexcludes.split(',')
    for idx, excl in enumerate(addexcludes):
        addexcludes[idx] = excl.strip()
else:
    addexcludes = []

repos = {}

# cycle over all 3 repos types, adding them to repos
# add things to exclude list on-the-fly
for key, value in os.environ.iteritems():
    if key.startswith("CFG_repos__olpc_frozen_"):
        for_excludes, name = value.split(',', 1)
        for_excludes = int(for_excludes)
        url = "http://mock.laptop.org/repos/%s" % name
        if for_excludes:
            add_to_excludes(url, excludepkgs)
        repos[name] = ("baseurl", url)
    elif key.startswith("CFG_repos__olpc_publicrpms_"):
        for_excludes, name = value.split(',', 1)
        for_excludes = int(for_excludes)
        url = "http://xs-dev.laptop.org/~dsd/repos/%s" % name
        if for_excludes:
            add_to_excludes(url, excludepkgs)
        repos[name] = ("baseurl", url)


if fedora is not None:
    for repo in fedora.split(','):
        repo = repo.strip()
        if repo == "fedora":
            repos["fedora"] = ("mirrorlist", "http://mirrors.fedoraproject.org/mirrorlist?repo=fedora-%s&arch=i386" % fver)
        elif repo == "fedora-updates":
            repos["fedora-updates"] = ("mirrorlist", "http://mirrors.fedoraproject.org/mirrorlist?repo=updates-released-f%s&arch=i386" % fver)
        elif repo == "fedora-updates-testing":
            repos["fedora-updates-testing"] = ("mirrorlist", "http://mirrors.fedoraproject.org/mirrorlist?repo=updates-testing-f%s&arch=i386" % fver)

# generate repo lines including excludes
excludepkgs = list(excludepkgs)
excludepkgs.sort()
for key, value in repos.iteritems():
    sys.stdout.write("repo --name=%s " % key)
    if value[0] == "mirrorlist":
        sys.stdout.write("--mirrorlist=%s" % value[1])
    else:
        sys.stdout.write("--baseurl=%s" % value[1])
    if len(excludepkgs) > 0 and key in addexcludes:
        sys.stdout.write(" --excludepkgs=%s" % ','.join(excludepkgs))
    sys.stdout.write("\n")


