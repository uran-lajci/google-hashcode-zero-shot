# Experimental protocol

Study: zero-shot solvers written by large language models for Google Hash Code problems.

## 1. Research questions

1. How good is a solver that a model writes in one reply, measured against the teams of the original competition?
2. How much does the result vary across repeated generations by the same model?
3. How do the three models differ in quality, spread, failure rate and cost?

## 2. Problems

All Hash Code qualification and final rounds from 2014 to 2022 are used: 17 problems.

The four practice rounds are excluded because the archive holds no scoreboard for them, so their results cannot be compared with the competition.

| Round | Problem | Instances |
|---|---|---:|
| 2014 final | Street View Routing | 1 |
| 2015 qualification | Optimize a Data Center | 1 |
| 2015 final | Loon | 1 |
| 2016 qualification | Delivery | 3 |
| 2016 final | Satellites | 4 |
| 2017 qualification | Streaming Videos | 4 |
| 2017 final | Router Placement | 4 |
| 2018 qualification | Self-driving Rides | 5 |
| 2018 final | City Plan | 6 |
| 2019 qualification | Photo Slideshow | 5 |
| 2019 final | Compiling Google | 6 |
| 2020 qualification | Book Scanning | 6 |
| 2020 final | Assembling Smartphones | 6 |
| 2021 qualification | Traffic Signaling | 6 |
| 2021 final | Software Engineering at Scale | 6 |
| 2022 qualification | Mentorship and Teamwork | 6 |
| 2022 final | Santa Tracker | 6 |
| | **Total** | **76** |

Every instance is run, including the examples, because the scoreboard totals include them.

