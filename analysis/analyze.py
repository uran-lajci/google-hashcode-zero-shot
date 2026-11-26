"""The analysis of section 9 of PROTOCOL.md.

.venv/bin/python -m analysis.analyze

Reads results/runs.csv, results/programs.csv and results/replies.csv and the scoreboards.
Writes analysis/tables/*.csv, analysis/figures/* and analysis/RESULTS.md.
"""

import bisect
import collections
import csv
import math
import statistics
from pathlib import Path

from harness import config

OUT = config.ROOT / 'analysis'
MODEL_NAMES = {'claude-opus-5-5': 'Opus 5.5', 'claude-sonnet-5-5': 'Sonnet 5.5', 'claude-haiku-5-5': 'Haiku 5.5'}
PROBLEM_NAMES = {
    'street_view_routing': 'Street View Routing', 'optimize_a_data_center': 'Optimize a Data Center',
    'loon': 'Loon', 'delivery': 'Delivery', 'satellites': 'Satellites', 'streaming_videos': 'Streaming Videos',
    'router_placement': 'Router Placement', 'self_driving_rides': 'Self-driving Rides', 'city_plan': 'City Plan',
    'photo_slideshow': 'Photo Slideshow', 'compiling_google': 'Compiling Google',
    'book_scanning': 'Book Scanning', 'assembling_smartphones': 'Assembling Smartphones',
    'traffic_signaling': 'Traffic Signaling', 'software_engineering_at_scale': 'Software Engineering at Scale',
    'mentorship_and_teamwork': 'Mentorship and Teamwork', 'santa_tracker': 'Santa Tracker',
}
STRICT_TIME_LIMIT_SECONDS = 10
# Critical value of the Nemenyi test for 3 models at the 5% level (Demsar 2006, table 5).
NEMENYI_Q = 2.343


def read_csv(csv_file: Path) -> list[dict]:
    with open(csv_file, encoding='utf-8', newline='') as file:
        return list(csv.DictReader(file))


def write_csv(name: str, rows: list[dict]) -> None:
    (OUT / 'tables').mkdir(parents=True, exist_ok=True)
    with open(OUT / 'tables' / name, 'w', encoding='utf-8', newline='') as file:
        writer = csv.DictWriter(file, fieldnames=list(rows[0]), lineterminator='\n')
        writer.writeheader()
        writer.writerows(rows)


def quartiles(values: list[float]) -> tuple[float, float, float]:
    """The quartiles with linear interpolation between the sorted values."""
    return tuple(statistics.quantiles(values, n=4, method='inclusive'))


def friedman(table: list[list[float]]) -> dict:
    """The Friedman test on rows of paired values, one column per model. A higher value is better and ranks first.

    Returns the mean rank of each column, the statistic corrected for ties and its p-value
    from the chi-square distribution with (columns - 1) degrees of freedom.
    """
    rows, columns = len(table), len(table[0])
    rank_sums = [0.0] * columns
    ties = 0
    for values in table:
        order = sorted(range(columns), key=lambda column: -values[column])
        position = 0
        while position < columns:
            end = position
            while end + 1 < columns and values[order[end + 1]] == values[order[position]]:
                end += 1
            # Tied values share the mean of the ranks they cover.
            for column in order[position:end + 1]:
                rank_sums[column] += (position + end) / 2 + 1
            size = end - position + 1
            ties += size ** 3 - size
            position = end + 1
    statistic = 12 / (rows * columns * (columns + 1)) * sum(total ** 2 for total in rank_sums) - 3 * rows * (columns + 1)
    statistic /= 1 - ties / (rows * columns * (columns ** 2 - 1))
    if columns != 3:
        raise ValueError('the p-value below is written for 3 models')
    return {
        'mean_ranks': [total / rows for total in rank_sums],
        'statistic': statistic,
        # With 2 degrees of freedom the chi-square survival function is exp(-x / 2).
        'p_value': math.exp(-statistic / 2),
        'critical_difference': NEMENYI_Q * math.sqrt(columns * (columns + 1) / (6 * rows)),
    }


def expected_best(values: list[float], k: int) -> float:
    """The expected best of k values drawn without replacement, exact over all subsets of size k."""
    ordered = sorted(values)
    return sum(value * math.comb(index, k - 1) for index, value in enumerate(ordered)) / math.comb(len(ordered), k)


