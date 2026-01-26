idf_component_register(SRCS "src/hub75.cpp"
	PRIV_REQUIRES esp_driver_gpio
	INCLUDE_DIRS "include")

  # C++20
  target_compile_features(${COMPONENT_LIB} PUBLIC cxx_std_20)
