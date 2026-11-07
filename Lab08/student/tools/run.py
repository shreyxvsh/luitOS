import argparse, json
from pathlib import Path
from disklab.model import Request,simulate,metrics
p=argparse.ArgumentParser()
p.add_argument('scenario')
p.add_argument('--policy',choices=['fcfs','sstf','look','satf'],default='fcfs')
a=p.parse_args()
data=json.loads(Path(a.scenario).read_text())
reqs=[Request(**d) for d in data['requests']]
trace=simulate(reqs,a.policy,data.get('initial_cylinder',0),data.get('initial_time',0),data.get('initial_direction',1))
for e in trace:print(f'{e.rid:>3}: cyl={e.cylinder:3} dispatch={e.dispatch:5} seek={e.seek:4} rotate={e.rotation:3} transfer={e.transfer:3} complete={e.complete:5} response={e.response:5}')
print(metrics(trace))
