import unittest
from disklab.model import *
class Baseline(unittest.TestCase):
    def test_fcfs_trace(self):
        t=simulate([Request('B',0,0,2),Request('A',0,0,1)],'fcfs')
        self.assertEqual([e.rid for e in t],['A','B'])
        self.assertEqual((t[0].seek,t[0].rotation,t[0].transfer,t[0].complete),(0,10,10,20))
        self.assertEqual((t[1].seek,t[1].rotation,t[1].complete),(0,0,30))
    def test_rotation_continues_during_seek(self):
        r=Request('X',0,1,0)
        self.assertEqual(service_estimate(r,0,0),(3,7,10))
    def test_idle_time(self):
        t=simulate([Request('A',50,0,5)],'fcfs')
        self.assertEqual((t[0].dispatch,t[0].rotation,t[0].complete),(50,0,60))
    def test_sstf_ready_only(self):
        r=[Request('A',0,5,0),Request('B',1000,0,0)]
        self.assertEqual(simulate(r,'sstf')[0].rid,'A')
    def test_invalid_geometry_and_state(self):
        for r in (Request('A',0,-1,0),Request('A',0,200,0),
                  Request('A',0,1,12),Request('A',0,1,0,0),
                  Request('A',0,2.5,0),Request('A',0,2,0.5),
                  Request('A',True,2,0),Request(42,0,2,0)):
            with self.subTest(r=r):
                with self.assertRaises(ValueError): simulate([r])
        for kwargs in ({'initial_cylinder':200},{'initial_time':-1},
                       {'initial_direction':0},{'initial_cylinder':1.5},
                       {'initial_time':3.5}):
            with self.subTest(kwargs=kwargs):
                with self.assertRaises(ValueError): simulate([Request('A',0,1,0)],**kwargs)

    def test_duplicate_id(self):
        with self.assertRaises(ValueError):
            simulate([Request('A',0,1,0),Request('A',1,2,0)])

    def test_bad_transfer(self):
        with self.assertRaises(ValueError):simulate([Request('A',0,0,11,2)])
if __name__=='__main__':unittest.main()
