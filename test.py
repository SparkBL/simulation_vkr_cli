import rq
import numpy as np
d1 = rq.ExponentialDelay(0.1)

model = rq.RqModel()
print(dir(model.routers))
model.init()
model.add_producer(rq.SimpleInput(d1,0,0),"input")
model.add_producer(rq.SimpleInput(d1,0,0),"call")
model.add_producer(rq.RqtNode(d1,d1),"node")
model.add_producer(rq.Orbit(d1),"orbit")
model.add_producer(rq.StatCollector(5),"stats")


routers ={
    "input_node": rq.Router(),
    "call_node": rq.Router(),
    "orbit_node": rq.Router(),
    "orbit_append_node": rq.Router(),
    "node_stat": rq.Router()
}
print(dir(model.routers))
#print(model.components["input"])
#model.add_connection(model.components["input"],"out_slot",model.components["node"],"in_slot")



#nexts = s.produce(0)[0]
#print(nexts)
#nexts1 = s.produce(nexts)[0]
#print(nexts1)
#a = rout.len()

#print(rout.pop().__members__)

#print(s.queue)
#m.init()
#print(m.e)
#print(m.produce(1))P