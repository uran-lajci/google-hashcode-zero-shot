"""Gather the stored records into three CSV files and print a summary of the stage."""

import csv
import statistics
from pathlib import Path

from . import config
from .execute import Runs
from .store import load_json as load, reply_file


def cost(model: str, input_tokens: int, output_tokens: int) -> float:
    """The price of one request in USD at the list prices in config.py."""
    input_price, output_price = config.PRICES[model]
    if model == 'claude-haiku-5-5' and input_tokens > config.HAIKU_LONG_PROMPT_TOKENS:
        input_price, output_price = config.HAIKU_LONG_PROMPT_PRICES
    return (input_tokens * input_price + output_tokens * output_price) / 1_000_000


def reply_rows(stage: config.Stage) -> list[dict]:
    rows = []
    for program in stage.programs():
        reply = load(reply_file(stage, program))
        if reply is None:
            continue
        message = reply['message']
        usage = message['usage']
        rows.append({
            'problem': program.problem, 'model': program.model, 'generation': program.generation,
            'started_utc': reply['started_utc'],
            'model_returned': message['model'],
            'stop_reason': message['stop_reason'],
            'input_tokens': usage['input_tokens'],
            'output_tokens': usage['output_tokens'],
            'thinking_tokens': (usage.get('output_tokens_details') or {}).get('thinking_tokens', ''),
            'seconds': reply['seconds'],
            'seconds_to_first_text': reply['seconds_to_first_text'] or '',
            'cost_usd': round(cost(program.model, usage['input_tokens'], usage['output_tokens']), 4),
            'attempts': reply['attempts'],
            'request_id': reply['request_id'],
        })
    return rows


def program_rows(stage: config.Stage) -> list[dict]:
    runs = Runs(stage)
    rows = []
    for program in stage.programs():
        record = load(stage.out_dir / 'programs' / program.key.parent / f'{program.key.name}.json')
        if record is None:
            continue
        repeat = load(runs.repeat_file(program)) or {}
        rows.append({
            'problem': program.problem, 'model': program.model, 'generation': program.generation,
            'state': record['state'],
            'reason': record['reason'],
            'code_blocks': record['code_blocks'],
            'flagged': record['flagged'],
            'source_bytes': record.get('source_bytes', ''),
            'source_sha256': record.get('source_sha256', ''),
            'compile_seconds': record.get('compile_seconds', ''),
            'repeated_instance': repeat.get('instance', ''),
            'same_output_when_repeated': repeat.get('same_output', ''),
        })
    return rows


def run_rows(stage: config.Stage, programs: list[dict]) -> list[dict]:
    runs = Runs(stage)
    rows = []
    for row in programs:
        program = config.Program(row['problem'], row['model'], row['generation'])
        for instance in stage.instances(program.problem):
            identity = {
                'problem': program.problem, 'model': program.model, 'generation': program.generation,
                'instance': instance.stem,
            }
            if row['state'] != 'program':
                # Section 7: a reply without a program, or a program that does not compile, scores 0 everywhere.
                rows.append(identity | {'state': row['state'], 'score': 0})
                continue
            record = runs.load(program, instance)
            if record is None:
                continue
            rows.append(identity | {
                'state': record['state'],
                'score': record.get('score', 0),
                'seconds_wall': record['seconds_wall'],
                'exit_code': record['exit_code'],
                'output_bytes': record['output_bytes'],
                'judge_seconds': record.get('judge_seconds', ''),
                'judge_message': record.get('judge_message', ''),
            })
    return rows


RUN_COLUMNS = [
    'problem', 'model', 'generation', 'instance', 'state', 'score', 'seconds_wall', 'exit_code',
    'output_bytes', 'judge_seconds', 'judge_message',
]


def write_csv(csv_file: Path, rows: list[dict], columns: list[str] | None = None) -> None:
    if not rows:
        return
    with open(csv_file, 'w', encoding='utf-8', newline='') as file:
        writer = csv.DictWriter(file, fieldnames=columns or list(rows[0]), restval='', lineterminator='\n')
        writer.writeheader()
        writer.writerows(rows)


def table(header: list[str], rows: list[list]) -> None:
    cells = [header] + [[f'{cell:,}' if isinstance(cell, int) else str(cell) for cell in row] for row in rows]
    widths = [max(len(row[column]) for row in cells) for column in range(len(header))]
    for row in cells:
        print('  ' + '  '.join(cell.ljust(width) for cell, width in zip(row, widths)).rstrip())


