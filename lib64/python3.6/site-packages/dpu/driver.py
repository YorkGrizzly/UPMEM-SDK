# Copyright 2020 UPMEM. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Features to drive DPUs"""

import array
import io
import os
import struct
import sys
import tempfile

import dpu.compiler as compiler
import dpu.ffi as ffi
from .ffi import DPU_ALLOCATE_ALL as ALLOCATE_ALL

from typing import Dict, Iterator, List, Optional, Sequence, Tuple, TypeVar, Union

T = TypeVar('T')
OneOrMore = Union[T, List[T]]

Buffer = Union[bytes, bytearray, memoryview, array.array]
DpuCopyBuffer = Union[str, 'DpuSymbol', Buffer, List[Optional[Buffer]]]
DpuConstructorLogObject = OneOrMore[io.IOBase]
DpuExecLogObject = Union[OneOrMore[io.IOBase], Dict['DpuSet', io.IOBase]]
DpuBinaryProgram = Union[bytes, str]
DpuSourceFile = Union[io.IOBase, str]
DpuSourceProgram = OneOrMore[DpuSourceFile]


class DpuError(Exception):
    """Error raised by the API"""
    pass


class DpuVariable():
    def __init__(self, contents: List[memoryview]):
        self.contents = contents

    def data(self) -> OneOrMore[memoryview]:
        return _unwrap_single_element_seq(self.contents)

    def int8(self) -> Union[OneOrMore[int], OneOrMore[Sequence[int]]]:
        return _cast_and_unwrap_view_list(self.contents, 'b')

    def uint8(self) -> Union[OneOrMore[int], OneOrMore[Sequence[int]]]:
        return _cast_and_unwrap_view_list(self.contents, 'B')

    def int16(self) -> Union[OneOrMore[int], OneOrMore[Sequence[int]]]:
        return _cast_and_unwrap_view_list(self.contents, 'h')

    def uint16(self) -> Union[OneOrMore[int], OneOrMore[Sequence[int]]]:
        return _cast_and_unwrap_view_list(self.contents, 'H')

    def int32(self) -> Union[OneOrMore[int], OneOrMore[Sequence[int]]]:
        return _cast_and_unwrap_view_list(self.contents, 'i')

    def uint32(self) -> Union[OneOrMore[int], OneOrMore[Sequence[int]]]:
        return _cast_and_unwrap_view_list(self.contents, 'I')

    def int64(self) -> Union[OneOrMore[int], OneOrMore[Sequence[int]]]:
        return _cast_and_unwrap_view_list(self.contents, 'q')

    def uint64(self) -> Union[OneOrMore[int], OneOrMore[Sequence[int]]]:
        return _cast_and_unwrap_view_list(self.contents, 'Q')

    def float(self) -> Union[OneOrMore[float], OneOrMore[Sequence[float]]]:
        return _cast_and_unwrap_view_list(self.contents, 'f')

    def double(self) -> Union[OneOrMore[float], OneOrMore[Sequence[float]]]:
        return _cast_and_unwrap_view_list(self.contents, 'd')


def _cast_and_unwrap_view_list(views, fmt):
    cast_views = [_unwrap_single_element_seq(view.cast(fmt)) for view in views]
    return _unwrap_single_element_seq(cast_views)


def _unwrap_single_element_seq(seq):
    return seq if len(seq) != 1 else seq[0]


class DpuSymbol():
    """
    Symbol in a DPU program

    Attributes:
    name (str): the name of the symbol
    """

    def __init__(self, name: str,
                 c_symbol: Union[ffi.struct_dpu_symbol_t, Tuple[int, int]]):
        self.name = name
        if isinstance(c_symbol, ffi.struct_dpu_symbol_t):
            self.c_symbol = c_symbol
        else:
            self.c_symbol = ffi.struct_dpu_symbol_t()
            self.c_symbol.address = c_symbol[0]
            self.c_symbol.size = c_symbol[1]

    def value(self) -> int:
        """
        Fetch the symbol value (ie. when done on a variable, its address)

        Returns:
            int: the symbol value
        """
        return self.c_symbol.address

    def size(self) -> int:
        """
        Fetch the symbol size

        Returns:
            int: the symbol size
        """
        return self.c_symbol.size


class DpuProgram:
    """
    Representation of DPU program
    """

    def __init__(self, c_program: ffi.POINTER(ffi.struct_dpu_program_t)):
        self.c_program = c_program
        self.symbols = {}

    def get_symbol(self, symbol: str) -> Optional[DpuSymbol]:
        """
        Fetch a symbol in the program

        Args:
            symbol (str): the name of the symbol to look for in the program

        Returns:
            Optional[DpuSymbol]: the symbol if found, None otherwise
        """
        if symbol not in self.symbols:
            c_symbol = ffi.struct_dpu_symbol_t()
            err = ffi.dpu_get_symbol(
                self.c_program, symbol, ffi.byref(c_symbol))
            if err != ffi.DPU_OK:
                return None
            self.symbols[symbol] = DpuSymbol(symbol, c_symbol)
        return self.symbols[symbol]

    def fetch_all_symbols(self) -> Dict[str, DpuSymbol]:
        symbol_names = ffi.Py_dpu_get_symbol_names(self.c_program)
        for name in symbol_names:
            self.get_symbol(name)
        return self.symbols


