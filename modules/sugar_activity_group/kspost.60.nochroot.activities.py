# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

from __future__ import with_statement
from __future__ import division

import sys
import os.path
import urllib
import urllib2
import urlparse
import time
import pickle

from bitfrost.update import microformat

import ooblib

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
            grpurlcache = os.path.join(cache, os.path.basename(baseurl)
                                              + '-' + suffix + ".html")
        else:
            grpurl = baseurl
            grpurlcache = os.path.join(cache, os.path.basename(baseurl)
                                              + ".html")

        if ooblib.cacheonly:
            print >>sys.stderr, "Trying group URL cache file", grpurlcache
            if os.path.exists(grpurlcache):
                name, desc, results = pickle.load(open(grpurlcache))
            else:
                continue
        else:
            print >>sys.stderr, "Trying group URL", grpurl
            try:
                name, desc, results = microformat.parse_url(grpurl)
            except urllib2.HTTPError, e:
                if e.code == 404:
                    continue
                raise e
            if len(results) == 0:
                continue
            print >>sys.stderr, "Found activity group:", name
            pickle.dump([name, desc, results], open(grpurlcache, 'w'))

        if results:
            break #process only the first URL (or cached file)

    if not results:
        print >>sys.stderr, "No Activity Group URL found"
        sys.exit(1)

    for name, info in results.items():
        (version, url) = microformat.only_best_update(info)
        print >>sys.stderr, "Examining %s v%s: %s" % (name, version, url)

        if ooblib.cacheonly:
            path = urlparse.urlsplit(url)[2]
            path = os.path.basename(path)

            localpath = os.path.join(cache, path)
            if os.path.exists(localpath):
                print >>sys.stderr, "Using: ", localpath
                ooblib.install_sugar_bundle(localpath)
                continue
            else:
                print >>sys.stderr, "Cannot find cache for ", url
                sys.exit(1)

        fd = None
        for attempts in range(5):
            if attempts > 0:
                print >>sys.stderr, 'Retrying.'
                time.sleep(1)
            try:
                fd = urllib2.urlopen(url)
                break
            except urllib2.HTTPError, e:
                print >>sys.stderr, 'HTTP error: ', e.code
            except urllib2.URLError, e:
                print >>sys.stderr, 'Network or server error: ', e.reason

        if not fd:
            print >>sys.stderr, 'Could not reach ', url
            sys.exit(1)

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
                ooblib.install_sugar_bundle(localpath)
                continue

        print >>sys.stderr, "Downloading (%dkB)..." % (length/1024)
        localfd = open(localpath, 'w')
        localfd.write(fd.read())
        fd.close()
        localfd.close()
        ooblib.install_sugar_bundle(localpath)

if systemwide:
    print "mkdir -p $INSTALL_ROOT/etc/olpc-update"
    print "echo '%s' > $INSTALL_ROOT/etc/olpc-update/activity-groups" % baseurl
else:
    print "echo '%s' > $INSTALL_ROOT/home/olpc/Activities/.groups" % baseurl

