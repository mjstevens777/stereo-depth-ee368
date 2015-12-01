for scale in [0.25, 1.0/3.0, 0.5, 1.0]: # 3
	alg = "ncc"
	for window_size in [3, 5, 7, 9, 11, 13, 15, 17]: # 8
		print("./bin/stereo-depth-stats %g %s %d" % (scale, alg, window_size))

	alg = "gc"
	for Cp in [25, 50, 75, 100, 150, 200, 250, 300, 400]: # 9
		for V in [25, 50, 75, 100, 150, 200, 250, 300, 400]: # 9
			print("./bin/stereo-depth-stats %g %s %d %d" % (scale, alg, Cp, V))