def price(model: str, input_tokens: int, output_tokens: int) -> float:
    input_price, output_price = config.PRICES[model]
    if model == 'claude-haiku-5-5' and input_tokens > config.HAIKU_LONG_PROMPT_TOKENS:
        input_price, output_price = config.HAIKU_LONG_PROMPT_PRICES
    return (input_tokens * input_price + output_tokens * output_price) / 1_000_000


def load() -> dict:
    """Everything the analysis needs, with one record per program."""
    rounds = sorted(read_csv(config.ROOT / 'scoreboards' / 'best_totals.csv'),
                    key=lambda row: (int(row['year']), row['round'] == 'final'))
    problems = [row['problem'] for row in rounds]
    best_team_total = {row['problem']: int(row['best_total']) for row in rounds}
    team_totals = {
        problem: sorted(int(row['total']) for row in read_csv(config.ROOT / 'scoreboards' / f'{problem}.csv'))
        for problem in problems
    }
    open_source = {row['problem']: int(row['best_open_source_total'])
                   for row in read_csv(OUT / 'open_source_totals.csv')}

    runs = read_csv(config.ROOT / 'results' / 'runs.csv')
    for run in runs:
        run['score'] = int(run['score'])
        run['seconds_wall'] = float(run['seconds_wall']) if run['seconds_wall'] else None
        run['example'] = 'example' in run['instance']
        # Section 6: the sensitivity check counts a run longer than 10 seconds as a timeout.
        late = run['seconds_wall'] is not None and run['seconds_wall'] > STRICT_TIME_LIMIT_SECONDS
        run['score_strict'] = 0 if late else run['score']

    best_on_instance: dict[tuple, int] = collections.defaultdict(int)
    for run in runs:
        key = (run['problem'], run['instance'])
        best_on_instance[key] = max(best_on_instance[key], run['score'])

    replies = {(row['problem'], row['model'], row['generation']): row
               for row in read_csv(config.ROOT / 'results' / 'replies.csv')}
    extracted = {(row['problem'], row['model'], row['generation']): row
                 for row in read_csv(config.ROOT / 'results' / 'programs.csv')}

    by_program = collections.defaultdict(list)
    for run in runs:
        by_program[(run['problem'], run['model'], run['generation'])].append(run)

    programs = []
    for (problem, model, generation), program_runs in by_program.items():
        reply = replies[(problem, model, generation)]
        program = extracted[(problem, model, generation)]
        total = sum(run['score'] for run in program_runs)
        total_strict = sum(run['score_strict'] for run in program_runs)
        ratios = [
            run['score'] / best_on_instance[(problem, run['instance'])]
            for run in program_runs
            if not run['example'] and best_on_instance[(problem, run['instance'])] > 0
        ]
        input_tokens, output_tokens = int(reply['input_tokens']), int(reply['output_tokens'])
        programs.append({
            'problem': problem, 'model': model, 'generation': int(generation),
            'state': program['state'],
            'total': total,
            'relative_total': total / best_team_total[problem],
            'rank': 1 + len(team_totals[problem]) - bisect.bisect_right(team_totals[problem], total),
            'per_instance_score': statistics.mean(ratios),
            'valid_on_every_instance': all(run['state'] == 'valid' for run in program_runs),
            'valid_runs': sum(run['state'] == 'valid' for run in program_runs),
            'runs': len(program_runs),
            'runs_over_10_seconds': sum(
                run['seconds_wall'] is not None and run['seconds_wall'] > STRICT_TIME_LIMIT_SECONDS
                for run in program_runs
            ),
            'runs_that_lose_their_score': sum(run['score_strict'] != run['score'] for run in program_runs),
            'total_strict': total_strict,
            'relative_total_strict': total_strict / best_team_total[problem],
            'several_code_blocks': program['flagged'] == 'True',
            'different_output_when_repeated': program['same_output_when_repeated'] == 'False',
            'input_tokens': input_tokens,
            'output_tokens': output_tokens,
            'thinking_tokens': int(reply['thinking_tokens'] or 0),
            'latency_seconds': float(reply['seconds']),
            'cost_usd': price(model, input_tokens, output_tokens),
        })
    programs.sort(key=lambda row: (problems.index(row['problem']), config.MODELS.index(row['model']), row['generation']))

    skipped = sorted(key for key, best in best_on_instance.items() if best == 0 and 'example' not in key[1])
    return {
        'problems': problems, 'rounds': {row['problem']: row for row in rounds},
        'best_team_total': best_team_total, 'team_totals': team_totals, 'open_source': open_source,
        'programs': programs, 'runs': runs, 'instances_without_a_score': skipped,
    }


