# NOTE: Order matters - this script must run right after replace_images
#       and before custom_scripts (currently at 80)
import ooblib
from pipes import quote

welcome_screen_cmd=ooblib.read_config('sugar_welcome_activity', 'welcome_screen_cmd')
images_path=ooblib.read_config('sugar_welcome_activity', 'images_path')

if welcome_screen_cmd:
    print 'mkdir -p /home/olpc'
    print 'echo %s > /home/olpc/.welcome_screen' % quote(welcome_screen_cmd)
    print 'chown olpc:olpc /home/olpc/.welcome_screen'
    print 'chmod u+w /home/olpc/.welcome_screen'

if images_path:
    print 'chown -R olpc:olpc /home/olpc/Activities/Welcome.activity/images/'
