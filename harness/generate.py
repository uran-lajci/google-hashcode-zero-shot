"""Ask each model for its programs: one request per generation, as sections 3 to 5 of PROTOCOL.md describe."""

import base64
import concurrent.futures
import datetime
import hashlib
import json
import os
import random
import subprocess
import threading
import time
from pathlib import Path

import anthropic

from . import config
from .store import reply_file, repository_state, write_json

# Error types the API can send inside a stream that has already started with status 200.
TRANSIENT_STREAM_ERRORS = {'api_error', 'overloaded_error', 'rate_limit_error', 'timeout_error'}
HTTP_LIBRARIES = {'httpx', 'httpx2', 'httpcore', 'httpcore2'}


def now() -> str:
    return datetime.datetime.now(datetime.timezone.utc).isoformat(timespec='seconds')


def sha256(data: bytes) -> str:
    return hashlib.sha256(data).hexdigest()


def load_api_key() -> None:
    """Take ANTHROPIC_API_KEY from .env unless the environment already has it."""
    env_file = config.ROOT / '.env'
    if os.environ.get('ANTHROPIC_API_KEY') or not env_file.exists():
        return
    for line in env_file.read_text(encoding='utf-8').splitlines():
        name, _, value = line.partition('=')
        if name.strip() == 'ANTHROPIC_API_KEY' and value.strip():
            os.environ['ANTHROPIC_API_KEY'] = value.strip().strip('"\'')


def request_settings(model: str) -> dict:
    """Everything in the request apart from the message. No system prompt, no tools, no fallback model."""
    return {
        'model': model,
        'max_tokens': config.OUTPUT_LIMIT_TOKENS,
        'thinking': config.THINKING,
        'output_config': {'effort': config.EFFORT},
    }


def user_message(statement: bytes) -> list[dict]:
    """The one user message: the PDF statement as a document, then the prompt."""
    return [{
        'role': 'user',
        'content': [
            {
                'type': 'document',
                'source': {
                    'type': 'base64',
                    'media_type': 'application/pdf',
                    'data': base64.standard_b64encode(statement).decode('ascii'),
                },
            },
            {'type': 'text', 'text': config.prompt()},
        ],
    }]


def is_transient(error: Exception) -> bool:
    """Whether the error is a transport or server error, the only kind after which a request is repeated."""
    if isinstance(error, anthropic.APIConnectionError):
        return True
    if isinstance(error, anthropic.APIStatusError):
        if error.status_code in (408, 429) or error.status_code >= 500:
            return True
        if error.status_code == 200 and isinstance(error.body, dict):
            return (error.body.get('error') or {}).get('type') in TRANSIENT_STREAM_ERRORS
        return False
    # A connection that breaks in the middle of a stream can surface as an error of the HTTP library.
    return type(error).__module__.split('.')[0] in HTTP_LIBRARIES


def retry_delay(error: Exception, attempt: int) -> float:
    headers = getattr(getattr(error, 'response', None), 'headers', None) or {}
    try:
        return float(headers.get('retry-after'))
    except (TypeError, ValueError):
        return min(300, 10 * 2 ** (attempt - 1)) + random.uniform(0, 5)


class Log:
    """The request log: one JSON line per event, appended as it happens."""

    def __init__(self, log_file: Path) -> None:
        log_file.parent.mkdir(parents=True, exist_ok=True)
        self.log_file = log_file
        self.lock = threading.Lock()

    def write(self, event: str, **fields: object) -> None:
        line = json.dumps({'utc': now(), 'event': event, **fields})
        with self.lock, open(self.log_file, 'a', encoding='utf-8') as file:
            file.write(line + '\n')


