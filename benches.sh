#!/usr/bin/env bash 
#change directory to /build for the correct execution 
cd ~/Dokumente/bfm

# Usage:
# ./bfm [-r|--reduced] -t|--threads UINT64 -q|--queries PATH -ref|--reference PATH -e|--allowed_errors UINT64 [--help]
# Subcommand usage:
# ./bfm  [-r|--reduced] -t|--threads UINT64 -q|--queries PATH -ref|--reference PATH -e|--allowed_errors UINT64 [--help]
# Options:
# -r, --reduced               - enter reduced mode 
# -t, --threads UINT64        - enter thread count (required)
# -q, --queries PATH          - input query for fasta file location (required)
# -ref, --reference PATH      - input reference fasta file location (required)
# -e, --allowed_errors UINT64 - hamming/levinthal distance (required)
echo "benchmarking commences"
/usr/bin/time -a -o ../runtimelog -v ./bfm  -t 16 -q /home/mi/nikov76/Dokumente/bfm/0quer.fasta -ref /srv/public/nikov76/ncbi_dataset/data/GCF_000001405.40/GCF_000001405.40_GRCh38.p14_genomic.fna
