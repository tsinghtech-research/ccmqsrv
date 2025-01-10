PREFIX=$CONDA_PREFIX
cmake .. -DCMAKE_BUILD_TYPE=Debug \
         -DBUILD_SHARED_LIBS=ON \
         -DCMAKE_INSTALL_PREFIX=$PREFIX \
         -DCMAKE_INSTALL_INCLUDEDIR=$PREFIX/include/ccmqsrv \
         -DCMAKE_PREFIX_PATH=$PREFIX \
         -DCMAKE_CXX_FLAGS="-lpthread -Wl,-rpath=$PREFIX/lib -L$PREFIX/lib" \
         -DBUILD_TESTS=ON \
         -DBUILD_EXAMPLES=ON
