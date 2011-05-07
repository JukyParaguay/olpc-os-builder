# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

from __future__ import with_statement
from __future__ import division

import sys
import os.path
import urllib
import urllib2
import urlparse

from bitfrost.update import microformat

import ooblib

def generate_install_cmd(path):
    if path.endswith(".xol"):
        print "unzip -d $INSTALL_ROOT/home/olpc/Library -q '%s'" % path
    else:
        print "unzip -d $INSTALL_ROOT/home/olpc/Activities -q '%s'" % path


cache = os.path.join(ooblib.cachedir, 'activities')
if not os.path.exists(cache):
    os.makedirs(cache)

baseurl = ooblib.read_config('sugar_activity_group', 'url')
install_activities = ooblib.read_config_bool('sugar_activity_group',
                                             'install_activities')
systemwide = ooblib.read_config_bool('sugar_activity_group',
                                     'activity_group_systemwide')

if install_activities:
    vmaj = int(ooblib.read_config('global', 'olpc_version_major'))
    vmin = int(ooblib.read_config('global', 'olpc_version_minor'))
    vrel = int(ooblib.read_config('global', 'olpc_version_release'))

    suffixes = ["%d.%d.%d" % (vmaj, vmin, vrel), "%d.%d" % (vmaj, vmin), ""]

    for suffix in suffixes:
        if len(suffix) > 0:
            grpurl = urlparse.urljoin(baseurl + "/", urllib.quote(suffix))
        else:
            grpurl = baseurl

        print >>sys.stderr, "Trying group URL", grpurl
        try:
            name, desc, results = microformat.parse_url(grpurl)
        except urllib2.HTTPError, e:
            if e.code == 404:
                continue
            raise e
        if len(results) == 0 or (name is None and desc is None):
            continue
        print >>sys.stderr, "Found activity group:", name

        for name, info in results.items():
            (version, url) = microformat.only_best_update(info)
            print >>sys.stderr, "Examining %s v%s: %s" % (name, version, url)
            fd = urllib2.urlopen(url)
            headers = fd.info()
            if not 'Content-length' in headers:
                raise Exception("No content length for %s" % url)
            length = int(headers['Content-length'])
            path = urlparse.urlsplit(fd.geturl())[2]
            path = os.path.basename(path)

            localpath = os.path.join(cache, path)
            if os.path.exists(localpath):
                localsize = os.stat(localpath).st_size
                if localsize == length:
                    print >>sys.stderr, "Not downloading, already in cache."
                    generate_install_cmd(localpath)
                    continue

            print >>sys.stderr, "Downloading (%dkB)..." % (length/1024)
            localfd = open(localpath, 'w')
            localfd.write(fd.read())
            fd.close()
            localfd.close()
            generate_install_cmd(localpath)

        # only process the first working URL
        break

if systemwide:
    print "mkdir -p $INSTALL_ROOT/etc/olpc-update"
    print "echo '%s' > $INSTALL_ROOT/etc/olpc-update/activity-groups" % baseurl
else:
    print "echo '%s' > $INSTALL_ROOT/home/olpc/Activities/.groups" % baseurl

print "chown -R 500:500 $INSTALL_ROOT/home/olpc/{Activities,Library}"