def of(programs: list[dict], problem: str, model: str, field: str) -> list:
    return [row[field] for row in programs if row['problem'] == problem and row['model'] == model]


def analyse(data: dict) -> dict:
    programs, problems = data['programs'], data['problems']

    # Question 1: quality, and question 2: spread.
    quality, spread = [], []
    for problem in problems:
        for model in config.MODELS:
            relative = of(programs, problem, model, 'relative_total')
            ranks = of(programs, problem, model, 'rank')
            lower, median, upper = quartiles(relative)
            quality.append({
                'problem': problem, 'model': model, 'teams': len(data['team_totals'][problem]),
                'median_relative_total': median, 'best_relative_total': max(relative),
                'worst_relative_total': min(relative),
                'median_rank': statistics.median(ranks), 'best_rank': min(ranks), 'worst_rank': max(ranks),
                'best_open_source_relative_total': (
                    data['open_source'][problem] / data['best_team_total'][problem]
                    if problem in data['open_source'] else ''
                ),
            })
            spread.append({
                'problem': problem, 'model': model,
                'interquartile_range': upper - lower, 'range': max(relative) - min(relative),
                'generations_valid_on_every_instance': sum(of(programs, problem, model, 'valid_on_every_instance')),
            })

    spread_summary = [{
        'model': model,
        **{f'median_{field}': statistics.median(row[field] for row in spread if row['model'] == model)
           for field in ('interquartile_range', 'range', 'generations_valid_on_every_instance')},
    } for model in config.MODELS]

    # Question 3: the models compared on the median of each problem.
    def medians(field: str) -> list[list[float]]:
        return [[statistics.median(of(programs, problem, model, field)) for model in config.MODELS]
                for problem in problems]

    tests = {
        'relative_total': friedman(medians('relative_total')),
        'per_instance_score': friedman(medians('per_instance_score')),
        'relative_total_strict': friedman(medians('relative_total_strict')),
    }
    per_problem = [{
        'problem': problem,
        **{f'{field}_{model}': value
           for field in ('relative_total', 'per_instance_score', 'relative_total_strict')
           for model, value in zip(config.MODELS, medians(field)[index])},
    } for index, problem in enumerate(problems)]

    best_of_k = []
    for model in config.MODELS:
        cost = statistics.mean(row['cost_usd'] for row in programs if row['model'] == model)
        for k in range(1, 11):
            expected = [expected_best(of(programs, problem, model, 'relative_total'), k) for problem in problems]
            best_of_k.append({
                'model': model, 'k': k, 'cost_usd': k * cost,
                'expected_best_relative_total_mean': statistics.mean(expected),
                'expected_best_relative_total_median': statistics.median(expected),
            })

    models = []
    for index, model in enumerate(config.MODELS):
        rows = [row for row in programs if row['model'] == model]
        runs = [run for run in data['runs'] if run['model'] == model]
        models.append({
            'model': model,
            'programs': len(rows),
            'programs_that_compile': sum(row['state'] == 'program' for row in rows),
            'runs': len(runs),
            'valid_runs': sum(run['state'] == 'valid' for run in runs),
            'programs_valid_on_every_instance': sum(row['valid_on_every_instance'] for row in rows),
            'programs_with_different_output_when_repeated': sum(row['different_output_when_repeated'] for row in rows),
            'runs_over_10_seconds': sum(row['runs_over_10_seconds'] for row in rows),
            'runs_that_lose_their_score': sum(row['runs_that_lose_their_score'] for row in rows),
            'median_of_problem_medians_relative_total': statistics.median(
                row[f'relative_total_{model}'] for row in per_problem),
            'median_of_problem_medians_strict': statistics.median(
                row[f'relative_total_strict_{model}'] for row in per_problem),
            'mean_rank_relative_total': tests['relative_total']['mean_ranks'][index],
            'mean_rank_per_instance_score': tests['per_instance_score']['mean_ranks'][index],
            'mean_rank_relative_total_strict': tests['relative_total_strict']['mean_ranks'][index],
            'mean_input_tokens': statistics.mean(row['input_tokens'] for row in rows),
            'mean_output_tokens': statistics.mean(row['output_tokens'] for row in rows),
            'mean_thinking_tokens': statistics.mean(row['thinking_tokens'] for row in rows),
            'programs_without_thinking': sum(row['thinking_tokens'] == 0 for row in rows),
            'mean_cost_usd': statistics.mean(row['cost_usd'] for row in rows),
            'total_cost_usd': sum(row['cost_usd'] for row in rows),
            'median_latency_seconds': statistics.median(row['latency_seconds'] for row in rows),
        })

    return {
        'quality': quality, 'spread': spread, 'spread_summary': spread_summary, 'tests': tests,
        'per_problem': per_problem, 'best_of_k': best_of_k, 'models': models,
    }


