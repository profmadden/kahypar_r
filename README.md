# kahypar_r
KaHyPar wrapper library for Rust

--

Install kahypar.ini somewhere readable on the machine; the partitioner needs to be able to find this file and load it in for a partitioning context.  Essentially, a configuration file.

The KaHyPar library also needs to be installed.  Download and build from [kaypar.org](https://kahypar.org)

<pre>
git clone --depth=1 --recursive https://github.com/kahypar/kahypar.git
cd kahypar
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RELEASE -DKAHYPAR_USE_MINIMAL_BOOST=ON
make
</pre>

