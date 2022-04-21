#!/usr/bin/env python3
#
#   Symbol TABle Strings parser
#

"""
References:
https://sourceware.org/gdb/current/onlinedocs/stabs.html
https://opensource.apple.com/source/gdb/gdb-213/doc/stabs.pdf
https://github.com/astrelsky/ghidra/blob/StabsParser/Ghidra/Features/Base/src/main/java/ghidra/app/util/bin/format/stabs/

TODO notes

Handle continuation: https://github.com/bminor/binutils-gdb/blob/3451a2d7a3501e9c3fc344cbc4950c495f30c16d/gdb/mdebugread.c#L2761
"""

from enum import IntEnum

class STABSContext:
    """
    context for sharing type information across multiple STABS 
    """

    def __init__(self) -> None:
        self.type_map = {} # { type number : stab }
        self.global_indent = 0

    def indent_raise(self):
        self.global_indent += 4

    def indent_lower(self):
        self.global_indent -= 4

    def indent(self):
        return " " * self.global_indent

    def add_type(self, number, stab):
        self.type_map.update({ number : stab })

    def has_type(self, number):
        return number in self.type_map

    def lookup_type(self, number):
        return self.type_map[number]

class STABSStream:

    def __init__(self, str):
        self.str = str
        self.stream = str

    def has(self, c):
        return c in self.stream

    def rewind(self, s):
        self.stream = str(s) + self.stream

    def reset(self):
        self.stream = self.str

    def is_empty(self):
        return len(self.stream) == 0

    def peek_char(self):
        if self.is_empty():
            return None
        return self.stream[0]

    def next_char(self):
        if self.is_empty():
            return None
        ret,self.stream = self.stream[0],self.stream[1:]
        return ret

    def next_str_until(self, c):
        ret,self.stream = self.stream.split(c, 1)
        return ret

    def is_next(self, c):
        return not self.is_empty() and self.stream[0] == c

    def has_next(self, s):
        return not self.is_empty() and self.stream[0] in s

    def next_is_num(self):
        return self.has_next("0123456789")

    def consume_if(self, c):
        if self.is_next(c):
            return self.next_char()
        return None

    def next_int(self):
        ret = ""
        while self.next_is_num():
            ret += self.next_char()
        return int(ret) if ret != "" else None

    def next_sint(self):
        sgn = -1 if self.consume_if("-") is not None else 1
        return sgn * self.next_int()

    def next_int_until(self, c):
        return int(self.next_str_until(c))

