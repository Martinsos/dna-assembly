# MODIFIED BY Martin Sosic <sosic.martin@gmail.com>
# Makefile for Structural-SVM, 11/11/08

#Use the following to compile under unix or cygwin
CPP = g++
CC  = gcc
#CPP = mpiCC
#CC  = mpicc

#Uncomment the following line to make CYGWIN produce stand-alone Windows executables
#SFLAGS = -mno-cygwin

CFLAGS  = $(SFLAGS) -O3 -fomit-frame-pointer -ffast-math -Wall
LDFLAGS = $(SFLAGS) -O3 -lm -Wall -lpng
#LIBS   = -L. -lm

LOBJ = common.o dataset.o learn.o bundle_ssvm_learn.o qp/qpas.o levelset/segment.o levelset/array.o lapack/f2c.o lapack/lapack.o curvL/curvL.o
COBJ = common.o dataset.o ssvm_classify.o levelset/segment.o levelset/array.o lapack/f2c.o lapack/lapack.o curvL/curvL.o

all: bundle_ssvm_learn ssvm_classify

bundle_ssvm_learn: $(LOBJ)
	$(CPP) $(LDFLAGS) $(LOBJ) -o bundle_ssvm_learn $(LIBS)


ssvm_classify: $(COBJ)
	$(CPP) $(LDFLAGS) $(COBJ) -o ssvm_classify $(LIBS)

common.o: common.cpp common.h
	$(CPP) -c $(CFLAGS) common.cpp -o common.o

dataset.o: dataset.cpp dataset.h lapack/lapack.h
	$(CPP) -c $(CFLAGS) dataset.cpp -o dataset.o

learn.o: learn.cpp learn.h
	$(CPP) -c $(CFLAGS) learn.cpp -o learn.o

bundle_ssvm_learn.o: bundle_ssvm_learn.cpp dataset.h common.h learn.h qp/qpas.h
	$(CPP) -c $(CFLAGS) bundle_ssvm_learn.cpp -o bundle_ssvm_learn.o

ssvm_classify.o: ssvm_classify.cpp dataset.h common.h
	$(CPP) -c $(CFLAGS) ssvm_classify.cpp -o ssvm_classify.o

qp/qpas.o: qp/qpas.cpp lapack/lapack.h qp/qpas.h
	$(CPP) -c $(CFLAGS) qp/qpas.cpp -o qp/qpas.o

levelset/segment.o: levelset/segment.cpp levelset/segment.h
	$(CPP) -c $(CFLAGS) levelset/segment.cpp -o levelset/segment.o
	
levelset/array.o: levelset/array.cpp levelset/array.h
	$(CPP) -c $(CFLAGS) levelset/array.cpp -o levelset/array.o

lapack/f2c.o: lapack/f2c.cpp lapack/f2c.h
	$(CPP) -c $(CFLAGS) lapack/f2c.cpp -o lapack/f2c.o

lapack/lapack.o: lapack/lapack.cpp lapack/lapack.h
	$(CPP) -c $(CFLAGS) lapack/lapack.cpp -o lapack/lapack.o
	
curvL/curvL.o: curvL/curvL.cpp curvL/curvL.h
	$(CPP) -c $(CFLAGS) curvL/curvL.cpp -o curvL/curvL.o
	
common.h: dataset.h levelset/segment.h curvL/curvL.h

levelset/segment.h: levelset/array.h

lapack/lapack.h: lapack/f2c.h

learn.h: dataset.h

curvL/curvL.h: curvL/dt.h

curvL/dt.h: curvL/image.h

clean:
	\rm *.o levelset/*.o lapack/*.o qp/*.o curvL/*.o bundle_ssvm_learn ssvm_classify