def separated(test: dict) -> list[tuple[str, str, float, bool]]:
    """Each pair of models, the difference of their mean ranks and whether the Nemenyi test separates them."""
    pairs = []
    for first in range(len(config.MODELS)):
        for second in range(first + 1, len(config.MODELS)):
            difference = abs(test['mean_ranks'][first] - test['mean_ranks'][second])
            pairs.append((config.MODELS[first], config.MODELS[second], difference,
                          difference > test['critical_difference']))
    return pairs


def markdown_table(header: list[str], rows: list[list], right: int = 1) -> str:
    """A Markdown table whose columns from index `right` on are right-aligned."""
    lines = ['| ' + ' | '.join(header) + ' |',
             '|' + '|'.join('---' if column < right else '---:' for column in range(len(header))) + '|']
    lines += ['| ' + ' | '.join(str(cell) for cell in row) + ' |' for row in rows]
    return '\n'.join(lines)


def number(value: float) -> str:
    return f'{value:,.0f}' if value == int(value) else f'{value:,.1f}'


def report(data: dict, result: dict) -> str:
    problems, programs = data['problems'], data['programs']
    names = [MODEL_NAMES[model] for model in config.MODELS]
    quality = {(row['problem'], row['model']): row for row in result['quality']}
    spread = {(row['problem'], row['model']): row for row in result['spread']}
    models = {row['model']: row for row in result['models']}
    text = ['# Results', '',
            'The analysis of section 9 of [PROTOCOL.md](../PROTOCOL.md). '
            'This file is written by `analysis/analyze.py`; the tables are also in `analysis/tables/` as CSV files.', '',
            f'The data are {len(programs)} programs ({len(problems)} problems, {len(config.MODELS)} models, '
            f'10 generations) and {len(data["runs"])} run records.', '']

    text += ['## Question 1: quality', '',
             'The relative total is the total of a program divided by the best team total of the round. '
             'Each cell gives the median over the 10 generations, then the worst and the best in brackets. '
             'The last column is the best open-source total of '
             '[google-hashcode-solvers](https://github.com/uran-lajci/google-hashcode-solvers) (commit `781e0a0`) '
             'on the same scale; it was measured with a 10-minute limit and without the example instance.', '']
    text.append(markdown_table(
        ['Problem', *names, 'Open source'],
        [[PROBLEM_NAMES[problem],
          *[f'{quality[(problem, model)]["median_relative_total"]:.3f} '
            f'({quality[(problem, model)]["worst_relative_total"]:.3f} to '
            f'{quality[(problem, model)]["best_relative_total"]:.3f})' for model in config.MODELS],
          f'{quality[(problem, config.MODELS[0])]["best_open_source_relative_total"]:.3f}'
          if problem in data['open_source'] else '']
         for problem in problems]
        + [['**Median over the problems**',
            *[f'**{models[model]["median_of_problem_medians_relative_total"]:.3f}**' for model in config.MODELS], '']],
    ))
    text += ['', 'The rank is the position the total would take among the teams of the round. '
             'Each cell gives the median rank over the 10 generations, then the best and the worst in brackets.', '']
    text.append(markdown_table(
        ['Problem', 'Teams', *names],
        [[PROBLEM_NAMES[problem], f'{quality[(problem, config.MODELS[0])]["teams"]:,}',
          *[f'{number(quality[(problem, model)]["median_rank"])} '
            f'({quality[(problem, model)]["best_rank"]:,} to {quality[(problem, model)]["worst_rank"]:,})'
            for model in config.MODELS]]
         for problem in problems],
    ))

    text += ['', '## Question 2: spread', '',
             'For the relative total over the 10 generations: the interquartile range (IQR), the range, '
             'and the number of generations with a valid output on every instance.', '']
    text.append(markdown_table(
        ['Problem', *[f'IQR, {name}' for name in names], *[f'Range, {name}' for name in names],
         *[f'Valid, {name}' for name in names]],
        [[PROBLEM_NAMES[problem],
          *[f'{spread[(problem, model)]["interquartile_range"]:.3f}' for model in config.MODELS],
          *[f'{spread[(problem, model)]["range"]:.3f}' for model in config.MODELS],
          *[spread[(problem, model)]['generations_valid_on_every_instance'] for model in config.MODELS]]
         for problem in problems]
        + [['**Median over the problems**',
            *[f'**{row["median_interquartile_range"]:.3f}**' for row in result['spread_summary']],
            *[f'**{row["median_range"]:.3f}**' for row in result['spread_summary']],
            *[f'**{number(row["median_generations_valid_on_every_instance"])}**' for row in result['spread_summary']]]],
    ))

    text += ['', '## Question 3: models', '', '### Friedman and Nemenyi tests', '',
             f'Each model has {len(problems)} paired observations: its median over the 10 generations on each problem. '
             'Rank 1 is the best model on a problem. The p-value comes from the chi-square distribution '
             f'with 2 degrees of freedom. The Nemenyi test at the 5% level separates two models whose mean ranks '
             f'differ by more than {result["tests"]["relative_total"]["critical_difference"]:.3f}.', '']
    labels = {'relative_total': 'Relative total', 'per_instance_score': 'Per-instance score'}
    text.append(markdown_table(
        ['Measure', *[f'Mean rank, {name}' for name in names], 'Friedman statistic', 'p-value', 'Pairs separated'],
        [[label, *[f'{rank:.3f}' for rank in result['tests'][key]['mean_ranks']],
          f'{result["tests"][key]["statistic"]:.2f}', f'{result["tests"][key]["p_value"]:.2g}',
          '; '.join(f'{MODEL_NAMES[first]} and {MODEL_NAMES[second]}'
                    for first, second, _, apart in separated(result['tests'][key]) if apart) or 'none']
         for key, label in labels.items()],
    ))
    text += ['', '![Critical-difference diagram](figures/critical_difference.png)', '']

    text += ['### Best of k', '',
             'The expected best relative total among k of the 10 generations of a model, exact over all choices '
             'of k generations, averaged over the 17 problems. The cost is k times the mean cost of one generation.', '']
    best = {(row['model'], row['k']): row for row in result['best_of_k']}
    text.append(markdown_table(
        ['k', *[f'{name}, expected best' for name in names], *[f'{name}, cost (USD)' for name in names]],
        [[k, *[f'{best[(model, k)]["expected_best_relative_total_mean"]:.3f}' for model in config.MODELS],
          *[f'{best[(model, k)]["cost_usd"]:.3f}' for model in config.MODELS]] for k in range(1, 11)],
    ))
    text += ['', '![Best of k against cost](figures/best_of_k.png)', '']

    text += ['### Validity and cost', '',
             f'The prices are the list prices in `harness/config.py`, read on {config.PRICES_DATE}. '
             'They were checked against the pricing page on 2026-11-26, the second run date, and had not changed.', '']
    rows = [
        ('Programs that compile', lambda row: f'{row["programs_that_compile"]} of {row["programs"]} '
                                              f'({row["programs_that_compile"] / row["programs"]:.1%})'),
        ('Valid runs', lambda row: f'{row["valid_runs"]} of {row["runs"]} ({row["valid_runs"] / row["runs"]:.1%})'),
        ('Programs valid on every instance', lambda row: row['programs_valid_on_every_instance']),
        ('Programs with a different output when repeated', lambda row: row['programs_with_different_output_when_repeated']),
        ('Input tokens per program (mean)', lambda row: f'{row["mean_input_tokens"]:,.0f}'),
        ('Output tokens per program (mean)', lambda row: f'{row["mean_output_tokens"]:,.0f}'),
        ('Of which thinking tokens (mean)', lambda row: f'{row["mean_thinking_tokens"]:,.0f}'),
        ('Programs written without thinking', lambda row: row['programs_without_thinking']),
        ('Cost per program in USD (mean)', lambda row: f'{row["mean_cost_usd"]:.4f}'),
        ('Cost of the 170 programs in USD', lambda row: f'{row["total_cost_usd"]:.2f}'),
        ('Latency per program in seconds (median)', lambda row: f'{row["median_latency_seconds"]:.0f}'),
    ]
    text.append(markdown_table(['', *names], [[label, *[cell(models[model]) for model in config.MODELS]]
                                              for label, cell in rows]))

    strict = result['tests']['relative_total_strict']
    text += ['', '## Sensitivity: a strict 10-second limit', '',
             f'Section 6 repeats the main results with every run longer than {STRICT_TIME_LIMIT_SECONDS} seconds '
             'counted as a timeout with score 0.', '']
    text.append(markdown_table(
        ['', *names],
        [['Runs longer than 10 seconds', *[models[model]['runs_over_10_seconds'] for model in config.MODELS]],
         ['Of which runs that lose a score above 0',
          *[models[model]['runs_that_lose_their_score'] for model in config.MODELS]],
         ['Median relative total over the problems, 60-second limit',
          *[f'{models[model]["median_of_problem_medians_relative_total"]:.3f}' for model in config.MODELS]],
         ['Median relative total over the problems, 10-second limit',
          *[f'{models[model]["median_of_problem_medians_strict"]:.3f}' for model in config.MODELS]],
         ['Mean rank, 60-second limit', *[f'{rank:.3f}' for rank in result['tests']['relative_total']['mean_ranks']]],
         ['Mean rank, 10-second limit', *[f'{rank:.3f}' for rank in strict['mean_ranks']]]],
    ))
    text += ['', f'With the 10-second limit the Friedman statistic is {strict["statistic"]:.2f} '
             f'(p = {strict["p_value"]:.2g}); the Nemenyi test separates: '
             + ('; '.join(f'{MODEL_NAMES[first]} and {MODEL_NAMES[second]}'
                          for first, second, _, apart in separated(strict) if apart) or 'no pair') + '.', '']
    return '\n'.join(text)


