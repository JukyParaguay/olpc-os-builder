#!/usr/bin/python
#
# Copyright (C) 2009, One Laptop per Child
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
#

# canonical source of version number
VERSION="3.0.0_pre2"

# "make install" modifies this in the installed copy
INSTALLED=0

import sys
import os
import os.path
from glob import glob
from ConfigParser import SafeConfigParser
from StringIO import StringIO
import subprocess
import shutil
import re
import time
from optparse import OptionParser

class StageException(Exception):
    def __init__(self, module, part, code):
        self.module = module
        self.part = part
        self.code = code

class Stage(object):
    def __init__(self, osb, name, console_output=True, ignore_failures=False):
        self.console_output = console_output
        self.name = name
        self.osb = osb
        self.ignore_failures = ignore_failures
        pass

    def _make_environment(self):
        env = {}

        # copy some bits from parent environment
        for key in ('HOSTNAME', 'USER', 'USERNAME', 'LANG', 'HOME', 'LOGNAME',
                    'SHELL', 'PATH'):
            if key in os.environ:
                env[key] = os.environ[key]

        env['PYTHONPATH'] = self.osb.libdir
        env['OOB__shlib'] = os.path.join(self.osb.libdir, 'shlib.sh')
        env['OOB__libdir'] = self.osb.libdir
        env['OOB__bindir'] = self.osb.bindir
        env['OOB__builddir'] = self.osb.builddir
        env['OOB__cachedir'] = self.osb.cachedir
        env['OOB__intermediatesdir'] = self.osb.intermediatesdir
        env['OOB__outputdir'] = self.osb.outputdir
        env['OOB__statedir'] = self.osb.statedir
        env['OOB__fsmount'] = self.osb.fsmount

        env['oob_config_dir'] = os.path.dirname(self.osb.build_config)

        envpath = env['PATH'].split(':')
        for dir in ('/sbin', '/usr/sbin'):
            if envpath.count(dir) == 0:
                env['PATH'] = env['PATH'] + ':' + dir

        for section in self.osb.cfg.sections():
            for option in self.osb.cfg.options(section):
                val = self.osb.cfg.get(section, option)
                env["CFG_%s__%s" % (section, option)] = val
        return env

    def _run_part(self, mod, part, output):
        print " * Running part %s %s %s..." % (self.name, mod, part)
        self.on_run_part(mod, part, output)

        if self.console_output:
            outtype = None
        else:
            outtype = subprocess.PIPE

        path = os.path.join(self.osb.moddir, mod, part)
        if path.endswith(".inc"):
            fd = open(path)
            for line in fd:
                output.write(line)
        elif path.endswith(".sh"):
            shenv = self._make_environment()
            proc = subprocess.Popen(["/bin/bash", path], shell=False,
                                    stdout=outtype, env=shenv)
            try:
                (out, err) = proc.communicate()
            except (Exception, KeyboardInterrupt), e:
                proc.terminate()
                raise StageException(mod, part, repr(e))

            if not self.ignore_failures and proc.returncode != 0:
                raise StageException(mod, part, proc.returncode)
            if not self.console_output:
                output.write(out)
        elif path.endswith(".py"):
            shenv = self._make_environment()
            proc = subprocess.Popen(["/usr/bin/python", path], shell=False,
                                    stdout=outtype, env=shenv)
            try:
                (out, err) = proc.communicate()
            except (Exception, KeyboardInterrupt), e:
                proc.terminate()
                raise StageException(mod, part, repr(e))

            if not self.ignore_failures and proc.returncode != 0:
                raise StageException(mod, part, proc.returncode)
            if not self.console_output:
                output.write(out)

        self.on_run_part_done(mod, part, output)

    def run(self):
        output = StringIO()
        self.on_run(output)

        # find all parts to execute for this stage
        partlist = []
        for mod in self.osb.modules:
            for ext in ('py', 'sh', 'inc'):
                matches = glob('%s/%s/%s.[0-9][0-9].*.%s' \
                               % (self.osb.moddir, mod, self.name, ext))
                partlist.extend(matches)

        # sort them
        parts = {}
        for part in partlist:
            bname = os.path.basename(part)
            mod = os.path.basename(os.path.dirname(part))
            parts[bname + "//" + mod] = os.path.join(mod, bname)
        items = parts.keys()
        items.sort()

        # execute in order
        for key in items:
            part = parts[key]
            self._run_part(os.path.dirname(part), os.path.basename(part),
                           output)

        self.on_run_done(output)

    # hooks for subclasses
    def on_run(self, output): pass
    def on_run_done(self, output): pass
    def on_run_part(self, mod, part, output): pass
    def on_run_part_done(self, mod, part, output): pass

