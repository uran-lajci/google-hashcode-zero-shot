"""Run the compiled programs on the instances and score the outputs (sections 6 and 7 of PROTOCOL.md)."""

import concurrent.futures
import datetime
import hashlib
import json
import os
import platform
import queue
import random
import re
import shutil
import signal
import subprocess
import sys
import tempfile
import threading
import time
from pathlib import Path

from . import config, programs
from .store import load_json, repository_state, write_json

SELF_TEST_SOURCE = '''#include <iostream>
#include <string>
int main() { std::string line; std::getline(std::cin, line); std::cout << line << "\\n"; }
'''
STDERR_KEPT_BYTES = 64 * 1024


class HarnessError(Exception):
    """The harness or a judge failed. This is never an outcome of a program."""


def run_program(binary: Path, instance: Path, cpu: int, output: Path) -> dict:
    """Run a program once: one core, the time limit and the memory limit of section 6, inside the sandbox."""
    command = [
        'prlimit', f'--as={config.MEMORY_LIMIT_BYTES}', f'--fsize={config.OUTPUT_LIMIT_BYTES}', '--',
        'taskset', '-c', str(cpu),
        *programs.sandbox({binary: '/program'}), '/program',
    ]
    output.parent.mkdir(parents=True, exist_ok=True)
    stderr_file = output.with_name(output.name + '.stderr')
    timed_out = threading.Event()

    with open(instance, 'rb') as stdin, open(output, 'wb') as stdout, open(stderr_file, 'wb') as stderr:
        started = time.monotonic()
        process = subprocess.Popen(command, stdin=stdin, stdout=stdout, stderr=stderr, start_new_session=True)

        def kill() -> None:
            timed_out.set()
            try:
                os.killpg(process.pid, signal.SIGKILL)
            except ProcessLookupError:
                pass

        timer = threading.Timer(config.TIME_LIMIT_SECONDS, kill)
        timer.start()
        _, status, _ = os.wait4(process.pid, 0)
        seconds = time.monotonic() - started
        timer.cancel()
        # The process is already reaped; tell Popen so that it does not wait for it again.
        process.returncode = os.waitstatus_to_exitcode(status)

    stderr_text = stderr_file.read_bytes()[:STDERR_KEPT_BYTES]
    if stderr_text:
        stderr_file.write_bytes(stderr_text)
    else:
        stderr_file.unlink()
    if stderr_text.startswith(b'bwrap: '):
        raise HarnessError(f'the sandbox failed: {stderr_text.decode(errors="replace").strip()}')

    if timed_out.is_set() and process.returncode == -signal.SIGKILL:
        state = 'timeout'
    elif process.returncode != 0:
        state = 'runtime_error'
    else:
        state = 'finished'
    return {
        'state': state,
        # bubblewrap reports a program that a signal killed as 128 plus the signal number.
        'exit_code': process.returncode,
        'seconds_wall': round(seconds, 3),
        'output_bytes': output.stat().st_size,
        'output_sha256': hashlib.sha256(output.read_bytes()).hexdigest(),
    }


def judge(problem: str, instance: Path, output: Path) -> dict:
    """Score an output with the judge of the problem."""
    command = [sys.executable, '-I', str(config.JUDGE_DIR / problem / 'judge.py'), '-i', str(instance), '-o', str(output)]
    started = time.monotonic()
    done = subprocess.run(command, capture_output=True, text=True)
    seconds = round(time.monotonic() - started, 3)
    stdout, stderr = done.stdout.strip(), done.stderr.strip()

    if done.returncode == 0 and re.fullmatch(r'[0-9]+', stdout):
        return {'state': 'valid', 'score': int(stdout), 'judge_seconds': seconds, 'judge_message': ''}
    if done.returncode == 1 and stderr.startswith('Invalid output: ') and '\n' not in stderr:
        return {'state': 'invalid_output', 'score': 0, 'judge_seconds': seconds, 'judge_message': stderr}
    raise HarnessError(f'the judge of {problem} failed on {output}:\n{stderr or stdout}')


def tool_version(*command: str) -> str:
    return subprocess.run(command, capture_output=True, text=True, check=True).stdout.splitlines()[0]