def figures(result: dict) -> None:
    import matplotlib
    matplotlib.use('Agg')
    import matplotlib.pyplot as plt

    colors = {'claude-opus-5-5': '#257de0', 'claude-sonnet-5-5': '#f47230', 'claude-haiku-5-5': '#00b894'}
    ink, secondary, grid, axis = '#191919', '#666666', '#e7e7e7', '#c2c2c2'
    plt.rcParams.update({'font.family': 'sans-serif', 'font.size': 10, 'text.color': ink,
                         'axes.edgecolor': axis, 'axes.labelcolor': secondary,
                         'xtick.color': secondary, 'ytick.color': secondary, 'svg.fonttype': 'none'})
    (OUT / 'figures').mkdir(parents=True, exist_ok=True)

    # Critical-difference diagrams: the mean rank of each model on one axis per measure.
    panels = [('relative_total', 'Relative total'), ('per_instance_score', 'Per-instance score')]
    figure, axes = plt.subplots(len(panels), 1, figsize=(7.2, 3.6))
    for plot, (key, title) in zip(axes, panels):
        test = result['tests'][key]
        difference = test['critical_difference']
        order = sorted(range(len(config.MODELS)), key=lambda index: test['mean_ranks'][index])
        plot.set_xlim(0.9, 3.1)
        plot.set_ylim(-0.75, 1.5)
        plot.axhline(0, color=axis, linewidth=1, zorder=1)
        for tick in (1, 1.5, 2, 2.5, 3):
            plot.plot([tick, tick], [-0.06, 0.06], color=axis, linewidth=1, zorder=1)
            plot.text(tick, -0.2, f'{tick:g}', ha='center', va='top', color=secondary, fontsize=9)
        # A bar joins models that the Nemenyi test does not separate.
        level, drawn_to = -0.48, -1
        for start in range(len(order)):
            end = start
            while end + 1 < len(order) and test['mean_ranks'][order[end + 1]] - test['mean_ranks'][order[start]] <= difference:
                end += 1
            # A group inside the group drawn before it adds nothing.
            if end > start and end > drawn_to:
                plot.plot([test['mean_ranks'][order[start]] - 0.02, test['mean_ranks'][order[end]] + 0.02],
                          [level, level], color=secondary, linewidth=3, solid_capstyle='round', zorder=2)
                drawn_to, level = end, level - 0.12
        # The middle label runs to the left of its point and the others to the right, so that close points stay readable.
        sides = ['left', 'right', 'left']
        for place, index in enumerate(order):
            model, rank = config.MODELS[index], test['mean_ranks'][index]
            plot.plot([rank, rank], [0, 0.42], color=axis, linewidth=1, zorder=1)
            plot.plot(rank, 0, 'o', markersize=9, color=colors[model], markeredgecolor='white',
                      markeredgewidth=2, zorder=3)
            plot.text(rank + (0.015 if sides[place] == 'left' else -0.015), 0.45,
                      f'{MODEL_NAMES[model]}  {rank:.2f}', ha=sides[place], va='bottom', fontsize=10)
        plot.plot([3.05 - difference, 3.05], [1.3, 1.3], color=ink, linewidth=1.5, solid_capstyle='butt')
        plot.text(3.05 - difference / 2, 1.36, f'critical difference {difference:.2f}', ha='center', va='bottom',
                  color=secondary, fontsize=9)
        plot.text(0.9, 1.3, title, ha='left', va='center', fontsize=11, fontweight='bold')
        plot.axis('off')
    axes[-1].text(2, -0.72, 'Mean rank over the 17 problems (1 is best). A bar joins models that are not separated.',
                  ha='center', va='center', color=secondary, fontsize=9)
    figure.subplots_adjust(left=0.03, right=0.97, top=0.97, bottom=0.05, hspace=0.25)
    for suffix in ('png', 'pdf'):
        figure.savefig(OUT / 'figures' / f'critical_difference.{suffix}', dpi=200)
    plt.close(figure)

    # Best of k against the cost of k generations.
    figure, plot = plt.subplots(figsize=(7.2, 4.2))
    for model in config.MODELS:
        rows = [row for row in result['best_of_k'] if row['model'] == model]
        costs = [row['cost_usd'] for row in rows]
        values = [row['expected_best_relative_total_mean'] for row in rows]
        plot.plot(costs, values, color=colors[model], linewidth=2, solid_capstyle='round', solid_joinstyle='round',
                  marker='o', markersize=8, markeredgecolor='white', markeredgewidth=2, label=MODEL_NAMES[model],
                  zorder=3)
        plot.annotate(MODEL_NAMES[model], (costs[-1], values[-1]), xytext=(0, 9), textcoords='offset points',
                      ha='center', va='bottom', color=ink, fontsize=10)
        plot.annotate('k = 1', (costs[0], values[0]), xytext=(0, -11), textcoords='offset points',
                      ha='center', va='top', color=secondary, fontsize=9)
        plot.annotate('k = 10', (costs[-1], values[-1]), xytext=(0, -11), textcoords='offset points',
                      ha='center', va='top', color=secondary, fontsize=9)
    plot.set_xscale('log')
    plot.set_xticks([0.01, 0.1, 1], ['$0.01', '$0.10', '$1.00'])
    plot.minorticks_off()
    plot.set_xlabel('Cost of k generations (USD, logarithmic scale)')
    plot.set_ylabel('Expected best relative total')
    plot.grid(True, which='major', color=grid, linewidth=1)
    plot.set_axisbelow(True)
    for side in ('top', 'right'):
        plot.spines[side].set_visible(False)
    plot.tick_params(length=0)
    plot.margins(x=0.08, y=0.18)
    plot.legend(loc='lower right', frameon=False, labelcolor=secondary)
    figure.tight_layout()
    for suffix in ('png', 'pdf'):
        figure.savefig(OUT / 'figures' / f'best_of_k.{suffix}', dpi=200)
    plt.close(figure)


def main() -> None:
    data = load()
    if data['instances_without_a_score']:
        print('No program scores on:', data['instances_without_a_score'])
    result = analyse(data)

    write_csv('programs.csv', data['programs'])
    write_csv('quality.csv', result['quality'])
    write_csv('spread.csv', result['spread'])
    write_csv('spread_summary.csv', result['spread_summary'])
    write_csv('problem_medians.csv', result['per_problem'])
    write_csv('best_of_k.csv', result['best_of_k'])
    write_csv('models.csv', result['models'])
    write_csv('tests.csv', [{
        'measure': key, 'friedman_statistic': test['statistic'], 'p_value': test['p_value'],
        'critical_difference': test['critical_difference'],
        **{f'mean_rank_{model}': rank for model, rank in zip(config.MODELS, test['mean_ranks'])},
        'pairs_separated': '; '.join(f'{first} / {second}' for first, second, _, apart in separated(test) if apart),
    } for key, test in result['tests'].items()])

    figures(result)
    (OUT / 'RESULTS.md').write_text(report(data, result) + '\n', encoding='utf-8')
    print((OUT / 'RESULTS.md').read_text(encoding='utf-8'))


if __name__ == '__main__':
    main()