The statements, instances and scoreboards come from [google-hashcode-archive](https://github.com/pierreavn/google-hashcode-archive). The statements are the official PDF files. The judges come from [google-hashcode-judge](https://github.com/uran-lajci/google-hashcode-judge).

The statements, the instances and a simplified copy of the scoreboards are stored in this repository, described in [DATA.md](DATA.md).

## 3. Models

| Model | API identifier |
|---|---|
| Claude Opus 5.5 | `claude-opus-5-5` |
| Claude Sonnet 5.5 | `claude-sonnet-5-5` |
| Claude Haiku 5.5 | `claude-haiku-5-5` |

The request settings are the same for all three models:

| Setting | Value | Reason |
|---|---|---|
| Effort | `medium`, set explicitly | The default differs by model: `high` on Sonnet 5.5, `medium` on Opus 5.5 and Haiku 5.5. Leaving it unset would compare the models at different effort levels |
| Thinking | `adaptive`, set explicitly | The only mode with thinking on that all three accept. It is also what they run when the parameter is omitted; setting it keeps the request independent of the defaults |
| Output limit | 64,000 tokens, streamed | High enough that thinking plus a full program is not cut off; all three allow up to 128,000 |

Every call is logged with its date, model identifier, token counts, stop reason and latency.

## 4. Generation

For each problem, each model is asked 10 times: 17 problems × 3 models × 10 generations = 510 programs.

**Zero-shot** means:

- one request with one user message and no system prompt;
- one reply;
- no tools, no web access, no examples of solutions;
- no feedback of any kind, and no repair of a program that fails.

Each generation is a fresh request with the identical prompt. Its only source of variation is the model's own sampling.

A request is repeated only when the API returns a transport or server error. A refusal or a cut-off reply is not repeated; it counts as a failure (section 7). No fallback model is enabled, so every program is written by the model it is attributed to.

All 510 programs are generated and stored before any program is run.

## 5. Prompt

The user message has two parts: the official PDF statement of the problem, attached as a document, followed by the text below. The text is the same for every problem and model.

~~~
The attached PDF is the problem statement of a Google Hash Code problem.

Write a complete program that solves it. The program will be run once on each
input data set of the problem. The data sets are not shown to you. The goal is
the highest total score over all of them, with the score computed as the
statement defines. An output that breaks a rule of the statement scores zero
for that data set.

Requirements:
1. Language: C++17, in a single source file, using only the C++ standard library.
2. The program reads one input data set from standard input and writes the
   submission to standard output, in the format the problem statement defines.
3. It runs on a single thread.
4. It is deterministic: two runs on the same input give the same output.
5. It finishes within 10 seconds on any input data set of the problem, on one
   core of an AMD Ryzen 5 5600H processor.
6. It uses at most 3.5 GB of memory.
7. It will be compiled with: g++ -O2 -std=c++17

Reply with the program in one ```cpp code block.
~~~

The PDF is the file `problems/<problem>/statement.pdf`, unchanged, so the model reads the statement with its figures as the competitors did.

The program is the content of the code block. If a reply holds several code blocks, the longest is taken and the reply is flagged.

## 6. Running

| Setting | Value |
|---|---|
| Machine | AMD Ryzen 5 5600H (6 cores, 12 threads), 16 GB RAM, Ubuntu 24.04 |
| Compiler | g++ 13.3.0, `g++ -O2 -std=c++17` |
| Runs | One per program and instance: 30 programs × 76 instances = 2,280 runs |
| Cores | One core per run, pinned with `taskset`; at most 6 runs at once, on CPUs 0, 2, 4, 6, 8, 10 |
| Time | Killed at 60 seconds of wall-clock time |
| Memory | 3.5 GB |
| Scoring | The judge of the problem, at the commit in section 11 |

The prompt asks for 10 seconds and the harness allows 60. A model cannot measure its program's run time in a zero-shot setting, so the difference is a tolerance. Every run time is recorded, and the main results are repeated with a strict 10-second limit as a sensitivity check.

One run per instance is enough because the programs are required to be deterministic. As a check, one randomly chosen valid run per program is repeated; a different output is recorded as a breach of requirement 4.

## 7. Outcomes of a program and of a run

Each program ends in one of two states before running:

| State | Meaning |
|---|---|
| No program | The reply has no code block, is cut off, or is a refusal |
| Compile error | `g++` fails |

Each run of a compiled program ends in one of four states:

| State | Meaning | Score |
|---|---|---:|
| Runtime error | Non-zero exit, crash or memory limit | 0 |
| Timeout | Not finished at 60 seconds | 0 |
| Invalid output | The judge rejects the output | 0 |
| Valid | The judge accepts the output | Judge score |

A program in the first table scores 0 on every instance.

## 8. Measures

**Total.** The total of a program is the sum of its scores over all instances of its problem, examples included.

**Relative total (main measure).** The total of a program divided by the highest team total on the archived scoreboard of that round. It is comparable across problems.

**Rank.** The position the total would take among the teams on the scoreboard.

**Per-instance score (second measure).** The score of a program on an instance, divided by the best score any of the 30 programs reached on that instance. A program's value is the mean over the non-example instances, so each instance weighs the same. This measure guards against one instance deciding a total.

**Validity.** The share of programs that compile, and the share of runs that are valid.

**Cost.** Input and output tokens, price at the list prices of the run date, and latency, per program.

## 9. Analysis

**Question 1, quality.** For each model and problem: the median, best and worst relative total and rank over the 10 generations. Where [google-hashcode-solvers](https://github.com/uran-lajci/google-hashcode-solvers) has benchmarked the problem, the best open-source total is shown beside them.

**Question 2, spread.** For each model and problem: the interquartile range and the range of the relative total over the 10 generations, and the number of generations with a valid output on every instance. Across problems: the median of these.

**Question 3, models.**

- Each model's median relative total per problem gives 17 paired observations per model. A Friedman test compares the three models, followed by the Nemenyi test at the 5% level, shown as a critical-difference diagram.
- The same test is repeated with the per-instance measure.
- Best of k: for k from 1 to 10, the expected best relative total among k generations of a model, computed exactly from its 10 generations. Plotted against the cost of k generations, this shows whether several cheap generations beat one expensive one.
- Validity and cost per model.

With 17 problems and 3 models, the Nemenyi test separates two models only when their mean ranks differ by about 0.80.

## 10. Pilot

The pilot uses Pizza (2017 practice round), which is not part of the study. Each model generates 2 programs.

The pilot tests the harness from request to judge, checks that no reply is cut off at the output limit, and measures token use and how long the judges take. Its programs are not reported as results.

## 11. Freeze and reporting

The study uses these commits:

| Repository | Commit |
|---|---|
| [google-hashcode-archive](https://github.com/pierreavn/google-hashcode-archive) | `a156717b2d61` |
| [google-hashcode-judge](https://github.com/uran-lajci/google-hashcode-judge) | `d65156d612e5` |

1. Run the pilot and correct the harness. The protocol may still change at this stage, and each change is committed.
2. Tag the commit `protocol-v1` and push the tag. The push time is the freeze time.
3. Make the first study request only after the tag is public.
4. Log every later deviation from this protocol, with its reason, in `DEVIATIONS.md`.
5. Report every generation. No generation is dropped, repeated or replaced.
6. Publish all 510 replies, the programs, the outputs, the judge results and the run log in this repository.

## 12. Limits

- **Unequal conditions.** Competition teams had several hours, several people, any language and no limit on run time. The comparison shows what a zero-shot program with a short run time reaches, not an equal contest.
- **Public problems.** The statements and many solutions are public, and the models may have seen them in training. This affects the level of the scores more than the comparison between three models of one family.
- **One prompt, one language.** Another wording or language could change the results.
