import q_analysis.simulation as rq
models = []

for i in range(0,10):
    m = rq.Model()
    m.add_producer(rq.MMPPInput([1,1],[[-0.5,0.5],[0.5,-0.5]],0,0),'node')
    models += [m]

for m in models:
    print(m)
    print(m.components())

    print(m.components())
    print(m.component_at('node'))
    print(m.component_at('node').produce(0))
    print(m.component_at('node'))