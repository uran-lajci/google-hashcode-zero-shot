"""The settings of the study. Every value is taken from PROTOCOL.md unless a comment says otherwise."""

import dataclasses
import functools
import re
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent

# Section 3: models and request settings.
MODELS = ('claude-opus-5-5', 'claude-sonnet-5-5', 'claude-haiku-5-5')
EFFORT = 'medium'
THINKING = {'type': 'adaptive'}
OUTPUT_LIMIT_TOKENS = 64_000

# Section 6: running.
COMPILER = ('g++', '-O2', '-std=c++17')
COMPILER_VERSION = '13.3.0'
CPUS = (0, 2, 4, 6, 8, 10)
TIME_LIMIT_SECONDS = 60
MEMORY_LIMIT_BYTES = int(3.5 * 2**30)

# Section 11: the judges.
JUDGE_REPOSITORY = 'https://github.com/uran-lajci/google-hashcode-judge'
JUDGE_COMMIT = 'd65156d612e5'
JUDGE_DIR = ROOT / '.cache' / 'google-hashcode-judge'
FREEZE_TAG = 'protocol-v1'

# Not in the protocol: guards that keep a faulty program from stalling the harness or filling the disk.
COMPILE_TIME_LIMIT_SECONDS = 600
OUTPUT_LIMIT_BYTES = 2**30

# Not in the protocol: the seed for the order of the runs and for the choice of the repeated run.
SEED = 2026

# Not in the protocol: a request that fails with a transport or server error is tried this many times.
REQUEST_ATTEMPTS = 8

# List prices in USD per million tokens, as (input, output), read on PRICES_DATE.
# Section 8 asks for the prices of the run date, so check them again on that date.
PRICES_DATE = '2026-11-03'
PRICES = {
    'claude-opus-5-5': (4.00, 20.00),
    'claude-sonnet-5-5': (2.00, 10.00),
    'claude-haiku-5-5': (0.10, 0.50),
}
# Claude Haiku 5.5 costs more when the prompt is longer than this many tokens.
HAIKU_LONG_PROMPT_TOKENS = 100_000
HAIKU_LONG_PROMPT_PRICES = (0.50, 2.50)


@dataclasses.dataclass(frozen=True)
class Program:
    """One generation: the program a model writes for a problem."""

    problem: str
    model: str
    generation: int

    @property
    def key(self) -> Path:
        return Path(self.problem) / self.model / f'{self.generation:02d}'

    def __str__(self) -> str:
        return f'{self.problem} {self.model} {self.generation:02d}'


@dataclasses.dataclass(frozen=True)
class Stage:
    name: str
    problems_dir: Path
    out_dir: Path
    generations: int

    def problems(self) -> list[str]:
        return sorted(folder.name for folder in self.problems_dir.iterdir() if folder.is_dir())

    def statement(self, problem: str) -> Path:
        return self.problems_dir / problem / 'statement.pdf'

    def instances(self, problem: str) -> list[Path]:
        return sorted((self.problems_dir / problem / 'instances').glob('*.in'))

    def programs(self) -> list[Program]:
        return [
            Program(problem, model, generation)
            for generation in range(1, self.generations + 1)
            for problem in self.problems()
            for model in MODELS
        ]


STAGES = {
    # Section 10: Pizza, 2 programs per model.
    'pilot': Stage('pilot', ROOT / 'pilot' / 'problems', ROOT / 'pilot', generations=2),
    # Section 4: 17 problems, 10 programs per model.
    'study': Stage('study', ROOT / 'problems', ROOT / 'results', generations=10),
}


@functools.cache
def prompt() -> str:
    """The text of the user message: the block of section 5 of PROTOCOL.md, character for character."""
    protocol = (ROOT / 'PROTOCOL.md').read_text(encoding='utf-8')
    section = protocol.split('\n## 5. Prompt\n', 1)[1].split('\n## ', 1)[0]
    blocks = re.findall(r'^~~~\n(.*?)\n~~~$', section, flags=re.DOTALL | re.MULTILINE)
    if len(blocks) != 1:
        raise SystemExit(f'PROTOCOL.md: found {len(blocks)} prompt blocks in section 5, expected 1')
    return blocks[0]