class KsStage(Stage):
    def __init__(self, osb, name):
        super(KsStage, self).__init__(osb, name, console_output=False)

    def on_run_done(self, output):
        ksfd = open(self.osb.get_ks_file_path(), 'a')
        ksfd.write(output.getvalue())
        ksfd.close()

class PrepareStage(Stage):
    def __init__(self, osb):
        super(PrepareStage, self).__init__(osb, "prepare")

    def on_run_part(self, mod, part, output):
        print >>output, "\n# Processing %s:%s/%s\n" % (self.name, mod, part)

class KsmainStage(KsStage):
    def __init__(self, osb):
        super(KsmainStage, self).__init__(osb, "ksmain")

    def on_run_part(self, mod, part, output):
        print >>output, "\n# Processing %s:%s/%s\n" % (self.name, mod, part)

class KspkglistStage(KsStage):
    def __init__(self, osb):
        super(KspkglistStage, self).__init__(osb, "kspkglist")

    def on_run(self, output):
        print >>output, "\n\n\n%packages --excludedocs",
        if self.osb.cfg.has_option("global", "langs"):
            langs = self.osb.cfg.get("global", "langs").replace(",", ":")
            print >>output, " --instLangs %s" % langs,
        print >>output

    def on_run_done(self, output):
        print >>output, "%end"
        super(KspkglistStage, self).on_run_done(output)

    def on_run_part(self, mod, part, output):
        print >>output, "\n# Processing %s:%s/%s\n" % (self.name, mod, part)

class KspostStage(KsStage):
    def __init__(self, osb):
        super(KspostStage, self).__init__(osb, "kspost")

    def on_run_part(self, mod, part, output):
        output.write("\n%post --erroronfail")
        if ".nochroot." in part:
            output.write(" --nochroot")
        print >>output, "\necho 'Executing code generated by %s:%s/%s...'" \
            % (self.name, mod, part)
        print >>output, "set -e"

    def on_run_part_done(self, mod, part, output):
        print >>output, "%end"

class BuildStage(Stage):
    def __init__(self, osb):
        super(BuildStage, self).__init__(osb, "build")

class MountFSStage(Stage):
    def __init__(self, osb):
        super(MountFSStage, self).__init__(osb, "mountfs")

class PreImageStage(Stage):
    def __init__(self, osb):
        super(PreImageStage, self).__init__(osb, "preimage")

class ImageStage(Stage):
    def __init__(self, osb):
        super(ImageStage, self).__init__(osb, "image")

class PostImageStage(Stage):
    def __init__(self, osb):
        super(PostImageStage, self).__init__(osb, "postimage")

class UnmountFSStage(Stage):
    def __init__(self, osb):
        super(UnmountFSStage, self).__init__(osb, "unmountfs")

class FinalizeStage(Stage):
    def __init__(self, osb):
        super(FinalizeStage, self).__init__(osb, "finalize")

class CleanupStage(Stage):
    def __init__(self, osb):
        super(CleanupStage, self).__init__(osb, "cleanup", ignore_failures=True)

class OsBuilderException(Exception):
    pass

