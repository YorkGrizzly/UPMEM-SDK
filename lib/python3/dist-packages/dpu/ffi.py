# Copyright 2020 UPMEM. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

r"""Wrapper for dpu.h

Do not modify this file.
"""

import glob
import re
import os.path
import ctypes.util
import ctypes
import platform
__docformat__ = "restructuredtext"

# Begin preamble for Python v(3, 2)

import ctypes
import os
import sys
from ctypes import *

_int_types = (c_int16, c_int32)
if hasattr(ctypes, "c_int64"):
    # Some builds of ctypes apparently do not have c_int64
    # defined; it's a pretty good bet that these builds do not
    # have 64-bit pointers.
    _int_types += (c_int64,)
for t in _int_types:
    if sizeof(t) == sizeof(c_size_t):
        c_ptrdiff_t = t
del t
del _int_types


class UserString:
    def __init__(self, seq):
        if isinstance(seq, bytes):
            self.data = seq
        elif isinstance(seq, UserString):
            self.data = seq.data[:]
        else:
            self.data = str(seq).encode()

    def __bytes__(self):
        return self.data

    def __str__(self):
        return self.data.decode()

    def __repr__(self):
        return repr(self.data)

    def __int__(self):
        return int(self.data.decode())

    def __long__(self):
        return int(self.data.decode())

    def __float__(self):
        return float(self.data.decode())

    def __complex__(self):
        return complex(self.data.decode())

    def __hash__(self):
        return hash(self.data)

    def __cmp__(self, string):
        if isinstance(string, UserString):
            return cmp(self.data, string.data)
        else:
            return cmp(self.data, string)

    def __le__(self, string):
        if isinstance(string, UserString):
            return self.data <= string.data
        else:
            return self.data <= string

    def __lt__(self, string):
        if isinstance(string, UserString):
            return self.data < string.data
        else:
            return self.data < string

    def __ge__(self, string):
        if isinstance(string, UserString):
            return self.data >= string.data
        else:
            return self.data >= string

    def __gt__(self, string):
        if isinstance(string, UserString):
            return self.data > string.data
        else:
            return self.data > string

    def __eq__(self, string):
        if isinstance(string, UserString):
            return self.data == string.data
        else:
            return self.data == string

    def __ne__(self, string):
        if isinstance(string, UserString):
            return self.data != string.data
        else:
            return self.data != string

    def __contains__(self, char):
        return char in self.data

    def __len__(self):
        return len(self.data)

    def __getitem__(self, index):
        return self.__class__(self.data[index])

    def __getslice__(self, start, end):
        start = max(start, 0)
        end = max(end, 0)
        return self.__class__(self.data[start:end])

    def __add__(self, other):
        if isinstance(other, UserString):
            return self.__class__(self.data + other.data)
        elif isinstance(other, bytes):
            return self.__class__(self.data + other)
        else:
            return self.__class__(self.data + str(other).encode())

    def __radd__(self, other):
        if isinstance(other, bytes):
            return self.__class__(other + self.data)
        else:
            return self.__class__(str(other).encode() + self.data)

    def __mul__(self, n):
        return self.__class__(self.data * n)

    __rmul__ = __mul__

    def __mod__(self, args):
        return self.__class__(self.data % args)

    # the following methods are defined in alphabetical order:
    def capitalize(self):
        return self.__class__(self.data.capitalize())

    def center(self, width, *args):
        return self.__class__(self.data.center(width, *args))

    def count(self, sub, start=0, end=sys.maxsize):
        return self.data.count(sub, start, end)

    def decode(self, encoding=None, errors=None):  # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.decode(encoding, errors))
            else:
                return self.__class__(self.data.decode(encoding))
        else:
            return self.__class__(self.data.decode())

    def encode(self, encoding=None, errors=None):  # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.encode(encoding, errors))
            else:
                return self.__class__(self.data.encode(encoding))
        else:
            return self.__class__(self.data.encode())

    def endswith(self, suffix, start=0, end=sys.maxsize):
        return self.data.endswith(suffix, start, end)

    def expandtabs(self, tabsize=8):
        return self.__class__(self.data.expandtabs(tabsize))

    def find(self, sub, start=0, end=sys.maxsize):
        return self.data.find(sub, start, end)

    def index(self, sub, start=0, end=sys.maxsize):
        return self.data.index(sub, start, end)

    def isalpha(self):
        return self.data.isalpha()

    def isalnum(self):
        return self.data.isalnum()

    def isdecimal(self):
        return self.data.isdecimal()

    def isdigit(self):
        return self.data.isdigit()

    def islower(self):
        return self.data.islower()

    def isnumeric(self):
        return self.data.isnumeric()

    def isspace(self):
        return self.data.isspace()

    def istitle(self):
        return self.data.istitle()

    def isupper(self):
        return self.data.isupper()

    def join(self, seq):
        return self.data.join(seq)

    def ljust(self, width, *args):
        return self.__class__(self.data.ljust(width, *args))

    def lower(self):
        return self.__class__(self.data.lower())

    def lstrip(self, chars=None):
        return self.__class__(self.data.lstrip(chars))

    def partition(self, sep):
        return self.data.partition(sep)

    def replace(self, old, new, maxsplit=-1):
        return self.__class__(self.data.replace(old, new, maxsplit))

    def rfind(self, sub, start=0, end=sys.maxsize):
        return self.data.rfind(sub, start, end)

    def rindex(self, sub, start=0, end=sys.maxsize):
        return self.data.rindex(sub, start, end)

    def rjust(self, width, *args):
        return self.__class__(self.data.rjust(width, *args))

    def rpartition(self, sep):
        return self.data.rpartition(sep)

    def rstrip(self, chars=None):
        return self.__class__(self.data.rstrip(chars))

    def split(self, sep=None, maxsplit=-1):
        return self.data.split(sep, maxsplit)

    def rsplit(self, sep=None, maxsplit=-1):
        return self.data.rsplit(sep, maxsplit)

    def splitlines(self, keepends=0):
        return self.data.splitlines(keepends)

    def startswith(self, prefix, start=0, end=sys.maxsize):
        return self.data.startswith(prefix, start, end)

    def strip(self, chars=None):
        return self.__class__(self.data.strip(chars))

    def swapcase(self):
        return self.__class__(self.data.swapcase())

    def title(self):
        return self.__class__(self.data.title())

    def translate(self, *args):
        return self.__class__(self.data.translate(*args))

    def upper(self):
        return self.__class__(self.data.upper())

    def zfill(self, width):
        return self.__class__(self.data.zfill(width))


