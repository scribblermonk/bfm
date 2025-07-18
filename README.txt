Command to execute:
    [0. ./bfm --help]
    1. ./bfm -t 16 -q /home/mi/nikov76/Dokumente/bfm/quer1.fasta -ref /home/mi/nikov76/Dokumente/bfm/ref.fna

Commands to Compile:
    1. mkdir build 
    2. cd build 
    3. cmake .. (flags can be added optionally)
    4. make 

Commands for using masonsimulator:
    1. export PATH=$HOME/Dokumente/seqan/build/bin/$PATH
    2. masonsimulator
    3. masonsimulator -o quer1.fasta

Cmake command for debugging:
    0. cmake .. -DCMAKE_BUILD_TYPE=Release
    1. cmake .. -DCMAKE_BUILD_TYPE=Debug