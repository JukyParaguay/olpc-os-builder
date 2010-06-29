# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.

from __future__ import with_statement
from __future__ import division

import sys
import os.path
import sys
import urllib
import urllib2

from HTMLParser import HTMLParser, HTMLParseError
import urlparse

import ooblib

# copied from 
_DEBUG_PARSER = False
class _UpdateHTMLParser(HTMLParser):
    """HTML parser to pull out data expressed in our microformat."""
    def _unrelative(self, url):
        if self.base_href is None: return url # no worries.
        return urlparse.urljoin(self.base_href, url)
    def __init__(self, base_href=None, try_hard=False):
        """The `base_href` parameter helps resolve relative URLS; the
        `try_hard` parameter tries to make up for deficient activity
        records with additional network queries."""
        HTMLParser.__init__(self)
        self.base_href = base_href
        self.try_hard = try_hard
        self.results = {}
        self.group_name = self.group_desc = None
        self.in_group_name = self.in_group_desc = self.in_activity = 0
        self._clear_info()
    def _clear_info(self):
        self.in_activity_id=self.in_activity_url=self.in_activity_version = 0
        self.last_id = self.last_version = None
        self.last_urls = []
    def handle_starttag(self, tag, attrs):
        classes = ' '.join([val for attr, val in attrs if attr=='class'])\
                  .split()
        if self.in_group_name == 0:
            if ('id','olpc-activity-group-name') in attrs:
                self.in_group_name = 1
        else:
            self.in_group_name += 1
        if self.in_group_desc == 0:
            if ('id','olpc-activity-group-desc') in attrs:
                self.in_group_desc = 1
        else:
            self.in_group_desc += 1
        if self.in_activity == 0:
            if 'olpc-activity-info' in classes:
                self.in_activity = 1
                self._clear_info()
            return
        self.in_activity += 1    
        if self.in_activity_id == 0:
            if 'olpc-activity-id' in classes:
                self.in_activity_id = 1
        else:
            self.in_activity_id += 1
        if self.in_activity_version == 0:
            if 'olpc-activity-version' in classes:
                self.in_activity_version = 1
        else:
            self.in_activity_version += 1
        if self.in_activity_url == 0:
            if 'olpc-activity-url' in classes:
                self.in_activity_url = 1
        else:
            self.in_activity_url += 1
        # an href inside activity_url is the droid we are looking for.
        if self.in_activity_url > 0:
            self.last_urls += [self._unrelative(v) for
                               a,v in attrs if a=='href']
    def handle_data(self, data):
        if self.in_group_name:
            self.group_name = data.strip()
        if self.in_group_desc:
            self.group_desc = data.strip()
        if self.in_activity_id > 0:
            self.last_id = data.strip()
        if self.in_activity_version > 0:
            try:
                self.last_version = long(data.strip())
            except:
                if _DEBUG_PARSER:
                    print "BAD VERSION NUMBER:", self.last_id, data
    def handle_endtag(self, tag):
        if self.in_group_name > 0:
            self.in_group_name -= 1
        if self.in_group_desc > 0:
            self.in_group_desc -= 1
        if self.in_activity_id > 0:
            self.in_activity_id -= 1
        if self.in_activity_version > 0:
            self.in_activity_version -= 1
        if self.in_activity_url > 0:
            self.in_activity_url -= 1
        if self.in_activity > 0:
            self.in_activity -= 1
            if self.in_activity == 0:
                if _DEBUG_PARSER:
                    assert self.in_activity_id == 0
                    assert self.in_activity_version == 0
                    assert self.in_activity_url == 0
                if self.last_id is not None and self.last_id.strip() == '':
                    self.last_id = None
                if len(self.last_urls)>0 and (self.last_id is None or
                                              self.last_version is None):
                    # grumble.  look inside the URL to get the id/version
                    if self.try_hard:
                        from actutils import id_and_version_from_url
                        self.last_id, self.last_version = \
                                      id_and_version_from_url(self.last_urls[0])
                if self.last_version is not None and self.last_id is not None\
                   and len(self.last_urls)>0:
                    rl = self.results.get(self.last_id, [])
                    for url in self.last_urls:
                        if (self.last_version, url) not in rl:
                            rl.append((self.last_version, url))
                    self.results[self.last_id] = rl


def only_best_update(version_list):
    """Take a list of version, url pairs (like those returned from
    `parse_html` and return the "best" one."""
    bestv, bestu = None, None
    for ver, url in version_list:
        if bestv is None or ver > bestv:
            bestv, bestu = ver, url
    return bestv, bestu

def parse_html(html_data, base_href=None):
    """Parse the activity information embedded in the given string
    containing HTML data.  Returns a triple: the activity short name,
    if present (else `None`), then the activity description if present
    (else `None`), and finally a mapping from activity ids to a list
    of (version, url) pairs.

    Raises `HTMLParseError` if `html_data` can't be parsed as valid HTML.
    """
    hp = _UpdateHTMLParser(base_href)
    hp.feed(html_data)
    hp.close()
    return hp.group_name, hp.group_desc, hp.results

def parse_url(url, **urlopen_args):
    """Parse the activity information at the given URL. Returns the same
    information as `parse_html` does, and raises the same exceptions.
    The `urlopen_args` can be any keyword arguments accepted by
    `bitfrost.util.urlrange.urlopen`."""
    import bitfrost.util.urlrange as urlrange
    with urlrange.urlopen(url, **urlopen_args) as f:
        return parse_html(f.read(), f.url)

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
            name, desc, results = parse_url(grpurl)
        except urllib2.HTTPError, e:
            if e.code == 404:
                continue
            raise e
        if len(results) == 0 or (name is None and desc is None):
            continue
        print >>sys.stderr, "Found activity group:", name

        for name, info in results.items():
            (version, url) = only_best_update(info)
            print >>sys.stderr, "Examining", name, "v%d" % version
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

