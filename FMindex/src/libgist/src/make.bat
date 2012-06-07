#	Batchfile to make libgist 
#	$Header: /usr/local/devel/GiST/libgist/src/make.bat,v 1.1.1.1 1997/11/13 02:43:52 marcel Exp $

cd libgist
nmake /f Makefile.NT clean
nmake /f Makefile.NT
cd ..
cd librtree
nmake /f Makefile.NT clean
nmake /f Makefile.NT
cd ..
cd libbtree
nmake /f Makefile.NT clean
nmake /f Makefile.NT
cd ..
cd examples
cd btree
nmake /f Makefile.NT clean
nmake /f Makefile.NT
cd ..
cd rtree
nmake /f Makefile.NT clean
nmake /f Makefile.NT

