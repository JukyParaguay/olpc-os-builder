# NOTE: Order matters - this script must run after activity unpacking (currently 75)
#       and before welcome_cmd
import ooblib
import os, sys
from pipes import quote

images_path=ooblib.read_config('sugar_welcome_activity', 'images_path')

if images_path:
    if not os.path.exists(images_path):
	print >> sys.stderr, "ERROR: sugar_welcome_activity.images_path must point to an existing directory in your build environment"
	sys.exit(1)

    # synchronize the files within the path, keeping directory structure
    print 'rsync -rlpt %s/ "$INSTALL_ROOT/home/olpc/Activities/Welcome.activity/images/"' % quote(images_path)
    # note - chown happens in kspost.80.wecome_cmd as it reads /etc/passwd
    print 'chmod -R u+rwx "$INSTALL_ROOT/home/olpc/Activities/Welcome.activity/images/"'

