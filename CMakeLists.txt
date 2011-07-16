cmake_minimum_required(VERSION 2.8)
PROJECT(SpeedKernel)
ADD_LIBRARY (SpeedKernel CIniFile.cpp SKernel_Opts.cpp SKernel_Sim.cpp SKernel_Misc.cpp SKernel_Read.cpp SKernel_Optimized.cpp SKernel_Result.cpp SpeedKernel.cpp)