class MutableString(UserString):
    """mutable string objects

    Python strings are immutable objects.  This has the advantage, that
    strings may be used as dictionary keys.  If this property isn't needed
    and you insist on changing string values in place instead, you may cheat
    and use MutableString.

    But the purpose of this class is an educational one: to prevent
    people from inventing their own mutable string class derived
    from UserString and than forget thereby to remove (override) the
    __hash__ method inherited from UserString.  This would lead to
    errors that would be very hard to track down.

    A faster and better solution is to rewrite your program using lists."""

    def __init__(self, string=""):
        self.data = string

    def __hash__(self):
        raise TypeError("unhashable type (it is mutable)")

    def __setitem__(self, index, sub):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data):
            raise IndexError
        self.data = self.data[:index] + sub + self.data[index + 1:]

    def __delitem__(self, index):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data):
            raise IndexError
        self.data = self.data[:index] + self.data[index + 1:]

    def __setslice__(self, start, end, sub):
        start = max(start, 0)
        end = max(end, 0)
        if isinstance(sub, UserString):
            self.data = self.data[:start] + sub.data + self.data[end:]
        elif isinstance(sub, bytes):
            self.data = self.data[:start] + sub + self.data[end:]
        else:
            self.data = self.data[:start] + str(sub).encode() + self.data[end:]

    def __delslice__(self, start, end):
        start = max(start, 0)
        end = max(end, 0)
        self.data = self.data[:start] + self.data[end:]

    def immutable(self):
        return UserString(self.data)

    def __iadd__(self, other):
        if isinstance(other, UserString):
            self.data += other.data
        elif isinstance(other, bytes):
            self.data += other
        else:
            self.data += str(other).encode()
        return self

    def __imul__(self, n):
        self.data *= n
        return self


class String(MutableString, Union):

    _fields_ = [("raw", POINTER(c_char)), ("data", c_char_p)]

    def __init__(self, obj=""):
        if isinstance(obj, (bytes, UserString)):
            self.data = bytes(obj)
        else:
            self.raw = obj

    def __len__(self):
        return self.data and len(self.data) or 0

    def from_param(cls, obj):
        # Convert None or 0
        if obj is None or obj == 0:
            return cls(POINTER(c_char)())

        # Convert from String
        elif isinstance(obj, String):
            return obj

        # Convert from bytes
        elif isinstance(obj, bytes):
            return cls(obj)

        # Convert from str
        elif isinstance(obj, str):
            return cls(obj.encode())

        # Convert from c_char_p
        elif isinstance(obj, c_char_p):
            return obj

        # Convert from POINTER(c_char)
        elif isinstance(obj, POINTER(c_char)):
            return obj

        # Convert from raw pointer
        elif isinstance(obj, int):
            return cls(cast(obj, POINTER(c_char)))

        # Convert from c_char array
        elif isinstance(obj, c_char * len(obj)):
            return obj

        # Convert from object
        else:
            return String.from_param(obj._as_parameter_)

    from_param = classmethod(from_param)


def ReturnString(obj, func=None, arguments=None):
    return String.from_param(obj)


# As of ctypes 1.0, ctypes does not support custom error-checking
# functions on callbacks, nor does it support custom datatypes on
# callbacks, so we must ensure that all callbacks return
# primitive datatypes.
#
# Non-primitive return values wrapped with UNCHECKED won't be
# typechecked, and will be converted to c_void_p.
def UNCHECKED(type):
    if hasattr(
            type,
            "_type_") and isinstance(
            type._type_,
            str) and type._type_ != "P":
        return type
    else:
        return c_void_p


# ctypes doesn't have direct support for variadic functions, so we have to write
# our own wrapper class
class _variadic_function(object):
    def __init__(self, func, restype, argtypes, errcheck):
        self.func = func
        self.func.restype = restype
        self.argtypes = argtypes
        if errcheck:
            self.func.errcheck = errcheck

    def _as_parameter_(self):
        # So we can pass this variadic function as a function pointer
        return self.func

    def __call__(self, *args):
        fixed_args = []
        i = 0
        for argtype in self.argtypes:
            # Typecheck what we can
            fixed_args.append(argtype.from_param(args[i]))
            i += 1
        return self.func(*fixed_args + list(args[i:]))


def ord_if_char(value):
    """
    Simple helper used for casts to simple builtin types:  if the argument is a
    string type, it will be converted to it's ordinal value.

    This function will raise an exception if the argument is string with more
    than one characters.
    """
    return ord(value) if (
        isinstance(
            value,
            bytes) or isinstance(
            value,
            str)) else value

# End preamble


_libs = {}
_libdirs = []

# Begin loader

