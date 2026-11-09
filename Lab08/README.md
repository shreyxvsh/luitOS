# CS3106L — Lab 8: Disk Scheduling and Rotational Latency

**Non-graded laboratory practice. No submission is required.**

Use the handout (`Lab08_Student_Handout.pdf` or `.docx`) for the disk model and exercises. This is a small, deterministic HDD timing simulator; it is **not** a patch to the LUIT kernel or a measurement of a real disk. Work in a scratch directory. Python 3 standard library is sufficient.

From this `Lab08/` directory, first run the built-in baseline checks and FCFS example:

```bash
PYTHONPATH=. python3 -m unittest discover -s tests -p 'test_baseline.py' -v
PYTHONPATH=. python3 tools/run.py scenarios/classic.json --policy fcfs
```

Complete the two functions marked `TODO` in `disklab/model.py`:

- `select_look()` — nearest ready request in the current scan direction; reverse only if that side is empty.
- `select_satf()` — choose the ready request with the shortest estimated `seek + rotation + transfer`.

After editing, check both features and explore the scenarios:

```bash
PYTHONPATH=. python3 -m unittest discover -s tests -v
PYTHONPATH=. python3 tools/run.py scenarios/classic.json --policy look
PYTHONPATH=. python3 tools/run.py scenarios/rotation_trap.json --policy sstf
PYTHONPATH=. python3 tools/run.py scenarios/rotation_trap.json --policy satf
```

Baseline tests deliberately pass before student work is complete. The separate feature tests deliberately fail with `NotImplementedError` on the untouched starter. They pass after correct implementations. This is expected.

For class discussion, you may use `OBSERVATION_SHEET.md` to organize calculations. There is **nothing to upload or submit**. The `makespan` field is the final absolute completion tick from time zero, including any idle time; it is not accumulated busy time.
