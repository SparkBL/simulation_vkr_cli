import rq_analysis.simulation as rq
print(dir(rq))
print("Test inheritance")
d1 = rq.ExponentialDelay(0.1)
d2 = rq.GammaDelay(0.1,0.1)
d3 = rq.UniformDelay(0.1,0.1)
r1 = rq.SimpleInput(d1,0,0)
r2 = rq.RqtNode(d1,d1)
r3 = rq.MMPPInput([1,1],[[-0.5,0.5],[0.5,-0.5]],0,0)
o = rq.Orbit(d1)

print(type(d1),type(d2),type(d3))
print(type(r1),type(r2),type(r3))
print(type(o))
print("Produce:")
#print(r1.produce(0))


m = rq.Model()

print(m.components())
m.add_producer(r1,"in")
print(m.components())
print(m.components())
print(m.component_at("in"))
print(m.component_at("in"))
print(m.component_at("in"))
print(m.component_at("in").produce(0))

mods = [m]

print(mods[0].components())
print(mods[0].components())
print(mods[0].component_at("in"))
print(mods[0].component_at("in"))
print(mods[0].component_at("in").produce(0))
print(mods[0].component_at("in").produce(0))
print(mods[0].component_at("in").produce(0))