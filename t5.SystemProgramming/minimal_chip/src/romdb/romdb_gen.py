from urllib.request import urlretrieve
from tempfile import TemporaryDirectory
from typing import NamedTuple, TextIO, Any, Mapping
from collections.abc import Iterable
from datetime import datetime

import zipfile
import json

from pathlib import Path
from types import SimpleNamespace as Named

class C89Config(NamedTuple):
    platforms:list[str]
    header_name: str 
    source_name: str
    array_name:str
    romdbtype:str
    gettername: str
    platform_enum_name:str
    platform_enum_prefix:str
    @property
    def header_guard(self):
        return C89Writer.sanitize_identifier(f"{self.header_name}".upper())

class WebConfig(NamedTuple):
    download:bool
    url:str
    prefix:str | None
    allow_overwrite:bool

class Config(NamedTuple):
    web:WebConfig
    c89:C89Config

CONFIG = Config(
    web=WebConfig(
        download=False,
        url='https://github.com/chip-8/chip-8-database/archive/refs/heads/master.zip',
        prefix = 'chip-8-database-master/database/',
        allow_overwrite=True
    ),
    c89=C89Config(
        platforms=['originalChip8', 'hybridVIP', 'modernChip8', 'CHIP-8X', 'chip48', 'superchip1', 'superchip'],
        header_name='romdb.h',
        source_name='romdb.c',
        array_name='romdb_entries',
        romdbtype='romdb_entry',
        gettername='romdb_get',
        platform_enum_name='romdb_platforms',
        platform_enum_prefix='ROMDB_PLATFORM' 
    )
)


class ZipDownloader:
    @classmethod
    def check(cls, cond:bool, msg:str):
        if not cond:
            raise Exception(msg)

    @classmethod
    def download_impl(cls, url:str, prefix:str | None, allow_overwrite:bool, out_path:Path):
        cls.check(out_path.is_dir(), 'The output path must be a directory')
        tmp, _hdrs = urlretrieve(url)
        if prefix is None: prefix = ''
        with zipfile.ZipFile(tmp, 'r') as zip:
            for info in zip.infolist():
                if not info.is_dir() and info.filename.startswith(prefix):
                    dest = out_path / info.filename[len(prefix):]
                    dest.parent.mkdir(parents=True, exist_ok=True)
                    if not allow_overwrite and dest.exists(): 
                        raise FileExistsError(f"Some files already exist: {dest}")
                    with zip.open(info) as src:
                        dest.write_bytes(src.read())

    @classmethod
    def download(cls, cfg:WebConfig, out_path:Path):
        if not cfg.download:
            return
        cls.download_impl(cfg.url, cfg.prefix, cfg.allow_overwrite, out_path)


def get_first_value_from_dicts(key: str, default = None, *dicts: Mapping[str, Any]) -> Any:
    _MISSING = object()
    for d in dicts:
        v = d.get(key, _MISSING)
        if v is not _MISSING: return v
    return default

class C89EnumLayout(NamedTuple):
    name:str
    items:dict[str, str]

