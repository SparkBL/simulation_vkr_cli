import rq_simulation as rq
print(dir(rq))
print("Test inheritance")
d1 = rq.ExponentialDelay(0.1)
d2 = rq.GammaDelay(0.1,0.1)
d3 = rq.UniformDelay(0.1,0.1)
r1 = rq.SimpleInput(d1,0,0)
r2 = rq.RqtNode(d1,d1)
r3 = rq.MMPPInput([1,1],[[-0.5,0.5],[0.5,-0.5]],0,0)
o = rq.Orbit(d1)
s =rq.StatCollector(5)
print(type(d1),type(d2),type(d3))
print(type(r1),type(r2),type(r3))
print(type(o),type(s),type(rq.CustomCollector()))
print("Produce:")
print(r1.produce(0))


m = rq.RqModel()

print(m.components)
m.add_producer(r1,"in")
print(m.components["in"].produce(0))