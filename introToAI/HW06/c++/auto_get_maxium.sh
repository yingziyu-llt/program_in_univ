maxn=0
continuous=249
for ((i = 250; i <= 400; i++)); do
	timeout 5 ./queens_simulated_anneal.run $i >/dev/null
	if [ $? -eq 0 ]; then
		maxn=$i
		if [[ $continuous -eq $i-1 ]]; then
			continuous=$i
		fi
	fi
done
echo "Maxn = $maxn"
echo "Continuous = $continuous"
