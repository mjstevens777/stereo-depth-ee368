ncc_list = []
gc_list = []

for scale in [1.0/3.0, 0.5]: # 2
	alg = "ncc"
	for window_size in [3, 5, 7, 9, 11, 13, 15, 17]: # 8
		ncc_list.append("./bin/stereo-depth-stats %g %s %d" % (scale, alg, window_size))

	alg = "gc"
	for Cp in [25, 50, 100, 150, 200, 250, 300, 400]: # 8
		for V in [25, 50, 100, 150, 200, 250, 300, 400]: # 8
			gc_list.append("./bin/stereo-depth-stats %g %s %d %d" % (scale, alg, Cp, V))

for i in range(max(len(gc_list), len(ncc_list))):
	if i < len(gc_list):
		print(gc_list[i])
	if i < len(ncc_list):
		print(ncc_list[i])