class OsBuilder(object):
    def __init__(self, build_config, additional_defaults):
        self.build_config = os.path.abspath(build_config)
        self.additional_defaults = additional_defaults

        print " * OLPC OS builder v%s" % VERSION
        if INSTALLED:
            self.moddir = "/usr/share/olpc-os-builder/modules.d"
            self.libdir = "/usr/share/olpc-os-builder/lib"
            self.bindir = "/usr/libexec/olpc-os-builder"
            self.builddir = "/var/tmp/olpc-os-builder"
            self.cachedir = "/var/cache/olpc-os-builder"
        else:
            self.moddir = os.path.join(sys.path[0], 'modules')
            self.libdir = os.path.join(sys.path[0], 'lib')
            self.bindir = os.path.join(sys.path[0], 'bin')
            self.builddir = os.path.join(sys.path[0], 'build')
            self.cachedir = os.path.join(self.builddir, 'cache')

        self.intermediatesdir = os.path.join(self.builddir, 'intermediates')
        self.outputdir = os.path.join(self.builddir, 'output')
        self.statedir = os.path.join(self.builddir, 'state')
        self.fsmount = os.path.join(self.builddir, 'mnt-fs')

        # load config to find module list
        # and set interpolation default for oob_config_dir
        self.cfg = SafeConfigParser({'oob_config_dir':
                                     os.path.dirname(self.build_config)})
        self.cfg.read(self.build_config)

        # read in defaults specified on the command line
        if self.additional_defaults is not None:
            self.cfg.read(self.additional_defaults)

        if self.cfg.has_option('global', 'suggested_oob_version'):
            suggested = self.cfg.get('global','suggested_oob_version')
            if suggested != VERSION:
                print
                print "WARNING: The build configuration you are using suggests that"
                print "olpc-os-builder version v%s should be used." % suggested
                print
                print "You are using v%s" % VERSION
                print
                print "Proceeding may result in a build that differs from what was intended by the "
                print "provided configuration."
                print
                print "Press Ctrl+C to abort. Continuing in 15 seconds."
                time.sleep(15)

        self.modules = []
        for option in self.cfg.options('global'):
            if not option.startswith('modules_') and option != "modules":
                continue
            self.modules.extend(self.cfg.get('global', option).split(','))

        # clean up
        for idx, mod in enumerate(self.modules):
            self.modules[idx] = mod.strip()

        # remove duplicates
        self.modules = list(set(self.modules))

        self.read_config()

    def get_ks_file_path(self):
        return os.path.join(self.intermediatesdir, 'build.ks')

    def read_config(self):
        """Read and validate config (including module defaults)"""
        # reset config since we want to load the module defaults first
        self.cfg = SafeConfigParser({'oob_config_dir':
                                     os.path.dirname(self.build_config)})

        for mod in self.modules:
            m = re.match(r"[A-Za-z_][A-Za-z0-9_]*$", mod)
            if not m:
                raise OsBuilderException("Invalid module name in config: %s" \
                                         % mod)

            modpath = os.path.join(self.moddir, mod)
            if not os.path.isdir(modpath):
                raise OsBuilderException("Module %s doesn't exist" % mod)

            # read in defaults
            self.cfg.read(os.path.join(modpath, 'defaults.ini'))

        # read in defaults specified on the command line
        if self.additional_defaults is not None:
            self.cfg.read(self.additional_defaults)

        # now load the users config, overriding other settings where specified
        self.cfg.read(self.build_config)

        for section in self.cfg.sections():
            m = re.match(r"[A-Za-z_][A-Za-z0-9_]*$", section)
            if not m:
                raise OsBuilderException("Invalid section in config: %s"
                                         % section)
            for option in self.cfg.options(section):
                m = re.match(r"[A-Za-z_][A-Za-z0-9_]*$", option)
                if not m:
                    raise OsBuilderException("Invalid option in config: %s.%s"
                                             % (section, option))

    stages = (
        PrepareStage,
        KsmainStage,
        KspkglistStage,
        KspostStage,
        BuildStage,
        MountFSStage,
        PreImageStage,
        ImageStage,
        PostImageStage,
        UnmountFSStage,
        FinalizeStage,
        # cleanup stage not listed here as its a bit of a special case
    )

    def build(self, clean_output=True, clean_intermediates=True):
        # cleanup from previous runs
        if clean_intermediates and os.path.exists(self.intermediatesdir):
            shutil.rmtree(self.intermediatesdir)
        if clean_output and os.path.exists(self.outputdir):
            shutil.rmtree(self.outputdir)

        for dir in (self.builddir, self.cachedir, self.intermediatesdir,
                    self.outputdir, self.statedir, self.fsmount):
            if not os.path.exists(dir):
                os.makedirs(dir)

        # truncate file and write header
        ksfd = open(self.get_ks_file_path(), 'w')
        ksfd.write("# Generated with OLPC OS builder\n")
        ksfd.close()

        for stage in self.stages:
            try:
                stage(self).run()
            except StageException, ex:
                print " * Caught error, cleanup and then bail out."
                try:
                    CleanupStage(self).run()
                except:
                    pass
                raise OsBuilderException("Failure in %s: module %s, part %s, error code %s" % (stage.__name__, ex.module, ex.part, ex.code))

        # cleanup
        CleanupStage(self).run()
        if clean_intermediates:
            shutil.rmtree(self.intermediatesdir)

        print " * Build completed successfully."
        print " * Output is in", self.outputdir

def main():
    op = OptionParser(usage="%prog [options] buildconfig", version=VERSION)
    op.add_option('--no-clean-output', dest="clean_output",
                  action="store_false", default=True,
                  help="Don't clean output directory on startup")
    op.add_option('--no-clean-intermediates', dest="clean_intermediates",
                  action="store_false", default=True,
                  help="Don't clean intermediates directory on startup or exit")
    op.add_option('--additional-defaults', dest="additional_defaults",
                  action="store", default=None,
                  help="Additional config file with default settings")
    (options, args) = op.parse_args()

    if len(args) != 1:
        op.error("incorrect number of arguments")

    try:
        osb = OsBuilder(args[0], options.additional_defaults)
        osb.build(clean_output=options.clean_output,
                  clean_intermediates=options.clean_intermediates)
    except OsBuilderException, e:
        print >>sys.stderr, "ERROR:", e
        sys.exit(1)

if __name__ == "__main__":
	main()