def main(stage: config.Stage) -> None:
    replies = reply_rows(stage)
    programs = program_rows(stage)
    runs = run_rows(stage, programs)
    write_csv(stage.out_dir / 'replies.csv', replies)
    write_csv(stage.out_dir / 'programs.csv', programs)
    write_csv(stage.out_dir / 'runs.csv', runs, RUN_COLUMNS)

    expected = len(stage.programs())
    print(f'Stage {stage.name}: {len(replies)} of {expected} replies, {len(programs)} programs extracted, '
          f'{len(runs)} run records')
    if not replies:
        return

    print(f'\nReplies (output limit {config.OUTPUT_LIMIT_TOKENS:,} tokens, prices of {config.PRICES_DATE})')
    table(
        ['model', 'replies', 'input tokens', 'output tokens (median)', 'output tokens (max)',
         'seconds (median)', 'seconds (max)', 'cost USD'],
        [[
            model, len(rows),
            max(row['input_tokens'] for row in rows),
            round(statistics.median(row['output_tokens'] for row in rows)),
            max(row['output_tokens'] for row in rows),
            f'{statistics.median(row["seconds"] for row in rows):.0f}',
            f'{max(row["seconds"] for row in rows):.0f}',
            f'{sum(row["cost_usd"] for row in rows):.2f}',
        ] for model in config.MODELS if (rows := [row for row in replies if row['model'] == model])],
    )
    cut_off = [row for row in replies if row['stop_reason'] == 'max_tokens']
    other = [row for row in replies if row['stop_reason'] not in ('end_turn', 'max_tokens')]
    print(f'  Cut off at the output limit: {len(cut_off)}. Other stop reasons than end_turn: {len(other)}.')
    for row in cut_off + other:
        print(f'    {row["problem"]} {row["model"]} {row["generation"]:02d}: {row["stop_reason"]}')
    served_by_other = [row for row in replies if not row['model_returned'].startswith(row['model'])]
    for row in served_by_other:
        print(f'  {row["problem"]} {row["model"]} {row["generation"]:02d}: answered by {row["model_returned"]}')

    if programs:
        print('\nPrograms')
        states = ['program', 'compile_error', 'no_program']
        table(
            ['model', *states, 'several code blocks', 'different output when repeated'],
            [[
                model,
                *[sum(row['state'] == state for row in rows) for state in states],
                sum(row['flagged'] for row in rows),
                sum(row['same_output_when_repeated'] is False for row in rows),
            ] for model in config.MODELS if (rows := [row for row in programs if row['model'] == model])],
        )

    if runs:
        print('\nRuns')
        states = ['valid', 'invalid_output', 'timeout', 'runtime_error', 'compile_error', 'no_program']
        table(
            ['model', *states, 'not judged yet'],
            [[
                model,
                *[sum(row['state'] == state for row in rows) for state in states],
                sum(row['state'] == 'finished' for row in rows),
            ] for model in config.MODELS if (rows := [row for row in runs if row['model'] == model])],
        )

        print('\nTotals (sum of the scores over the instances of the problem)')
        totals: dict[tuple, int] = {}
        for row in runs:
            key = (row['problem'], row['model'], row['generation'])
            totals[key] = totals.get(key, 0) + row['score']
        table(['problem', 'model', 'generation', 'total'], [[*key[:2], f'{key[2]:02d}', total] for key, total in totals.items()])

        executed = [row for row in runs if 'seconds_wall' in row]
        if executed:
            print(f'\nInstances (time limit {config.TIME_LIMIT_SECONDS} seconds)')
            rows = []
            for problem in stage.problems():
                for instance in stage.instances(problem):
                    on_instance = [
                        row for row in executed if row['problem'] == problem and row['instance'] == instance.stem
                    ]
                    if not on_instance:
                        continue
                    judged = [row['judge_seconds'] for row in on_instance if row['judge_seconds'] != '']
                    rows.append([
                        problem, instance.stem, len(on_instance),
                        max((row['score'] for row in on_instance), default=0),
                        f'{max(row["seconds_wall"] for row in on_instance):.2f}',
                        f'{max(judged):.2f}' if judged else '',
                    ])
            table(
                ['problem', 'instance', 'runs', 'best score', 'longest run (s)', 'longest judging (s)'],
                rows,
            )