def generate(client: anthropic.Anthropic, stage: config.Stage, program: config.Program, log: Log) -> dict:
    """Make the request of one generation and store the reply."""
    statement = stage.statement(program.problem).read_bytes()
    settings = request_settings(program.model)
    messages = user_message(statement)
    identity = {'problem': program.problem, 'model': program.model, 'generation': program.generation}

    for attempt in range(1, config.REQUEST_ATTEMPTS + 1):
        log.write('request', **identity, attempt=attempt)
        started_utc = now()
        started = time.monotonic()
        seconds_to_first_text = None
        try:
            with client.messages.stream(**settings, messages=messages) as stream:
                for event in stream:
                    if (
                        seconds_to_first_text is None
                        and event.type == 'content_block_delta'
                        and event.delta.type == 'text_delta'
                    ):
                        seconds_to_first_text = round(time.monotonic() - started, 3)
                message = stream.get_final_message()
                request_id = stream.request_id
        except Exception as error:
            transient = is_transient(error)
            log.write(
                'error', **identity, attempt=attempt, transient=transient, error=type(error).__name__,
                status=getattr(error, 'status_code', None), message=str(error)[:500],
                seconds=round(time.monotonic() - started, 3),
            )
            if not transient:
                raise
            time.sleep(retry_delay(error, attempt))
            continue

        record = {
            **identity,
            'request': {
                **settings,
                'statement_sha256': sha256(statement),
                'prompt_sha256': sha256(config.prompt().encode('utf-8')),
                'sdk': f'anthropic {anthropic.__version__}',
            },
            'request_id': request_id,
            'attempts': attempt,
            'started_utc': started_utc,
            'finished_utc': now(),
            'seconds': round(time.monotonic() - started, 3),
            'seconds_to_first_text': seconds_to_first_text,
            'message': json.loads(message.to_json()),
        }
        write_json(reply_file(stage, program), record)
        log.write(
            'reply', **identity, attempt=attempt, request_id=request_id, stop_reason=message.stop_reason,
            input_tokens=message.usage.input_tokens, output_tokens=message.usage.output_tokens,
            seconds=record['seconds'],
        )
        return record

    raise SystemExit(f'{program}: no reply after {config.REQUEST_ATTEMPTS} attempts; run the command again')


def require_frozen_protocol() -> None:
    """Refuse a study request until the tag that freezes the protocol is public (section 11)."""
    def tag_lines(*command: str) -> str:
        return subprocess.run(
            ['git', '-C', str(config.ROOT), *command], capture_output=True, text=True, check=True
        ).stdout.strip()

    if not tag_lines('tag', '--list', config.FREEZE_TAG):
        raise SystemExit(f'the tag {config.FREEZE_TAG} does not exist: the protocol is not frozen yet')
    if not tag_lines('ls-remote', '--tags', 'origin', config.FREEZE_TAG):
        raise SystemExit(f'the tag {config.FREEZE_TAG} is not pushed: the protocol is not public yet')


def count_tokens(stage: config.Stage) -> None:
    """Send no generation request: count the input tokens of each request, which checks the key and the models."""
    load_api_key()
    client = anthropic.Anthropic()
    print(f'{"problem":32}{"model":20}{"input tokens":>13}')
    for problem in stage.problems():
        messages = user_message(stage.statement(problem).read_bytes())
        for model in config.MODELS:
            settings = request_settings(model)
            count = client.messages.count_tokens(
                model=model, messages=messages, thinking=settings['thinking'],
                output_config=settings['output_config'],
            )
            print(f'{problem:32}{model:20}{count.input_tokens:>13,}')


def main(stage: config.Stage, workers: int) -> None:
    if stage.name == 'study':
        require_frozen_protocol()
    load_api_key()
    # The harness repeats requests itself, so that every attempt is in the log.
    client = anthropic.Anthropic(max_retries=0)
    log = Log(stage.out_dir / 'log' / 'requests.jsonl')

    programs = stage.programs()
    pending = [program for program in programs if not reply_file(stage, program).exists()]
    print(f'{len(programs)} generations, {len(programs) - len(pending)} stored, {len(pending)} to request')
    if not pending:
        return
    log.write('start', stage=stage.name, pending=len(pending), workers=workers, **repository_state())

    with concurrent.futures.ThreadPoolExecutor(max_workers=workers) as pool:
        futures = {pool.submit(generate, client, stage, program, log): program for program in pending}
        try:
            for future in concurrent.futures.as_completed(futures):
                record = future.result()
                usage = record['message']['usage']
                print(
                    f'{futures[future]}: {record["message"]["stop_reason"]}, '
                    f'{usage["input_tokens"]:,} tokens in, {usage["output_tokens"]:,} out, '
                    f'{record["seconds"]:.0f} s'
                )
        except BaseException:
            # Requests that have not started are dropped; the ones under way finish and are stored.
            pool.shutdown(wait=True, cancel_futures=True)
            raise
