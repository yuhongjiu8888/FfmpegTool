if [ -e build ]
then
    rm -rf build
fi

mkdir build && cd build && cmake .. && make -j && make install && cd .. && rm -rf build