# ----------------------------------------------------------------------------
# Copyright (c) 2008 David James
# Copyright (c) 2006-2008 Alex Holkner
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#  * Neither the name of pyglet nor the names of its
#    contributors may be used to endorse or promote products
#    derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------


def _environ_path(name):
    if name in os.environ:
        return os.environ[name].split(":")
    else:
        return []


class LibraryLoader(object):
    # library names formatted specifically for platforms
    name_formats = ["%s"]

    class Lookup(object):
        mode = ctypes.DEFAULT_MODE

        def __init__(self, path):
            super(LibraryLoader.Lookup, self).__init__()
            self.access = dict(cdecl=ctypes.CDLL(path, self.mode))

        def get(self, name, calling_convention="cdecl"):
            if calling_convention not in self.access:
                raise LookupError(
                    "Unknown calling convention '{}' for function '{}'".format(
                        calling_convention, name
                    )
                )
            return getattr(self.access[calling_convention], name)

        def has(self, name, calling_convention="cdecl"):
            if calling_convention not in self.access:
                return False
            return hasattr(self.access[calling_convention], name)

        def __getattr__(self, name):
            return getattr(self.access["cdecl"], name)

    def __init__(self):
        self.other_dirs = []

    def __call__(self, libname):
        """Given the name of a library, load it."""
        paths = self.getpaths(libname)

        for path in paths:
            try:
                return self.Lookup(path)
            except BaseException:
                pass

        raise ImportError("Could not load %s." % libname)

    def getpaths(self, libname):
        """Return a list of paths where the library might be found."""
        if os.path.isabs(libname):
            yield libname
        else:
            # search through a prioritized series of locations for the library

            # we first search any specific directories identified by user
            for dir_i in self.other_dirs:
                for fmt in self.name_formats:
                    # dir_i should be absolute already
                    yield os.path.join(dir_i, fmt % libname)

            # then we search the directory where the generated python interface
            # is stored
            for fmt in self.name_formats:
                yield os.path.abspath(os.path.join(os.path.dirname(__file__), fmt % libname))

            # now, use the ctypes tools to try to find the library
            for fmt in self.name_formats:
                path = ctypes.util.find_library(fmt % libname)
                if path:
                    yield path

            # then we search all paths identified as platform-specific lib
            # paths
            for path in self.getplatformpaths(libname):
                yield path

            # Finally, we'll try the users current working directory
            for fmt in self.name_formats:
                yield os.path.abspath(os.path.join(os.path.curdir, fmt % libname))

    def getplatformpaths(self, libname):
        return []


# Darwin (Mac OS X)


class DarwinLibraryLoader(LibraryLoader):
    name_formats = [
        "lib%s.dylib",
        "lib%s.so",
        "lib%s.bundle",
        "%s.dylib",
        "%s.so",
        "%s.bundle",
        "%s",
    ]

    class Lookup(LibraryLoader.Lookup):
        # Darwin requires dlopen to be called with mode RTLD_GLOBAL instead
        # of the default RTLD_LOCAL.  Without this, you end up with
        # libraries not being loadable, resulting in "Symbol not found"
        # errors
        mode = ctypes.RTLD_GLOBAL

    def getplatformpaths(self, libname):
        if os.path.pathsep in libname:
            names = [libname]
        else:
            names = [format % libname for format in self.name_formats]

        for dir in self.getdirs(libname):
            for name in names:
                yield os.path.join(dir, name)

    def getdirs(self, libname):
        """Implements the dylib search as specified in Apple documentation:

        http://developer.apple.com/documentation/DeveloperTools/Conceptual/
            DynamicLibraries/Articles/DynamicLibraryUsageGuidelines.html

        Before commencing the standard search, the method first checks
        the bundle's ``Frameworks`` directory if the application is running
        within a bundle (OS X .app).
        """

        dyld_fallback_library_path = _environ_path(
            "DYLD_FALLBACK_LIBRARY_PATH")
        if not dyld_fallback_library_path:
            dyld_fallback_library_path = [
                os.path.expanduser("~/lib"), "/usr/local/lib", "/usr/lib"]

        dirs = []

        if "/" in libname:
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))
        else:
            dirs.extend(_environ_path("LD_LIBRARY_PATH"))
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))

        if hasattr(sys, "frozen") and sys.frozen == "macosx_app":
            dirs.append(
                os.path.join(
                    os.environ["RESOURCEPATH"],
                    "..",
                    "Frameworks"))

        dirs.extend(dyld_fallback_library_path)

        return dirs


# Posix


