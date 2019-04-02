set(RTOW_SRC_CH06
	camera.h
	hitable.h
	hitable_list.h
	main.cc
	#material.h
	ray.h
	sphere.h
	vec3.h
)

add_executable(rtow_ch06_antialiasing ${RTOW_SRC_CH06})