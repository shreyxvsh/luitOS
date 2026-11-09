# Lab 8 — Optional observation sheet

This is a **non-graded, in-class practice exercise**. No work needs to be submitted. Use this sheet only if it helps you organize your observations.

## Worked calculation
For A and B in `classic.json`, record dispatch, seek, angular phase after the seek, target sector angle, rotational wait, transfer time and completion.

## Policy comparison
| Scenario | Policy | Service order | Seek ticks | Mean response ticks | Max response ticks | Final completion tick |
|---|---|---|---:|---:|---:|---:|
| classic.json | FCFS | | | | | |
| classic.json | SSTF | | | | | |
| classic.json | LOOK | | | | | |
| classic.json | SATF | | | | | |
| rotation_trap.json | FCFS | | | | | |
| rotation_trap.json | SSTF | | | | | |
| rotation_trap.json | LOOK | | | | | |
| rotation_trap.json | SATF | | | | | |

## Construct a new counterexample
Create at least four requests and look for a workload where SSTF uses fewer seek ticks but SATF has lower mean response. Include your chosen initial cylinder, direction, arrival times, sector numbers and full service traces.

## Discussion
- Why does minimizing seek time not always minimize total service time?
- Why does `arrival_trap.json` matter?
- When should LOOK reverse direction?
- Can a greedy policy guarantee bounded waiting under unbounded arrivals?
- Why do these timing formulas not apply to an SSD?
