"""Where the records of a stage are stored, and how they are written."""

import json
import os
import subprocess
from pathlib import Path

from . import config


def reply_file(stage: config.Stage, program: config.Program) -> Path:
    return stage.out_dir / 'replies' / program.key.parent / f'{program.key.name}.json'


def load_json(json_file: Path) -> dict | None:
    return json.loads(json_file.read_text(encoding='utf-8')) if json_file.exists() else None


def write_json(json_file: Path, record: dict) -> None:
    """Write the file in one step, so that a crash cannot leave half a file behind."""
    json_file.parent.mkdir(parents=True, exist_ok=True)
    temporary = json_file.with_name(json_file.name + '.tmp')
    temporary.write_text(json.dumps(record, indent=1) + '\n', encoding='utf-8')
    os.replace(temporary, json_file)


def repository_state() -> dict:
    """The commit of this repository, to record which harness produced a result."""
    def git(*command: str) -> str:
        return subprocess.run(
            ['git', '-C', str(config.ROOT), *command], capture_output=True, text=True
        ).stdout.strip()

    return {'commit': git('rev-parse', 'HEAD'), 'uncommitted_changes': bool(git('status', '--porcelain'))}