class STABS:
    """
    Symbol TABle String
    """

    def read_enum(self, stream):
        enum_fields = {} # (name : value)

        while stream.consume_if(";") is None:
            field_name = stream.next_str_until(":")
            field_value = stream.next_sint()
            assert field_value is not None
            assert stream.next_char() == ","

            enum_fields[field_name] = field_value

        assert len(enum_fields) != 0

        out = f"enum {{\n"
        for field in enum_fields:
            out += f"{self.ctx.indent()}    {field} = {enum_fields[field]},\n"
        out = out[:-2] + f"\n{self.ctx.indent()}}}"
        return out

    def read_int_range(self, stream):
        range_type = stream.next_str_until(";")
        range_start = stream.next_int_until(";")
        range_end = stream.next_int_until(";")
        return "BUILTIN"

    def read_struct(self, stream):
        struct_fields = []

        struct_size = stream.next_int()
        while stream.consume_if(";") is None:
            field_name = stream.next_str_until(":")
            field_type = self.read_type(stream, False)
            assert stream.next_char() == ","
            field_offset_bits = stream.next_int()
            assert stream.next_char() == ","
            field_size_bits = stream.next_int()
            assert stream.next_char() == ";"

            struct_fields.append((field_name, field_type, field_offset_bits, field_size_bits))

        out = f"struct /* size=0x{struct_size:X} */ {{\n"

        for field in struct_fields:
            field_name, field_type, field_offset_bits, field_size_bits = field

            # TODO this is very rough, it misses some things that should be marked as bitfields
            if field_offset_bits % 8 == 0 and field_size_bits % 8 == 0:
                bitfield_bits = ""
            else:
                bitfield_bits = f" : {field_size_bits}"

            out += f"{self.ctx.indent()}    /* 0x{field_offset_bits // 8:04X} */ {field_type} {field_name}{bitfield_bits};\n"
        out += f"{self.ctx.indent()}}}"

        return out

    def read_union(self, stream):
        union_fields = []

        union_size = stream.next_int()
        while stream.consume_if(";") is None:
            field_name = stream.next_str_until(":")
            field_type = self.read_type(stream, False)
            assert stream.next_char() == ","
            i1 = stream.next_int()
            assert stream.next_char() == ","
            i2 = stream.next_int()
            assert stream.next_char() == ";"

            union_fields.append((field_name, field_type, i1, i2))

        out = f"union /* size=0x{union_size:X} */ {{\n"

        for field in union_fields:
            field_name, field_type, field_offset_bits, field_size_bits = field
            out += f"{self.ctx.indent()}    /* 0x{field_offset_bits // 8:04X} */ {field_type} {field_name};\n"
        out += f"{self.ctx.indent()}}}"

        return out

    def read_array(self, stream):
        unk_number = stream.next_int()
        assert stream.next_char() == ";"
        idx_lo = stream.next_int()
        assert idx_lo == 0 , "C arrays should always have lowest index = 0"
        assert stream.next_char() == ";"
        idx_hi = stream.next_sint()
        assert stream.next_char() == ";"
        arr_type = self.read_type(stream, False)

        return f"{arr_type}" + ("[]" if idx_hi + 1 == 0 else f"[{idx_hi + 1}]")

    def read_type(self, stream, typedef, recursive=False):
        tnum = stream.next_int()

        ptr = False
        func = False
        xref = False

        if stream.consume_if("=") is not None:
            # equals some other type, with possible modifiers

            # check modifiers
            if stream.has_next("xf*"):
                qualifier = stream.next_char()
                ptr = qualifier == "*"
                func = qualifier == "f"
                xref = qualifier == "x"

        elif stream.is_empty() or stream.has_next((',', ';')): # self.ctx.has_type(tnum)
            # reuse of already defined type
            return self.ctx.lookup_type(tnum)

        if stream.next_is_num():
            # type number that this type is equal to (modulo above modifiers)
            if self.name == "void":
                # builtin void type
                assert tnum == stream.next_int()
                self.ctx.add_type(tnum, "void")
                return "BUILTIN void"

            tname = self.read_type(stream, False, recursive=True)

            if typedef:
                self.ctx.add_type(tnum, self.name)
            else:
                self.ctx.add_type(tnum, tname + ("*" if ptr else "") + ("()" if func else ""))

            if typedef:
                return f"{tname}{'*' if ptr else ''} {self.name}{'()' if func else ''}"
            else:
                return f"{tname}{'*' if ptr else ''}{'()' if func else ''}"
        else:
            if typedef:
                self.ctx.add_type(tnum, self.name + ("*" if ptr else ""))
            type_type = stream.next_char()

            assert not ptr
            assert not func

            if type_type == "r":
                type_def = self.read_int_range(stream)
            elif type_type == "s":
                type_def = f"struct {stream.next_str_until(':')}" if xref else self.read_struct(stream)
            elif type_type == "u":
                type_def = f"union {stream.next_str_until(':')}" if xref else self.read_union(stream)
            elif type_type == "e":
                type_def = self.read_enum(stream)
            elif type_type == "a":
                assert stream.next_char() == "r"
                type_def = self.read_array(stream)
            else:
                assert False

            if typedef:
                return f"{type_def} {self.name}"
            else:
                self.ctx.add_type(tnum, type_def + ("*" if ptr else ""))
                return f"{type_def}"

    def constant_parser(self, stream):
        assert False , "Unimplemented"

    def caught_exception_parser(self, stream):
        assert False , "Should be unused in C"

    def parameter_parser(self, stream):
        var_type = stream.next_char()
        self.c_repr = f"/* function param */ {self.read_type(stream, False)} {self.name},"
        #print(self.c_repr)

    def function_parser(self, stream):
        self.func_type = stream.next_char()
        if self.func_type in ['f', 'Q']:
            self.qualifiers.append("static")

        return_type = self.read_type(stream, False)

        self.c_repr = ""
        if len(self.qualifiers) != 0:
            self.c_repr += " ".join(self.qualifiers) + " "

        self.c_repr += f"{return_type} {self.name}();"
        #print(self.c_repr)

    def variable_parser(self, stream):
        var_type = "stack" if stream.next_is_num() else stream.next_char()

        qualifiers = " "
        if var_type == "S" or var_type == "V":
            qualifiers += "static"
        # elif var_type == "r":
        #     qualifiers += "register"

        var_type_name = "variable"
        if var_type == "G" or var_type == "S":
            var_type_name = "global " + var_type_name
        else:
            var_type_name = "local " + var_type_name

        self.c_repr = f"/* {var_type_name} */{qualifiers.rstrip()} {self.read_type(stream, False)} {self.name};"
        #print(self.c_repr)

    def type_parser(self, stream):
        # discard t
        assert stream.next_char() == 't'
        # read type
        self.c_repr = "typedef " + self.read_type(stream, True) + ";"
        #print(self.c_repr + "\n")

    def composite_parser(self, stream):
        # discard T
        assert stream.next_char() == 'T'
        # read type
        self.c_repr = "typedef " + self.read_type(stream, True) + ";"
        #print(self.c_repr + "\n")

    def other_parser(self, stream):
        assert False , f"Unrecognized: {{ {stream.stream} }}"

    dispatch_table = {
        # constant
        'c': constant_parser,
        # caught exception
        'C': caught_exception_parser,
        # parameter
        'p': parameter_parser, # stack
        'P': parameter_parser, # register (GNU)
        'R': parameter_parser, # register (IBM)
        'v': parameter_parser, # passed by reference in arg list
        'a': parameter_parser, # passed by reference in register
        # function
        'f': function_parser, # file
        'F': function_parser, # global
        'I': function_parser, # nested
        'J': function_parser, # nested
        'm': function_parser, # module
        'Q': function_parser, # static
        # variable
        '(': variable_parser, # stack
        '-': variable_parser, # stack
        '0': variable_parser, # stack
        '1': variable_parser, # stack
        '2': variable_parser, # stack
        '3': variable_parser, # stack
        '4': variable_parser, # stack
        '5': variable_parser, # stack
        '6': variable_parser, # stack
        '7': variable_parser, # stack
        '8': variable_parser, # stack
        '9': variable_parser, # stack
        'b': variable_parser, # based (allocated fortran array)
        'd': variable_parser, # float register
        'r': variable_parser, # register
        'G': variable_parser, # global
        's': variable_parser, # local
        'S': variable_parser, # global static
        'V': variable_parser, # local static
        # type
        't': type_parser,
        # composite
        'T': composite_parser,
    }

    def __init__(self, stabs, ctx):
        self.stabs = stabs
        self.ctx = ctx
        self.qualifiers = []

        self.c_repr = None

        self.name,info = stabs.split(":", 1) if ':' in stabs else (stabs, None)

        if self.name.startswith("$LB"):
            # block scope info

            assert info is None
            blockno = int(self.name[4:])
            if self.name[3] == 'B':
                self.c_repr = "{"
            elif self.name[3] == 'E':
                self.c_repr = "}"
        elif info is not None:
            # other info
            stream = STABSStream(info)
            STABS.dispatch_table.get(stream.peek_char(), STABS.other_parser)(self, stream)
            # stream should be fully parsed
            assert stream.is_empty() , stream.stream
