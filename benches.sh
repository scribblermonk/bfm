#!/usr/bin/env bash 
#change directory to /build for the correct execution 
cd ~/Dokumente/bfm/build

# Usage:
# ./bfm [-r|--reduced] -t|--threads UINT64 -q|--queries PATH -ref|--reference PATH -e|--allowed_errors UINT64 [--help]
# Subcommand usage:
# ./bfm  [-r|--reduced] -t|--threads UINT64 -q|--queries PATH -ref|--reference PATH -e|--allowed_errors UINT64 [--help]
# Options:
# -r, --reduced               - enter reduced mode 
# -t, --threads UINT64        - enter thread count (required)
# -q, --queries PATH          - input query for fasta file location (required)
# -ref, --reference PATH      - input reference fasta file location (required)
# -e, --allowed_errors UINT64 - hamming/Levenshtein distance (required)
echo "Benchmarking commences"

echo "Registering number of reads"
export N=${1:-1000}

#togggle for shortened reduced genome
#export enhanced="/srv/public/nikov76/enhanced_hg38"
export enhanced="/srv/public/nikov76/short_enhanced_hg38"

echo "Not reduced"
for i in $(seq 0 3); do
echo "Hamming/Levinthal distance ${i}"
/usr/bin/time -o /home/mi/nikov76/Dokumente/bfm/lab/test_${N}/runtimelog_${i}_n${N} -v ./bfm  -t 16 -e ${i} -q /home/mi/nikov76/Dokumente/bfm/lab/test_${N}/test_quer${i}_${N}.fasta -ref $enhanced
mv ../hit_log.txt /home/mi/nikov76/Dokumente/bfm/lab/test_${N}/hit_log_${i}_n${N}
done 

echo "Reduced"
for i in $(seq 0 3); do
echo "Hamming/Levinthal distance ${i}"
/usr/bin/time -o /home/mi/nikov76/Dokumente/bfm/lab/test_${N}/runtimelog_r_${i}_n${N} -v ./bfm -r -t 16 -e ${i} -q /home/mi/nikov76/Dokumente/bfm/lab/test_${N}/test_quer${i}_${N}.fasta -ref $enhanced
mv ../hit_log.txt /home/mi/nikov76/Dokumente/bfm/lab/test_${N}/hit_log_r_${i}_n${N}
done 