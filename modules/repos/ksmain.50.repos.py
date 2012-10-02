# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

import os
import sys
import ooblib
from gzip import GzipFile
from StringIO import StringIO

excludepkgs = set()
addexcludes = ooblib.read_config('repos', 'add_excludes_to')
fedora = ooblib.read_config('repos', 'fedora')
fver = ooblib.read_config('global', 'fedora_release').strip()
farch = ooblib.read_config('global', 'fedora_arch').strip()

def add_to_excludes(baseurl, addexcludes):
    print >>sys.stderr, "Reading repository information for", baseurl
    repomd = ooblib.get_repomd(baseurl)
    url = baseurl + '/' + repomd['primary']

    print >>sys.stderr, "Reading package information from", url
    fd = ooblib.cachedurlopen(url)
    data = fd.read()
    fd.close()
    fd = GzipFile(fileobj=StringIO(data))
    ooblib.add_packages_from_xml(fd, addexcludes, farch)

# clean up addexcludes list
if addexcludes is not None:
    addexcludes = addexcludes.split(',')
    for idx, excl in enumerate(addexcludes):
        excl = excl.strip()

        # Support hyphenated fedora repo notation, as this matches the
        # default files in /etc/yum.repos.d
        if excl == 'fedora-updates-testing':
            excl = 'fedora_updates_testing'
        elif excl == 'fedora-updates':
            excl = 'fedora_updates'

        addexcludes[idx] = excl
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
        url = "http://rpmdropbox.laptop.org/%s" % name
        if for_excludes:
            add_to_excludes(url, excludepkgs)
        repos[name] = ("baseurl", url)
    elif key.startswith("CFG_repos__custom_repo_"):
        for_excludes, name, url = value.split(',', 2)
        for_excludes = int(for_excludes)
        if for_excludes:
            add_to_excludes(url, excludepkgs)
        repos[name] = ("baseurl", url)

FEDORA_URLS = {
    'fedora' : 'http://mirrors.fedoraproject.org/mirrorlist?repo=fedora-%(version)s&arch=%(arch)s',
    'fedora_updates' : 'http://mirrors.fedoraproject.org/mirrorlist?repo=updates-released-f%(version)s&arch=%(arch)s',
    'fedora_updates_testing' : 'http://mirrors.fedoraproject.org/mirrorlist?repo=updates-testing-f%(version)s&arch=%(arch)s',
    'rawhide' : 'http://mirrors.fedoraproject.org/mirrorlist?repo=rawhide&arch=%(arch)s',
}

def get_fedora_repo(name, version, arch):
    override_url = ooblib.read_config('repos', 'url_%s' % name)
    if override_url is not None:
        return "baseurl", override_url

    if name not in FEDORA_URLS:
        return None, None

    return "mirrorlist", FEDORA_URLS[name] % { 'version': version, 'arch': arch }

if fedora is not None:
    for repo in fedora.split(','):
        repo = repo.strip().replace('-', '_')
        repotype, url = get_fedora_repo(repo, fver, farch)
        if repotype:
            repos[repo] = (repotype, url)
        else:
            print >>sys.stderr, "Unknown Fedora repo:", repo

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