class C89Writer:
    def __init__(self, out: TextIO):
        self.out = out
        self._indent = 0
        self._need_indent = False

    def indent(self): self._indent += 1
    def dedent(self): self._indent = max(0, self._indent - 1)

    def line(self, s: str):
        if self._need_indent:
            self.out.write("   " * self._indent)
            self._need_indent = False
        self.out.write(s)

    def linenl(self, s: str = ""):
        self.line(s+'\n')
        self._need_indent = True

    def write_block_comment(self, text:str):
        lines = text.split('\n')
        self.linenl("/*")
        for l in lines:
            self.linenl(f" * {l}")
        self.linenl(" */")

    def write_include_guard(self, guard:str, is_open: bool):
        if is_open:
            self.linenl(f"#ifndef {guard}")
            self.linenl(f"#define {guard}")
        else:
            self.linenl(f"#endif /*{guard} */")    

    def write_include(self, name:str, double_quote:bool):
        q = '"' if double_quote else "<"
        q2 = '"' if double_quote else ">"
        self.linenl(f"#include {q}{name}{q2}")

    def write_enum(self, enum_id: str, prefix:str, elems:list[str], write_max:bool = False) -> C89EnumLayout:
        self.linenl(f'typedef enum {enum_id} {{')
        self.indent()

        #used = set()
        items_map = {}
        n = len(elems)

        for i, raw in enumerate(elems):
            raw = str(raw)
            item = f"{prefix}_{self.sanitize_identifier(raw).upper()}"
            if item in items_map.values():
                raise ValueError(f"Duplicate enum item after sanitizing: {raw} -> {item}")
            items_map[raw] = item

            is_last = (i == n - 1) and not write_max
            comma = "" if is_last else ","
            self.linenl(f"{item} = {i}{comma}")

        if write_max:
            self.linenl(f"{prefix}_MAX = {n}")

        self.dedent()
        self.linenl(f"}} {enum_id};")
        return C89EnumLayout(enum_id, items_map)

    #TODO write string literal instead of escape
    @classmethod
    def escape(cls, s: str) -> str:
        """Escape Python string into a valid C string literal content (no surrounding quotes).
        Uses \\xNN escapes and inserts \"\" to prevent hex-escape swallowing.
        """
        isxdigit = lambda ch: ch.isdigit() or ('a' <= ch.lower() <= 'f')
        out: list[str] = []
        prev_was_hex_escape = False

        for ch in s:
            o = ord(ch)

            # If previous emitted sequence was \xNN and current is hex digit => split literal
            if prev_was_hex_escape and isxdigit(ch):
                out.append('""')  # end current literal + start new literal
                prev_was_hex_escape = False

            if ch == '\\':
                out.append('\\\\'); prev_was_hex_escape = False
            elif ch == '"':
                out.append('\\"'); prev_was_hex_escape = False
            elif ch == '\n':
                out.append('\\n'); prev_was_hex_escape = False
            elif ch == '\r':
                out.append('\\r'); prev_was_hex_escape = False
            elif ch == '\t':
                out.append('\\t'); prev_was_hex_escape = False
            elif 32 <= o <= 126:
                out.append(ch); prev_was_hex_escape = False
            else:
                # emit bytes; for unicode >255 encode as utf-8 bytes
                if o <= 0xFF:
                    out.append(f'\\x{o:02X}')
                    prev_was_hex_escape = True
                else:
                    for bb in ch.encode('utf-8'):
                        out.append(f'\\x{bb:02X}')
                        prev_was_hex_escape = True  # last emitted is hex escape

        return ''.join(out)

    @classmethod
    def sanitize_identifier(cls, name: str) -> str:
        import re
        sanitized = re.sub(r'\W', '_', name)
        if not re.match(r'[A-Za-z_]', sanitized[0]):
            sanitized = '_' + sanitized
        return sanitized

class RomEntry(NamedTuple):
    sha1:str
    platform:str
    tickrate:int
    desc:str

class C89Generator:
    def __init__(self, cfg: C89Config, romdb_map:dict[str, RomEntry]):
        self.cfg = cfg
        self.romdb_map = romdb_map
        #used_platforms = {entry.platform for entry in romdb_map.values()}
        #self.used_platforms = [platform for platform in cfg.platforms if platform in used_platforms]
        #We store all platforms so as not to break the code that depends on this enumeration.
        self.used_platforms = cfg.platforms
        self.platform_enum = None
        self.start_datetime = datetime.now()
        
    def write_platform_enum(self, w:C89Writer):
        self.platform_enum = w.write_enum(self.cfg.platform_enum_name, self.cfg.platform_enum_prefix, self.used_platforms, True)

    def write_romdb_def(self, w: C89Writer):
        model = f"""
typedef struct {self.cfg.romdbtype} {{
   const char *sha1;
   {self.platform_enum.name} platform;
   int tickrate;
   const char* desc;
}} {self.cfg.romdbtype};
"""
        w.linenl(model[1:-1])

    def write_romdb_val(self, entry:RomEntry, w: C89Writer):
        sha1 = C89Writer.escape(entry.sha1)
        platform = self.platform_enum.items[entry.platform]
        tickrate = entry.tickrate
        desc = C89Writer.escape(entry.desc)
        w.line(f'{{ "{sha1}", {platform}, {tickrate}, "{desc}"}}')

    def write_romdb_getter(self, w: C89Writer, isdef:bool):
        romdbtype = self.cfg.romdbtype
        gettername= self.cfg.gettername
        arrname = self.cfg.array_name

        getterdef = f"const {romdbtype}* {gettername}(const char* sha1)"

        if isdef:
            w.linenl(f"{getterdef};")
            return

        getterimpl = f"""
{getterdef} {{
    int i;
    if (!sha1) return NULL;
    for (i = 0; i < (int)(sizeof({arrname}) / sizeof({arrname}[0])); ++i) {{
        if (strcmp({arrname}[i].sha1, sha1) == 0) return &{arrname}[i];
    }}
    return NULL;
}}
"""
        w.linenl(getterimpl[1:-1])


    def write_top_comment(self, w: C89Writer):
        dt= self.start_datetime.strftime("%Y.%m.%d %H:%M:%S")
        comment = f"""
Auto-generated by romdb v0.0.1. Do not edit manually.
Timestamp: {dt}
"""
        w.write_block_comment(comment[1:-1])

    def write_data_array(self, w:C89Writer):
        entries = sorted(self.romdb_map.values(), key=lambda e: e.sha1)
        w.linenl(f"const {self.cfg.romdbtype} {self.cfg.array_name}[] = {{")
        w.indent()
        for i, e in enumerate(entries):
            self.write_romdb_val(e, w)
            if i != len(entries) - 1:
                w.linenl(",")

        w.dedent()
        w.linenl("\n};")
        #w.line(f'const int romdb_entries_count = (int)(sizeof({self.cfg.array_name}) / sizeof({self.cfg.array_name}[0]));')   

    def write_header(self):
        with open(self.cfg.header_name, 'w', encoding='utf-8', newline='\n') as f:
            w = C89Writer(f)
            self.write_top_comment(w)
            w.write_include_guard(self.cfg.header_guard,True)
            self.write_platform_enum(w)
            self.write_romdb_def(w)
            w.linenl(f'extern const {self.cfg.romdbtype} {self.cfg.array_name}[];')
            self.write_romdb_getter(w, True)
            w.write_include_guard(self.cfg.header_guard, False)

    def write_source(self):
        with open(self.cfg.source_name, 'w', encoding='utf-8', newline='\n') as f:
            w = C89Writer(f)
            self.write_top_comment(w)
            w.write_include(self.cfg.header_name, True)
            w.write_include('stddef.h', False)#Needed by NULL
            w.write_include('string.h', False)
            self.write_data_array(w)
            self.write_romdb_getter(w, False)

    def write_all(self):
        self.write_header()
        self.write_source()
  
