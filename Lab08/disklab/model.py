"""Deterministic single-platter disk timing model; ticks are integer units.
No request preemption, caching, queue depth cap, or background operations.
"""
from dataclasses import dataclass

CYLINDERS=200
SECTORS=12
REV_TICKS=120
SECTOR_TICKS=10
SEEK_PER_CYL=3
TRACK_SKEW_SECTORS=1

@dataclass(frozen=True)
class Request:
    rid: str
    arrival: int
    cylinder: int
    sector: int
    length: int = 1

@dataclass(frozen=True)
class Entry:
    rid: str
    cylinder: int
    dispatch: int
    complete: int
    seek: int
    rotation: int
    transfer: int
    response: int

def validate(reqs):
    ids=set()
    for r in reqs:
        if not isinstance(r.rid,str) or not r.rid or r.rid in ids: raise ValueError('Request IDs must be unique nonempty strings')
        ids.add(r.rid)
        if type(r.arrival) is not int or r.arrival<0: raise ValueError('Invalid arrival')
        if type(r.cylinder) is not int or not (0<=r.cylinder<CYLINDERS): raise ValueError('Invalid cylinder')
        if type(r.sector) is not int or not (0<=r.sector<SECTORS): raise ValueError('Invalid sector')
        if type(r.length) is not int or not (1<=r.length<=SECTORS-r.sector): raise ValueError('Transfer must fit within one track')

def service_estimate(req, now, cylinder):
    seek=abs(req.cylinder-cylinder)*SEEK_PER_CYL
    phase=(now+seek)%REV_TICKS
    target=((req.sector+req.cylinder*TRACK_SKEW_SECTORS)%SECTORS)*SECTOR_TICKS
    rotation=(target-phase)%REV_TICKS
    transfer=req.length*SECTOR_TICKS
    return seek,rotation,transfer

def select_fcfs(ready, now, cylinder, direction):
    return min(ready,key=lambda r:(r.arrival,r.rid)), direction

def select_sstf(ready, now, cylinder, direction):
    return min(ready,key=lambda r:(abs(r.cylinder-cylinder),r.arrival,r.rid)), direction

def select_look(ready, now, cylinder, direction):
    # TODO (students): LOOK, using only arrived requests; preserve direction
    # until no requests remain on that side, then reverse. Tie by arrival, rid.
    raise NotImplementedError('Implement LOOK')

def select_satf(ready, now, cylinder, direction):
    # TODO (students): minimize modeled seek+rotation+transfer time;
    # tie by arrival, rid. Do not minimize only seek distance.
    raise NotImplementedError('Implement SATF')

POLICIES={'fcfs':select_fcfs,'sstf':select_sstf,'look':select_look,'satf':select_satf}

def simulate(reqs, policy='fcfs', initial_cylinder=0, initial_time=0, initial_direction=1):
    reqs=list(reqs)
    validate(reqs)
    if policy not in POLICIES: raise ValueError('Unknown policy')
    if (type(initial_cylinder) is not int or not 0<=initial_cylinder<CYLINDERS
        or type(initial_time) is not int or initial_time<0
        or type(initial_direction) is not int or initial_direction not in (-1,1)):
        raise ValueError('Invalid initial state')
    outstanding={r.rid:r for r in reqs}
    now=initial_time
    cylinder=initial_cylinder
    direction=initial_direction
    trace=[]
    while outstanding:
        ready=[r for r in outstanding.values() if r.arrival<=now]
        if not ready:
            now=min(r.arrival for r in outstanding.values())
            ready=[r for r in outstanding.values() if r.arrival<=now]
        chosen,direction=POLICIES[policy](ready,now,cylinder,direction)
        if chosen not in ready: raise AssertionError('Policy selected a non-ready request')
        seek,rot,transfer=service_estimate(chosen,now,cylinder)
        end=now+seek+rot+transfer
        trace.append(Entry(chosen.rid,chosen.cylinder,now,end,seek,rot,transfer,end-chosen.arrival))
        now=end
        cylinder=chosen.cylinder
        del outstanding[chosen.rid]
    return trace

def metrics(trace):
    # 'makespan' is the final absolute completion tick measured from time zero,
    # including any initial idle interval. It is not total head-busy time.
    if not trace: return {'mean_response':0.0,'max_response':0,'total_seek':0,'makespan':0}
    return {'mean_response':sum(e.response for e in trace)/len(trace),
            'max_response':max(e.response for e in trace),
            'total_seek':sum(e.seek for e in trace),
            'makespan':trace[-1].complete}