class PosixLibraryLoader(LibraryLoader):
    _ld_so_cache = None

    _include = re.compile(r"^\s*include\s+(?P<pattern>.*)")

    class _Directories(dict):
        def __init__(self):
            self.order = 0

        def add(self, directory):
            if len(directory) > 1:
                directory = directory.rstrip(os.path.sep)
            # only adds and updates order if exists and not already in set
            if not os.path.exists(directory):
                return
            o = self.setdefault(directory, self.order)
            if o == self.order:
                self.order += 1

        def extend(self, directories):
            for d in directories:
                self.add(d)

        def ordered(self):
            return (i[0] for i in sorted(self.items(), key=lambda D: D[1]))

    def _get_ld_so_conf_dirs(self, conf, dirs):
        """
        Recursive funtion to help parse all ld.so.conf files, including proper
        handling of the `include` directive.
        """

        try:
            with open(conf) as f:
                for D in f:
                    D = D.strip()
                    if not D:
                        continue

                    m = self._include.match(D)
                    if not m:
                        dirs.add(D)
                    else:
                        for D2 in glob.glob(m.group("pattern")):
                            self._get_ld_so_conf_dirs(D2, dirs)
        except IOError:
            pass

    def _create_ld_so_cache(self):
        # Recreate search path followed by ld.so.  This is going to be
        # slow to build, and incorrect (ld.so uses ld.so.cache, which may
        # not be up-to-date).  Used only as fallback for distros without
        # /sbin/ldconfig.
        #
        # We assume the DT_RPATH and DT_RUNPATH binary sections are omitted.

        directories = self._Directories()
        for name in (
            "LD_LIBRARY_PATH",
            "SHLIB_PATH",  # HPUX
            "LIBPATH",  # OS/2, AIX
            "LIBRARY_PATH",  # BE/OS
        ):
            if name in os.environ:
                directories.extend(os.environ[name].split(os.pathsep))

        self._get_ld_so_conf_dirs("/etc/ld.so.conf", directories)

        bitage = platform.architecture()[0]

        unix_lib_dirs_list = []
        if bitage.startswith("64"):
            # prefer 64 bit if that is our arch
            unix_lib_dirs_list += ["/lib64", "/usr/lib64"]

        # must include standard libs, since those paths are also used by 64 bit
        # installs
        unix_lib_dirs_list += ["/lib", "/usr/lib"]
        if sys.platform.startswith("linux"):
            # Try and support multiarch work in Ubuntu
            # https://wiki.ubuntu.com/MultiarchSpec
            if bitage.startswith("32"):
                # Assume Intel/AMD x86 compat
                unix_lib_dirs_list += ["/lib/i386-linux-gnu",
                                       "/usr/lib/i386-linux-gnu"]
            elif bitage.startswith("64"):
                # Assume Intel/AMD x86 compat
                unix_lib_dirs_list += ["/lib/x86_64-linux-gnu",
                                       "/usr/lib/x86_64-linux-gnu"]
            else:
                # guess...
                unix_lib_dirs_list += glob.glob("/lib/*linux-gnu")
        directories.extend(unix_lib_dirs_list)

        cache = {}
        lib_re = re.compile(r"lib(.*)\.s[ol]")
        ext_re = re.compile(r"\.s[ol]$")
        for dir in directories.ordered():
            try:
                for path in glob.glob("%s/*.s[ol]*" % dir):
                    file = os.path.basename(path)

                    # Index by filename
                    cache_i = cache.setdefault(file, set())
                    cache_i.add(path)

                    # Index by library name
                    match = lib_re.match(file)
                    if match:
                        library = match.group(1)
                        cache_i = cache.setdefault(library, set())
                        cache_i.add(path)
            except OSError:
                pass

        self._ld_so_cache = cache

    def getplatformpaths(self, libname):
        if self._ld_so_cache is None:
            self._create_ld_so_cache()

        result = self._ld_so_cache.get(libname, set())
        for i in result:
            # we iterate through all found paths for library, since we may have
            # actually found multiple architectures or other library types that
            # may not load
            yield i


# Windows


class WindowsLibraryLoader(LibraryLoader):
    name_formats = ["%s.dll", "lib%s.dll", "%slib.dll", "%s"]

    class Lookup(LibraryLoader.Lookup):
        def __init__(self, path):
            super(WindowsLibraryLoader.Lookup, self).__init__(path)
            self.access["stdcall"] = ctypes.windll.LoadLibrary(path)


# Platform switching

# If your value of sys.platform does not appear in this dict, please contact
# the Ctypesgen maintainers.

loaderclass = {
    "darwin": DarwinLibraryLoader,
    "cygwin": WindowsLibraryLoader,
    "win32": WindowsLibraryLoader,
    "msys": WindowsLibraryLoader,
}

load_library = loaderclass.get(sys.platform, PosixLibraryLoader)()


def add_library_search_dirs(other_dirs):
    """
    Add libraries to search paths.
    If library paths are relative, convert them to absolute with respect to this
    file's directory
    """
    for F in other_dirs:
        if not os.path.isabs(F):
            F = os.path.abspath(F)
        load_library.other_dirs.append(F)


del loaderclass

# End loader

add_library_search_dirs([os.path.dirname(os.path.dirname(
    os.path.dirname((os.path.dirname(__file__)))))])

# Begin libraries
_libs["libdpu.so"] = load_library("libdpu.so")

# 1 libraries
# End libraries

# No modules

__off_t = c_long  # /usr/include/x86_64-linux-gnu/bits/types.h: 150

__off64_t = c_long  # /usr/include/x86_64-linux-gnu/bits/types.h: 151

# /usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h: 49


class struct__IO_FILE(Structure):
    pass


FILE = struct__IO_FILE  # /usr/include/x86_64-linux-gnu/bits/types/FILE.h: 7

# /usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h: 36


class struct__IO_marker(Structure):
    pass

# /usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h: 37


class struct__IO_codecvt(Structure):
    pass

# /usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h: 38


class struct__IO_wide_data(Structure):
    pass


_IO_lock_t = None  # /usr/include/x86_64-linux-gnu/bits/types/struct_FILE.h: 43

