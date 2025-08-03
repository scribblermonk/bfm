#!/usr/bin/env bash 
#change directory to /build for the correct execution 
cd ~/Dokumente/iVtools/build

# fasta
#   filter [PATH]...                     - filters or transforms fasta files 
#   -o, --output STRING                - path to a fasta file (default: /dev/stdout)
#   -u, --upper-case                   - translates words to upper case 
#   -r, --randomize STRING             - a list of letters, if they not appear, replace with one of them randomly (default: "")
#   -t, --truncate UINT64              - truncate after given numbers of nucleotides (default: 18446744073709551615)
#echo "Preprocessing of the reference"
#./bin/iv fasta filter "/srv/public/nikov76/ncbi_dataset/data/GCF_000001405.40/GCF_000001405.40_GRCh38.p14_genomic.fna" -r "ACGT" -o "/srv/public/nikov76/enhanced_hg38" -u

# read_simulator                         - simulates reads of a certain length 
#   -i, --input PATH                     - path to a fasta file (default: "")
#   -o, --output PATH                    - path to the output fasta file (required)
#   --fasta_line_length UINT64           - How long should each fasta line be (0: infinite) (default: 80)
#   -l, --read_length UINT64             - length of the simulated reads (default: 150)
#   -n, --number_of_reads UINT64         - number of reads to simulate (default: 1000)
#   --substitution_errors UINT64         - number of substitution errors per read (default: 0)
#   --insertion_errors UINT64            - number of insert errors per read (default: 0)
#   --deletion_errors UINT64             - number of deletion errors per read (default: 0)
#   -e, --errors UINT64                  - number of errors (randomly chosen S, I or D) (default: 0)
#   --seed UINT32                        - seed to initialize the random generator (default: 0)
#   -d, --direction [rev_compl|fwd|both] - read direction, possible values: both: both direction, fwd: forward strand, rev_compl: reverse complement strand (default: both)
echo "Generation of queries"

echo "Registering number of reads"
export N=${1:-1000}

#toggle for shortened reduced genome
#export enhanced="/srv/public/nikov76/enhanced_hg38"
export enhanced="/srv/public/nikov76/short_enhanced_hg38"

# creating test enviroment
mkdir -p /srv/public/nikov76/lab/test_${N}

./bin/iv read_simulator -n ${N} -d fwd -i $enhanced -o "/srv/public/nikov76/lab/test_${N}/test_quer0_${N}.fasta"
./bin/iv read_simulator -n ${N} -d fwd -i $enhanced -o "/srv/public/nikov76/lab/test_${N}/test_quer1_${N}.fasta" -e 1 
./bin/iv read_simulator -n ${N} -d fwd -i $enhanced -o "/srv/public/nikov76/lab/test_${N}/test_quer2_${N}.fasta" -e 2
./bin/iv read_simulator -n ${N} -d fwd -i $enhanced -o "/srv/public/nikov76/lab/test_${N}/test_quer3_${N}.fasta" -e 3