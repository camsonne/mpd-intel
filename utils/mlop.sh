./Daq -out out/1109_mainz/pulser -evt 10000 -min 10  -tsource 2 -inf "Ch=3900, 3900, 3900, ThickScint=1700V, SlimScinti=1100V, Ibeam=25.2nA, Ipico0=1250nA - 1 sample APV mode, 75 ns"

sleep 5

./Daq -out out/1109_mainz/beam -evt 100000 -min 20  -tsource 1 -inf "Ch=3900, 3900, 3900, ThickScint=1700V, SlimScinti=1100V, Ibeam=25.2nA, Ipico0=1250nA, 1 sample APV mode, 75 ns"

sleep 5

./Daq -out out/1109_mainz/pulser -evt 10000 -min 10  -tsource 2 -inf "Ch=3900, 3900, 3900, ThickScint=1700V, SlimScinti=1100V, Ibeam=25.2nA, Ipico0=1250nA - 1 sample APV mode, 75 ns"

sleep 5

./Daq -out out/1109_mainz/beam -evt 100000 -min 20  -tsource 1 -inf "Ch=3900, 3900, 3900, ThickScint=1700V, SlimScinti=1100V, Ibeam=25.2nA, Ipico0=1250nA, 1 sample APV mode, 75 ns"

sleep 5

./Daq -out out/1109_mainz/pulser -evt 10000 -min 10  -tsource 2 -inf "Ch=3900, 3900, 3900, ThickScint=1700V, SlimScinti=1100V, Ibeam=25.2nA, Ipico0=1250nA - 1 sample APV mode, 75 ns"

sleep 5

./Daq -out out/1109_mainz/beam -evt 100000 -min 20  -tsource 1 -inf "Ch=3900, 3900, 3900, ThickScint=1700V, SlimScinti=1100V, Ibeam=25.2nA, Ipico0=1250nA, 1 sample APV mode, 75 ns"