def build_romdb_map(sha1_hashes_path: Path, platforms_path: Path, programs_path: Path, supported_platforms:list[str]) -> dict[str, RomEntry]:
    sha_map = json.loads(sha1_hashes_path.read_text(encoding='utf-8'))
    platforms = json.loads(platforms_path.read_text(encoding='utf-8'))
    platforms = {p['id']: p for p in platforms if 'id' in p}
    programs = json.loads(programs_path.read_text(encoding='utf-8'))

    out: dict[str, RomEntry] = {}

    for sha1, idx_raw in sha_map.items():
        idx = int(idx_raw)
        if idx < 0 or idx >= len(programs):
            continue

        prog = programs[idx]
        rom = prog['roms'][sha1]
        rom_platforms = rom['platforms']
        if not rom_platforms:
            print(f"Platform is not specified for {rom['file']}, skiped" )
            continue
        
        best_platform = None if supported_platforms else rom_platforms[0]
        for platform in rom_platforms:
            if platform in supported_platforms:
                best_platform = platform
                break
        if best_platform is None:
            print(f"Rom uses an unsupported platform: {rom['file']}")
            continue
        
        tickrate = 0
        if 'tickrate' in rom:
            tickrate = int(rom['tickrate'])
        if tickrate==0:
            tickrate = int(platforms[best_platform]['defaultTickrate'])

        title = get_first_value_from_dicts('title', '', rom, prog)
        release = get_first_value_from_dicts('release', '', rom, prog)
        authors = ','.join(get_first_value_from_dicts('authors', '', rom, prog))
        details = get_first_value_from_dicts('description', '', rom, prog)
        desc = f"""
Title: {title}
Release: {release}
Authors: {authors}
Platform: {best_platform}
{details}
"""[1:-1]
        out[sha1] = RomEntry(sha1=sha1, platform=best_platform, tickrate=tickrate, desc=desc)
    return out

def main():
   tmp = Path(".")
   ZipDownloader.download(CONFIG.web, tmp)
   romdb_map = build_romdb_map(tmp/'sha1-hashes.json', tmp/'platforms.json', tmp/'programs.json', CONFIG.c89.platforms)
   gen = C89Generator(CONFIG.c89, romdb_map)
   gen.write_all()

if __name__ == "__main__":
    main()

#with TemporaryDirectory() as tmp:
    #build(tmp)
#self.used_platforms = list({entry.platform for entry in self.romdb_map.values()})
#self.used_platforms = list(dict.fromkeys(entry.platform for entry in self.romdb_map.values()))