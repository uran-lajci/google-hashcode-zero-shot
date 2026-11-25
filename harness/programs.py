"""Take the program out of each reply and compile it (sections 5 to 7 of PROTOCOL.md)."""

import concurrent.futures
import hashlib
import re
import shutil
import subprocess
import time
from pathlib import Path

from . import config
from .store import load_json, reply_file, write_json

FENCE = re.compile(r' {0,3}(`{3,}|~{3,})(.*)')


def sandbox(read_only: dict[Path, str], writable: dict[Path, str] | None = None, directory: str = '/tmp') -> list[str]:
    """The start of a command that runs inside bubblewrap.

    The command sees the system libraries and the files named here, and nothing else of the machine:
    no home folder, no network. Compilers and programs made from model output run this way.
    """
    command = [
        'bwrap', '--unshare-all', '--die-with-parent', '--clearenv',
        '--setenv', 'PATH', '/usr/bin', '--setenv', 'LC_ALL', 'C',
        '--ro-bind', '/usr', '/usr',
        '--symlink', 'usr/lib', '/lib', '--symlink', 'usr/lib64', '/lib64', '--symlink', 'usr/bin', '/bin',
        '--proc', '/proc', '--dev', '/dev', '--tmpfs', '/tmp',
    ]
    for source, target in read_only.items():
        command += ['--ro-bind', str(source), target]
    for source, target in (writable or {}).items():
        command += ['--bind', str(source), target]
    return command + ['--chdir', directory]


def code_blocks(text: str) -> list[str]:
    """The contents of the fenced code blocks of a Markdown text. A block that is never closed is left out."""
    blocks = []
    fence = None
    lines: list[str] = []
    for line in text.split('\n'):
        if fence is None:
            opening = FENCE.fullmatch(line)
            # The line after a backtick fence names the language and cannot hold a backtick.
            if opening and not (opening.group(1)[0] == '`' and '`' in opening.group(2)):
                fence = opening.group(1)
                lines = []
        elif re.fullmatch(rf' {{0,3}}{fence[0]}{{{len(fence)},}}[ \t\r]*', line):
            blocks.append(''.join(f'{code_line}\n' for code_line in lines))
            fence = None
        else:
            lines.append(line)
    return blocks


def extract(message: dict) -> dict:
    """Decide whether the reply holds a program, and which code block it is."""
    text = ''.join(block['text'] for block in message['content'] if block['type'] == 'text')
    blocks = code_blocks(text)
    result = {'state': 'no_program', 'reason': '', 'code_blocks': len(blocks), 'flagged': len(blocks) > 1}

    if message['stop_reason'] == 'refusal':
        return result | {'reason': 'refusal'}
    if message['stop_reason'] == 'max_tokens':
        return result | {'reason': 'cut off at the output limit'}
    if not blocks:
        return result | {'reason': 'no code block'}
    # With several code blocks the longest is the program; max() keeps the first of equally long ones.
    return result | {'state': 'program', 'source': max(blocks, key=len)}


def compile_program(source_file: Path, build_dir: Path) -> dict:
    """Compile inside the sandbox. The binary is build_dir/program."""
    shutil.rmtree(build_dir, ignore_errors=True)
    build_dir.mkdir(parents=True)
    command = [
        *sandbox({source_file: '/source/program.cpp'}, {build_dir: '/build'}, directory='/build'),
        *config.COMPILER, '-o', '/build/program', '/source/program.cpp',
    ]
    started = time.monotonic()
    try:
        done = subprocess.run(
            command, stdin=subprocess.DEVNULL, capture_output=True, timeout=config.COMPILE_TIME_LIMIT_SECONDS
        )
        compiled = done.returncode == 0 and (build_dir / 'program').exists()
        output = (done.stdout + done.stderr).decode('utf-8', errors='replace')
    except subprocess.TimeoutExpired:
        compiled = False
        output = f'stopped after {config.COMPILE_TIME_LIMIT_SECONDS} seconds\n'
    return {'compiled': compiled, 'compile_seconds': round(time.monotonic() - started, 3), 'compile_output': output}


def prepare_one(stage: config.Stage, program: config.Program) -> dict:
    """Extract and compile the program of one reply. Returns its record, which is also stored."""
    reply = load_json(reply_file(stage, program))
    record = {
        'problem': program.problem, 'model': program.model, 'generation': program.generation,
        **extract(reply['message']),
    }
    folder = stage.out_dir / 'programs' / program.key.parent
    folder.mkdir(parents=True, exist_ok=True)
    source = record.pop('source', None)

    if source is not None:
        source_file = folder / f'{program.key.name}.cpp'
        source_file.write_text(source, encoding='utf-8')
        record['source_bytes'] = len(source.encode('utf-8'))
        record['source_sha256'] = hashlib.sha256(source.encode('utf-8')).hexdigest()

        compiled = compile_program(source_file, stage.out_dir / 'build' / program.key)
        compile_log = folder / f'{program.key.name}.compile.txt'
        if compiled['compile_output']:
            compile_log.write_text(compiled['compile_output'], encoding='utf-8')
        else:
            compile_log.unlink(missing_ok=True)
        record['compile_seconds'] = compiled['compile_seconds']
        if not compiled['compiled']:
            record |= {'state': 'compile_error', 'reason': 'g++ fails'}

    write_json(folder / f'{program.key.name}.json', record)
    return record


def binary(stage: config.Stage, program: config.Program) -> Path:
    return stage.out_dir / 'build' / program.key / 'program'


def require_replies(stage: config.Stage) -> None:
    """Refuse to go on while a reply is missing: all programs are generated before any program is run (section 4)."""
    programs = stage.programs()
    missing = [program for program in programs if not reply_file(stage, program).exists()]
    if missing:
        raise SystemExit(
            f'{len(missing)} of {len(programs)} replies are missing, the first is {missing[0]}: '
            'all programs are generated before any program is run'
        )


def prepare(stage: config.Stage) -> dict[config.Program, dict]:
    """Extract and compile every program."""
    require_replies(stage)
    programs = stage.programs()
    with concurrent.futures.ThreadPoolExecutor(max_workers=len(config.CPUS)) as pool:
        records = list(pool.map(lambda program: prepare_one(stage, program), programs))
    return dict(zip(programs, records))