def fetch_judges() -> None:
    """Clone the judges if needed and check that they are at the commit of section 11, unchanged."""
    def git(*command: str) -> str:
        return subprocess.run(
            ['git', '-C', str(config.JUDGE_DIR), *command], capture_output=True, text=True, check=True
        ).stdout.strip()

    if not config.JUDGE_DIR.exists():
        config.JUDGE_DIR.parent.mkdir(parents=True, exist_ok=True)
        subprocess.run(['git', 'clone', '--quiet', config.JUDGE_REPOSITORY, str(config.JUDGE_DIR)], check=True)
        git('checkout', '--quiet', '--detach', config.JUDGE_COMMIT)
    if not git('rev-parse', 'HEAD').startswith(config.JUDGE_COMMIT):
        raise HarnessError(f'{config.JUDGE_DIR} is not at commit {config.JUDGE_COMMIT}')
    if git('status', '--porcelain', '--untracked-files=no'):
        raise HarnessError(f'{config.JUDGE_DIR} has changed files')


def check_machine(stage: config.Stage) -> dict:
    """Check the tools, the compiler version and the cores, then run a known program through the whole path."""
    for tool in ('g++', 'bwrap', 'prlimit', 'taskset', 'git'):
        if shutil.which(tool) is None:
            raise HarnessError(f'{tool} is not installed')

    compiler = tool_version('g++', '--version')
    if config.COMPILER_VERSION not in compiler:
        raise HarnessError(f'found {compiler!r}, expected g++ {config.COMPILER_VERSION}')

    cores = set()
    for cpu in config.CPUS:
        siblings = Path(f'/sys/devices/system/cpu/cpu{cpu}/topology/thread_siblings_list')
        if not siblings.exists():
            raise HarnessError(f'CPU {cpu} does not exist on this machine')
        cores.add(siblings.read_text().strip())
    if len(cores) != len(config.CPUS):
        raise HarnessError(f'CPUs {config.CPUS} are not on {len(config.CPUS)} different cores')

    with tempfile.TemporaryDirectory(dir=stage.out_dir) as folder:
        source_file = Path(folder) / 'self_test.cpp'
        source_file.write_text(SELF_TEST_SOURCE)
        compiled = programs.compile_program(source_file, Path(folder) / 'build')
        if not compiled['compiled']:
            raise HarnessError(f'the self-test does not compile:\n{compiled["compile_output"]}')
        instance = Path(folder) / 'self_test.in'
        instance.write_text('self-test\n')
        output = Path(folder) / 'self_test.out'
        result = run_program(Path(folder) / 'build' / 'program', instance, config.CPUS[0], output)
        if result['state'] != 'finished' or output.read_text() != 'self-test\n':
            raise HarnessError(f'the self-test run failed: {result}')

    return {
        'utc': datetime.datetime.now(datetime.timezone.utc).isoformat(timespec='seconds'),
        'stage': stage.name,
        'compiler': compiler,
        'compiler_command': ' '.join(config.COMPILER),
        'python': platform.python_version(),
        'kernel': platform.release(),
        'cpu': next(
            (line.split(':', 1)[1].strip() for line in Path('/proc/cpuinfo').read_text().splitlines()
             if line.startswith('model name')), '',
        ),
        'cpus_used': list(config.CPUS),
        'time_limit_seconds': config.TIME_LIMIT_SECONDS,
        'memory_limit_bytes': config.MEMORY_LIMIT_BYTES,
        'judge_commit': config.JUDGE_COMMIT,
        'harness': repository_state(),
    }


class Runs:
    """The runs of a stage: one JSON record and one output file per program and instance."""

    def __init__(self, stage: config.Stage) -> None:
        self.stage = stage

    def record_file(self, program: config.Program, instance: Path) -> Path:
        return self.stage.out_dir / 'runs' / program.key / f'{instance.stem}.json'

    def output_file(self, program: config.Program, instance: Path) -> Path:
        return self.stage.out_dir / 'outputs' / program.key / f'{instance.stem}.out'

    def repeat_file(self, program: config.Program) -> Path:
        return self.stage.out_dir / 'repeats' / program.key.parent / f'{program.key.name}.json'

    def load(self, program: config.Program, instance: Path) -> dict | None:
        return load_json(self.record_file(program, instance))


