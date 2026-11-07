"""Functional checks: expected to fail until BOTH TODO policies are implemented."""
import unittest
from disklab.model import Request,simulate,metrics,service_estimate

class Features(unittest.TestCase):
    def test_look_forward_then_reverse(self):
        reqs=[Request('A',0,20,0),Request('B',0,70,0),Request('C',0,60,0)]
        self.assertEqual([e.rid for e in simulate(reqs,'look',50,0,1)],['C','B','A'])

    def test_look_at_current_cylinder(self):
        reqs=[Request('Z',0,40,7),Request('A',0,41,8)]
        self.assertEqual(simulate(reqs,'look',40,0,-1)[0].rid,'Z')

    def test_satf_rotational_counterexample(self):
        reqs=[Request('A',0,48,5),Request('B',0,43,1),Request('C',0,39,4)]
        sstf=simulate(reqs,'sstf',40)
        satf=simulate(reqs,'satf',40)
        self.assertEqual([e.rid for e in sstf],['C','B','A'])
        self.assertEqual([e.rid for e in satf],['A','B','C'])
        self.assertLess(metrics(satf)['mean_response'],metrics(sstf)['mean_response'])
        self.assertGreater(metrics(satf)['total_seek'],metrics(sstf)['total_seek'])

    def test_satf_accounts_for_transfer_length(self):
        reqs=[Request('A',0,40,0,3),Request('B',0,40,0,1)]
        self.assertEqual(simulate(reqs,'satf',40)[0].rid,'B')

    def test_ready_only_with_non_preemption(self):
        reqs=[Request('A',0,45,10),Request('B',1,40,0),Request('C',2,41,4)]
        for policy in ('look','satf'):
            trace=simulate(reqs,policy,40)
            self.assertEqual(trace[0].rid,'A')
            self.assertTrue(all(x.dispatch>=next(r.arrival for r in reqs if r.rid==x.rid) for x in trace))

if __name__=='__main__': unittest.main()
