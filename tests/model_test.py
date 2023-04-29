import rq_analysis.simulation as rq

m = rq.Model()
print(m)
print(m.components())
m.add_producer(rq.MMPPInput([1,1],[[-0.5,0.5],[0.5,-0.5]],0,0),'node')
print(m.components())
print(m.component_at('node'))
print(m.component_at('node').produce(0))
print(m.component_at('node').outputs())