# Copyright (C) 2009 One Laptop Per Child
# Licensed under the terms of the GNU GPL v2 or later; see COPYING for details.
import logging
import os
import sys
from time import gmtime, strftime

import imgcreate
import ooblib

cache_dir = os.path.join(ooblib.cachedir, 'imgcreate')

def main():
    if not os.path.exists(cache_dir):
        os.mkdir(cache_dir)

    logging.getLogger().setLevel(logging.INFO)

    name = ooblib.image_name()
    kspath = os.path.join(ooblib.intermediatesdir, 'build.ks')
    ks = imgcreate.read_kickstart(kspath)

    make_iso = ooblib.read_config_bool('base', 'make_iso')
    if make_iso:
        print "Building ISO image..."
        creator = imgcreate.LiveImageCreator(ks, name, name,
                                             tmpdir=ooblib.builddir,
                                             cacheonly=ooblib.cacheonly)
        compress = ooblib.read_config_bool('base', 'compress_iso')
        if compress is None:
            compress = False
        creator.skip_compression = not compress
    else:
        print "Building directly into FS image..."
        creator = imgcreate.LoopImageCreator(ks, 'imgcreatefs', name,
                                             tmpdir=ooblib.builddir,
                                             cacheonly=ooblib.cacheonly)

    try:
        creator.mount(cachedir=cache_dir)
        creator.install()
        creator.configure()
        creator.unmount()
        if make_iso:
            creator.package(destdir=ooblib.outputdir)
        else:
            creator.package(destdir=ooblib.intermediatesdir)
    except imgcreate.CreatorError, e:
        logging.error("Error creating Live CD : %s" % e)
        return 1
    finally:
        creator.cleanup()

    return 0

if __name__ == "__main__":
    sys.exit(main())