def on_cores(tasks: list, work) -> None:
    """Do the tasks with at most one at a time on each of the pinned CPUs."""
    free_cpus: queue.Queue[int] = queue.Queue()
    for cpu in config.CPUS:
        free_cpus.put(cpu)

    def pinned(task) -> None:
        cpu = free_cpus.get()
        try:
            work(task, cpu)
        finally:
            free_cpus.put(cpu)

    in_parallel(tasks, pinned, len(config.CPUS))


def in_parallel(tasks: list, work, workers: int) -> None:
    done = 0
    with concurrent.futures.ThreadPoolExecutor(max_workers=workers) as pool:
        futures = [pool.submit(work, task) for task in tasks]
        try:
            for future in concurrent.futures.as_completed(futures):
                future.result()
                done += 1
                print(f'\r  {done} of {len(tasks)}', end='', flush=True)
        except BaseException:
            pool.shutdown(wait=True, cancel_futures=True)
            raise
    if tasks:
        print()


def main(stage: config.Stage, judge_workers: int) -> None:
    programs.require_replies(stage)
    stage.out_dir.mkdir(parents=True, exist_ok=True)
    fetch_judges()
    environment = check_machine(stage)
    log_file = stage.out_dir / 'log' / 'runs.jsonl'
    log_file.parent.mkdir(parents=True, exist_ok=True)
    with open(log_file, 'a', encoding='utf-8') as file:
        file.write(json.dumps(environment) + '\n')

    print('Extracting and compiling the programs')
    records = programs.prepare(stage)
    compiled = [program for program, record in records.items() if record['state'] == 'program']
    print(f'  {len(compiled)} of {len(records)} programs compile')

    runs = Runs(stage)

    def identity(program: config.Program, instance: Path) -> dict:
        return {
            'problem': program.problem, 'model': program.model, 'generation': program.generation,
            'instance': instance.stem,
        }

    tasks = [(program, instance) for program in compiled for instance in stage.instances(program.problem)]
    random.Random(config.SEED).shuffle(tasks)

    # Nothing else runs during this phase, so that the judges do not disturb the run times.
    print('Running the programs')

    def run(task: tuple[config.Program, Path], cpu: int) -> None:
        program, instance = task
        result = run_program(programs.binary(stage, program), instance, cpu, runs.output_file(program, instance))
        write_json(runs.record_file(program, instance), identity(program, instance) | result | {'cpu': cpu})

    on_cores([task for task in tasks if runs.load(*task) is None], run)

    print('Judging the outputs')

    def score(task: tuple[config.Program, Path]) -> None:
        program, instance = task
        record = runs.load(program, instance)
        record |= judge(program.problem, instance, runs.output_file(program, instance))
        write_json(runs.record_file(program, instance), record)

    in_parallel([task for task in tasks if runs.load(*task)['state'] == 'finished'], score, judge_workers)

    # Section 6: one randomly chosen valid run of each program is repeated to check requirement 4.
    print('Repeating one valid run of each program')
    repeats = []
    for program in compiled:
        valid = [
            instance for instance in stage.instances(program.problem)
            if runs.load(program, instance)['state'] == 'valid'
        ]
        if valid and not runs.repeat_file(program).exists():
            repeats.append((program, random.Random(f'{config.SEED} {program}').choice(valid)))

    def repeat(task: tuple[config.Program, Path], cpu: int) -> None:
        program, instance = task
        first = runs.load(program, instance)
        with tempfile.TemporaryDirectory(dir=stage.out_dir) as folder:
            second = run_program(programs.binary(stage, program), instance, cpu, Path(folder) / 'repeat.out')
        write_json(runs.repeat_file(program), identity(program, instance) | {
            'same_output': second['state'] == 'finished' and second['output_sha256'] == first['output_sha256'],
            'state': second['state'],
            'seconds_wall': second['seconds_wall'],
            'first_seconds_wall': first['seconds_wall'],
        })

    on_cores(repeats, repeat)
