for IDX in {0..1} 9;
do
python3 utils/renderTiles.py outputs/case09/case09-output-$IDX.txt -o outputs/case09/case09-output-$IDX.png   
done

for IDX in {0..0};
do
python3 utils/renderTiles.py outputs/case09/case09-output-debug-$IDX.txt -o outputs/case09/case09-output-debug-$IDX.png   
done