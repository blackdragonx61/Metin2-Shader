///Add
#if defined(__BL_SHADER__)
	PyModule_AddIntConstant(poModule, "__BL_SHADER__", true);
#else
	PyModule_AddIntConstant(poModule, "__BL_SHADER__", false);
#endif