struct__IO_FILE.__slots__ = [
    '_flags',
    '_IO_read_ptr',
    '_IO_read_end',
    '_IO_read_base',
    '_IO_write_base',
    '_IO_write_ptr',
    '_IO_write_end',
    '_IO_buf_base',
    '_IO_buf_end',
    '_IO_save_base',
    '_IO_backup_base',
    '_IO_save_end',
    '_markers',
    '_chain',
    '_fileno',
    '_flags2',
    '_old_offset',
    '_cur_column',
    '_vtable_offset',
    '_shortbuf',
    '_lock',
    '_offset',
    '_codecvt',
    '_wide_data',
    '_freeres_list',
    '_freeres_buf',
    '__pad5',
    '_mode',
    '_unused2',
]
struct__IO_FILE._fields_ = [
    ('_flags', c_int),
    ('_IO_read_ptr', String),
    ('_IO_read_end', String),
    ('_IO_read_base', String),
    ('_IO_write_base', String),
    ('_IO_write_ptr', String),
    ('_IO_write_end', String),
    ('_IO_buf_base', String),
    ('_IO_buf_end', String),
    ('_IO_save_base', String),
    ('_IO_backup_base', String),
    ('_IO_save_end', String),
    ('_markers', POINTER(struct__IO_marker)),
    ('_chain', POINTER(struct__IO_FILE)),
    ('_fileno', c_int),
    ('_flags2', c_int),
    ('_old_offset', __off_t),
    ('_cur_column', c_ushort),
    ('_vtable_offset', c_char),
    ('_shortbuf', c_char * int(1)),
    ('_lock', POINTER(_IO_lock_t)),
    ('_offset', __off64_t),
    ('_codecvt', POINTER(struct__IO_codecvt)),
    ('_wide_data', POINTER(struct__IO_wide_data)),
    ('_freeres_list', POINTER(struct__IO_FILE)),
    ('_freeres_buf', POINTER(None)),
    ('__pad5', c_size_t),
    ('_mode', c_int),
    ('_unused2', c_char * int((((15 * sizeof(c_int)) - (4 * sizeof(POINTER(None)))) - sizeof(c_size_t)))),
]

enum_dpu_error_t = c_int  # dpu_error.h: 82

DPU_OK = 0  # dpu_error.h: 82

DPU_ERR_INTERNAL = (DPU_OK + 1)  # dpu_error.h: 82

DPU_ERR_SYSTEM = (DPU_ERR_INTERNAL + 1)  # dpu_error.h: 82

DPU_ERR_DRIVER = (DPU_ERR_SYSTEM + 1)  # dpu_error.h: 82

DPU_ERR_ALLOCATION = (DPU_ERR_DRIVER + 1)  # dpu_error.h: 82

DPU_ERR_INVALID_DPU_SET = (DPU_ERR_ALLOCATION + 1)  # dpu_error.h: 82

DPU_ERR_INVALID_THREAD_ID = (DPU_ERR_INVALID_DPU_SET + 1)  # dpu_error.h: 82

DPU_ERR_INVALID_NOTIFY_ID = (DPU_ERR_INVALID_THREAD_ID + 1)  # dpu_error.h: 82

DPU_ERR_INVALID_WRAM_ACCESS = (
    DPU_ERR_INVALID_NOTIFY_ID +
    1)  # dpu_error.h: 82

DPU_ERR_INVALID_IRAM_ACCESS = (
    DPU_ERR_INVALID_WRAM_ACCESS +
    1)  # dpu_error.h: 82

DPU_ERR_INVALID_MRAM_ACCESS = (
    DPU_ERR_INVALID_IRAM_ACCESS +
    1)  # dpu_error.h: 82

DPU_ERR_INVALID_SYMBOL_ACCESS = (
    DPU_ERR_INVALID_MRAM_ACCESS +
    1)  # dpu_error.h: 82

DPU_ERR_MRAM_BUSY = (DPU_ERR_INVALID_SYMBOL_ACCESS + 1)  # dpu_error.h: 82

DPU_ERR_TRANSFER_ALREADY_SET = (DPU_ERR_MRAM_BUSY + 1)  # dpu_error.h: 82

DPU_ERR_DIFFERENT_DPU_PROGRAMS = (
    DPU_ERR_TRANSFER_ALREADY_SET +
    1)  # dpu_error.h: 82

DPU_ERR_CORRUPTED_MEMORY = (
    DPU_ERR_DIFFERENT_DPU_PROGRAMS +
    1)  # dpu_error.h: 82

DPU_ERR_DPU_DISABLED = (DPU_ERR_CORRUPTED_MEMORY + 1)  # dpu_error.h: 82

DPU_ERR_DPU_ALREADY_RUNNING = (DPU_ERR_DPU_DISABLED + 1)  # dpu_error.h: 82

DPU_ERR_INVALID_MEMORY_TRANSFER = (
    DPU_ERR_DPU_ALREADY_RUNNING +
    1)  # dpu_error.h: 82

DPU_ERR_INVALID_LAUNCH_POLICY = (
    DPU_ERR_INVALID_MEMORY_TRANSFER +
    1)  # dpu_error.h: 82

DPU_ERR_DPU_FAULT = (DPU_ERR_INVALID_LAUNCH_POLICY + 1)  # dpu_error.h: 82

DPU_ERR_ELF_INVALID_FILE = (DPU_ERR_DPU_FAULT + 1)  # dpu_error.h: 82

DPU_ERR_ELF_NO_SUCH_FILE = (DPU_ERR_ELF_INVALID_FILE + 1)  # dpu_error.h: 82

DPU_ERR_ELF_NO_SUCH_SECTION = (DPU_ERR_ELF_NO_SUCH_FILE + 1)  # dpu_error.h: 82

DPU_ERR_TIMEOUT = (DPU_ERR_ELF_NO_SUCH_SECTION + 1)  # dpu_error.h: 82

DPU_ERR_INVALID_PROFILE = (DPU_ERR_TIMEOUT + 1)  # dpu_error.h: 82

DPU_ERR_UNKNOWN_SYMBOL = (DPU_ERR_INVALID_PROFILE + 1)  # dpu_error.h: 82

DPU_ERR_LOG_FORMAT = (DPU_ERR_UNKNOWN_SYMBOL + 1)  # dpu_error.h: 82

