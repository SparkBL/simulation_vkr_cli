import rq

d1 = rq.ExponentialDelay(0.1)

m = rq.RqModel()
m.init()
rout = rq.Router()
rout.push(rq.Request())
print(rout.pop().status_change_at)
s = rq.SimpleInput(d1,0,0)
s.output_connect('out_slot',rout)
#n.produce(1)
print(s.produce(1))
#print(s.queue)
#m.init()
#print(m.e)
#print(m.produce(1))