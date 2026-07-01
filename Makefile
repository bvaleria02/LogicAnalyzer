CC = gcc
#FLAGS = `pkg-config --cflags gtk+-3.0` -Wall -Werror -Wextra -std=gnu2x -static
FLAGS = `pkg-config --cflags gtk+-3.0` -Wall -Werror -Wextra -fsanitize=address -std=gnu2x
NMFLAGS = -O3 -fopt-info-vec-optimized -ffast-math -funroll-loops -march=native 
TARGET = main
SRCS = main.c logicanalyzer/*.c ./logicanalyzer/compiler/*.c ./logicanalyzer/dataLoader/*.c
FP_SRCS = ./logicanalyzer/fixedpoint/*.c ./logicanalyzer/fixedpoint/utils/*.c ./logicanalyzer/fixedpoint/fixedtrig/*.c
LINKS = `pkg-config --cflags --libs gtk+-3.0` -lm -lpthread -lrt -lgfortran -lepoxy
MINGW =  x86_64-w64-mingw32-gcc
WINTARGET = main.exe

# LUT generator
GEN_SRCS = ./logicanalyzer/fixedpoint/generators/*.c ./logicanalyzer/fixedpoint/convert.c
GEN_TARGET = ./logicanalyzer/fixedpoint/generators/fixedgenerators
GEN_LINKS = -lm

# fortran
F90_SRCS = ./logicanalyzer/dataLoader/*.f90
F90_TARGET = clampfuncs.o
F90_FLAGS = -Wall -Werror -Wextra -O3 -fopt-info-vec-optimized -std=f2018 -ffast-math -funroll-loops -march=native 
F90_FLAGS_2 = -fopenmp
F90_LINKS = -lgfortran
FC = gfortran

#bigint
TARGET_BI = bigint

#opengl
OGL_FLAGS = `pkg-config --cflags glfw3` 
OGL_LIBS  = `pkg-config --libs glfw3 epoxy`

all:
	$(CC) $(FLAGS) -o $(GEN_TARGET) $(GEN_SRCS) $(GEN_LINKS)
	$(GEN_TARGET)
	$(FC) $(F90_FLAGS) -c ./logicanalyzer/dataLoader/clampfuncs.f90 -o ./bin/fortranClampFuncs.o
	$(FC) $(F90_FLAGS) $(F90_FLAGS_2) -c ./logicanalyzer/numericMethods/dft.f90 -o ./bin/fortranDft.o
	$(FC) $(F90_FLAGS) $(F90_FLAGS_2) -c ./logicanalyzer/windowFunctions/windowFunctions.f90 -o ./bin/fortranWindowFunctions.o
	$(CC) $(FLAGS) -c ./logicanalyzer/binarysizewidget.c -o ./bin/binarysizewidget.o
	$(CC) $(FLAGS) -c ./logicanalyzer/bucket.c -o ./bin/bucket.o
	$(CC) $(FLAGS) -c ./logicanalyzer/channel.c -o ./bin/channel.o
	$(CC) $(FLAGS) -c ./logicanalyzer/clockset.c -o ./bin/clockset.o
	$(CC) $(FLAGS) -c ./logicanalyzer/clocksync.c -o ./bin/clocksync.o
	$(CC) $(FLAGS) -c ./logicanalyzer/colors.c -o ./bin/colors.o
	$(CC) $(FLAGS) -c ./logicanalyzer/connect.c -o ./bin/connect.o
	$(CC) $(FLAGS) $(NMFLAGS) -r ./logicanalyzer/matrix/*.c -o ./bin/matrixAll.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/create.c -o ./bin/matrixCreate.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/init.c -o ./bin/matrixInit.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/destroy.c -o ./bin/matrixDestroy.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/flags.c -o ./bin/matrixFlags.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/access.c -o ./bin/matrixAccess.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/print.c -o ./bin/matrixPrint.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/add.c -o ./bin/matrixAdd.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/sub.c -o ./bin/matrixSub.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/mul.c -o ./bin/matrixMul.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/matlab.c -o ./bin/matrixMatlab.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/swap.c -o ./bin/matrixSwap.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/getHighest.c -o ./bin/matrixGetHighest.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/lu.c -o ./bin/matrixLU.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/gaussjordan.c -o ./bin/matrixGaussJordan.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/copy.c -o ./bin/matrixCopy.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/norm.c -o ./bin/matrixNorm.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/qr.c -o ./bin/matrixQR.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/set.c -o ./bin/matrixSet.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/kronecker.c -o ./bin/matrixKronecker.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/matrix/opengl.c -o ./bin/matrixOpengl.o
	$(CC) $(FLAGS) $(NMFLAGS) -r ./logicanalyzer/la_bigint/*.c -o ./bin/bigIntAll.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/la_bigint/create.c -o ./bin/bigintCreate.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/la_bigint/add.c -o ./bin/bigintAdd.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/la_bigint/sub.c -o ./bin/bigintSub.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/la_bigint/mul.c -o ./bin/bigintMul.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/la_bigint/div.c -o ./bin/bigintDiv.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/la_bigint/logical.c -o ./bin/bigintLogical.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/la_bigint/boolean.c -o ./bin/bigintBoolean.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/la_bigint/print.c -o ./bin/bigintPrint.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/la_bigint/utils.c -o ./bin/bigintUtils.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/la_bigint/clz.c -o ./bin/bigintCLZ.o
	$(CC) $(FLAGS) $(NMFLAGS) -r ./logicanalyzer/numericMethods/*.c -o ./bin/numericMethodsAll.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/dft.c -o ./bin/numericMethodsDft.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/fft.c -o ./bin/numericMethodsFft.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/solver.c -o ./bin/numericMethodsSolver.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/solverArray.c -o ./bin/numericMethodsSolverArray.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/solverConsts.c -o ./bin/numericMethodsSolverConsts.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/ss.c -o ./bin/numericMethodsSS.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/derivative.c -o ./bin/numericMethodsDerivative.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/normalize.c -o ./bin/numericMethodsNormalize.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/convolve.c -o ./bin/numericMethodsConvolve.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/newtonRaphson.c -o ./bin/numericMethodsNewtonRaphson.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/secant.c -o ./bin/numericMethodsSecant.o
#	$(CC) $(FLAGS) $(NMFLAGS) -c ./logicanalyzer/numericMethods/operations.c -o ./bin/numericMethodsOperations.o
	$(CC) $(FLAGS) -c ./logicanalyzer/structures/linkstore.c -o ./bin/structsLinkStore.o
	$(CC) $(FLAGS) -c ./logicanalyzer/structures/model.c -o ./bin/structsModel.o
	$(CC) $(FLAGS) -c ./logicanalyzer/shaders/loader.c -o ./bin/shadersLoader.o
	$(CC) $(FLAGS) -c ./logicanalyzer/draw/fftfreq.c -o ./bin/drawFFTFreq.o
	$(CC) $(FLAGS) -c ./logicanalyzer/dialogs.c -o ./bin/dialogs.o
	$(CC) $(FLAGS) -c ./logicanalyzer/error.c -o ./bin/error.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fileio.c -o ./bin/fileio.o
	$(CC) $(FLAGS) -c ./logicanalyzer/filteriir.c -o ./bin/filteriir.o
	$(CC) $(FLAGS) -c ./logicanalyzer/filterwindow.c -o ./bin/filterwindow.o
	$(CC) $(FLAGS) -c ./logicanalyzer/filterwindowfunc.c -o ./bin/filterwindowfunc.o
	$(CC) $(FLAGS) -c ./logicanalyzer/hexview.c -o ./bin/hexview.o
	$(CC) $(FLAGS) -c ./logicanalyzer/labelspincombo.c -o ./bin/labelspincombo.o
	$(CC) $(FLAGS) -c ./logicanalyzer/linspace.c -o ./bin/linspace.o
	$(CC) $(FLAGS) -c ./logicanalyzer/menu.c -o ./bin/menu.o
	$(CC) $(FLAGS) -c ./logicanalyzer/pianotest.c -o ./bin/pianotest.o
	$(CC) $(FLAGS) -c ./logicanalyzer/preseteditor.c -o ./bin/preseteditor.o
	$(CC) $(FLAGS) -c ./logicanalyzer/protocol.c -o ./bin/protocol.o
	$(CC) $(FLAGS) -c ./logicanalyzer/record.c -o ./bin/record.o
	$(CC) $(FLAGS) -c ./logicanalyzer/scope.c -o ./bin/scope.o
	$(CC) $(FLAGS) $(OGL_FLAGS) -c ./logicanalyzer/spectrumanalyzer.c -o ./bin/spectrumanalyzer.o $(OGL_LIBS)
	$(CC) $(FLAGS) -c ./logicanalyzer/status.c -o ./bin/status.o
	$(CC) $(FLAGS) -c ./logicanalyzer/streamfile.c -o ./bin/streamfile.o
	$(CC) $(FLAGS) -c ./logicanalyzer/threads.c -o ./bin/threads.o
	$(CC) $(FLAGS) -c ./logicanalyzer/waveformeditor.c -o ./bin/waveformeditor.o
	$(CC) $(FLAGS) -c ./logicanalyzer/window.c -o ./bin/window.o
	$(CC) $(FLAGS) -c ./logicanalyzer/windowWidget.c -o ./bin/windowWidget.o
	$(CC) $(FLAGS) -c ./logicanalyzer/dataLoader/dataloader.c -o ./bin/dataLoaderMain.o
	$(CC) $(FLAGS) -c ./logicanalyzer/dataLoader/clamp.c -o ./bin/dataLoaderClamp.o
	$(CC) $(FLAGS) -c ./logicanalyzer/dataLoader/clampfuncs.c -o ./bin/dataLoaderClampFuncs.o
	$(CC) $(FLAGS) -c ./logicanalyzer/dataLoader/clampInst.c -o ./bin/dataLoaderClampInst.o
	$(CC) $(FLAGS) -c ./logicanalyzer/dataLoader/endianness.c -o ./bin/dataLoaderEndianness.o
	$(CC) $(FLAGS) -c ./logicanalyzer/dataLoader/format.c -o ./bin/dataLoaderFormat.o
	$(CC) $(FLAGS) -c ./logicanalyzer/dataLoader/norm.c -o ./bin/dataLoaderNorm.o
	$(CC) $(FLAGS) -c ./logicanalyzer/compiler/creatememory.c -o ./bin/compilerCreateMemory.o
	$(CC) $(FLAGS) -c ./logicanalyzer/compiler/define.c -o ./bin/compilerDefine.o
	$(CC) $(FLAGS) -c ./logicanalyzer/compiler/filereader.c -o ./bin/compilerFileReader.o
	$(CC) $(FLAGS) -c ./logicanalyzer/compiler/mmapfile.c -o ./bin/compilerMmapFile.o
	$(CC) $(FLAGS) -c ./logicanalyzer/compiler/munmapfile.c -o ./bin/compilerMunmapFile.o
	$(CC) $(FLAGS) -c ./logicanalyzer/compiler/mwrite.c -o ./bin/compilerMwrite.o
	$(CC) $(FLAGS) -c ./logicanalyzer/compiler/split.c -o ./bin/compilerSplit.o
	$(CC) $(FLAGS) -c ./logicanalyzer/pzmap/pzmap.c -o ./bin/pzmapMain.o
	$(CC) $(FLAGS) -c ./logicanalyzer/filter/circuitsim.c -o ./bin/filterCircuitSim.o
	$(CC) $(FLAGS) -c ./logicanalyzer/laComplex/laComplex.c -o ./bin/laComplexMain.o
	$(CC) $(FLAGS) -c ./logicanalyzer/laComplex/roots.c -o ./bin/laComplexRoots.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/convert.c -o ./bin/fixedpointConvert.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/fixedgamma.c -o ./bin/fixedpointGamma.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/fixedcserp.c -o ./bin/fixedpointCSerp.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/fixedneg.c -o ./bin/fixedpointNeg.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/fixedmul.c -o ./bin/fixedpointMul.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/fixedrecp.c -o ./bin/fixedpointRecp.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/multin.c -o ./bin/fixedpointMultiN.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/fixederrno.c -o ./bin/fixedpointErrno.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/fixedconstants.c -o ./bin/fixedpointConstants.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/fixedabs.c -o ./bin/fixedpointAbs.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/utils/getlimits.c -o ./bin/fixedpointUtilsGetLimits.o
	$(CC) $(FLAGS) -c ./logicanalyzer/fixedpoint/utils/exponent.c -o ./bin/fixedpointUtilsExponent.o
	$(CC) $(FLAGS) $(OGL_FLAGS) -c ./logicanalyzer/glad/glad.c -o ./bin/glad.o $(OGL_LIBS)
	$(CC) $(FLAGS) -c ./main.c -o ./bin/main.o
	$(CC) $(FLAGS) $(F90_FLAGS_2) ./bin/*.o -o $(TARGET) $(LINKS)
	./$(TARGET)

bigint:
	rm ./$(TARGET_BI) &
	$(CC) $(FLAGS) -c ./logicanalyzer/la_bigint/create.c -o ./bin/bigintCreate.o
	$(CC) $(FLAGS) -c ./logicanalyzer/la_bigint/add.c -o ./bin/bigintAdd.o
	$(CC) $(FLAGS) -c ./logicanalyzer/la_bigint/sub.c -o ./bin/bigintSub.o
	$(CC) $(FLAGS) -c ./logicanalyzer/la_bigint/mul.c -o ./bin/bigintMul.o
	$(CC) $(FLAGS) -c ./logicanalyzer/la_bigint/div.c -o ./bin/bigintDiv.o
	$(CC) $(FLAGS) -c ./logicanalyzer/la_bigint/logical.c -o ./bin/bigintLogical.o
	$(CC) $(FLAGS) -c ./logicanalyzer/la_bigint/boolean.c -o ./bin/bigintBoolean.o
	$(CC) $(FLAGS) -c ./logicanalyzer/la_bigint/print.c -o ./bin/bigintPrint.o
	$(CC) $(FLAGS) -c ./logicanalyzer/la_bigint/utils.c -o ./bin/bigintUtils.o
	$(CC) $(FLAGS) -c ./logicanalyzer/la_bigint/clz.c -o ./bin/bigintCLZ.o
	$(CC) $(FLAGS) -c ./bigint.c -o ./bin/main.o
	$(CC) $(FLAGS) $(F90_FLAGS_2) ./bin/*.o -o $(TARGET_BI) $(LINKS)
	rm -r ./*.mod
	./$(TARGET_BI)

windows:
	$(MINGW) $(FLAGS) -o $(WINTARGET) $(SRCS) $(LINKS)

opengl:
	#$(CC) `pkg-config --cflags glfw3` -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl o.c -o ./o_test.elf `pkg-config --libs glfw3`
	$(CC) $(FLAGS) $(NMFLAGS) -r ./logicanalyzer/matrix/*.c -o ./bin/matrixAll.o
	$(CC) $(FLAGS) `pkg-config --cflags glfw3` ./logicanalyzer/shaders/*.c ./logicanalyzer/glad/glad.c o2.c ./bin/matrixAll.o -o ./o_test.elf `pkg-config --libs glfw3` -lm
	./o_test.elf