class DpuSet:
    """
    A set of DPUs

    Args:
        nr_dpus (int): the number of DPUs to allocate
            ALLOCATE_ALL will allocate all available DPUs
        profile (str): the profile of the DPUs
        binary (Optional[DpuBinaryProgram]): a DPU binary program to load after the allocation
        c_source (Optional[DpuSourceProgram]): a DPU C source code to compile and load after the allocation
        asm_source (Optional[DpuSourceProgram]): a DPU assembly source code to compile and load after the allocation
        log (Optional[DpuConstructorLogObject]): a object where the DPU log will be written.
            The object can either be a file object or a list of file object
            with one object per dpu

    Raises:
        DpuError: if the allocation fails
    """

    def __init__(
            self,
            nr_dpus: int = ALLOCATE_ALL,
            profile: str = '',
            binary: Optional[DpuBinaryProgram] = None,
            c_source: Optional[DpuSourceProgram] = None,
            asm_source: Optional[DpuSourceProgram] = None,
            log: Optional[DpuConstructorLogObject] = None,
            _do_internals=True):
        self._symbols = {}
        self._dpus = []
        self._ranks = []
        self._top_set = None
        self._rank_set = None
        self.c_set = None
        if not isinstance(log, list):
            self._log = log

        if not _do_internals:
            return

        self.c_set = ffi.struct_dpu_set_t()
        _wrap_ffi(ffi.dpu_alloc(nr_dpus, profile, ffi.byref(self.c_set)))

        rank_iterator = _dpu_set_iter(
            ffi.dpu_set_rank_iterator_from(self.c_set),
            ffi.dpu_set_rank_iterator_next)

        for c_rank in rank_iterator:
            rank_set = DpuSet(log=log, _do_internals=False)
            rank_set.c_set = c_rank
            self._ranks.append(rank_set)
            rank_set._ranks.append(rank_set)
            rank_set._top_set = self

            dpu_iterator = _dpu_set_iter(
                ffi.dpu_set_dpu_iterator_from(c_rank),
                ffi.dpu_set_dpu_iterator_next)

            for c_dpu in dpu_iterator:
                dpu = DpuSet(log=log, _do_internals=False)
                dpu.c_set = c_dpu
                self._dpus.append(dpu)
                rank_set._dpus.append(dpu)
                dpu._dpus.append(dpu)
                dpu._ranks.append(rank_set)
                rank_set._top_set = self
                dpu._top_set = self
                dpu._rank_set = rank_set

        try:
            if isinstance(log, list):
                if len(log) != len(self._dpus):
                    raise DpuError(
                        'log list length does not match number of DPUs in DpuSet')
                for dpu, dpu_log in zip(self._dpus, log):
                    dpu._log = dpu_log

            if c_source is not None or asm_source is not None or binary is not None:
                self.load(binary, c_source, asm_source)
        except BaseException:
            ffi.dpu_free(self.c_set)
            raise

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, exc_traceback):
        self.free()

    def __iter__(self) -> Iterator['DpuSet']:
        return iter(self.dpus())

    def __len__(self) -> int:
        return len(self._dpus)

    def __getattribute__(self, name):
        if name == '_symbols':
            return super(DpuSet, self).__getattribute__(name)

        symbol = self._symbols.get(name)

        if symbol is None:
            return super(DpuSet, self).__getattribute__(name)

        return self.__get_symbol(symbol)

    def __setattr__(self, name, value):
        if name == '_symbols':
            super(DpuSet, self).__setattr__(name, value)
            return

        symbol = self._symbols.get(name)

        if symbol is None:
            super(DpuSet, self).__setattr__(name, value)
        else:
            self.copy(symbol, value)

    def dpus(self) -> List['DpuSet']:
        """Provides a List on the DPUs of the DPU set"""
        return self._dpus

    def ranks(self) -> List['DpuSet']:
        """Provides a List on the ranks of the DPU set"""
        return self._ranks

    def free(self):
        """Release the DPUs of the set"""
        _wrap_ffi(ffi.dpu_free(self.c_set))

    def load(
            self,
            binary: Optional[DpuBinaryProgram] = None,
            c_source: Optional[DpuSourceProgram] = None,
            asm_source: Optional[DpuSourceProgram] = None) -> DpuProgram:
        """
        Load a program in the DPUs

        Args:
            binary (Optional[DpuBinaryProgram]): the DPU binary program to load
            c_source (Optional[DpuSourceProgram]): the DPU C source code to compile and load
            asm_source (Optional[DpuSourceProgram]): the DPU assembly source code to compile and load

        Returns:
            DpuProgram: a representation of the loaded program

        Raises:
            DpuError: if the program cannot be loaded
        """
        if (binary is None) == (c_source is None and asm_source is None):
            raise DpuError(
                'one and only one source or binary program must be defined')

        with tempfile.TemporaryDirectory() as tmpdir:
            sources = []
            if c_source is not None:
                if not isinstance(c_source, list):
                    source = [c_source]

                for idx, src in enumerate(source):
                    if not isinstance(src, str):
                        source_name = os.path.join(
                            tmpdir, 'src' + str(idx) + '.c')
                        with open(source_name, 'w') as source_file:
                            source_file.write(src.read())
                        src = source_name

                    sources.append(src)
            if asm_source is not None:
                if not isinstance(asm_source, list):
                    source = [asm_source]

                for idx, src in enumerate(source):
                    if not isinstance(src, str):
                        source_name = os.path.join(
                            tmpdir, 'src' + str(idx) + '.S')
                        with open(source_name, 'w') as source_file:
                            source_file.write(src.read())
                        src = source_name

                    sources.append(src)

            if len(sources) != 0:
                options = []
                if c_source is None:
                    options.append("-nostartfiles")
                binary = os.path.join(tmpdir, 'main.dpu')
                compiler.DEFAULT_DPU_COMPILER.compile(
                    sources,
                    output=binary,
                    options=options,
                    opt_lvl=compiler.O2)

            c_program = ffi.POINTER(ffi.struct_dpu_program_t)()
            c_program_ref = ffi.byref(c_program)
            if isinstance(binary, str):
                ffi_result = ffi.dpu_load(self.c_set, binary, c_program_ref)
            else:
                program_len = len(binary)
                program_buf = (ffi.c_byte * program_len).from_buffer(binary)
                ffi_result = ffi.dpu_load_from_memory(
                    self.c_set, program_buf, program_len, c_program_ref)
        _wrap_ffi(ffi_result)
        program = DpuProgram(c_program)
        self.__update_symbol_attributes(program)
        return program

    def __update_symbol_attributes(self, program: DpuProgram):
        symbols = program.fetch_all_symbols()

        if self._top_set is None:
            for rank in self.ranks():
                rank.__del_symbol_attributes()
                rank.__add_symbol_attributes(symbols)
        else:
            self._top_set.__del_symbol_attributes()

        if self._rank_set is None:
            for dpu in self.dpus():
                dpu.__del_symbol_attributes()
                dpu.__add_symbol_attributes(symbols)
        else:
            self._rank_set.__del_symbol_attributes()

        self.__del_symbol_attributes()
        self.__add_symbol_attributes(symbols)

    def __add_symbol_attributes(self, symbols: Dict[str, DpuSymbol]):
        for symbol in symbols:
            setattr(self, symbol, None)
        self._symbols = symbols

    def __del_symbol_attributes(self):
        for symbol in self._symbols:
            delattr(self, symbol)
        self._symbols = {}

    def __get_symbol(self, symbol: DpuSymbol) -> DpuVariable:
        nr_dpus = len(self)
        size = symbol.size()
        view = memoryview(bytearray(nr_dpus * size))
        buf = [view[i * size:(i + 1) * size] for i in range(nr_dpus)]
        self.copy(buf, symbol)
        return DpuVariable(buf)

    def copy(
            self,
            dst: DpuCopyBuffer,
            src: DpuCopyBuffer,
            size: Optional[int] = None,
            offset: int = 0):
        """
        Copy data between the Host and the DPUs

        Args:
            dst (DpuCopyBuffer): the destination buffer for the copy.
                If the buffer is on the Host, this must be a bytearray, or a list of optional bytearrays.
                If the buffer is on the DPUs, this must be a str, representing a DPU symbol, or directly a DpuSymbol.
            src (DpuCopyBuffer): the source buffer for the copy.
                If the buffer is on the Host, this must be a bytearray, or a list of optional bytearrays.
                If the buffer is on the DPUs, this must be a str, representing a DPU symbol, or directly a DpuSymbol.
            size (Optional[int]): number of bytes to transfer.
                By default set to None, which will use the length of the Host buffer. If multiple Host buffers are
                provided, all buffers must have the same size.
            offset (int): offset in bytes for the DPU symbol.
                By default set to 0.

        Raises:
            DpuError: if the copy fails, because of invalid arguments, unkown symbols or out-of-bound accesses
        """
        dpu_symbol_is_str = isinstance(dst, str) or isinstance(src, str)
        multiple_buffers = isinstance(dst, list) or isinstance(src, list)
        to_dpu = isinstance(dst, DpuSymbol) or isinstance(dst, str)
        from_dpu = isinstance(src, DpuSymbol) or isinstance(src, str)

        if to_dpu == from_dpu:
            raise DpuError('can only execute Host <-> DPU transfers')

        if to_dpu:
            host = src
            device = dst
            if multiple_buffers:
                ffi_fn_str = ffi.dpu_push_xfer
                ffi_fn_sym = ffi.dpu_push_xfer_symbol
                xfer_dir = ffi.DPU_XFER_TO_DPU
            else:
                ffi_fn_str = ffi.dpu_copy_to
                ffi_fn_sym = ffi.dpu_copy_to_symbol
        else:
            host = dst
            device = src
            if multiple_buffers:
                ffi_fn_str = ffi.dpu_push_xfer
                ffi_fn_sym = ffi.dpu_push_xfer_symbol
                xfer_dir = ffi.DPU_XFER_FROM_DPU
            else:
                ffi_fn_str = ffi.dpu_copy_from
                ffi_fn_sym = ffi.dpu_copy_from_symbol

        if dpu_symbol_is_str:
            ffi_fn = ffi_fn_str
        else:
            ffi_fn = ffi_fn_sym
            device = device.c_symbol

        buffer_size = None
        if multiple_buffers:
            dpu_list = self._dpus

            if len(host) != len(dpu_list):
                raise DpuError(
                    'invalid number of host buffers for this DPU set')

            for buff in host:
                if buff is not None:
                    buff_len = len(buff)
                    if buffer_size is None:
                        buffer_size = buff_len
                    elif buffer_size != buff_len:
                        raise DpuError(
                            'can only copy buffers of the same size')

            if buffer_size is None:
                raise DpuError('no host buffer has been defined')

            _wrap_ffi(ffi.Py_dpu_prepare_xfers(self.c_set, host))
        else:
            buffer_size = len(host)
            host = (ffi.c_byte * buffer_size).from_buffer(host)

        if size is None:
            size = buffer_size
        elif buffer_size < size:
            raise DpuError('host buffer is too small')

        if multiple_buffers:
            _wrap_ffi(
                ffi_fn(
                    self.c_set,
                    xfer_dir,
                    device,
                    offset,
                    size,
                    ffi.DPU_XFER_DEFAULT))
        else:
            _wrap_ffi(ffi_fn(self.c_set, device, offset, host, size))

    def exec(
            self,
            log: Optional[DpuExecLogObject] = None):
        """
        Execute a program on the DPUs

        Args:
            log (Optional[DpuExecLogObject]): a object where the DPU log will be written.
                The object can either be a file object or a list of file object
                with one object per dpu or a dictionary of file object

        Raises:
            DpuError: if the program cannot be loaded, or the execution triggers a fault
        """
        _wrap_ffi(ffi.dpu_launch(self.c_set, ffi.DPU_SYNCHRONOUS))
        log_streams = {dpu: dpu._log for dpu in self._dpus}
        if log is not None:
            if isinstance(log, list):
                if len(log) != len(self._dpus):
                    raise DpuError(
                        'cannot log, log list length does not match the number of DPUs in DpuSet')
                for dpu, dpu_log in zip(self._dpus, log):
                    if dpu_log is not None:
                        log_streams[dpu] = dpu_log
            elif isinstance(log, dict):
                for dpu, dpu_log in log.items():
                    log_streams[dpu] = dpu_log
            else:
                for dpu in self._dpus:
                    log_streams[dpu] = log
        for dpu, log_stream in log_streams.items():
            if log_stream is not None:
                dpu.log(log_stream)

    def log(self, stream: Optional[io.IOBase] = None):
        """
        Print the DPU log on the Host standard output

        Args:
            stream: a writable object. If not specify, uses either the one
                specified in DpuSet's constructor or sys.stdout otherwise

        Raises:
            DpuError: if the DPU log buffer cannot be fetched, or the DPU set is invalid for this operation
        """
        if stream is None:
            if self._log is not None:
                stream = self._log
            else:
                stream = sys.stdout
        if not callable(getattr(stream, "write", None)):
            raise DpuError(
                "stream argument is not an object with a 'write' method")
        _wrap_ffi(ffi.Py_dpu_log_read(self.c_set, stream))


def _wrap_ffi(err: ffi.dpu_error_t):
    if err != ffi.DPU_OK:
        raise DpuError(ffi.dpu_error_to_string(err))


def _dpu_set_iter(c_iterator, next_fn) -> Iterator[ffi.struct_dpu_set_t]:
    while c_iterator.has_next:
        c_set = type(c_iterator.next)()
        ffi.pointer(c_set)[0] = c_iterator.next
        next_fn(ffi.byref(c_iterator))
        yield c_set
