# Data

The inputs of the study: the problem statements, the instances and the competition scores for the 17 Google Hash Code qualification and final rounds, 2014 to 2022.

## Layout

```
problems/<problem>/statement.pdf    the official statement, attached to the prompt
problems/<problem>/instances/*.in   the input data sets
scoreboards/<problem>.csv           every team of the round: rank, team, total
scoreboards/best_totals.csv         the best team total of each round
SHA256SUMS                          checksum of every file above
```

`<problem>` is the folder name the problem has in [google-hashcode-judge](https://github.com/uran-lajci/google-hashcode-judge), for example `mentorship_and_teamwork`.

To check the files: `sha256sum -c SHA256SUMS`.

## Sources

Everything comes from [google-hashcode-archive](https://github.com/pierreavn/google-hashcode-archive), commit `a156717b2d61`.

| Files | What was done |
|---|---|
| Statements (17 PDF files, about 21 MB) | Copied unchanged and renamed to `statement.pdf` |
| Instances (76 files, about 62 MB) | Contents unchanged. Every file now ends in `.in`; the archive mixes `.in`, `.txt` and `.in.txt` |
| Scoreboards | The `*_scoreboard.json` files, reduced to rank, team name and total |

## What the scoreboards hold

- `scoreboards/<problem>.csv` has one row per team. The total is the sum over all instances of the round, the example included. These files give the rank a score would have taken.
- `best_totals.csv` has one row per round: the number of teams, the best total and the team that reached it.

The study compares totals only. The archive publishes scores per instance for two of the 17 rounds, so they are not used.

## Ownership

The Hash Code problem statements and instances belong to Google. They are reproduced here so that the prompts and the runs of this study can be rebuilt exactly. The scores belong to the teams that reached them.
