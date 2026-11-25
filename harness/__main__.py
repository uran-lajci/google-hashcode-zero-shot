"""The harness of the study.

python3 -m harness generate    ask the models for the programs
python3 -m harness run         compile the programs, run them, judge the outputs
python3 -m harness report      write the CSV files and print a summary

Each command works on the pilot unless --stage study is given.
"""

import argparse
import dataclasses
from pathlib import Path

from . import config


def main() -> None:
    parser = argparse.ArgumentParser(prog='python3 -m harness', description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument('command', choices=['generate', 'run', 'report'])
    parser.add_argument('--stage', choices=list(config.STAGES), default='pilot')
    parser.add_argument('--out', type=Path, help='store the results in this folder, not in the folder of the stage')
    parser.add_argument('--count-tokens', action='store_true',
                        help='generate: send no generation request, only count the input tokens of each request')
    parser.add_argument('--workers', type=int, default=3, help='generate: requests under way at once')
    parser.add_argument('--judge-workers', type=int, default=6, help='run: outputs judged at once')
    args = parser.parse_args()

    stage = config.STAGES[args.stage]
    if args.out is not None:
        stage = dataclasses.replace(stage, out_dir=args.out.resolve())

    # Each command imports its own module, so that run and report work without the anthropic package.
    if args.command == 'generate':
        from . import generate
        if args.count_tokens:
            generate.count_tokens(stage)
        else:
            generate.main(stage, args.workers)
    elif args.command == 'run':
        from . import execute, report
        try:
            execute.main(stage, args.judge_workers)
        except execute.HarnessError as error:
            raise SystemExit(f'Harness error: {error}')
        print()
        report.main(stage)
    else:
        from . import report
        report.main(stage)


if __name__ == '__main__':
    main()
