make && \
echo "----------MAKE COMPLETE----------" && \
./main_exe --file ./sample/sample3.in --init-g 0.2 --tau 100 --finil-g 0.0 --func sqa && \
echo "----------EXECUTION COMPLETE-----" && \
bat tmp*