DPU_ERR_LOG_CONTEXT_MISSING = (DPU_ERR_LOG_FORMAT + 1)  # dpu_error.h: 82

DPU_ERR_LOG_BUFFER_TOO_SMALL = (
    DPU_ERR_LOG_CONTEXT_MISSING +
    1)  # dpu_error.h: 82

DPU_ERR_VPD_INVALID_FILE = (
    DPU_ERR_LOG_BUFFER_TOO_SMALL +
    1)  # dpu_error.h: 82

DPU_ERR_NO_THREAD_PER_RANK = (DPU_ERR_VPD_INVALID_FILE + 1)  # dpu_error.h: 82

dpu_error_t = enum_dpu_error_t  # dpu_error.h: 82

# dpu_error.h: 90
if _libs["libdpu.so"].has("dpu_error_to_string", "cdecl"):
    dpu_error_to_string = _libs["libdpu.so"].get(
        "dpu_error_to_string", "cdecl")
    dpu_error_to_string.argtypes = [dpu_error_t]
    dpu_error_to_string.restype = c_char_p

mram_addr_t = c_uint32  # dpu_types.h: 94

dpu_mem_max_addr_t = mram_addr_t  # dpu_types.h: 100

mram_size_t = c_uint32  # dpu_types.h: 118

dpu_mem_max_size_t = mram_size_t  # dpu_types.h: 124

# dpu_types.h: 197


class struct_dpu_rank_t(Structure):
    pass

# dpu_types.h: 203


class struct_dpu_t(Structure):
    pass


enum__dpu_set_kind_t = c_int  # dpu_types.h: 208

dpu_set_kind_t = enum__dpu_set_kind_t  # dpu_types.h: 208

# dpu_types.h: 213


class struct_anon_24(Structure):
    pass


struct_anon_24.__slots__ = [
    'nr_ranks',
    'ranks',
]
struct_anon_24._fields_ = [
    ('nr_ranks', c_uint32),
    ('ranks', POINTER(POINTER(struct_dpu_rank_t))),
]

# dpu_types.h: 212


class union_anon_25(Union):
    pass


union_anon_25.__slots__ = [
    'list',
    'dpu',
]
union_anon_25._fields_ = [
    ('list', struct_anon_24),
    ('dpu', POINTER(struct_dpu_t)),
]

# dpu_types.h: 210


class struct_dpu_set_t(Structure):
    pass


struct_dpu_set_t.__slots__ = [
    'kind',
    'unnamed_1',
]
struct_dpu_set_t._anonymous_ = [
    'unnamed_1',
]
struct_dpu_set_t._fields_ = [
    ('kind', dpu_set_kind_t),
    ('unnamed_1', union_anon_25),
]

# dpu_types.h: 221


class struct_dpu_program_t(Structure):
    pass

# dpu_types.h: 227


class struct_dpu_symbol_t(Structure):
    pass


struct_dpu_symbol_t.__slots__ = [
    'address',
    'size',
]
struct_dpu_symbol_t._fields_ = [
    ('address', dpu_mem_max_addr_t),
    ('size', dpu_mem_max_size_t),
]

# dpu_types.h: 236


class struct_dpu_incbin_t(Structure):
    pass


struct_dpu_incbin_t.__slots__ = [
    'buffer',
    'size',
    'path',
]
struct_dpu_incbin_t._fields_ = [
    ('buffer', POINTER(c_uint8)),
    ('size', c_size_t),
    ('path', String),
]

enum__dpu_launch_policy_t = c_int  # dpu.h: 38

DPU_ASYNCHRONOUS = 0  # dpu.h: 38

DPU_SYNCHRONOUS = (DPU_ASYNCHRONOUS + 1)  # dpu.h: 38

dpu_launch_policy_t = enum__dpu_launch_policy_t  # dpu.h: 38

enum__dpu_xfer_t = c_int  # dpu.h: 49

DPU_XFER_TO_DPU = 0  # dpu.h: 49

DPU_XFER_FROM_DPU = (DPU_XFER_TO_DPU + 1)  # dpu.h: 49

dpu_xfer_t = enum__dpu_xfer_t  # dpu.h: 49

enum__dpu_xfer_flags_t = c_int  # dpu.h: 60

DPU_XFER_DEFAULT = 0  # dpu.h: 60

DPU_XFER_NO_RESET = (DPU_XFER_DEFAULT + 1)  # dpu.h: 60

dpu_xfer_flags_t = enum__dpu_xfer_flags_t  # dpu.h: 60

# dpu.h: 104
if _libs["libdpu.so"].has("dpu_alloc", "cdecl"):
    dpu_alloc = _libs["libdpu.so"].get("dpu_alloc", "cdecl")
    dpu_alloc.argtypes = [c_uint32, String, POINTER(struct_dpu_set_t)]
    dpu_alloc.restype = dpu_error_t

# dpu.h: 116
if _libs["libdpu.so"].has("dpu_free", "cdecl"):
    dpu_free = _libs["libdpu.so"].get("dpu_free", "cdecl")
    dpu_free.argtypes = [struct_dpu_set_t]
    dpu_free.restype = dpu_error_t

# dpu.h: 126
if _libs["libdpu.so"].has("dpu_get_nr_ranks", "cdecl"):
    dpu_get_nr_ranks = _libs["libdpu.so"].get("dpu_get_nr_ranks", "cdecl")
    dpu_get_nr_ranks.argtypes = [struct_dpu_set_t, POINTER(c_uint32)]
    dpu_get_nr_ranks.restype = dpu_error_t

