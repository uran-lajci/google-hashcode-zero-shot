# Harness

The code that carries out [PROTOCOL.md](../PROTOCOL.md): it asks the models for programs, compiles and runs the programs, and scores the outputs with the judges. The pilot and the study use the same code.

## Setup

Needs Linux with Python 3.12, `g++` 13.3.0, `git`, `bwrap` (bubblewrap), `prlimit` and `taskset`.

```
python3 -m venv .venv
.venv/bin/pip install -r requirements.txt
```

Put the API key in `.env` as `ANTHROPIC_API_KEY=...`. The file is not committed.

## Commands

Each command works on the pilot. Add `--stage study` for the study.

| Command | What it does |
|---|---|
| `.venv/bin/python -m harness generate --count-tokens` | Sends no generation request. Counts the input tokens of each request, which checks the key, the models and the statement |
| `.venv/bin/python -m harness generate` | Makes one request per generation and stores each reply. A stored reply is never requested again |
| `python3 -m harness run` | Takes the program out of each reply, compiles it, runs it on every instance, judges the outputs, repeats one valid run per program, then prints the report |
| `python3 -m harness report` | Writes the CSV files again and prints the report |
| `python3 -m unittest harness.test_programs` | Tests the code that takes the program out of a reply |

`run` refuses to start while a reply is missing, because all programs are generated before any program is run. A second `run` continues where the first stopped and repeats no run. `generate --stage study` refuses to start until the tag `protocol-v1` is pushed.

The first `run` clones the judges into `.cache/` and checks that they are at the commit of the protocol.

## What is stored

The pilot is stored in `pilot/`, the study in `results/`.

| Path | Contents |
|---|---|
| `replies/<problem>/<model>/<n>.json` | The full reply of the API, the request settings, the date, the token counts, the stop reason and the latency |
| `log/requests.jsonl` | Every request attempt, also the failed ones |
| `programs/<problem>/<model>/<n>.cpp` | The program taken from the reply |
| `programs/<problem>/<model>/<n>.json` | Whether the reply holds a program, how many code blocks it has, whether it compiles |
| `programs/<problem>/<model>/<n>.compile.txt` | What `g++` printed, if anything |
| `outputs/<problem>/<model>/<n>/<instance>.out` | What the program wrote to standard output. `.out.stderr` holds the first 64 KB of standard error, if any |
| `runs/<problem>/<model>/<n>/<instance>.json` | The outcome of the run: state, score, wall-clock time, exit code, judge message, judging time |
| `repeats/<problem>/<model>/<n>.json` | The repeated run and whether its output is the same |
| `log/runs.jsonl` | The machine, the compiler and the commits of each `run` |
| `replies.csv`, `programs.csv`, `runs.csv` | The records above as three tables |
| `build/` | The compiled programs. Not committed |

## Pilot data

`pilot/problems/pizza` holds the statement and the four instances of Pizza, the practice round of 2017, copied unchanged from [google-hashcode-archive](https://github.com/pierreavn/google-hashcode-archive) at commit `a156717b2d61`.

## Choices the protocol does not state

| Choice | Value | Reason |
|---|---|---|
| Sandbox | Compilers and programs run inside bubblewrap: no network, and of the machine only `/usr` is visible | The programs are model output |
| Memory limit | A limit of 3.5 GiB on the address space, set with `prlimit` | A program that asks for more gets no memory and ends as a runtime error |
| Output limit | 1 GiB per run | A faulty program cannot fill the disk |
| Compile time | 10 minutes, then a compile error | A faulty program cannot stall the harness |
| Repeated requests | After a transport error, HTTP 408, 429 or 5xx, or an error inside a stream: up to 8 attempts | In all these cases the model returned no reply |
| Order of the runs | Shuffled with a fixed seed. The same seed picks the run that is repeated | The models are spread evenly over the time of the runs |
| Judging | After all runs have ended | Judging does not disturb the run times |
