import q_analysis.simulation as rq
import q_analysis.utils as uu
import numpy as np
import json
import argparse

parser = argparse.ArgumentParser(description='Model runner')
parser.add_argument('inp_i', type=float,
                    help='Input intensity')
parser.add_argument('orb_i', type=float,
                    help='Sigma')
parser.add_argument('node_i', type=float,
                    help='Mu')

args = parser.parse_args()
mod = {'input_intensity':args.inp_i,'orbit_intesity':args.orb_i,'node_intensity':args.node_i}

model = rq.Model()
model.set_time(0) 
model.set_end(200000)
#Добавление элементов
model.add_producer(rq.SimpleInput(rq.ExponentialDelay(args.inp_i),1,0),"input")
model.add_producer(rq.SimpleInput(rq.ExponentialDelay(1),1,0),"call")
model.add_producer(rq.Orbit(rq.ExponentialDelay(args.orb_i)),"orbit")
model.add_producer(rq.RqtNode(rq.ExponentialDelay(args.node_i),rq.ExponentialDelay(1.15)),"node")
#Добавление маршрутизаторов
model.add_connection("input","out_slot","node","in_slot")
model.add_connection("call","out_slot","node","call_slot")
orb_i = model.add_connection("node","orbit_append_slot","orbit","in_slot")
orb_o = model.add_connection("orbit","out_slot","node","orbit_slot")
output = model.add_hanging_output_noqueue("node","out_slot")
#Добавление сборщиков статистики
model.router_at(orb_i).add_reader(rq.AttemptCounter(),"attempt_count")
model.router_at(output).add_reader(rq.AttemptCounter(),"attempt_count")
model.router_at(orb_i).add_reader(rq.TimeCounter(),"count")
model.router_at(orb_o).add_reader(rq.TimeCounter(),"count")


t = c = 0
while True:
    c+=1 
    t = model.next_step()
    model.aggregate(model.component_at("input").produce(t))
    model.aggregate(model.component_at("orbit").produce(t))
    model.aggregate(model.component_at("call").produce(t))
    model.aggregate(model.component_at("node").produce(t))
    model.aggregate(model.component_at("orbit").append(t))
    if model.is_done():
        break
  
r = []
r += [(x,'i') for x in model.router_at(orb_i).reader_at("count").counts]
r += [(x,'o') for x in model.router_at(orb_o).reader_at("count").counts]
r.sort(key=lambda tup: tup[0])
cur_count = 0
cur_time = 0
tmp = [0]*1000
# orbit_size_distrs.append( )
for v in r:
  # while cur_count+1 > len(tmp):
    #  tmp.append(0)
    if cur_count < len(tmp):
        tmp[cur_count] += v[0] - cur_time

    cur_time = v[0]
    if v[1] == 'i':
      cur_count+=1
    if v[1] == 'o':
      cur_count-=1
if cur_time != 0:
    dist = list([x/cur_time for x in  tmp[:1000]])
    upd = {
    'os_distr' : dist,
    'os_mean':np.sum([ i * dist[i] for i in range(0,len(dist))]), #sum(idx * prob)
    'os_std': np.sqrt(np.sum([ (i**2) * dist[i] for i in range(0,len(dist))]) - np.sum([ i * dist[i] for i in range(0,len(dist))]) ** 2),
    'os_var': np.sqrt(np.sum([ (i**2) * dist[i] for i in range(0,len(dist))]) - np.sum([ i * dist[i] for i in range(0,len(dist))]) ** 2)/np.sum([ i * dist[i] for i in range(0,len(dist))]),
    'os_q95':uu.find_nearest(dist,np.quantile(dist,0.95)),
    'os_q99':uu.find_nearest(dist,np.quantile(dist,0.99))
    }
    mod.update(upd)
wt =  model.router_at(orb_i).reader_at("attempt_count").wait_time
wt = [v for k,v in wt.items()]
wtt = np.histogram(wt, density=True,bins=list(range(0,1001)))
upd = {
  'wt_distr': list(wtt[0]),
  'wt_bins': list(wtt[1]),
  'wt_mean': np.mean(wt),
  'wt_std': np.std(wt),
  'wt_var': np.std(wt)/np.mean(wt),
  'wt_q95': np.quantile(wt,0.95),
  'wt_q99': np.quantile(wt,0.99)
  }
mod.update(upd)

class NpEncoder(json.JSONEncoder):
    def default(self, obj):
        if isinstance(obj, np.integer):
            return int(obj)
        if isinstance(obj, np.floating):
            return float(obj)
        if isinstance(obj, np.ndarray):
            return obj.tolist()
        return super(NpEncoder, self).default(obj)
    
with open(f'results/mod_{args.inp_i}_{args.orb_i}_{args.node_i}.json','w', encoding='utf-8') as ff:
    json.dump(mod,ff,cls =NpEncoder)