# dpu.h: 136
if _libs["libdpu.so"].has("dpu_get_nr_dpus", "cdecl"):
    dpu_get_nr_dpus = _libs["libdpu.so"].get("dpu_get_nr_dpus", "cdecl")
    dpu_get_nr_dpus.argtypes = [struct_dpu_set_t, POINTER(c_uint32)]
    dpu_get_nr_dpus.restype = dpu_error_t

# dpu.h: 179


class struct_dpu_set_rank_iterator_t(Structure):
    pass


struct_dpu_set_rank_iterator_t.__slots__ = [
    'set',
    'count',
    'next_idx',
    'has_next',
    'next',
]
struct_dpu_set_rank_iterator_t._fields_ = [
    ('set', POINTER(struct_dpu_set_t)),
    ('count', c_uint32),
    ('next_idx', c_uint32),
    ('has_next', c_bool),
    ('next', struct_dpu_set_t),
]

# dpu.h: 193


class struct_dpu_set_dpu_iterator_t(Structure):
    pass


struct_dpu_set_dpu_iterator_t.__slots__ = [
    'rank_iterator',
    'count',
    'next_idx',
    'has_next',
    'next',
]
struct_dpu_set_dpu_iterator_t._fields_ = [
    ('rank_iterator', struct_dpu_set_rank_iterator_t),
    ('count', c_uint32),
    ('next_idx', c_uint32),
    ('has_next', c_bool),
    ('next', struct_dpu_set_t),
]

# dpu.h: 211
if _libs["libdpu.so"].has("dpu_set_rank_iterator_from", "cdecl"):
    dpu_set_rank_iterator_from = _libs["libdpu.so"].get(
        "dpu_set_rank_iterator_from", "cdecl")
    dpu_set_rank_iterator_from.argtypes = [POINTER(struct_dpu_set_t)]
    dpu_set_rank_iterator_from.restype = struct_dpu_set_rank_iterator_t

# dpu.h: 222
if _libs["libdpu.so"].has("dpu_set_rank_iterator_next", "cdecl"):
    dpu_set_rank_iterator_next = _libs["libdpu.so"].get(
        "dpu_set_rank_iterator_next", "cdecl")
    dpu_set_rank_iterator_next.argtypes = [
        POINTER(struct_dpu_set_rank_iterator_t)]
    dpu_set_rank_iterator_next.restype = None

# dpu.h: 234
if _libs["libdpu.so"].has("dpu_set_dpu_iterator_from", "cdecl"):
    dpu_set_dpu_iterator_from = _libs["libdpu.so"].get(
        "dpu_set_dpu_iterator_from", "cdecl")
    dpu_set_dpu_iterator_from.argtypes = [POINTER(struct_dpu_set_t)]
    dpu_set_dpu_iterator_from.restype = struct_dpu_set_dpu_iterator_t

# dpu.h: 245
if _libs["libdpu.so"].has("dpu_set_dpu_iterator_next", "cdecl"):
    dpu_set_dpu_iterator_next = _libs["libdpu.so"].get(
        "dpu_set_dpu_iterator_next", "cdecl")
    dpu_set_dpu_iterator_next.argtypes = [
        POINTER(struct_dpu_set_dpu_iterator_t)]
    dpu_set_dpu_iterator_next.restype = None

# dpu.h: 258
if _libs["libdpu.so"].has("dpu_load_from_memory", "cdecl"):
    dpu_load_from_memory = _libs["libdpu.so"].get(
        "dpu_load_from_memory", "cdecl")
    dpu_load_from_memory.argtypes = [
        struct_dpu_set_t, POINTER(c_uint8), c_size_t, POINTER(
            POINTER(struct_dpu_program_t))]
    dpu_load_from_memory.restype = dpu_error_t

# dpu.h: 270
if _libs["libdpu.so"].has("dpu_load_from_incbin", "cdecl"):
    dpu_load_from_incbin = _libs["libdpu.so"].get(
        "dpu_load_from_incbin", "cdecl")
    dpu_load_from_incbin.argtypes = [
        struct_dpu_set_t, POINTER(struct_dpu_incbin_t), POINTER(
            POINTER(struct_dpu_program_t))]
    dpu_load_from_incbin.restype = dpu_error_t

# dpu.h: 282
if _libs["libdpu.so"].has("dpu_load", "cdecl"):
    dpu_load = _libs["libdpu.so"].get("dpu_load", "cdecl")
    dpu_load.argtypes = [
        struct_dpu_set_t, String, POINTER(
            POINTER(struct_dpu_program_t))]
    dpu_load.restype = dpu_error_t

# dpu.h: 293
if _libs["libdpu.so"].has("dpu_get_symbol", "cdecl"):
    dpu_get_symbol = _libs["libdpu.so"].get("dpu_get_symbol", "cdecl")
    dpu_get_symbol.argtypes = [
        POINTER(struct_dpu_program_t),
        String,
        POINTER(struct_dpu_symbol_t)]
    dpu_get_symbol.restype = dpu_error_t

# dpu.h: 303
if _libs["libdpu.so"].has("dpu_launch", "cdecl"):
    dpu_launch = _libs["libdpu.so"].get("dpu_launch", "cdecl")
    dpu_launch.argtypes = [struct_dpu_set_t, dpu_launch_policy_t]
    dpu_launch.restype = dpu_error_t

# dpu.h: 314
if _libs["libdpu.so"].has("dpu_status", "cdecl"):
    dpu_status = _libs["libdpu.so"].get("dpu_status", "cdecl")
    dpu_status.argtypes = [struct_dpu_set_t, POINTER(c_bool), POINTER(c_bool)]
    dpu_status.restype = dpu_error_t

