# -*- Mode: Python -*-

import sv3utils as u

print("Use 'scons -h' to show build help.")

Help("""
Usage: scons [force32=0/1] [cxx=COMPILER] [cpu=CPU]

force32=0/1   Force 32-bit build, if force32=1. Default is 0.
debug=0/1     Build a debug version, if debug=1. Default is 0.
cxx=COMPILER  Force build to use a specific C++ compiler.
cpu=CPU       Optimize for the given CPU. Passed to -march.
lto=0/1       Enable link-time optimization. Default is 0.
asserts=1/0   Enable assertions at runtime. Default is 1.
qemusrc=dir   Source directory of patched qemu. Default is ../qemu.
""")



host_env = Environment(CCFLAGS = ['-pthread'],
                       CXXFLAGS = [],
                       LINKFLAGS = ['-pthread'],
                       CPPPATH = ['#include'])

if 'cxx' in ARGUMENTS:
    print("Forcing host C++ compiler to %s." % ARGUMENTS['cxx'])
    host_env['CXX'] = ARGUMENTS['cxx']

debug_enabled   = (int(ARGUMENTS.get('debug', 1)) == 1)
lto_enabled     = (int(ARGUMENTS.get('lto', 0)) == 1)
asserts_enabled = (int(ARGUMENTS.get('asserts', 1)) == 1)

optflags = ['-g']
if debug_enabled:
    optflags += ['-O0']
else:
    optflags += ['-O3']

if int(ARGUMENTS.get('force32', 0)):
    optflags += ['-m32']

if lto_enabled:
    optflags += ['-flto']

if not asserts_enabled:
    host_env.Append(CPPFLAGS = ['-DNDEBUG'])

if not asserts_enabled and lto_enabled and not debug_enabled:
    host_env.Append(CPPFLAGS = ['-DSV3_BENCHMARK_OK'])

qemusrc = ARGUMENTS.get('qemusrc', 'contrib/qemu')
host_env.Append(CPPPATH = [qemusrc + "/include"])

host_env.Append(CCFLAGS = optflags, LINKFLAGS = optflags)

conf = Configure(host_env, custom_tests = { 'AddOptionalFlag' : u.AddOptionalFlag ,
                                            'CheckPreprocessorMacro' : u.CheckPreprocessorMacro,
                                            'CheckPKGConfig' : u.CheckPKGConfig,
                                            'CheckPKG' : u.CheckPKG,
                                            })
if not conf.CheckPKGConfig('0.15.0'):
    print('pkg-config >= 0.15.0 not found.')
    Exit(1)

if not debug_enabled:
    print("Enabling inline versions of Userspace RCU functions.")
    conf.env.Append(CPPFLAGS = ['-D_LGPL_SOURCE']) # to get the static definitions in liburcu



if not conf.CheckPKG('liburcu-qsbr'):
    print("Could not find userspace-rcu library via pkg-config. Debian system?")
    if not conf.CheckLibWithHeader('urcu-qsbr', 'urcu-qsbr.h', 'c'):
        Exit(1)
else:
    conf.env.ParseConfig('pkg-config --cflags --libs liburcu-qsbr')

if not conf.CheckPreprocessorMacro('linux/if_tun.h', 'TUNGETVNETHDRSZ'):
    conf.env.Append(CPPFLAGS = ['-DNO_GETVNETHDRSZ'])

if not conf.CheckType('struct virtio_net_hdr', '#include <pci/types.h>\n#include <linux/virtio_net.h>\n'):
    print("Your Linux headers are too old.")
    Exit(1)

if not conf.AddOptionalFlag('.cc', 'CXXFLAGS', '-std=c++11') and not conf.AddOptionalFlag('.cc', 'CXXFLAGS', '-std=c++0x'):
    print("Your compiler is too old.")
    Exit(1)

if not conf.CheckCXXHeader('list'):
    print("C++ STL seems broken.")
    Exit(1)

if not conf.CheckHeader('hw/misc/externalpci.h'):
    print("Pass proper qemusrc=... parameter. Could not find patched qemu at " + qemusrc)
    Exit(1)

if not 'cpu' in ARGUMENTS:
    opt_cpu = 'native'
else:
    opt_cpu = ARGUMENTS['cpu']

conf.AddOptionalFlag('.c', 'CCFLAGS',   "-march=%s" % opt_cpu )
conf.AddOptionalFlag('.c', 'LINKFLAGS', "-march=%s" % opt_cpu )

conf.AddOptionalFlag('.c', 'CCFLAGS', '-Wall')
#conf.AddOptionalFlag('.c', 'CCFLAGS', '-Wextra')
host_env = conf.Finish()

host_pcap_env = host_env.Clone()
conf = Configure(host_pcap_env)
if not conf.CheckLibWithHeader('pcap', 'pcap.h', 'c'):
    print 'Could not find libpcap.'
    pcap_is_available = False
else:
    pcap_is_available = True
host_pcap_env = conf.Finish()

## Version info
AlwaysBuild(Command('version.inc', ['sv3.cc'], """git describe --dirty --always | sed 's/^\\(.*\\)$/"\\1"/' > $TARGET"""))

## Switch library

common_objs = [host_env.Object(f) for f in Glob('switch/*.cc')]

## Programs
ts = host_env.Program('sv3', ['sv3.cc'] + common_objs)
# Clean leftover core files as well
Clean(ts, Glob("core.*"))

# Tests

host_env.Program('test/checksums', ['test/checksums.cc'] + common_objs)
Command('test/checksums.log', ['test/checksums'], '! $SOURCE | tee $TARGET | grep -q FAILED')

host_env.Program('test/etherhash', ['test/etherhash.cc'] + common_objs)
host_env.Command('test/etherhash.log', ['test/etherhash' ], '$SOURCE | tee $TARGET')

if pcap_is_available:
    host_pcap_env.Program('test/packets', ['test/packets.cc'] + common_objs)
    Command('test/packets-ipv4-tcp.log', ['test/packets', 'test/data/ipv4-tcp.pcap' ], '! ${SOURCES[0]} ${SOURCES[1]} | tee $TARGET | grep -q wrong')
    Command('test/packets-ipv6-tcp.log', ['test/packets', 'test/data/ipv6-tcp.pcap' ], '! ${SOURCES[0]} ${SOURCES[1]} | tee $TARGET | grep -q wrong')
else:
    print("Not building test/packets! Not running tests!")

# EOF