# dpu.h: 323
if _libs["libdpu.so"].has("dpu_sync", "cdecl"):
    dpu_sync = _libs["libdpu.so"].get("dpu_sync", "cdecl")
    dpu_sync.argtypes = [struct_dpu_set_t]
    dpu_sync.restype = dpu_error_t

# dpu.h: 336
if _libs["libdpu.so"].has("dpu_copy_to", "cdecl"):
    dpu_copy_to = _libs["libdpu.so"].get("dpu_copy_to", "cdecl")
    dpu_copy_to.argtypes = [
        struct_dpu_set_t,
        String,
        c_uint32,
        POINTER(None),
        c_size_t]
    dpu_copy_to.restype = dpu_error_t

# dpu.h: 349
if _libs["libdpu.so"].has("dpu_copy_from", "cdecl"):
    dpu_copy_from = _libs["libdpu.so"].get("dpu_copy_from", "cdecl")
    dpu_copy_from.argtypes = [
        struct_dpu_set_t,
        String,
        c_uint32,
        POINTER(None),
        c_size_t]
    dpu_copy_from.restype = dpu_error_t

# dpu.h: 362
if _libs["libdpu.so"].has("dpu_copy_to_symbol", "cdecl"):
    dpu_copy_to_symbol = _libs["libdpu.so"].get("dpu_copy_to_symbol", "cdecl")
    dpu_copy_to_symbol.argtypes = [
        struct_dpu_set_t,
        struct_dpu_symbol_t,
        c_uint32,
        POINTER(None),
        c_size_t]
    dpu_copy_to_symbol.restype = dpu_error_t

# dpu.h: 375
if _libs["libdpu.so"].has("dpu_copy_from_symbol", "cdecl"):
    dpu_copy_from_symbol = _libs["libdpu.so"].get(
        "dpu_copy_from_symbol", "cdecl")
    dpu_copy_from_symbol.argtypes = [
        struct_dpu_set_t,
        struct_dpu_symbol_t,
        c_uint32,
        POINTER(None),
        c_size_t]
    dpu_copy_from_symbol.restype = dpu_error_t

# dpu.h: 389
if _libs["libdpu.so"].has("dpu_prepare_xfer", "cdecl"):
    dpu_prepare_xfer = _libs["libdpu.so"].get("dpu_prepare_xfer", "cdecl")
    dpu_prepare_xfer.argtypes = [struct_dpu_set_t, POINTER(None)]
    dpu_prepare_xfer.restype = dpu_error_t

# dpu.h: 408
if _libs["libdpu.so"].has("dpu_push_xfer", "cdecl"):
    dpu_push_xfer = _libs["libdpu.so"].get("dpu_push_xfer", "cdecl")
    dpu_push_xfer.argtypes = [
        struct_dpu_set_t,
        dpu_xfer_t,
        String,
        c_uint32,
        c_size_t,
        dpu_xfer_flags_t]
    dpu_push_xfer.restype = dpu_error_t

# dpu.h: 432
if _libs["libdpu.so"].has("dpu_push_xfer_symbol", "cdecl"):
    dpu_push_xfer_symbol = _libs["libdpu.so"].get(
        "dpu_push_xfer_symbol", "cdecl")
    dpu_push_xfer_symbol.argtypes = [
        struct_dpu_set_t,
        dpu_xfer_t,
        struct_dpu_symbol_t,
        c_uint32,
        c_size_t,
        dpu_xfer_flags_t]
    dpu_push_xfer_symbol.restype = dpu_error_t

# dpu.h: 473
if _libs["libdpu.so"].has("dpu_log_read", "cdecl"):
    dpu_log_read = _libs["libdpu.so"].get("dpu_log_read", "cdecl")
    dpu_log_read.argtypes = [struct_dpu_set_t, POINTER(FILE)]
    dpu_log_read.restype = dpu_error_t

# /usr/include/limits.h: 84
try:
    UINT_MAX = 4294967295
except BaseException:
    pass

# dpu.h: 89
try:
    DPU_ALLOCATE_ALL = UINT_MAX
except BaseException:
    pass

dpu_set_rank_iterator_t = struct_dpu_set_rank_iterator_t  # dpu.h: 179

dpu_set_dpu_iterator_t = struct_dpu_set_dpu_iterator_t  # dpu.h: 193

# Begin inserted files
# Begin "<backends>/python/utils/ffi_footer.txt"

# Copyright 2020 UPMEM. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

if _libs["libdpu.so"].has("Py_dpu_log_read", "cdecl"):
    Py_dpu_log_read = _libs["libdpu.so"].get("Py_dpu_log_read", "cdecl")
    Py_dpu_log_read.argtypes = [struct_dpu_set_t, ctypes.py_object]
    Py_dpu_log_read.restype = dpu_error_t

if _libs["libdpu.so"].has("Py_dpu_prepare_xfers", "cdecl"):
    Py_dpu_prepare_xfers = _libs["libdpu.so"].get(
        "Py_dpu_prepare_xfers", "cdecl")
    Py_dpu_prepare_xfers.argtypes = [struct_dpu_set_t, py_object]
    Py_dpu_prepare_xfers.restype = dpu_error_t

if _libs["libdpu.so"].has("Py_dpu_get_symbol_names", "cdecl"):
    Py_dpu_get_symbol_names = _libs["libdpu.so"].get(
        "Py_dpu_get_symbol_names", "cdecl")
    Py_dpu_get_symbol_names.argtypes = [POINTER(struct_dpu_program_t)]
    Py_dpu_get_symbol_names.restype = py_object

# End "<backends>/python/utils/ffi_footer.txt"

# 1 inserted files
# End inserted files

# No prefix-stripping
